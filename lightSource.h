#pragma once
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H
#include "color.h"
#include "vec4.h"


class LightSource {
    public:
        Point center;
        Color power;
    LightSource(const Point& center, const Color& power) : center(center), power(power) {}

};


#endif // LIGHTSOURCE_H