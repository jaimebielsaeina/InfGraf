#ifndef PHOTON_H
#define PHOTON_H
#pragma once

#include "../coordinates/vec4.h"
#include "../light/color.h"

// Object that represents a photon.
class Photon {
public:

	// Position of the photon.
	Point pos;

	// Incident direction of the photon.
	Direction incident;

	// Flux of the photon.
	Color flux;

	// Figure that the photon is in.
	uint16_t figure;

	// Returns the axis i position (x, y or z)
	Photon (Point position, Direction incident, Color flux, uint16_t figure) :
			pos(position), incident(incident), flux(flux), figure(figure) {}

	// Returns the axis i position (x, y or z).
	float position(int i) const { return pos[i]; }

	// Writes the photon to the output stream.
	friend ostream& operator<<(ostream& os, Photon p) {
		os << p.pos << " " << p.incident << " " << p.flux;
		return os;
	}

};

#endif