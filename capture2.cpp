#define _USE_MATH_DEFINES

#include "vec4.h"
#include "figure.h"
#include "camera.h"
#include "randomGenerator.h"
#include "photon.h"
#include "photonMap.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>

using namespace std;


Color photonContribution (const Photon* photon, const Figure* figure, Phenomenom ph, Direction rayDirection, float r) {
	return figure->getFr(ph, rayDirection, photon->pos) * photon->flux / (M_PI * r * r);
}

void captureSection(Camera& camera, list<Figure*> figures, PhotonMap photonMap, int raysPerPixel, int maxBounces, int minH, int maxH, std::vector<Color>& pixelsValue) {
	
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

	for (int i = minH; i < maxH; i++) {
		//cout << i << endl;
		for (int j = 0; j < camera.width; j++) {
			pxColor = Color();
			for (int k = 0; k < raysPerPixel; k++) {
				// Ray color which starts being 0.
				rayColor = Color();

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
					else if (ph == LIGHT) {
						rayColor += closestFigure->kl * scatter;
						break;
					} 
					// REFLECTION
					else if (ph == REFLECTION) {
						//rayDirection = closestFigure->reflectionBounce(rayDirection, hit);
					} else if (ph == REFRACTION) {
						//rayDirection = closestFigure->refractionBounce(rayDirection, hit);
					} 
					// DIFFUSE
					else {
						cout << "diffuse" << endl;
						auto nearestPhotons = search_nearest(photonMap, hit, 100, 1e-1);
						cout << "nearest photons: " << nearestPhotons.size() << endl;
						for (const Photon* photon : nearestPhotons) {
							const Point p = photon->pos;
							cout << p << endl;
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

// Capture the scene from the camera's point of view
void capture(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources, int N, int raysPerPixel, int maxBounces, int threads, string fileName) {
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
	
	for (int i = 0; i < lightSources.size(); ++i) {
		for (int j = 0, k = 0; j < samplesForLightSource[i]; ++j) {
			Direction dir;
			dir = dir.randomDirection();
			Color flux = 4 * M_PI * lightSources[i].power / samplesForLightSource[i];
			Ray ray = Ray(lightSources[i].center, dir);
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
				/*if (ph == REFLECTION) {
					photons.push_back(Photon(hit, reflection, flux));
					ray = Ray(hit, reflection);
				} else if (ph == REFRACTION) {
					photons.push_back(Photon(hit, refraction, flux));
					ray = Ray(hit, refraction);
				} */
				if (ph == DIFFUSE) {
					Photon photon = Photon(hit, dir, flux);
					photons.push_back(photon);
					flux *= closestFigure->getFr(ph, dir, hit);
				} else break;
				hit = hit + 1e-4 * dir;
				ray = Ray(hit, dir);
				
			}
		}
	}
	
	PhotonMap photonMap = PhotonMap(photons, PhotonAxisPosition());
	
	// Divide the work between the threads. Each one will capture a section of the image.
	vector<thread> threadsArray(threads);
	for (int t = 0; t < threads; t++) {
		int minH = t * camera.height / threads;
		int maxH = (t + 1) * camera.height / threads;
		threadsArray[t] = thread(&captureSection, ref(camera), ref(figures), ref(photonMap), raysPerPixel, maxBounces, minH, maxH, ref(finalImage));
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
	if (argc != 5) {
		cout << "Usage: " << argv[0] << " <output_file> <rays per pixel> <max bounces> <number of threads>" << endl;
		return 1;
	}

	// Defining the camera.
	Direction l = Direction(-1, 0, 0); // l.rotateY(-15);
	Direction u = Direction(0, 1, 0); // u.rotateY(-15);
	Direction f = Direction(0, 0, 3); // f.rotateY(-15);
    Camera camera = Camera(Point(0, 0, -3.5), l, u, f, 512, 512);
	
	// Defining the scene.
    list<Figure*> listFigures = {};

	// Defining the figures.
	Plane* leftPlane = new Plane(Direction(1, 0, 0), 1, Color(1, 0, 0), Color(0), Color(0), Color(0), 0);
	Plane* rightPlane = new Plane(Direction(-1, 0, 0), 1, Color(0, 1, 0), Color(0), Color(0), Color(0), 0);
	Plane* floorPlane = new Plane(Direction(0, 1, 0), 1, Color(1), Color(0), Color(0), Color(0), 0);
	Plane* ceilingPlane = new Plane(Direction(0, -1, 0), 1, Color(0), Color(0), Color(0), Color(1), 0);
	Plane* backPlane = new Plane(Direction(0, 0, -1), 1, Color(1), Color(0), Color(0), Color(0), 0); // 0,8

	Sphere* leftSphere = new Sphere(Point(-0.5, -0.7, 0.25), 0.3, Color(0.94, 0.72, 0.95), Color(0), Color(0), Color(0), 0);
	//Sphere* leftSphere = new Sphere(Point(-0.5, -0.7, 0.25), 0.3, Color(0.2765, 0.5, 0.5), Color(0.5), Color(0), Color(0), 0);
	Sphere* rightSphere = new Sphere(Point(0.5, -0.7, -0.25), 0.3, Color(0.72, 0.94, 0.95), Color(0), Color(0), Color(0), 0);
	//Sphere* rightSphere = new Sphere(Point(0.5, -0.7, -0.25), 0.3, Color(0), Color(0.2), Color(0.8), Color(0), 1.5);

	/*
	Triangle* triangle = new Triangle(Point(-0.25, -0.5, -0.5), Point(1.5, 0, 1), Point(-0.25, 1, 0), Color(1, 0.5, 0), 1, 0, 0, 0);

	Disc* disc = new Disc(Point(-0.6, 0.6, 0), Direction(-2, 1, 1), 0.2, Color(0, 1, 1), 1, 0, 0, 0);
	PerforedDisc* perforedDisc = new PerforedDisc(Point(-0.6, 0.2, -0.2), Direction(1, 1, 1), 0.3, 0.2, Color(0, 1, 1), 1, 0, 0, 0);

	Cylinder* cylinder = new Cylinder(Point(-0.5, 0, 0.4), Direction(0, 0, 1), 0.2, 1.5, Color(0.5, 0, 1), 1, 0, 0, 0);

	Cone* cone = new Cone (Point(0.5, -0.5, 0), Direction(0, 1, -0.2), 0.5, 0.5, Color(1, 0.5, 0), 1, 0, 0, 0);
	*/
    listFigures.push_back(leftPlane);
    listFigures.push_back(rightPlane);
    listFigures.push_back(floorPlane);
    listFigures.push_back(ceilingPlane);
    listFigures.push_back(backPlane);

    listFigures.push_back(leftSphere);
    listFigures.push_back(rightSphere);
	
	// listFigures.push_back(centerSphere);

	// listFigures.push_back(triangle);
	// listFigures.push_back(disc);
	// listFigures.push_back(perforedDisc);
	// listFigures.push_back(cylinder);
	// listFigures.push_back(cone);
	
	// Defining the light sources.
	vector<LightSource> lightSources = {};
	lightSources.push_back(LightSource(Point(0, 0.5, 0), Color(0.99, 0.99, 0.99)));
	//lightSources.push_back(LightSource(Point(-0.5, 0, 0.4), Color(1, 1, 1)));
	
	// Capturing the scene and storing it at the specified file.
    capture(camera, listFigures, lightSources, 10000, stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), argv[1]);
}