#pragma once
#ifndef SCENELOADER_H
#define SCENELOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include "camera.h"
#include "figure.h"
#include "lightSource.h"

void populateList (Camera& camera, list<Figure*>& listFigures, vector<LightSource>& lightSources, string fileName, bool lightAreas) {

	vector <Point> points;
	vector <Direction> dirs;
	vector <Color> colors;

	ifstream file(fileName);
	string line, type;
	Color c1, c2, c3, c4;
	Point p;
	Direction d;
	float f1, f2, n;
	int i1, i2, i3, i4, i5, i6;
	stringstream ss;
	while (getline(file, line)) {
		ss = stringstream(line);
		ss >> type;
		// p point
		if (type == "p") {
			ss >> p;
			points.push_back(p);
		// d direction
		} else if (type == "d") {
			ss >> d;
			dirs.push_back(d);
		// c color
		} else if (type == "c") {
			ss >> c1;
			colors.push_back(c1);
		// a aspect
		} else if (type == "a") {
			ss >> i1 >> i2 >> i3 >> i4 >> n;
			c1 = colors[i1];
			c2 = colors[i2];
			c3 = colors[i3];
			if (lightAreas) c4 = colors[i4];
			cout << "aspecto" << colors[i1] << colors[i2] << colors[i3] << colors[i4] << n << endl;
		// pl plane
		} else if (type == "pl") {
			ss >> i1 >> f1;
			cout << "plano" << dirs[i1] << f1 << endl;
			listFigures.push_back(new Plane(dirs[i1], f1, c1, c2, c3, c4, n));
		// t triangle
		} else if (type == "t") {
			ss >> i1 >> i2 >> i3;
			cout << "triangulo" << points[i1] << points[i2] << points[i3] << endl;
			listFigures.push_back(new Triangle(points[i1], points[i2], points[i3], c1, c2, c3, c4, n));
		// sp sphere
		} else if (type == "sp") {
			ss >> i1 >> f1;
			cout << "esfera" << points[i1] << f1 << endl;
			listFigures.push_back(new Sphere(points[i1], f1, c1, c2, c3, c4, n));
		// cn cone
		} else if (type == "cn") {
			ss >> i1 >> i2 >> f1 >> f2;
			cout << "cono" << points[i1] << dirs[i2] << f1 << f2 << endl;
			listFigures.push_back(new Cone(points[i1], dirs[i2], f1, f2, c1, c2, c3, c4, n));
		// cl cilinder
		} else if (type == "cl") {
			ss >> i1 >> i2 >> f1 >> f2;
			cout << "cilindro" << points[i1] << dirs[i2] << f1 << f2 << endl;
			listFigures.push_back(new Cylinder(points[i1], dirs[i2], f1, f2, c1, c2, c3, c4, n));
		// ds disc
		} else if (type == "ds") {
			ss >> i1 >> i2 >> f1;
			cout << "disco" << points[i1] << dirs[i2] << f1 << endl;
			listFigures.push_back(new Disc(points[i1], dirs[i2], f1, c1, c2, c3, c4, n));
		// pd perfored disc
		} else if (type == "pd") {
			ss >> i1 >> i2 >> f1 >> f2;
			cout << "disco perforado" << points[i1] << dirs[i2] << f1 << f2 << endl;
			listFigures.push_back(new PerforedDisc(points[i1], dirs[i2], f1, f2, c1, c2, c3, c4, n));
		// cam camera
		} else if (type == "cam") {
			ss >> i1 >> i2 >> i3 >> i4 >> i5 >> i6;
			cout << "camara" << points[i1] << dirs[i2] << dirs[i3] << dirs[i4] << i5 << i6 << endl;
			camera = Camera(points[i1], dirs[i2], dirs[i3], dirs[i4], i5, i6);
		// ls light source
		} else if (type == "ls") {
			ss >> i1 >> i2;
			cout << "fuente de luz" << points[i1] << colors[i2] << endl;
			lightSources.push_back(LightSource(points[i1], colors[i2]));
		}
	}
}

#endif // SCENELOADER_H