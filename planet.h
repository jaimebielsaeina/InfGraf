#pragma once
#ifndef PLANET_H
#include "vec4.h"

class BasicPlanet {

    Point center;
    double radius;

public:
    BasicPlanet (Point center, double radius) : center(center), radius(radius) {}
    Point getCenter () const {
        return center;
    }
    double getRadius () const {
        return radius;
    }
    Point city (double theta, double phi) const {
        return Point(center.getX() + radius * cos(theta) * cos(phi),
                    center.getY() + radius * sin(theta) * cos(phi),
                    center.getZ() + radius * sin(phi));
    }

};

class Planet {

    Point center;
    Direction axis;
    double meridian;

public:
    // guarda el centro, el eje  y el meridiano como el angulo de desplaxamiento respecto del eje y
    Planet (Point center, Direction axis, Point meridian) : center(center), axis(axis), meridian() {
        
    }
    Point getCenter () const {
        return center;
    }
    Direction getAxis () const {
        return axis;
    }
    Point getMeridian () const {
        return meridian;
    }
    Point city (double theta, double phi) const {
        return Point(center.getX() + cos(theta) * cos(phi) * meridian.getX() + sin(theta) * cos(phi) * axis.getX() + sin(phi) * axis.getX(),
                    center.getY() + cos(theta) * cos(phi) * meridian.getY() + sin(theta) * cos(phi) * axis.getY() + sin(phi) * axis.getY(),
                    center.getZ() + cos(theta) * cos(phi) * meridian.getZ() + sin(theta) * cos(phi) * axis.getZ() + sin(phi) * axis.getZ());
    }

};

// devuelve cierto si el segmento de recta que une los puntos (th1, ph1) y (th2, ph2) cruza la superficie
// de p1 o p2. En caso contrario devuelve falso.
bool crossed (const Planet &p1, double th1, double ph1, const Planet &p2, double th2, double ph2) {
    bool result = true;
    Vec4 c1 = p1.city(th1, ph1);
    Vec4 c2 = p2.city(th2, ph2);
    Vec4 d = distance (c1, c2);
    Vec4 d1 = distance (c1, p1.getCenter());
    Vec4 d2 = distance (c2, p2.getCenter());
    if (angleBetweenDirections(d, d1) < 90) {
        cout << "colisión en el primer planeta" << endl;
        result = false;
    }
    if (angleBetweenDirections(-d, d2) < 90) {
        cout << "colisión en el segundo planeta" << endl;
        result = false;
    }
    return result;
}

#endif // PLANET_H