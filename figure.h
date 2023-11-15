#pragma once
#ifndef FIGURE_H
#define FIGURE_H
#include "vec4.h"
#include "planet.h"
#include "color.h"
#include "camera.h"
#include "lightSource.h"
#include "randomGenerator.h"
#include <cmath>

randomGenerator randGen(0, 1);
randomGenerator randPhi(0, 2*M_PI);

class Ray {

    Vec4 p, d;

public:
    Ray() {}
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
    virtual bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const = 0;
    virtual Direction getNormal(const Point& p) const = 0;
    Color getColor() const { return color / M_PI; }
    Direction nextDirection(const Point& p) const {
        //cout << p << endl;
        float theta = acos(sqrt(randGen.get()));
        float phi = randPhi.get();

        Direction normal = getNormal(p);
        Point randomPoint = BasicPlanet(p, 1).city(theta, phi);
        return distance(Planet(p, normal*2, p + cross(normal, distance(p, randomPoint)).normalize()).city(theta, phi), p).normalize();

        //Genera un punto aleatorio en la esfera unitaria
        
        /*Point randomPoint = BasicPlanet(Point(0,0,0), 1).city(theta, phi);
        cout << randomPoint.getZ() << endl;
        //Cambiar la base del punto aleatorio a la base del plano tangente en p
        Direction normal = this->getNormal(p);
        Direction perpendicular1 = cross(normal, (dot (normal, Direction(1,0,0)) < 0.9) ?  Direction (1,0,0) : Direction(0,1,0));
        Direction perpendicular2 = cross(normal, perpendicular1);
        
        randomPoint.baseChange(p, perpendicular1, perpendicular2, normal);
        
        // returns a random direction in the hemisphere defined by the normal of the figure (getNormal())
        return distance(randomPoint, p);*/

    }
    Figure (const Color& color) : color(color) {}
    virtual ~Figure() {}

};

class Plane : public Figure {
    
    Direction n;
    float d;

public:
    Plane (const Direction& normal, float distance, const Color& color) :
            n(normal.normalize()), d(distance), Figure(color) {}

    Plane (const Direction& normal, const Point& p, const Color& color) :
            n(normal.normalize()), d(abs(dot(n, distance(Point(0,0,0), p)))), Figure(color) {}
            
    Direction getNormal(const Point& p) const {
        return n;
    }

    bool intersect(const Ray& ray, float& t) const {
        float denom = dot(ray.getDirection(), n);
        if (abs(denom) > 1e-6) {
            t = -(d - dot(distance(Point(0,0,0), ray.getPoint()), n)) / denom;
            return t >= 0;
        }
        return false; // porque son prependiculares
    }

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        float t;
        Direction distanceToLight = distance(light.center, camera.o);
        return this->intersect(Ray(camera.o, distanceToLight), t) && t < 1;
    }
};

class Sphere : public Figure {

    Point c;
    float r;
    Color color;

public:
    Sphere(const Point& c, float r, const Color& color) :
            c(c), r(r), Figure(color) {}

    Direction getNormal(const Point& p) const {
        
        return distance(p, c).normalize();
    }
    
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

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        return false;
    }

};

class Triangle : public Figure {

    Point p1, p2, p3;
    Direction n;
    float d;

public:

    Triangle(const Point& p1, const Point& p2, const Point& p3, const Color& color) :
            p1(p1), p2(p2), p3(p3), Figure(color),
            n(cross(distance(p2, p1), distance(p3, p1))),
            d(dot(n, distance(p1, Point(0, 0, 0)))) {}

    Direction getNormal(const Point& p) const {
        return n;
    }

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

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        // Gets the plane in which the triangle is contained
        return Plane(n, d, color).planeAgainstLight(camera, light, p);
    }

};

class Disc : public Figure {

    Point c;
    Direction n;
    float r, d;

public:
    Disc (const Point& center, const Direction& normal, float radius, const Color& color)
        : c(center), n(normal.normalize()), r(radius), d(dot(n, distance(Point(0, 0, 0), center))), Figure(color) {}

    Direction getNormal(const Point& p) const {
        return n;
    }

    bool intersect(const Ray& ray, float& t) const {
        float denom = dot(ray.getDirection(), n);
        if (abs(denom) > 1e-6) {
            t = - dot(distance(ray.getPoint(), c), n) / denom;
            if (t < 0) return false;
            Point p = ray(t);
            if (dot(distance(p, c), distance(p, c)) <= r * r)
                return true;
            return false;
        }
        return false; // porque son prependiculares
    }

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        // Gets the plani in which the disc is contained
        return Plane(n, d, color).planeAgainstLight(camera, light, p);
    }
};

class PerforedDisc : public Figure {

    Point c;
    Direction n;
    float r, rp, d;

public:
    PerforedDisc (const Point& center, const Direction& normal, float radius, float radiusPerforation, const Color& color)
        : c(center), n(normal.normalize()), r(radius), rp(radiusPerforation), d(dot(n, distance(Point(0, 0, 0), center))), Figure(color) {}

    Direction getNormal(const Point& p) const {
        return n;
    }

    bool intersect(const Ray& ray, float& t) const {
        float denom = dot(ray.getDirection(), n);
        if (abs(denom) > 1e-6) {
            t = - dot(distance(ray.getPoint(), c), n) / denom;
            if (t < 0) return false;
            Point p = ray(t);
            if (dot(distance(p, c), distance(p, c)) <= r * r && dot(distance(p, c), distance(p, c)) >= rp * rp)
                return true;
            return false;
        }
        return false; // porque son prependiculares
    }

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        // Gets the plani in which the disc is contained
        return Plane(n, d, color).planeAgainstLight(camera, light, p);
    }
};

class Cylinder : public Figure {

    Point c;      // Centro del cilindro
    Direction ax; // Vector de dirección del eje del cilindro
    float r;      // Radio del cilindro
    float h;      // Altura del cilindro

public:
    Cylinder (const Point& center, const Direction& axis, float radius, float height, const Color& color)
        : c(center), ax(axis.normalize()), r(radius), h(height/2), Figure(color) {}

    Direction getNormal(const Point& p) const {
        return (distance(p, c) - dot(distance(p, c), ax) * ax).normalize();
    }
    bool intersect(const Ray& ray, float& t, bool noRestriction = false) const {
        Vec4 oc = distance(ray.getPoint(), c);
        float a = dot(ray.getDirection(), ray.getDirection()) - dot(ray.getDirection(), ax) * dot(ray.getDirection(), ax);
        float b = 2 * (dot(oc, ray.getDirection()) - dot(oc, ax) * dot(ray.getDirection(), ax));
        float g = dot(oc, oc) - dot(oc, ax) * dot(oc, ax) - r * r;
        float discriminant = b * b - 4 * a * g;
        if (discriminant < 0) {
            return false;
        }
        else {
            float t1 = (-b - sqrt(discriminant)) / (2 * a);
            float t2 = (-b + sqrt(discriminant)) / (2 * a);

            float tb; bool both = false;
            if (t1 >= 0 && t2 >= 0) {
                t = min(t1, t2);
                tb = max(t1, t2);
                both = true;
            }
            else if (t1 >= 0) t = t1;
            else if (t2 >= 0) t = t2;
            else return false;
            if (noRestriction) return true;
            Point p = ray(t);
            if (dot(distance(p, c), ax) < -h || dot(distance(p, c), ax) > h) {
                if (both) {
                    t = tb;
                    p = ray(t);
                    if (dot(distance(p, c), ax) < -h || dot(distance(p, c), ax) > h) return false;
                } else
                    return false;
            }
            return true;



            // if (t1 < 0 && t2 < 0)
            //     return false;
            // if (t1 >= 0) t = t1;
            // if (t2 < t1 && t2 >= 0) t = t2;
            // Point p = ray(t);
            // if (dot(distance(p, c), ax) < -h || dot(distance(p, c), ax) > h)
            //     return false;
            // return true;
        }
    }

    bool intersect(const Ray& ray, float& t) const {
        return intersect(ray, t, false);
    }

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        Direction planeNormal = distance(p, c) - dot(distance(p, c), ax) * ax;
        return Plane(planeNormal, p, color).planeAgainstLight(camera, light, p);
    }
};

class Cone : public Figure {

    Point c;      // Centro del cono
    Direction ax; // Vector de dirección del eje del cono
    float r;      // Radio del cono
    float h;      // Altura del cono
    Point c2;     // Centro de la base del cono
    float h2;     // Distancia del centro a un punto de la base del cono

public:

    Cone (const Point& center, const Direction& axis, float radius, float height, const Color& color)
        : c(center), ax(axis.normalize()), r(radius), h(height), c2 (center+ax*height), h2(sqrt(r*r+h*h)), Figure(color) {}

    Direction getNormal(const Point& p) const {
        return (distance(p, c) - dot(distance(p, c), ax) * ax).normalize();
    }
    
    bool intersect(const Ray& ray, float& t, bool noRestriction = false) const {
        Vec4 oc = distance(ray.getPoint(), c);
        float a = dot(ray.getDirection(), ray.getDirection()) - (1 + r * r / (h * h)) * dot(ray.getDirection(), ax) * dot(ray.getDirection(), ax);
        float b = 2 * (dot(oc, ray.getDirection()) - (1 + r * r / (h * h)) * dot(oc, ax) * dot(ray.getDirection(), ax));
        float g = dot(oc, oc) - (1 + r * r / (h * h)) * dot(oc, ax) * dot(oc, ax);
        float discriminant = b * b - 4 * a * g;
        if (discriminant < 0) {
            return false;
        }
        else {
            float t1 = (-b - sqrt(discriminant)) / (2 * a);
            float t2 = (-b + sqrt(discriminant)) / (2 * a);
            float tb; bool both = false;
            if (t1 >= 0 && t2 >= 0) {
                t = min(t1, t2);
                tb = max(t1, t2);
                both = true;
            }
            else if (t1 >= 0) t = t1;
            else if (t2 >= 0) t = t2;
            else return false;
            if (noRestriction) return true;
            Point p = ray(t);
            if (dot(distance(p, c), ax) < 0) return false;
            if (dot(distance(p, c), ax) > h) {
                if (both) {
                    t = tb;
                    p = ray(t);
                    if (dot(distance(p, c), ax) > h) return false;
                } else
                    return false;
            }
            return true;
        }
    }

    bool intersect(const Ray& ray, float& t) const {
        return intersect(ray, t, false);
    }

    bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
        // obtiene la normal del plano tangente al CONO en el que se encuentra p
        Direction vertexToP = distance(p, c);
        Direction perpendicular2 = cross(vertexToP, distance(c2, c + vertexToP.normalize()*h2));
        Direction planeNormal = cross(vertexToP, perpendicular2).normalize();
        return Plane(planeNormal, p, color).planeAgainstLight(camera, light, p);
    }
};

#endif // FIGURE_H