#include "vec4.h"
#include "figure.h"
#include "camera.h"
#include "randomGenerator.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>

// Capture the scene from the camera's point of view
void capture(Camera& camera, list<Figure*> figures, int raysPerPixel, string fileName) {
	std::ofstream output(fileName);
	output << "P3" << endl;
	//output << "# MAX" << endl;
	output << camera.width << " " << camera.height << endl;
	output << "255" << endl;

	Vec4 modL = camera.l.normalize(); 
    Vec4 modU = camera.u.normalize();
	Vec4 sightOrigin = camera.f + camera.l + camera.u;
	randomGenerator rand(0, 1);
	int r, g, b;
	for (int i = 0; i < camera.height; i++) {
		cout << i << endl;
		for (int j = 0; j < camera.width; j++) {
			r = 0; g = 0; b = 0;
			for (int k = 0; k < raysPerPixel; k++) {
				Ray ray = Ray(camera.o, Direction(sightOrigin - (j+rand.get())*camera.widthPerPixel*modL - (i+rand.get())*camera.heightPerPixel*modU));
				float t, minT = 1e6;
				Figure* closestFigure = nullptr;
				for (Figure* figure : figures)
					if (figure->intersect(ray, t) && t < minT) {
						minT = t;
						closestFigure = figure;
				}
				if (closestFigure != nullptr) {
					r += closestFigure->color.r;
					g += closestFigure->color.g;
					b += closestFigure->color.b;
				}
			}
			output << r/raysPerPixel << " " << g/raysPerPixel << " " << b/raysPerPixel << "    ";
		}
		output << endl;
	}
	output.close();
}

int main() {
    list<Figure*> listFigures = {};

    Camera camera = Camera(Point(0, 0, -3.5), Direction(-1, 0, 0), Direction(0, 1, 0), Direction(0, 0, 3), 256, 256);

	Plane* leftPlane = new Plane(Direction(1, 0, 0), 1, Color(255, 0, 0));
    Plane* rightPlane = new Plane(Direction(-1, 0, 0), 1, Color(0, 255, 0));
    Plane* floorPlane = new Plane(Direction(0, 1, 0), 1, Color(200, 200, 200));
    Plane* ceilingPlane = new Plane(Direction(0, -1, 0), 1, Color(200, 200, 200));
    Plane* backPlane = new Plane(Direction(0, 0, -1), 1, Color(200, 200, 200));

    Sphere* leftSphere = new Sphere(Point(-0.5, -0.7, 0.25), 0.3, Color(255, 0, 255));
    Sphere* rightSphere = new Sphere(Point(0.5, -0.7, -0.25), 0.3, Color(0, 255, 255));
    Sphere* centerSphere = new Sphere(Point(0, -0.5, 0), 0.5, Color(255, 255, 0));

	Triangle* triangle = new Triangle(Point(-0.25, -0.5, -0.5), Point(1.5, 0, 0), Point(-0.25, 1, 0), Color(255, 127, 0));

	Disc* disc = new Disc(Point(-0.6, 0.6, 0), Direction(-1, 1, 1), 0.2, Color(0, 0, 0));
	PerforedDisc* perforedDisc = new PerforedDisc(Point(-0.6, 0.2, -0.2), Direction(1, 1, 1), 0.3, 0.2, Color(0, 0, 0));

	Cylinder* cylinder = new Cylinder(Point(-0.5, 0, 0.4), Direction(3, 10, 0), 0.2, 1.5, Color(127, 0, 255));

	Cone* cone = new Cone (Point(0.5, -0.5, 0), Direction(0, 1, 0), 0.5, 1, Color(128, 0, 0));

    listFigures.push_back(leftPlane);
    listFigures.push_back(rightPlane);
    listFigures.push_back(floorPlane);
    listFigures.push_back(ceilingPlane);
    listFigures.push_back(backPlane);

    listFigures.push_back(leftSphere);
    listFigures.push_back(rightSphere);
	listFigures.push_back(centerSphere);

	listFigures.push_back(triangle);
	listFigures.push_back(disc);
	listFigures.push_back(perforedDisc);
	listFigures.push_back(cylinder);
	listFigures.push_back(cone);

    capture(camera, listFigures, 16, "output.ppm");
}