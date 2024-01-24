#include "coordinates/vec4.h"
#include "scene/figure.h"
#include "scene/camera.h"
#include "utilities/randomGenerator.h"
#include "scene/sceneLoader.h"
#include "utilities/spaceSectioner.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <sstream>
#include <ctime>

using namespace std;


// Returns the color of the ont hit.
Color getColorOfHit (const Figure* figure, list<Figure*> figures, const Point& hit,
					 LightSource& lightSource, const Camera& camera, Color& scatter,
					 Phenomenom ph, Direction rayDirection) {

	// Check if the plane is against the light.
	if (figure->planeAgainstLight(camera, lightSource, hit))
			return Color(0);

	// Check if the point is in shadow.
	Vec4 distanceToLight = distance(lightSource.center, hit);
	float modDistanceToLight = distanceToLight.mod();
	float t;
	for (Figure* fig : figures)
			if (fig->intersect(Ray(lightSource.center, -distanceToLight), t)
			&&	(1 - t) * modDistanceToLight > 1e-6
			&&	(  fig != figure
				|| distance(hit, lightSource.center - t*(distanceToLight)).mod() > 1e-4))
						return Color(0);

	// Calculate the color of the hit.
	Color Li = lightSource.power / (modDistanceToLight*modDistanceToLight);
	float cosine = abs(dot(figure->getNormal(hit), distanceToLight.normalize()));
	return Li * figure->getFr(ph, rayDirection, hit) * cosine * scatter;

}


// Captures a section of the image.
void captureSection(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources,
					int raysPerPixel, int minX, int maxX, int minY, int maxY,
					vector<Color>& pixelsValue) {
	
	Vec4 modL = camera.l.normalize();					// L and U normalized vectors.
	Vec4 modU = camera.u.normalize();
	Vec4 sightOrigin = camera.f + camera.l + camera.u;	// Point located at the top left corner of the image.
	randomGenerator rand(0, 1);							// Random number generator.
	Color pxColor, rayColor;							// Total color for one pixel / ray.
	Ray ray;											// Current ray.
	Direction rayDirection;								// Direction of ray.
	float t, minT;										// Distance to the closest figure.
	Point hit;											// Point on whith the ray hits.
	Figure* closestFigure;								// Closest figure to the camera.
	Color scatter;										// Color of the light scattered by the figure.
	Phenomenom ph;										// Phenomenom of a interaction.
	
	// For each pixel in the section.
	for (int i = minY; i < maxY; i++)
		for (int j = minX; j < maxX; j++) {

			pxColor = Color();

			// For each ray.
			for (int k = 0; k < raysPerPixel; k++) {

				// Ray color starts being 0.
				rayColor = Color();

				// At first, scatter doesn't affect the total color.
				scatter = Color(1);

				// Get the direction of the ray.
				rayDirection = Direction(sightOrigin - (j+rand.get())*camera.widthPerPixel*modL
						- (i+rand.get())*camera.heightPerPixel*modU);
				// Build the ray using the camera's origin and the direction.
				ray = Ray(camera.o, rayDirection);

				// While the ray hits a figure and it's not absorbed.
				while (true) {

					// Get the first figure that intersects the ray, and the
					// point of the intersection.
					minT = 1e6;
					closestFigure = nullptr;
					for (Figure* figure : figures)
						if (figure->intersect(ray, t) && t < minT) {
							minT = t;
							closestFigure = figure;
					}
					if (closestFigure == nullptr) break;
					hit = ray.getPoint() + minT*ray.getDirection();

					// Get the phenomenom of the interaction.
					ph = closestFigure->getPhenomenom();

					// ABSORPTION: finish.
					if (ph == NONE)
							break;

					// LIGHT: add the light color and finish.
					else if (ph == LIGHT) {
						rayColor += closestFigure->kl * scatter;
						break;
					} 

					// REFLECTION: update scatter and get new ray.
					else if (ph == REFLECTION)
							scatter *= closestFigure->getFr(ph, rayDirection, hit);

					// REFRACTION: update scatter and get new ray.
					else if (ph == REFRACTION)
							scatter *= closestFigure->getFr(ph, rayDirection, hit);

					// DIFFUSE: add contribution, update scatter and get new ray.
					else {
						for (int i = 0; i < lightSources.size(); ++i)
								rayColor += getColorOfHit(closestFigure, figures,
												hit, lightSources[i], camera,
												scatter, ph, rayDirection);
						scatter *= M_PI * closestFigure->getFr(ph, rayDirection, hit);
					}

					// Update the ray.
					hit = hit + 1e-4 * rayDirection;
					ray = Ray(hit, rayDirection);

				}

				// If valid, add the ray color to the pixel color.
				if (isnan(rayColor[0]) || isnan(rayColor[1]) || isnan(rayColor[2]))
						k--;
				else
						pxColor += rayColor;
			}

			// Store the pixel color.
			pixelsValue[j*camera.height + i] = pxColor;
	}
}

void captureSlave (Camera& camera, list<Figure*> figures, vector<LightSource> lightSources,
				int raysPerPixel, SpaceSectioner& tiles, vector<Color>& pixelsValue) {

	int minX, maxX, minY, maxY;
	// While there are sections left to capture, get one and capture it.
	while (tiles.getSection(minX, maxX, minY, maxY))
		captureSection(camera, figures, lightSources, raysPerPixel,
				minX, maxX, minY, maxY, pixelsValue);

}

// Capture the scene from the camera's point of view.
void pathTracing(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources,
				int raysPerPixel, int threads, string fileName) {

	// Open the file to write the image.
	ofstream output(fileName);
	if (!output.is_open()) {
		cerr << "Error opening output file \"" << fileName << "\"" << endl;
		exit(1);
	}

	// Write the header of the PPM file.
	output << "P3" << endl;
	output << camera.width << " " << camera.height << endl;
	
	// Structure to store the final image.
	vector<Color> finalImage(camera.height * camera.width);

	// Divide the image into sections.
	SpaceSectioner tiles(camera.width, camera.height, threads, threads);

	// Divide the work between the threads. Each one will be capturing a section
	// of the image while there are sections left.
	vector<thread> threadsArray(threads);
	for (int t = 0; t < threads; t++) {
		int minH = t * camera.height / threads;
		int maxH = (t + 1) * camera.height / threads;
		threadsArray[t] = thread(&captureSlave, ref(camera), ref(figures),
				ref(lightSources), raysPerPixel, ref(tiles), ref(finalImage));
	}

	// Wait for all threads to finish.
	for (int t = 0; t < threads; t++) threadsArray[t].join();

	// Find the maximum value of the pixels.
	float max = 0;
	for (int i = 0; i < camera.height; i++) {
		for (int j = 0; j < camera.width; j++) {
			Color pxColor = finalImage[i*camera.width + j];
			if (pxColor[0] > max) max = pxColor[0];
			if (pxColor[1] > max) max = pxColor[1];
			if (pxColor[2] > max) max = pxColor[2];
		}
	}
	output << max * 255 / raysPerPixel << endl;

	// Write the final image into the file.
	for (int i = 0; i < camera.height; i++) {
		for (int j = 0; j < camera.width; j++) {
			Color pxColor = finalImage[j*camera.height + i];
			output << pxColor*255/raysPerPixel << "    ";
		}
		output << endl;
	}
	output.close();

}


// Main function.
int main(int argc, char* argv[]) {

	// Check the number of arguments.
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " <scene_file> <output_file>" << endl;
		return 1;
	}

	// Camera, list of figures and light sources.
	Camera camera;
	list<Figure*> listFigures = {};
	vector<LightSource> lightSources = {};
	int raysPerPixel, threads, photons, neighbors;
	float radius;

	// Populate the camera, lights and figures.
	unsigned t0 = clock();
	bool popDone = populateList (camera, listFigures, lightSources, raysPerPixel, threads, photons, radius, neighbors, argv[1], false);
	unsigned t1 = clock();

	double time = (double(t1-t0)/CLOCKS_PER_SEC);
	cout << "File reading time: " << time << " s" << endl;

	// Check if the population was successful.
	if (!popDone) return 1;

	// Capture the scene and store it at the specified file.
	t0 = clock();
	pathTracing(camera, listFigures, lightSources, raysPerPixel, threads, argv[2]);
	t1 = clock();

	time = (double(t1-t0)/CLOCKS_PER_SEC);
	cout << "Image processing time: " << time << " s" << endl;

}