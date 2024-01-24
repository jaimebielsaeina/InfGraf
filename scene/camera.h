#ifndef CAMERA_H
#define CAMERA_H
#pragma once

#include "../coordinates/vec4.h"

// Object that represents a camera.
class Camera {

public:

	// Origin of the camera.
	Point o;

	// Vectors that define the camera: left, up and forward.
	Direction l, u, f;

	// Width and height of the image in pixels.
	int width, height;

	// Width and height of each pixel.
	double widthPerPixel, heightPerPixel;

public:

	// Constructor.
	Camera () {}

	// Constructor.
	Camera (const Point& origin, const Direction& left, const Direction& up,
			const Direction& forward, int width, int height) :
			o(origin), l(left), u(up), f(forward), width(width), height(height),
			// Calculates the width and height of each pixel.
			widthPerPixel(2*left.mod() / width),
			heightPerPixel(2*up.mod() / height) {

		// Checks if the vectors are perpendicular.
		if (dot(l, u) != 0 || dot(l, f) != 0 || dot(u, f) != 0)
				throw "The vectors are not perpendicular.";

	}

};


#endif // CAMERA_H