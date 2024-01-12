#ifndef SCENELOADER_H
#define SCENELOADER_H
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include "camera.h"
#include "figure.h"
#include "lightSource.h"

// Reads the scene file, populating the list of figures, the camera and the
// light sources.
void populateList (Camera& camera, list<Figure*>& listFigures,
				vector<LightSource>& lightSources, int& raysPerPixel,
				int& threads, int& photons, string fileName, bool lightAreas) {

	// Temporal vectors to store points, directions and colors.
	vector <Point> points;
	vector <Direction> dirs;
	vector <Color> colors;

	// More temporal variables so store the data read from the file.
	string line, type;
	Color c1, c2, c3, c4;
	Point p;
	Direction d;
	float f1, f2, n;
	int i1, i2, i3, i4, i5, i6;
	stringstream ss;
	uint16_t id;

	// Try to open the file.
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << "Error opening scene \"" << fileName << "\"" << endl;
		exit(1);
	}
	
	// Read the file line by line.
	while (getline(file, line)) {

		ss = stringstream(line);

		// Depending of the first word of the line, it means different things.
		ss >> type;

		// p -> point
		if (type == "p") {
			ss >> p;
			points.push_back(p);
		}

		// d -> direction
		else if (type == "d") {
			ss >> d;
			dirs.push_back(d);
		}

		// c -> color
		else if (type == "c") {
			ss >> c1;
			colors.push_back(c1);
		}
		
		// a -> aspect
		else if (type == "a") {
			ss >> i1 >> i2 >> i3 >> i4 >> n;
			c1 = colors[i1];
			c2 = colors[i2];
			c3 = colors[i3];
			if (lightAreas) c4 = colors[i4];
			cout << "aspecto" << colors[i1] << colors[i2] << colors[i3] << colors[i4] << n << endl;
		}
		
		// pl -> plane
		else if (type == "pl") {
			ss >> id >> i1 >> f1;
			cout << "plano" << id << dirs[i1] << f1 << endl;
			listFigures.push_back(new
					Plane(id, dirs[i1], f1, c1, c2, c3, c4, n));
		}
		
		// t -> triangle
		else if (type == "t") {
			ss >> id >> i1 >> i2 >> i3;
			cout << "triangulo" << id << points[i1] << points[i2] << points[i3] << endl;
			listFigures.push_back(new
					Triangle(id, points[i1], points[i2], points[i3],
							c1, c2, c3, c4, n));
		}
		
		// sp -> sphere
		else if (type == "sp") {
			ss >> id >> i1 >> f1;
			cout << "esfera" << points[i1] << f1 << endl;
			listFigures.push_back(new
					Sphere(id, points[i1], f1, c1, c2, c3, c4, n));
		}
		
		// cn -> cone
		else if (type == "cn") {
			ss >> id >> i1 >> i2 >> f1 >> f2;
			cout << "cono" << points[i1] << dirs[i2] << f1 << f2 << endl;
			listFigures.push_back(new
					Cone(id, points[i1], dirs[i2], f1, f2, c1, c2, c3, c4, n));
		}
		
		// cl -> cilinder
		else if (type == "cl") {
			ss >> id >> i1 >> i2 >> f1 >> f2;
			cout << "cilindro" << points[i1] << dirs[i2] << f1 << f2 << endl;
			listFigures.push_back(new
					Cylinder(id, points[i1], dirs[i2], f1, f2,
							c1, c2, c3, c4, n));
		}
		
		// ds -> disc
		else if (type == "ds") {
			ss >> id >> i1 >> i2 >> f1;
			cout << "disco" << points[i1] << dirs[i2] << f1 << endl;
			listFigures.push_back(new
					Disc(id, points[i1], dirs[i2], f1, c1, c2, c3, c4, n));
		}
		
		// pd -> perfored disc
		else if (type == "pd") {
			ss >> id >> i1 >> i2 >> f1 >> f2;
			cout << "disco perforado" << points[i1] << dirs[i2] << f1 << f2 << endl;
			listFigures.push_back(new
					PerforedDisc(id, points[i1], dirs[i2], f1, f2,
							c1, c2, c3, c4, n));
		}
		
		// cam -> camera
		else if (type == "cam") {
			ss >> i1 >> i2 >> i3 >> i4 >> i5 >> i6;
			cout << "camara" << points[i1] << dirs[i2] << dirs[i3] << dirs[i4] << i5 << i6 << endl;
			camera = Camera(points[i1], dirs[i2], dirs[i3], dirs[i4], i5, i6);
		}
		
		// ls -> light source
		else if (type == "ls") {
			ss >> i1 >> i2;
			cout << "fuente de luz" << points[i1] << colors[i2] << endl;
			lightSources.push_back(LightSource(points[i1], colors[i2]));
		}
		// rpp -> rays per pixel
		else if (type == "rpp") {
			ss >> i1;
			cout << "rayos por pixel" << i1 << endl;
			raysPerPixel = i1;
		}

		// th -> threads
		else if (type == "th") {
			ss >> i1;
			cout << "hilos" << i1 << endl;
			threads = i1;
		}

		// ph -> photons
		else if (type == "ph") {
			ss >> i1;
			cout << "fotones" << i1 << endl;
			photons = i1;
		}
	}
}

#endif // SCENELOADER_H