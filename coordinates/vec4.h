#ifndef VEC4_H
#define VEC4_H
#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <ostream>
#include <array>
#include "matrix.h"
#include "../utilities/randomGenerator.h"
using namespace std;

randomGenerator randPhi2(0, 2*M_PI);
randomGenerator randTheta2(-1, 1);

// Object that represents uns geometry primitive in 3D space.
class Vec4 {

protected:

	// Coordinates of the vector.
	float x, y, z;

	// 1 if the vector is a point, 0 if it is a direction.
	uint8_t isPoint;

public:

	// Constructor.
	Vec4 () : x(0), y(0), z(0), isPoint(0) {}

	// Constructor.
	Vec4 (float x, float y, float z, uint8_t isPoint) : x(x), y(y), z(z), isPoint(isPoint) {}

public:

	// X coordinate getter.
	double getX () const {
		return x;
	}

	// Y coordinate getter.
	double getY () const {
		return y;
	}

	// Z coordinate getter.
	double getZ () const {
		return z;
	}

	// Returns 1 if the vector is a point, 0 if it is a direction.
	uint8_t getIsPoint () const {
		return isPoint;
	}

	// Returns 1 if the vector is a direction, 0 if it is a point.
	uint8_t getIsDirection () const {
		return 1-isPoint;
	}

	// X coordinate setter.
	void setX (double x) {
		this->x = x;
	}

	// Y coordinate setter.
	void setY (double y) {
		this->y = y;
	}

	// Z coordinate setter.
	void setZ (double z) {
		this->z = z;
	}

	// Operation.
	friend Vec4 operator+ (const Vec4 &c1, const Vec4 &c2) {
		return Vec4(c1.x + c2.x, c1.y + c2.y, c1.z + c2.z, c1.isPoint);
	}

	// Operation.
	friend Vec4 operator- (const Vec4 &c1, const Vec4 &c2) {
		return Vec4(c1.x - c2.x, c1.y - c2.y, c1.z - c2.z, c1.isPoint);
	}

	// Operation.
	friend Vec4 operator- (const Vec4 &c) {
		return Vec4(-c.x, -c.y, -c.z, c.isPoint);
	}

	// Operation.
	friend Vec4 distance (Vec4 p1, Vec4 p2) {
		return Vec4(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, 0);
	}

	// Operation.
	friend Vec4 operator* (const Vec4 &c, const double &s) {
		return Vec4(c.x * s, c.y * s, c.z * s, 0);
	}

	// Operation.
	friend Vec4 operator* (const double &s, const Vec4 &c) {
		return c * s;
	}

	// Operation.
	friend Vec4 operator/ (const Vec4 &c, const double &s) {
		return Vec4(c.x / s, c.y / s, c.z / s, 0);
	}

	// Operation.
	friend Vec4 operator/ (const double &s, const Vec4 &c) {
		return c / s;
	}

	// Returns the module of the direction, or the distance from the point to the origin.
	float mod() const {
		return sqrt(x * x + y * y + z * z);
	}

	// Normalize the primitive. Should be used on a direction.
	Vec4 normalize() const {
		return *this / mod();
	}

	// Returns the dot product of two directions.
	friend float dot (const Vec4 &c1, const Vec4 &c2) {
		return c1.x * c2.x + c1.y * c2.y + c1.z * c2.z;
	}

	// Returns the cross product of two directions.
	friend Vec4 cross (const Vec4 &c1, const Vec4 &c2) {
		return Vec4 (c1.y * c2.z - c1.z * c2.y, 
					 c1.z * c2.x - c1.x * c2.z, 
					 c1.x * c2.y - c1.y * c2.x, 
					 0);
	}

	// Applies a transformation matrix to the primitive.
	void applyMatrix (Matrix m) {
		Vec4 aux = Vec4(0, 0, 0, 0);
		aux.x = m.get(0, 0) * x + m.get(0, 1) * y + m.get(0, 2) * z + m.get(0, 3) * isPoint;
		aux.y = m.get(1, 0) * x + m.get(1, 1) * y + m.get(1, 2) * z + m.get(1, 3) * isPoint;
		aux.z = m.get(2, 0) * x + m.get(2, 1) * y + m.get(2, 2) * z + m.get(2, 3) * isPoint;
		aux.isPoint = m.get(3, 0) * x + m.get(3, 1) * y + m.get(3, 2) * z + m.get(3, 3) * isPoint;

		*this = aux;
	}

	// Applies a translation matrix to the primitive.
	void translate (double x, double y, double z) {
		Matrix m = Matrix();
		m.translationMatrix(x, y, z);
		this->applyMatrix(m);
	}

	// Applies a scale matrix to the primitive.
	void scale(double x, double y, double ) {
		Matrix m = Matrix();
		m.scaleMatrix(x, y, z);
		this->applyMatrix(m);
	}

	// Applies a rotation matrix to the primitive to move it around the X axis.
	void rotateX(double degrees) {
		Matrix m = Matrix();
		m.rotateXMatrix(degrees);
		this->applyMatrix(m);
	}

	// Applies a rotation matrix to the primitive to move it around the Y axis.
	void rotateY(double degrees) {
		Matrix m = Matrix();
		m.rotateYMatrix(degrees);
		this->applyMatrix(m);
	}

	// Applies a rotation matrix to the primitive to move it around the Z axis.
	void rotateZ(double degrees) {
		Matrix m = Matrix();
		m.rotateZMatrix(degrees);
		this->applyMatrix(m);
	}

	// Applies a whole transformation matrix to the primitive to move it around
	// the space.
	void baseChange (Vec4 o, Vec4 u, Vec4 v, Vec4 w) {
		Matrix m = Matrix();
		m.set(0, 0, u.x);
		m.set(1, 0, u.y);
		m.set(2, 0, u.z);
		m.set(0, 1, v.x);
		m.set(1, 1, v.y);
		m.set(2, 1, v.z);
		m.set(0, 2, w.x);
		m.set(1, 2, w.y);
		m.set(2, 2, w.z);
		m.set(0, 3, o.x);
		m.set(1, 3, o.y);
		m.set(2, 3, o.z);
		m.set(3, 3, 1);
		
		this->applyMatrix(m);
	}

	// Reverts the transformation the transformation matrix has made to the
	// primitive.
	void baseChangeInverted (Vec4 o, Vec4 u, Vec4 v, Vec4 w) {
		Matrix m = Matrix();
		m.set(0, 0, u.x);
		m.set(1, 0, u.y);
		m.set(2, 0, u.z);
		m.set(0, 1, v.x);
		m.set(1, 1, v.y);
		m.set(2, 1, v.z);
		m.set(0, 2, w.x);
		m.set(1, 2, w.y);
		m.set(2, 2, w.z);
		m.set(0, 3, o.x);
		m.set(1, 3, o.y);
		m.set(2, 3, o.z);
		m.set(3, 3, 1);
		m.invertMatrix();

		this->applyMatrix(m);
	}

	// Operation.
	friend bool operator== (const Vec4 &c1, const Vec4 &c2) {
		return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z && c1.isPoint == c2.isPoint;
	}

	// Access to the coordinates of the vector.
	const float& operator[](int i) const {
		switch (i%3) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}
		return x;
	}

	// Transforms the vector into an array of 3 floats.
	array<float, 3> asThree() const {
		return {x, y, z};
	}

	// Returns the angle two directions make.
	friend double angleBetweenDirections (const Vec4 &c1, const Vec4 &c2) {
		return acos(dot(c1, c2) / (c1.mod() * c2.mod()));
	}

	// Writes the vector to the output stream.
	friend ostream &operator<< (ostream &o, const Vec4 &c){
		o << (c.isPoint?"(":"[") << c.x << ", " << c.y << ", " << c.z << (c.isPoint?")":"]");
		return o;
	}

};

// Object that represents a point in 3D space.
class Point : public Vec4 {

public:

	// Constructor.
	Point () : Vec4(0, 0, 0, 1) {}

	// Constructor.
	Point (float x, float y, float z) : Vec4(x, y, z, 1) {}

	// Constructor.
	Point (Vec4 v) : Vec4(v.getX(), v.getY(), v.getZ(), 1) {}

	// Returns the point that is the result of adding a direction to the point.
	friend Vec4 toVec4 (Point p) {
		return Vec4(p.getX(), p.getY(), p.getZ(), 1);
	}

	
	friend istream &operator>> (istream &i, Point &p) {
		i >> p.x >> p.y >> p.z;
		return i;
	}
};

class Direction : public Vec4 {
public:
	Direction () : Vec4(0, 0, 0, 0) {}
	Direction (float x, float y, float z) : Vec4(x, y, z, 0) {}
	Direction (Vec4 v) : Vec4(v.getX(), v.getY(), v.getZ(), 0) {}
	friend Vec4 toVec4 (Direction d) {
		return Vec4(d.getX(), d.getY(), d.getZ(), 0);
	}
	Direction randomDirection() {
		double x = randTheta2.get(), y = randTheta2.get(), z = randTheta2.get();
		while (x*x + y*y + z*z > 1) {
			x = randTheta2.get();
			y = randTheta2.get();
			z = randTheta2.get();
		}
		return Direction(x, y, z).normalize();
	}
	friend istream &operator>> (istream &i, Direction &d) {
		i >> d.x >> d.y >> d.z;
		return i;
	}
};

#endif // VEC4_H