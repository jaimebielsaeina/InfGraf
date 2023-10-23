#include "vec4.h"
#include "figure.h"
#include "camera.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>

void capture(Camera& camera, list<Figure*> figures, string fileName) {
	std::ofstream output(fileName);
	output << "P3" << endl;
	//output << "# MAX" << endl;
	output << camera.width << " " << camera.height << endl;
	output << "255" << endl;

	Vec4 modL = camera.l.normalize(); 
    Vec4 modU = camera.u.normalize();
	Vec4 sightOrigin = camera.f + camera.l + camera.u;
	for (int i = 0; i < camera.width; i++) {
		cout << i << endl;
		for (int j = 0; j < camera.height; j++) {
			Ray ray = Ray(camera.o, Direction(sightOrigin - (i+0.5)*camera.widthPerPixel*modL - (j+0.5)*camera.heightPerPixel*modU));
			float t, minT = 1e6;
			Figure* closestFigure = nullptr;
			for (Figure* figure : figures) {
				if (figure->intersect(ray, t) && t < minT) {
					minT = t;
					closestFigure = figure;
				}
			}
			if (closestFigure != nullptr) {
				output << (int)closestFigure->color.r << " " << (int)closestFigure->color.g << " " << (int)closestFigure->color.b << "    ";
			} else {
				output << "0 0 0    ";
			}
		}
		output << endl;
	}
	output.close();
}

int main() {
    list<Figure*> listFigures = {};

    Camera camera = Camera(Point(0, 0, -3.5), Direction(-1, 0, 0), Direction(0, 1, 0), Direction(0, 0, 3), 256, 256);

	Plane* leftPlane = new Plane(Point(-1, 0, 0), Direction(1, 0, 0), Color(255, 0, 0));
    Plane* rightPlane = new Plane(Point(1, 0, 0), Direction(-1, 0, 0), Color(0, 255, 0));
    Plane* floorPlane = new Plane(Point(0, -1, 0), Direction(0, 1, 0), Color(200, 200, 200));
    Plane* ceilingPlane = new Plane(Point(0, 1, 0), Direction(0, -1, 0), Color(200, 200, 200));
    Plane* backPlane = new Plane(Point(0, 0, 1), Direction(0, 0, -1), Color(200, 200, 200));

    Sphere* leftSphere = new Sphere(Point(-0.5, -0.7, 0.25), 0.3, Color(255, 0, 255));
    Sphere* rightSphere = new Sphere(Point(0.5, 0.7, -0.25), 0.3, Color(0, 255, 255));

    listFigures.push_back(leftPlane);
    listFigures.push_back(rightPlane);
    listFigures.push_back(floorPlane);
    listFigures.push_back(ceilingPlane);
    listFigures.push_back(backPlane);

    listFigures.push_back(leftSphere);
    listFigures.push_back(rightSphere);

    capture(camera, listFigures, "output.ppm");
}