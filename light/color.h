#ifndef COLOR_H
#define COLOR_H
#pragma once

#include <cstdint>

#define Light char
#define R 0
#define G 1
#define B 2

// Object that represents a color.
class Color {

private:

	// RGB values of the color.
	double c[3];

public:

	// RGB constructor.
	Color(double r, double g, double b) { c[0] = r; c[1] = g; c[2] = b; }

	// White constructor.
	Color(double w) : Color(w, w, w) {}

	// Constructor without parameters (black).
	Color() : Color(0) {}

	// Destructor.
	~Color() {}

	// Returns the maximum value (R, G or B) of the color.
	double maxC() const {
		return max(max(c[0], c[1]), c[2]);
	}

	// Returns the maximum value (R, G, B) of the color and the light it is.
	double maxC(Light& l) const {
		l = c[0] > c[1] ? (c[0] > c[2] ? 0 : 2) : (c[1] > c[2] ? 1 : 2);
		return c[l];
	}

	// Operation.
	friend Color operator+(const Color& c1, const Color& c2) {
		return Color(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2]);
	}

	// Operation.
	friend Color operator+=(Color& c1, const Color& c2) {
		c1 = c1 + c2;
		return c1;
	}

	// Operation.
	friend Color operator-(const Color& c1, const Color& c2) {
		return Color(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2]);
	}

	// Operation.
	friend Color operator-=(Color& c1, const Color& c2) {
		c1 = c1 - c2;
		return c1;
	}

	// Operation.
	friend Color operator*(const Color& c, double k) {
		return Color(c[0] * k, c[1] * k, c[2] * k);
	}

	// Operation.
	friend Color operator*=(Color& c, double k) {
		c = c * k;
		return c;
	}

	// Operation.
	friend Color operator*(double k, const Color& c) {
		return Color(c[0] * k, c[1] * k, c[2] * k);
	}

	// Operation.
	friend Color operator*(const Color& c1, const Color& c2) {
		return Color(c1[0] * c2[0], c1[1] * c2[1], c1[2] * c2[2]);
	}

	// Operation.
	friend Color operator*=(Color& c1, const Color& c2) {
		c1 = c1 * c2;
		return c1;
	}
	
	// Operation.
	friend Color operator/(const Color& c, double k) {
		return Color(c[0] / k, c[1] / k, c[2] / k);
	}

	// Operation.
	friend Color operator/=(Color& c, double k) {
		c = c / k;
		return c;
	}

	// Operation.
	friend bool operator==(Color c1, Color c2) {
		return c1[0] == c2[0] && c1[1] == c2[1] && c1[2] == c2[2];
	}

	// Operation.
	friend bool operator!=(Color c1, Color c2) {
		return !(c1 == c2);
	}

	// Access to the color values.
	const double& operator[](int i) const {
		return c[i];
	}

	// Sum of the color values.
	double sum(){
		return c[0] + c[1] + c[2];
	}

	// Writes the color to the output stream.
	friend ostream &operator<< (ostream &o, const Color &c){
		o << c[0] << " " << c[1] << " " << c[2];
		return o;
	}

	// Reads the color from the input stream.
	friend istream &operator>> (istream &i, Color &c){
		i >> c.c[0] >> c.c[1] >> c.c[2];
		return i;
	}
	
};

#endif // COLOR_H