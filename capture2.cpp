#include "coordinates/vec4.h"
#include "scene/figure.h"
#include "scene/camera.h"
#include "utilities/randomGenerator.h"
#include "photon/photon.h"
#include "photon/photonMap.h"
#include "scene/sceneLoader.h"
#include "utilities/spaceSectioner.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>

using namespace std;

float gauss (float sigma, float x) {
	return exp(-0.5 * x * x / sigma / sigma) / (sigma * sqrt(2 * M_PI));
}

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

	Color Li = lightSource.power / (modDistanceToLight*modDistanceToLight);
	float cosine = abs(dot(figure->getNormal(hit), distanceToLight.normalize()));
	return Li * figure->getFr(ph, rayDirection, hit) * cosine * scatter;

}

inline Color photonContribution (const Photon* photon, const Figure* figure, Phenomenom ph, Direction rayDirection, float r) {
	return figure->getFr(ph, rayDirection, photon->pos) * photon->flux / (M_PI * r * r);
}

void captureSection(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources, PhotonMap photonMap, int raysPerPixel, bool nextEventEstimation, int minX, int maxX, int minY, int maxY, std::vector<Color>& pixelsValue) {
	
	Vec4 modL = camera.l.normalize();					// Get L and U normalized vectors.
    Vec4 modU = camera.u.normalize();
	Vec4 sightOrigin = camera.f + camera.l + camera.u;	// Get the point located at the top left corner of the image.
	randomGenerator rand(0, 1);							// Random number generator.
	Color pxColor, rayColor;										// Stores the total color for the pixel.

	Direction rayDirection;								// Direction of the next ray.
	Ray ray;											// Next ray.
	float t, minT;										// Stores the distance to the closest figure.
	Point hit;											// Stores the point on whith the ray hits.
	Figure* closestFigure;								// Stores the closest figure to the camera.
	Color scatter;										// Stores the color of the light scattered by the figure.
	Phenomenom ph;
	float radius = 1e-1;

	for (int i = minX; i < maxX; i++) {
		for (int j = minY; j < maxY; j++) {
			pxColor = Color();
			for (int k = 0; k < raysPerPixel; k++) {
				// Ray color which starts being 0.
				rayColor = Color();
				scatter = Color(1);
				
				// Get the direction of the ray.
				rayDirection = Direction(sightOrigin - (j+rand.get())*camera.widthPerPixel*modL - (i+rand.get())*camera.heightPerPixel*modU);
				// Build the ray using the camera's origin and the direction.
				ray = Ray(camera.o, rayDirection);

				while(true) {

					minT = 1e6;
					closestFigure = nullptr;
					for (Figure* figure : figures)
						if (figure->intersect(ray, t) && t < minT) {
							minT = t;
							closestFigure = figure;
					}

					if (closestFigure == nullptr) break;
					hit = ray.getPoint() + minT*ray.getDirection();
					ph = closestFigure->getPhenomenom();

					if (ph == NONE) break;
					// REFLECTION
					else if (ph == REFLECTION) {
						rayDirection = closestFigure->reflectionBounce(rayDirection, hit);
					} else if (ph == REFRACTION) {
						closestFigure->getFr(ph, rayDirection, hit);
					} 
					// DIFFUSE
					else {
						if(nextEventEstimation){
							for(LightSource lightSource : lightSources){
								rayColor += getColorOfHit(closestFigure, figures, hit, lightSource, camera, scatter, ph, rayDirection);
							}
						}

						//cout << "diffuse" << endl;
						// vector <const Photon*> nearestPhotons = search_nearest(photonMap, hit, 100, 1e-1);
						vector <const Photon*> nearestPhotons = photonMap.nearest_neighbors(hit, 50, 1e-1);
						//cout << "nearest photons: " << nearestPhotons.size() << endl;
						for (const Photon* photon : nearestPhotons) {
							Photon p = *photon;
							//cout << p << endl;
							//float r = distance(hit, p.pos).mod();
							if(p.figure == closestFigure->id){
								//rayColor += closestFigure->getFrDiffuse() * p.flux * gauss(0.75, distance(hit, p.pos).mod()/1e-1)/ (M_PI * 1e-1);
								rayColor += closestFigure->getFrDiffuse() * p.flux / (M_PI * radius * radius) * (1 - distance(hit, p.pos).mod() / 1e-1);
							}
						}
						break;
					}
					hit = hit + 1e-4 * rayDirection;
					ray = Ray(hit, rayDirection);
				}
				if (isnan(rayColor.c[0]) || isnan(rayColor.c[1]) || isnan(rayColor.c[2]))
						k--;
				else
						pxColor += rayColor;
			}
			pixelsValue[i*camera.width + j] = pxColor;
		}
	}
}

void captureSlave (Camera& camera, list<Figure*> figures, vector<LightSource> lightSources, PhotonMap photonMap, int raysPerPixel, bool nextEventEstimation, SpaceSectioner& tiles, vector<Color>& pixelsValue) {
	int minX, maxX, minY, maxY;
	while (tiles.getSection(minX, maxX, minY, maxY))
		captureSection(camera, figures, lightSources, photonMap, raysPerPixel, nextEventEstimation, minX, maxX, minY, maxY, pixelsValue);
}

// Capture the scene from the camera's point of view
void capture(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources, int N, int raysPerPixel, int threads, bool nextEventEstimation, string fileName) {
	float t;											// Stores the distance to the closest figure.
	// Open the file to write the image.
	std::ofstream output(fileName);

	// Write the header of the PPM file.
	output << "P3" << endl;
	//output << "# MAX" << endl;
	output << camera.width << " " << camera.height << endl;
	

	// Structure to store the final image.
	vector<Color> finalImage(camera.height * camera.width);
	
	// Argumento N que indica el númeor de rayos que se lanzan
	int* samplesForLightSource = new int[lightSources.size()];
	float powerSum = 0;
	for (int i = 0; i < lightSources.size(); ++i){
			powerSum += lightSources[i].power.sum();
	}
	// For each light source, calculate the number of samples.
	for (int i = 0; i < lightSources.size(); ++i)
			samplesForLightSource[i] = N * lightSources[i].power.sum() / powerSum;

	list<Photon> photons;
	srand(time(NULL));
	
	for (int i = 0; i < lightSources.size(); ++i) {
		for (int j = 0, k = 0; j < samplesForLightSource[i]; ++j) {
			Direction dir;
			dir = dir.randomDirection();
			Color flux = 4 * M_PI * lightSources[i].power / samplesForLightSource[i];
			Ray ray = Ray(lightSources[i].center, dir);
			bool store = !nextEventEstimation;
			while (true) {				

				float minT = 1e6;
				Figure* closestFigure = nullptr;
				for (Figure* figure : figures){
					if (figure->intersect(ray, t) && t < minT) {
						minT = t;
						closestFigure = figure;
					}
				}
				if (closestFigure == nullptr) break;
				Vec4 hitVec = ray.getPoint() + minT*ray.getDirection();
				Point hit = Point(hitVec);
				//cout << "hit: " << hit << endl;
				Direction normal = closestFigure->getNormal(hit);
			
				Phenomenom ph = closestFigure->getPhenomenom();
				if (ph == REFLECTION) {
					dir = closestFigure->reflectionBounce(dir, hit);
					store = true;
				} else if (ph == REFRACTION) {
					closestFigure->getFr(ph, dir, hit);
					store = true;
				} 
				else if (ph == DIFFUSE) {
					if(store){
						Photon photon = Photon(hit, dir, flux, closestFigure->id);
						photons.push_back(photon);
					} else {
						store = true;
					}
					flux *= closestFigure->getFr(ph, dir, hit);
				} else break;
				hit = hit + 1e-4 * dir;
				ray = Ray(hit, dir);
				
			}
		}
	}
	
	PhotonMap photonMap = PhotonMap(photons, PhotonAxisPosition());
	SpaceSectioner tiles(camera.width, camera.height, threads, threads);
	
	// Divide the work between the threads. Each one will capture a section of the image.
	vector<thread> threadsArray(threads);
	for (int t = 0; t < threads; t++) {
		int minH = t * camera.height / threads;
		int maxH = (t + 1) * camera.height / threads;
		threadsArray[t] = thread(&captureSlave, ref(camera), ref(figures), ref(lightSources), ref(photonMap), raysPerPixel, nextEventEstimation, ref(tiles), ref(finalImage));
	}
	
	// Wait for all threads to finish.
	for (int t = 0; t < threads; t++) threadsArray[t].join();
	// Finds the maximum value of the pixels.
	float max = 0;
	for (int i = 0; i < camera.height; i++) {
		for (int j = 0; j < camera.width; j++) {
			Color pxColor = finalImage[i*camera.width + j];
			if (pxColor.c[0] > max) max = pxColor.c[0];
			if (pxColor.c[1] > max) max = pxColor.c[1];
			if (pxColor.c[2] > max) max = pxColor.c[2];
		}
	}
	output << max * 255 / raysPerPixel << endl;

	// Write the final image into the file.
	for (int i = 0; i < camera.height; i++) {
		for (int j = 0; j < camera.width; j++) {
			Color pxColor = finalImage[i*camera.width + j];
			output << pxColor*255/raysPerPixel << "    ";
		}
		output << endl;
	}
	output.close();
}

int main(int argc, char* argv[]) {

	// Check the number of arguments.
	if (argc != 4 || !(argv[3][0] == 'y' || argv[3][0] == 'n')) {
		cout << "Usage: " << argv[0] << " <scene_file> <output_file> <next_event_estimation y/n>" << endl;
		return 1;
	}

	// Camera, list of figures and light sources.
    Camera camera;
    list<Figure*> listFigures = {};
	vector<LightSource> lightSources = {};
	int raysPerPixel, threads, photons;

	// Populate the camera, lights and figures.
	populateList (camera, listFigures, lightSources, raysPerPixel, threads, photons, argv[1], true);

	// Capturing the scene and storing it at the specified file.
    capture(camera, listFigures, lightSources, photons, raysPerPixel, threads, argv[3][0] == 'y', argv[2]);

}