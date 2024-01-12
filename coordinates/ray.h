#ifndef RAY_H
#define RAY_H
#pragma once

#include "vec4.h"

class Ray {

    // Point of origin and direction of the ray.
    Vec4 p, d;

public:

    // Constructor.
    Ray() {}

    // Constructor.
    Ray(const Point& p, const Direction& d) : p(p), d(d) {}

    // Point getter.
    Vec4 getPoint() const { return p; }

    // Direction getter.
    Vec4 getDirection() const { return d; }

    // Returns the point of the ray located t*d.mod() from p.
    Vec4 operator()(float t) const {
        return p + t * d;
    }

};

#endif // RAY_H