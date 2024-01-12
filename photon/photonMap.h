#ifndef PHOTONMAP_H
#define PHOTONMAP_H
#pragma once

/*
Description :   This is an example of usage of the KDTree class. It does not
				compile, but should give you the idea of how to integrate the
				KDTree in your code
*/
#include "kdtree.h"
#include "photon.h"
#include <list>
/* 
	An additional struct that allows the KD-Tree to access your photon position
*/
struct PhotonAxisPosition {
	float operator()(const Photon& p, std::size_t i) const {
		return p.position(i);
	}
};

/* 
	The KD-Tree ready to work in 3 dimensions, with YourPhoton s, under a 
	brand-new name: YourPhotonMap 
*/
using PhotonMap = nn::KDTree<Photon,3,PhotonAxisPosition>;

/*
	Example function to generate the photon map with the given photons
*/
PhotonMap generation_of_photon_map(std::list<Photon> photons){
	return PhotonMap(photons, PhotonAxisPosition());
}

/*
	Example method to search for the nearest neighbors of the photon map
*/
auto search_nearest(PhotonMap map, Point p, unsigned long k, float r) {
	// Position to look for the nearest photons		Vec3D query_position = ...;
	// Maximum number of photons to look for		unsigned long nphotons_estimate = ...;
	// Maximum distance to look for photons			float radius_estimate = ...;

	// nearest is the nearest photons returned by the KDTree
	return map.nearest_neighbors(p.asThree(), k, r);
}

#endif // PHOTONMAP_H