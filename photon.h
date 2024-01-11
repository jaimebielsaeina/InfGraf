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
    uint16_t figure;
    // It returns the axis i position (x, y or z)
    Photon (Point position, Direction incident, Color flux, uint16_t figure) :
            pos(position), incident(incident), flux(flux), figure(figure) {}
    float position(int i) const { return pos[i]; }
    friend ostream& operator<<(ostream& os, Photon p) {
        os << p.pos << " " << p.incident << " " << p.flux;
        // os << "a";
        return os;
    }
};

#endif