#ifndef PHOTON_H
#define PHOTON_H
#include "vec4.h"
#include "color.h"

/* 
    Your Photon class implementation, which stores each 
    photon walk interaction 
*/
class Photon {
public:
    Point pos;    // 3D point of the interaction
    Direction incident;
    Color flux;
    // It returns the axis i position (x, y or z)
    Photon (Point position, Direction incident, Color flux) : pos(position), incident(incident), flux(flux) {}
    float position(int i) const { return pos[i]; }
};

#endif