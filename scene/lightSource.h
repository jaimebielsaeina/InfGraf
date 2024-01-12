#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H
#pragma once

#include "../light/color.h"
#include "../coordinates/vec4.h"

// Object that represents a light source.
class LightSource {

public:

    // Center of the light source.
    Point center;

    // Power of the light source.
    Color power;

    // Constructor.
    LightSource(const Point& center, const Color& power) : center(center), power(power) {}

};


#endif // LIGHTSOURCE_H