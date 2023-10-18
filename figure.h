#pragma once
#ifndef FIGURE_H
#define FIGURE_H
#include "vec4.h"

class Color {
public:
    char r, g, b;
    Color(char r, char g, char b) : r(r), g(g), b(b) {}
    Color() : r(0), g(0), b(0) {}
    ~Color() {}
};


class Ray {

    Vec4 p, d;

public:
    Ray(const Point& p, const Direction& d) : p(p), d(d) {}
    Vec4 operator()(float t) const {
        return p + t * d;
    }
    Vec4 getPoint() const { return p; }
    Vec4 getDirection() const { return d; }

};

class Figure {

public:
    Color color;
public:
    virtual bool intersect(const Ray& ray, float& t) const = 0;
    Figure (const Color& color) : color(color) {}
    virtual ~Figure() {}

};

class Plane : public Figure {

    Point p;
    Direction n;
    

public:
    Plane(const Point& p, const Direction& n, const Color& color) : p(p), n(n), Figure(color) {
        //Point pAux = p + 1e-6*n;
    }
    bool intersect(const Ray& ray, float& t) const {
        float denom = abs(dot(ray.getDirection(), n));
        if (denom > 1e-6) {
            t = dot(distance(ray.getPoint(), p), n) / denom;
            return t >= 0;
        }
        return false; // porque son prependiculares
    }

};

class Sphere : public Figure {

    Point c;
    float r;
    Color color;

public:
    Sphere(const Point& c, float r, const Color& color) : c(c), r(r), Figure(color) {}
    bool intersect(const Ray& ray, float& t) const {
        Vec4 oc = distance(ray.getPoint(), c);
        float a = dot(ray.getDirection(), ray.getDirection());
        float b = 2 * dot(oc, ray.getDirection());
        float c = dot(oc, oc) - r * r;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            return false;
        }
        else {
            float t1 = (-b - sqrt(discriminant)) / (2 * a);
            float t2 = (-b + sqrt(discriminant)) / (2 * a);
            if (t1 < 0 && t2 < 0)
                return false;
            if (t1 >= 0) t = t1;
            if (t2 < t1 && t2 >= 0) t = t2;
            return true;
        }
    }

};

#endif // FIGURE_H