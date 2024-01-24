#ifndef TONEMAPPING_H
#define TONEMAPPING_H
#pragma once

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

// Clamps the values of the pixels between 0 and 1, transforming higher values
// into 1.
void clamping(double &r, double &g, double &b) {
	if (r > 1) r = 1;
	if (g > 1) g = 1;
	if (b > 1) b = 1;
}


// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, following a clampling transformation.
void transform_clamping(string inFile, string outFile) {

	int w, h;
	double m = 1, c;
	bool still = true;

	// Opening files.
	ifstream ifs(inFile);
	ofstream ofs(outFile);

	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}
	if (!ofs.is_open()) {
		cerr << "Error opening file \"" << outFile << "\"" << endl;
		exit(1);
	}

	// Reading header.
	string header;
	getline(ifs, header);
	ofs << header << endl;

	// Reads comment lines and gets MAX value if found.
	string line;
	while (still) {
		getline(ifs, line);
		if (line[0] != '#') {
			still = false;
		} else if (line.substr(0, 5) == "#MAX=") {
			ofs << "#MAX=1\n";
			m = stod(line.substr(5, -1));
		} else {
			ofs << line << endl;
		}
	}

	// Manages width, height and c value.
	w = stoi(line.substr(0, line.find(' ')));
	h = stoi(line.substr(line.find(' ') + 1, -1));
	ofs << w << " " << h << endl;

	getline(ifs, line);
	c = stod(line);
	ofs << 255 << endl;

	double r, g, b;

	// Applies the clamping transformation to each pixel.
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){

			ifs >> r >> g >> b;

			r = r * m / c;
			g = g * m / c;
			b = b * m / c;

			clamping(r, g, b);
			
			r *= 255;
			g *= 255;
			b *= 255;
			
			// Writes the new pixel values.
			ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
		}
		ofs << endl;
	}
}

// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, dividing each value by the maximum one.
void equalization(double &r, double &g, double &b, double max){
	r /= max;
	g /= max;
	b /= max;
}

// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, following a linear transformation.
void transform_equalization(string inFile, string outFile){

	int w, h;
	int counter = 3;
	double m = 1, c;
	bool still = true;

	// Opening files.
	ifstream ifs(inFile);
	ofstream ofs(outFile);

	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}
	if (!ofs.is_open()) {
		cerr << "Error opening file \"" << outFile << "\"" << endl;
		exit(1);
	}

	// Reading header.
	string header;
	getline(ifs, header);
	ofs << header << endl;

	// Reads comment lines and gets MAX value if found.
	string line;
	while (still) {
		getline(ifs, line);
		if (line[0] != '#') {
			still = false;
		} else if (line.substr(0, 5) == "#MAX=") {
			ofs << "#MAX=1\n";
			m = stod(line.substr(5, -1));
			counter++;
		} else {
			ofs << line << endl;
			counter++;
		}
	}

	// Manages, width, height and c value.
	w = stoi(line.substr(0, line.find(' ')));
	h = stoi(line.substr(line.find(' ') + 1, -1));
	ofs << w << " " << h << endl;

	getline(ifs, line);
	c = stod(line);
	ofs << 255 << endl;

	double r, g, b;
	double max = 0;

	// Reads each pixel value and finds the maximum value.
	for (int i = 0; i < h; i++){
		for (int j = 0; j < w; j++){
			ifs >> r >> g >> b;
			if(r > max) max = r;
			if(g > max) max = g;
			if(b > max) max = b;
		}
	}

	max = max * m / c;
	cout << "Max value found: " << max << endl;

	// Resets the ifstream.
	ifs.close();
	ifs.open(inFile);
	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}

	// Ignoring the lines before the pixels.
	for (int i = 0; i < counter; i++){
		getline(ifs, line);
	}

	// Applies the equalization transformation.
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++) {

			ifs >> r >> g >> b;

			r = r * m / c;
			g = g * m / c;
			b = b * m / c;
			
			equalization(r, g, b, max);
			
			r *= 255;
			g *= 255;
			b *= 255;
			
			// Writes the new pixel values.
			ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
		}
		ofs << endl;
	}
}

// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, dividing each value by the threshold, and
// reducing it to 1 if higher.
void clamping_equalization(double &r, double &g, double &b, double threshold){
	r = r>threshold ? 1 : r/threshold;
	g = g>threshold ? 1 : g/threshold;
	b = b>threshold ? 1 : b/threshold;
}

// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, following a clamping transformation and then
// an equalization one.
void transform_clamp_equal(string inFile, string outFile, int threshold){

	int w, h;
	double m = 1, c;
	bool still = true;

	// Opening files.
	ifstream ifs(inFile);
	ofstream ofs(outFile);

	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}
	if (!ofs.is_open()) {
		cerr << "Error opening file \"" << outFile << "\"" << endl;
		exit(1);
	}

	// Reading header.
	string header;
	getline(ifs, header);
	ofs << header << endl;

	// Reads comment lines and gets MAX value if found.
	string line;
	while (still) {
		getline(ifs, line);
		if (line[0] != '#') {
			still = false;
		} else if (line.substr(0, 5) == "#MAX=") {
			ofs << "#MAX=1\n";
			m = stod(line.substr(5, -1));
		} else {
			ofs << line << endl;
		}
	}

	// Manages width, height and c value.
	w = stoi(line.substr(0, line.find(' ')));
	h = stoi(line.substr(line.find(' ') + 1, -1));
	ofs << w << " " << h << endl;

	getline(ifs, line);
	c = stod(line);
	ofs << 255 << endl;

	double r, g, b;

	// Applies the clamping-equalization transformation to each pixel.
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){

			ifs >> r >> g >> b;

			r = r * m / c;
			g = g * m / c;
			b = b * m / c;

			clamping_equalization(r, g, b, threshold);

			r *= 255;
			g *= 255;
			b *= 255;

			// Writes the new pixel values.
			ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
		}
		ofs << endl;
	}
}

// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, rooting each value.
void gamma(double &r, double &g, double &b, double gamma_coef){
	double factor = 255.0 / pow(255.0, 1/gamma_coef);
	r = pow (r, 1/gamma_coef) * factor;
	g = pow (g, 1/gamma_coef) * factor;
	b = pow (b, 1/gamma_coef) * factor;
}

// Transforms a ppm file into another with the same content but with the values
// of the pixels between 0 and 1, following a gamma transformation.
void transform_gamma(string inFile, string outFile, double gamma_coef){
	
	int w, h;
	int counter = 3;
	double m = 1, c;
	bool still = true;

	// Opening files.
	ifstream ifs(inFile);
	ofstream ofs(outFile);

	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}
	if (!ofs.is_open()) {
		cerr << "Error opening file \"" << outFile << "\"" << endl;
		exit(1);
	}

	// Reading header.
	string header;
	getline(ifs, header);
	ofs << header << endl;

	// Reads comment lines and gets MAX value if found.
	string line;
	while (still) {
		getline(ifs, line);
		if (line[0] != '#') {
			still = false;
		} else if (line.substr(0, 5) == "#MAX=") {
			ofs << "#MAX=1\n";
			m = stod(line.substr(5, -1));
			counter++;
		} else {
			ofs << line << endl;
			counter++;
		}
	}

	// Manages, width, height and c value.
	w = stoi(line.substr(0, line.find(' ')));
	h = stoi(line.substr(line.find(' ') + 1, -1));
	ofs << w << " " << h << endl;

	getline(ifs, line);
	c = stod(line);
	ofs << 255 << endl;

	double r, g, b;
	double max = 0;

	// Reads each pixel value and finds the maximum value.
	for (int i = 0; i < h; i++){
		for (int j = 0; j < w; j++){
			ifs >> r >> g >> b;
			if(r > max) max = r;
			if(g > max) max = g;
			if(b > max) max = b;
		}
	}

	max = max * m / c;
	cout << "Max value found: " << max << endl;

	// Resets the ifstream.
	ifs.close();
	ifs.open(inFile);
	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}

	// Ignoring the lines before the pixels.
	for (int i = 0; i < counter; i++){
		getline(ifs, line);
	}

	// Applies equalization then gamma transformation to each pixel.
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++) {

			ifs >> r >> g >> b;

			r = r * m / c;
			g = g * m / c;
			b = b * m / c;
			
			equalization (r, g, b, max);
			
			r *= 255;
			g *= 255;
			b *= 255;

			gamma (r, g, b, gamma_coef);

			// Writes the new pixel values.
			ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
		}
		ofs << endl;
	}
}

void transform_clamp_gamma(string inFile, string outFile, int threshold, double gamma_coef){

	int w, h;
	double m = 1, c;
	bool still = true;

	// Opening files.
	ifstream ifs(inFile);
	ofstream ofs(outFile);

	if (!ifs.is_open()) {
		cerr << "Error opening file \"" << inFile << "\"" << endl;
		exit(1);
	}
	if (!ofs.is_open()) {
		cerr << "Error opening file \"" << outFile << "\"" << endl;
		exit(1);
	}

	// Reading header.
	string header;
	getline(ifs, header);
	ofs << header << endl;

	// Reads comment lines and gets MAX value if found.
	string line;
	while (still) {
		getline(ifs, line);
		if (line[0] != '#') {
			still = false;
		} else if (line.substr(0, 5) == "#MAX=") {
			ofs << "#MAX=1\n";
			m = stod(line.substr(5, -1));
		} else {
			ofs << line << endl;
		}
	}

	// Manages width, height and c value.
	w = stoi(line.substr(0, line.find(' ')));
	h = stoi(line.substr(line.find(' ') + 1, -1));
	ofs << w << " " << h << endl;

	getline(ifs, line);
	c = stod(line);
	ofs << 255 << endl;

	double r, g, b;

	// Applies clamping-equalization then gamma transformation to each pixel.
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){

			ifs >> r >> g >> b;

			r = r * m / c;
			g = g * m / c;
			b = b * m / c;
			
			clamping_equalization(r, g, b, threshold);
			
			r *= 255;
			g *= 255;
			b *= 255;

			gamma(r, g, b, gamma_coef);

			// Writes the new pixel values.
			ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
		}
		ofs << endl;
	}
}

#endif // TONEMAPPING_H