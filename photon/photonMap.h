#ifndef PHOTONMAP_H
#define PHOTONMAP_H
#pragma once

#include "kdtree.h"
#include "photon.h"
#include <list>

// Allows the KD-Tree to access your photon position
struct PhotonAxisPosition {
	float operator()(const Photon& p, std::size_t i) const {
		return p.position(i);
	}
};

// Defines the PhotonMap as a KDTree with photons, 3 dimensions and the
// PhotonAxisPosition as the function to access the photon position.
using PhotonMap = nn::KDTree<Photon,3,PhotonAxisPosition>;

// Genera el mapa de fotones a partir de los mismos en una lista.
PhotonMap generation_of_photon_map(std::list<Photon> photons){
	return PhotonMap(photons, PhotonAxisPosition());
}

#endif // PHOTONMAP_H