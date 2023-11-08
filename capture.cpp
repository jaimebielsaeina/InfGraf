#include "vec4.h"
#include "figure.h"
#include "camera.h"
#include "randomGenerator.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>

using namespace std;


Color getColorOfHit (const Figure* figure, list<Figure*> figures, const Point& hit,
					 const Point& prevHit, LightSource& lightSource, 
					 const Camera& camera, Color& scatter, Direction& Wi) {

	Vec4 distanceToLight = distance(lightSource.center, hit);
	float modDistanceToLight = distanceToLight.mod();

	bool shadow = false;
	float t;

	Color Li = lightSource.power / (modDistanceToLight*modDistanceToLight);
	Color Fr = figure->color / M_PI;
	float cosine = abs(dot(figure->getNormal(hit), distanceToLight.normalize()));

	// Check if the plane is against the light.
	if (figure->planeAgainstLight(camera, lightSource, hit))
		shadow = true;
	else

	// Check if the point is in shadow.
	for (Figure* fig : figures)
			if (fig->intersect(Ray(lightSource.center, -distanceToLight), t)
			&&	(1 - t) * modDistanceToLight > 1e-6
			&&	(  fig != figure
				|| distance(hit, lightSource.center - t*(distanceToLight)).mod() > 1e-4)) {
					shadow = true;
					break;
	}

	if (shadow)	return Color(0);

	return Li * Fr * cosine * scatter;

}

void captureSection(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources, int raysPerPixel, int maxBounces, int minH, int maxH, std::vector<Color>& pixelsValue) {
	
	Vec4 modL = camera.l.normalize(); 
    Vec4 modU = camera.u.normalize();
	Vec4 sightOrigin = camera.f + camera.l + camera.u;
	randomGenerator rand(0, 1);
	Color pxColor;

	Direction rayDirection;
	Ray ray;
	float t, minT;
	Point hit, prevHit;
	Figure* closestFigure;
	vector<Color> scatter(lightSources.size());
	
	for (int i = minH; i < maxH; i++) {
		//cout << i << endl;
		for (int j = 0; j < camera.width; j++) {
			pxColor = Color();
			for (int k = 0; k < raysPerPixel; k++) {

				rayDirection = Direction(sightOrigin - (j+rand.get())*camera.widthPerPixel*modL - (i+rand.get())*camera.heightPerPixel*modU);
				ray = Ray(camera.o, rayDirection);
				prevHit = camera.o;

				for (int i = 0; i < scatter.size(); ++i) scatter[i] = Color(1);

				for (int b = 0; b < maxBounces; ++b) {

					minT = 1e6;
					closestFigure = nullptr;
					for (Figure* figure : figures)
						if (figure->intersect(ray, t) && t < minT) {
							minT = t;
							closestFigure = figure;
					}

					if (closestFigure == nullptr) break;
					hit = ray.getPoint() + minT*ray.getDirection();

					for (int i = 0; i < lightSources.size(); ++i)
						pxColor += getColorOfHit(closestFigure, figures, hit, prevHit, lightSources[i], camera, scatter[i], rayDirection);

					prevHit = hit;
					for (int i = 0; i < scatter.size(); ++i)
						scatter[i] *= closestFigure->color; // / M_PI * abs(dot(closestFigure->getNormal(hit), -rayDirection));
					rayDirection = closestFigure -> nextDirection(hit);
					ray = Ray(hit, rayDirection);

				}
			}
			pixelsValue[i*camera.width + j] = pxColor;
		}
	}
}

// Capture the scene from the camera's point of view
void capture(Camera& camera, list<Figure*> figures, vector<LightSource> lightSources, int raysPerPixel, int maxBounces, int threads, string fileName) {

	// Open the file to write the image.
	std::ofstream output(fileName);

	// Write the header of the PPM file.
	output << "P3" << endl;
	//output << "# MAX" << endl;
	output << camera.width << " " << camera.height << endl;
	output << "255" << endl;

	// Structure to store the final image.
	vector<Color> finalImage(camera.height * camera.width);

	// Divide the work between the threads. Each one will capture a section of the image.
	vector<thread> threadsArray(threads);
	for (int t = 0; t < threads; t++) {
		int minH = t * camera.height / threads;
		int maxH = (t + 1) * camera.height / threads;
		threadsArray[t] = thread(&captureSection, ref(camera), ref(figures), ref(lightSources), raysPerPixel, maxBounces, minH, maxH, ref(finalImage));
	}

	// Wait for all threads to finish.
	for (int t = 0; t < threads; t++) threadsArray[t].join();

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
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " <output_file> <number of threads" << endl;
		return 1;
	}

	// Defining the camera.
	Direction l = Direction(-1, 0, 0); // l.rotateY(-15);
	Direction u = Direction(0, 1, 0); // u.rotateY(-15);
	Direction f = Direction(0, 0, 3); // f.rotateY(-15);
    Camera camera = Camera(Point(0, 0, -3.5), l, u, f, 256, 256);
	
	// Defining the scene.
    list<Figure*> listFigures = {};

	// Defining the figures.
	Plane* leftPlane = new Plane(Direction(1, 0, 0), 1, Color(1, 0, 0));
	Plane* rightPlane = new Plane(Direction(-1, 0, 0), 1, Color(0, 1, 0));
	Plane* floorPlane = new Plane(Direction(0, 1, 0), 1, Color(0.8, 0.8, 0.8));
	Plane* ceilingPlane = new Plane(Direction(0, -1, 0), 1, Color(0.8, 0.8, 0.8));
	Plane* backPlane = new Plane(Direction(0, 0, -1), 1, Color(0.8, 0.8, 0.8));

	Sphere* leftSphere = new Sphere(Point(-0.5, -0.7, 0.25), 0.3, Color(1, 0, 1));
	Sphere* rightSphere = new Sphere(Point(0.5, -0.7, -0.25), 0.3, Color(0, 1, 1));

    Sphere* centerSphere = new Sphere(Point(0, -0.5, 0), 0.5, Color(1, 1, 0));

	Triangle* triangle = new Triangle(Point(-0.25, -0.5, -0.5), Point(1.5, 0, 1), Point(-0.25, 1, 0), Color(1, 0.5, 0));

	Disc* disc = new Disc(Point(-0.6, 0.6, 0), Direction(-2, 1, 1), 0.2, Color(0, 1, 1));
	PerforedDisc* perforedDisc = new PerforedDisc(Point(-0.6, 0.2, -0.2), Direction(1, 1, 1), 0.3, 0.2, Color(0, 1, 1));

	Cylinder* cylinder = new Cylinder(Point(-0.5, 0, 0.4), Direction(0, 0, 1), 0.2, 1.5, Color(0.5, 0, 1));

	Cone* cone = new Cone (Point(0.5, -0.5, 0), Direction(0, 1, -0.2), 0.5, 0.5, Color(1, 0.5, 0));

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
	lightSources.push_back(LightSource(Point(0, 0.5, 0), Color(1, 1, 1)));
	//lightSources.push_back(LightSource(Point(-0.5, 0, 0.4), Color(1, 1, 1)));

	// Capturing the scene and storing it at the specified file.
    capture(camera, listFigures, lightSources, 32, 5, stoi(argv[2]), argv[1]);
}