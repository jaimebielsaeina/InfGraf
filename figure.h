#pragma once
#ifndef FIGURE_H
#define FIGURE_H
#include "vec4.h"

class Color {
public:
    uint8_t r, g, b;
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
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
        float denom = dot(ray.getDirection(), n);
        if (abs(denom) > 1e-6) {
            t = - dot(distance(ray.getPoint(), p), n) / denom;
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

class Triangle : public Figure {

    Point p1, p2, p3;
    Direction n;

public:

    Triangle(const Point& p1, const Point& p2, const Point& p3, const Color& color) :
            p1(p1), p2(p2), p3(p3), Figure(color),
            n(cross(distance(p2, p1), distance(p3, p1))) {}
    bool intersect(const Ray& ray, float& t) const {
        float denom = dot(ray.getDirection(), n);
        if (abs(denom) > 1e-6) {
            t = - dot(distance(ray.getPoint(), p1), n) / denom;
            if (t < 0) return false;
            Point p = ray(t);
            Vec4 v1 = distance(p2, p1);
            Vec4 v2 = distance(p3, p2);
            Vec4 v3 = distance(p1, p3);
            Vec4 v4 = distance(p, p1);
            Vec4 v5 = distance(p, p2);
            Vec4 v6 = distance(p, p3);
            if (dot(cross(v1, v4), n) >= 0 && dot(cross(v2, v5), n) >= 0 && dot(cross(v3, v6), n) >= 0)
                return true;
            return false;
        }
        return false; // porque son prependiculares
    }

};

#endif // FIGURE_H