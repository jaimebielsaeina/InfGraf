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
#include <cstdint>

enum Phenomenom {DIFFUSE, REFLECTION, REFRACTION, LIGHT, NONE};

randomGenerator randGen(0, 1);
randomGenerator randUnit(-1, 1);
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
    Color kd, ks, kt, kl;
    float n;
    Light majorCh;
public:
    virtual bool intersect(const Ray& ray, float& t) const = 0;
    virtual bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const = 0;
    virtual Direction getNormal(const Point& p) const = 0;
    Phenomenom getPhenomenom () const {
        float r = randGen.get();
        float aux = kd.c[majorCh];
        if (r < aux) return DIFFUSE;
        aux += ks.c[majorCh];
        if (r < aux) return REFLECTION;
        aux += kt.c[majorCh];
        if (r < aux) return REFRACTION;
        aux += kl.c[majorCh];
        if (r < aux) return LIGHT;
        return NONE;
    }
    Color getFr(Phenomenom ph, Direction& d, Point p) const {
        switch (ph) {
            case DIFFUSE:
                d = randBounce(p);
                return kd / (M_PI * kd.c[majorCh]);
            case REFLECTION:
                d = reflectionBounce(d, p);
                return ks / (dot(d, getNormal(p)) * ks.c[majorCh]);
            case REFRACTION:
                d = refractionBounce(d, p);
                return kt;
            case LIGHT:
                return kl / kl.c[majorCh];
            default:
                return Color();
        }
    }
    Direction randBounce(const Point& p) const {
        //cout << p << endl;
        // float theta = acos(sqrt(randGen.get()));
        // float phi = randPhi.get();

        // Direction normal = getNormal(p);
        // Point randomPoint = BasicPlanet(p, 1).city(theta, phi);
        // return distance(Planet(p, normal*2, p + distance(p, randomPoint)).city(theta, phi), p);

        Direction normal = getNormal(p);
        Direction res = Direction(randUnit.get(), randUnit.get(), randUnit.get());
        if (res.mod() > 1)
                res = Direction(randUnit.get(), randUnit.get(), randUnit.get());
        return dot(res, normal) > 0 ? res.normalize() : -res.normalize();

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

    Direction reflectionBounce (const Direction& d, const Point& p) const {
        Direction normal = getNormal(p);
        Direction dNorm = d.normalize();
        return (dNorm - 2 * normal * dot(dNorm, normal)).normalize();
    }

    Direction refractionBounce (const Direction& d, const Point& p) const {
        Direction normal = getNormal(p);
        float cosTheta1 = dot(d, normal);
        float sinTheta1 = sqrt(1 - cosTheta1 * cosTheta1);
        float sinTheta2 = sinTheta1 / n;
        float nAux = n;
        if (cosTheta1 < 0) {
            nAux = 1 / n;
        }
        //float sinTheta2 = (n1 / n2) * sinTheta1;
        float cosTheta2 = sqrt(1 - sinTheta2 * sinTheta2);
        return (d + (n * cosTheta1 - cosTheta2) * normal).normalize();
        //return (n1 / n2) * d + (n1 / n2 * cosTheta1 - cosTheta2) * normal;
    }

    Figure (const Color& diffuse, const Color& reflex, const Color& refract, const Color& light, const float nCoef) :
            kd(diffuse), ks(reflex), kt(refract), kl(light), n(nCoef) {
        Light lD, lS, lT, lL;
        // Obtiene el canal con mayor contribución
        double maxD = diffuse.maxC(lD);
        double maxS = reflex.maxC(lS);
        double maxT = refract.maxC(lT);
        double maxL = light.maxC(lL);
        double maxDS = max(maxD, maxS);
        double maxTL = max(maxT, maxL);
        double maxContribution = max(maxDS, maxTL);
        if(maxD == maxContribution) majorCh = lD;
        else if(maxS == maxContribution) majorCh = lS;
        else if(maxT == maxContribution) majorCh = lT;
        else majorCh = lL;

        for (int i = 0; i < 3; ++i)
            if (kd.c[i] + ks.c[i] + kt.c[i] + kl.c[i] > 1) {
                cout << "Error: the sum of figure color coeficients must be for channel, as much, 1.\n";
                exit(1);
        }
        
    }
    virtual ~Figure() {}
};

class Plane : public Figure {
    
    Direction n;
    float d;

public:
    Plane (const Direction& normal, float distance, const Color& diffuse,
            const Color& reflex, const Color& refract, const Color& light,
            const float nCoef) :
            n(normal.normalize()), d(distance), Figure(diffuse, reflex, refract, light, nCoef) {}

    Plane (const Direction& normal, const Point& p, const Color& diffuse,
            const Color& reflex, const Color& refract, const Color& light,
            const float nCoef) :
            n(normal.normalize()), d(abs(dot(n, distance(Point(0,0,0), p)))), Figure(diffuse, reflex, refract, light, nCoef) {}
            
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
    Sphere(const Point& c, float r, const Color& diffuse,
            const Color& reflex, const Color& refract, const Color& light,
            const float nCoef) :
            c(c), r(r), Figure(diffuse, reflex, refract, light, nCoef) {}

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

    Triangle(const Point& p1, const Point& p2, const Point& p3, const Color& color,
            const float diffuse, const float reflex, const float refract, const float light) :
            p1(p1), p2(p2), p3(p3), Figure(color, diffuse, reflex, refract, light),
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
        return Plane(n, d, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
    }

};

class Disc : public Figure {

    Point c;
    Direction n;
    float r, d;

public:
    Disc (const Point& center, const Direction& normal, float radius, const Color& color,
            const float diffuse, const float reflex, const float refract, const float light)
        : c(center), n(normal.normalize()), r(radius), d(dot(n, distance(Point(0, 0, 0), center))), Figure(color, diffuse, reflex, refract, light) {}

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
        return Plane(n, d, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
    }
};

class PerforedDisc : public Figure {

    Point c;
    Direction n;
    float r, rp, d;

public:
    PerforedDisc (const Point& center, const Direction& normal, float radius, float radiusPerforation, const Color& color,
            const float diffuse, const float reflex, const float refract, const float light)
        : c(center), n(normal.normalize()), r(radius), rp(radiusPerforation), d(dot(n, distance(Point(0, 0, 0), center))), Figure(color, diffuse, reflex, refract, light) {}

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
        return Plane(n, d, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
    }
};

class Cylinder : public Figure {

    Point c;      // Centro del cilindro
    Direction ax; // Vector de dirección del eje del cilindro
    float r;      // Radio del cilindro
    float h;      // Altura del cilindro

public:
    Cylinder (const Point& center, const Direction& axis, float radius, float height, const Color& color,
            const float diffuse, const float reflex, const float refract, const float light)
        : c(center), ax(axis.normalize()), r(radius), h(height/2), Figure(color, diffuse, reflex, refract, light) {}

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
        return Plane(planeNormal, p, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
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

    Cone (const Point& center, const Direction& axis, float radius, float height, const Color& color,
            const float diffuse, const float reflex, const float refract, const float light)
        : c(center), ax(axis.normalize()), r(radius), h(height), c2 (center+ax*height),
          h2(sqrt(r*r+h*h)), Figure(color, diffuse, reflex, refract, light) {}

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
        return Plane(planeNormal, p, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
    }
};

#endif // FIGURE_H