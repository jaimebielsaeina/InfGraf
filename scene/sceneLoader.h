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
bool populateList (Camera& camera, list<Figure*>& listFigures,
				vector<LightSource>& lightSources, int& raysPerPixel,
				int& threads, int& photons, float& radius, int& neighbors,
				string fileName, bool photonMapping) {

	// Temporal vectors to store points, directions and colors.
	vector <Point> points;
	vector <Direction> dirs;
	vector <Color> colors;

	// Variables to certify that the elements have been defined.
	bool cameraDefined = false;
	bool aspectDefined = false;
	bool lightDefined = false;
	bool figureDefined = false;
	bool rppDefined = false;
	bool thDefined = false;
	bool phDefined = false;
	bool radDefined = false;
	bool ngbDefined = false;

	// More temporal variables so store the data read from the file.
	string line, type;
	Color c1, c2, c3, c4;
	if (photonMapping) c4 = Color(0);
	Point p;
	Direction d;
	float f1, f2, n;
	int i1, i2, i3, i4, i5, i6;
	stringstream ss;
	uint16_t id;

	// Try to open the file.
	ifstream file(fileName);
	if (!file.is_open()) {
		cerr << "Error opening scene \"" << fileName << "\"" << endl;
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
			if (!photonMapping) c4 = colors[i4];
			aspectDefined = true;
		}
		
		// pl -> plane
		else if (type == "pl") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a plane." << endl;
				return false;
			}
			ss >> id >> i1 >> f1;
			listFigures.push_back(new
					Plane(id, dirs[i1], f1, c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// t -> triangle
		else if (type == "t") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a triangle." << endl;
				return false;
			}
			ss >> id >> i1 >> i2 >> i3;
			listFigures.push_back(new
					Triangle(id, points[i1], points[i2], points[i3],
							c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// sp -> sphere
		else if (type == "sp") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a sphere." << endl;
				return false;
			}
			ss >> id >> i1 >> f1;
			listFigures.push_back(new
					Sphere(id, points[i1], f1, c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// cn -> cone
		else if (type == "cn") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a cone." << endl;
				return false;
			}
			ss >> id >> i1 >> i2 >> f1 >> f2;
			listFigures.push_back(new
					Cone(id, points[i1], dirs[i2], f1, f2, c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// cl -> cilinder
		else if (type == "cl") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a cylinder." << endl;
				return false;
			}
			ss >> id >> i1 >> i2 >> f1 >> f2;
			listFigures.push_back(new
					Cylinder(id, points[i1], dirs[i2], f1, f2,
							c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// ds -> disc
		else if (type == "ds") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a disc." << endl;
				return false;
			}
			ss >> id >> i1 >> i2 >> f1;
			listFigures.push_back(new
					Disc(id, points[i1], dirs[i2], f1, c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// pd -> perfored disc
		else if (type == "pd") {
			if (!aspectDefined) {
				cerr << "Error: aspect not defined before defining a perfored disc." << endl;
				return false;
			}
			ss >> id >> i1 >> i2 >> f1 >> f2;
			listFigures.push_back(new
					PerforedDisc(id, points[i1], dirs[i2], f1, f2,
							c1, c2, c3, c4, n));
			figureDefined = true;
			if (c4 != Color(0)) lightDefined = true;
		}
		
		// cam -> camera
		else if (type == "cam") {
			ss >> i1 >> i2 >> i3 >> i4 >> i5 >> i6;
			camera = Camera(points[i1], dirs[i2], dirs[i3], dirs[i4], i5, i6);
			cameraDefined = true;
		}
		
		// ls -> light source
		else if (type == "ls") {
			ss >> i1 >> i2;
			lightSources.push_back(LightSource(points[i1], colors[i2]));
			lightDefined = true;
		}

		// rpp -> rays per pixel
		else if (type == "rpp") {
			ss >> i1;
			raysPerPixel = i1;
			rppDefined = true;
		}

		// th -> threads
		else if (type == "th") {
			ss >> i1;
			threads = i1;
			thDefined = true;
		}

		// ph -> photons
		else if (type == "ph") {
			ss >> i1;
			photons = i1;
			phDefined = true;
		}

		// rad -> radius
		else if (type == "rad") {
			ss >> f1;
			radius = f1;
			radDefined = true;
		}

		//ngb -> neighbors
		else if (type == "ngb") {
			ss >> i1;
			neighbors = i1;
			ngbDefined = true;
		}

	}

	// Close the file.
	file.close();

	// Check if everything has been defined.
	if (!cameraDefined) cerr << "Camera not defined." << endl;
	if (!lightDefined) cerr << "Define at least one light source." << endl;
	if (!figureDefined) cerr << "Define at least one figure." << endl;
	if (!rppDefined) cerr << "Rays per pixel must be defined." << endl;
	if (!thDefined) cerr << "Number of threads must be defined." << endl;
	if (photonMapping) {
		if (!phDefined) cerr << "Number of photons must be defined." << endl;
		if (!radDefined) cerr << "Photons search radius must be defined." << endl;
		if (!ngbDefined) cerr << "Photons search number of neighbors must be defined." << endl;
	}

	// Return true if everything has been defined.
	return cameraDefined && lightDefined && figureDefined && rppDefined &&
			thDefined && (!photonMapping || (phDefined && radDefined && ngbDefined));

}

#endif // SCENELOADER_H