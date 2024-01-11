#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include "vec4.h"

class Camera {

public:
	Point o;
	Direction l, u, f; // left, up, forward
	int width, height;
	double widthPerPixel, heightPerPixel;

public:

	Camera () {}
	Camera (const Point& origin, const Direction& left, const Direction& up, const Direction& forward, int width, int height) :
		o(origin), l(left), u(up), f(forward), width(width), height(height),
		widthPerPixel(2*toVec4(left).mod() / width), heightPerPixel(2*toVec4(up).mod() / height)
	{
		if (dot(l, u) != 0 || dot(l, f) != 0 || dot(u, f) != 0) {
			throw "The vectors are not perpendicular";
		}
	}

};


#endif // CAMERA_H