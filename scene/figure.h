#ifndef FIGURE_H
#define FIGURE_H
#pragma once

#include "../coordinates/vec4.h"
#include "../coordinates/ray.h"
#include "../light/color.h"
#include "camera.h"
#include "lightSource.h"
#include "../utilities/randomGenerator.h"
#include <cmath>
#include <cstdint>

enum Phenomenom {DIFFUSE, REFLECTION, REFRACTION, LIGHT, NONE};

// Random generators.
randomGenerator russianRoulette(0, 1);
randomGenerator normalGenerator(-1, 1);

// Class that represents any figure.
class Figure {

public:

	// ID of the figure.
	uint16_t id;

	// Color (diffuse, specular, transmission and light) coeficients and
	// refraction index.
	Color kd, ks, kt, kl;
	float nRefraction;

	// Channel with the maximum contribution.
	Light majorCh;

public:

	// Checks if the ray intersects the figure and returns the distance to the
	// intersection point.
	virtual bool intersect(const Ray& ray, float& t) const = 0;

	// Checks if the plane is between the camera and the light source so that
	// means that the light source is not visible from the camera at that point.
	virtual bool planeAgainstLight(const Camera& camera,
							const LightSource& light, const Point& p) const = 0;

	// Gets the normal of the figure at the intersection point p.
	virtual Direction getNormal(const Point& p) const = 0;

	// Gets the phenomenom that the ray suffers when it intersects the figure.
	Phenomenom getPhenomenom () const {
		float r = russianRoulette.get();
		float aux = kd[majorCh];
		if (r < aux) return DIFFUSE;
		aux += ks[majorCh];
		if (r < aux) return REFLECTION;
		aux += kt[majorCh];
		if (r < aux) return REFRACTION;
		aux += kl[majorCh];
		if (r < aux) return LIGHT;
		return NONE;
	}

	// Gets the color of the figure at the intersection point p.
	Color getFrDiffuse() {
		return kd / (M_PI * kd[majorCh]);
	}

	// Gets the color of the figure at the intersection point p.
	Color getFr(Phenomenom ph, Direction& d, Point p) const {
		Direction wi;
		Direction normal, aux;
		float eta;
		switch (ph) {
			case DIFFUSE:
				aux = randBounce(p);
				d = dot(aux, d) < 0 ? aux : -aux;
				return kd / (M_PI * kd[majorCh]);
			case REFLECTION:
				d = reflectionBounce(d, p);
				return ks / ks[majorCh];
			case REFRACTION:
				wi = d;
				wi = -wi.normalize();
				normal = getNormal(p);
				if(dot(wi, normal) < 0){
					normal = -normal;
					eta = nRefraction / 1;
				} else{
					eta = 1 / nRefraction;
				}
				d = refractionBounce(wi, normal, eta);
				return kt / kt[majorCh];
			case LIGHT:
				return kl / kl[majorCh];
			default:
				return Color();
		}
	}

	// Gets a random direction to bounce the ray.
	Direction randBounce(const Point& p) const {
		Direction normal = getNormal(p);
		Direction res = Direction(normalGenerator.get(), normalGenerator.get(), normalGenerator.get());
		if (res.mod() > 1)
				res = Direction(normalGenerator.get(), normalGenerator.get(), normalGenerator.get());
		return dot(res, normal) > 0 ? res.normalize() : -res.normalize();
	}

	// Gets the direction of a reflection bounce.
	Direction reflectionBounce (const Direction& d, const Point& p) const {
		Direction normal = getNormal(p);
		Direction dNorm = d.normalize();
		if (dot(dNorm, normal) < 0)
			normal = -normal;
		return (dNorm - 2 * normal * dot(dNorm, normal)).normalize();
	}

	// Gets the direction of a refraction bounce, using eta as the ratio of
	// refraction indexes.
	Direction refractionBounce (const Direction& wi, const Direction& n, float eta) const {
		float cosThetaI = dot(n, wi);
		float sin2ThetaI = max(0.f, 1.0f - cosThetaI * cosThetaI);
		float sin2ThetaT = eta * eta * sin2ThetaI;
		float cosThetaT = sqrt(1 - sin2ThetaT);
		return eta * -wi + (eta * cosThetaI - cosThetaT) * n;
	}

	// Constructor.
	Figure (const uint16_t id, const Color& diffuse, const Color& reflex,
			const Color& refract, const Color& light, const float nCoef) :
			id(id), kd(diffuse), ks(reflex), kt(refract), kl(light),
			nRefraction(nCoef) {

		// Gets the channel with the maximum contribution.
		Light lD, lS, lT, lL;
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

		// Checks that the sum of the color coeficients is 1 or less.
		for (int i = 0; i < 3; ++i)
			if (kd[i] + ks[i] + kt[i] + kl[i] > 1) {
				cout << "Error: the sum of figure color coeficients must be for channel, as much, 1.\n";
				exit(1);
		}
		
	}

	// Destructor.
	virtual ~Figure() {}

};

// Class that represents a plane.
class Plane : public Figure {
	
	Direction n;	// Normal.
	float d;		// Distance to the coordinate origin.

public:

	// Constructor with normal and distance.
	Plane (const uint16_t id, const Direction& normal, float distance, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef) :
			n(normal.normalize()), d(distance), Figure(id, diffuse, reflex, refract, light, nCoef) {}

	// Constructor with normal and a point.
	Plane (const uint16_t id, const Direction& normal, const Point& p, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef) :
			n(normal.normalize()), d(abs(dot(n, distance(Point(0,0,0), p)))), Figure(id, diffuse, reflex, refract, light, nCoef) {}
			
	// Returns the normal of the plane.
	Direction getNormal(const Point& p) const {
		return n;
	}

	// Checks if the ray intersects the plane and returns the distance to the
	// intersection point.
	bool intersect(const Ray& ray, float& t) const {
		float denom = dot(ray.getDirection(), n);
		if (abs(denom) > 1e-6) {
			t = -(d - dot(distance(Point(0,0,0), ray.getPoint()), n)) / denom;
			return t >= 0;
		}
		// If not, then it's false (because tray and plane are parallel).
		return false;
	}

	// Checks if the plane is between the camera and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		float t;
		Direction distanceToLight = distance(light.center, camera.o);
		return this->intersect(Ray(camera.o, distanceToLight), t) && t < 1;
	}

};

// Class that represents a sphere.
class Sphere : public Figure {

	Point c;	// Center.
	float r;	// Radius.

public:

	// Constructor.
	Sphere(const uint16_t id, const Point& c, float r, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef) :
			c(c), r(r), Figure(id, diffuse, reflex, refract, light, nCoef) {}

	// Returns the normal of the sphere at the intersection point p.
	Direction getNormal(const Point& p) const {
		return distance(p, c).normalize();
	}
	
	// Checks if the ray intersects the sphere and returns the distance to the
	// intersection point.
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
			if (t1 >= 0){
				t = t1;
				return true;
			}
			if (t2 >= 0){
				t = t2;
				return true;
			}
			return true;
		}
	}

	// Checks if the plane is between the camera and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		return false;
	}

};

// Class that represents a triangle.
class Triangle : public Figure {

	Point p1, p2, p3;	// Vertices.
	Direction n;		// Normal of the plane that contains the triangle.
	float d;			// Distance of the plane to the origin.

public:

	// Constructor.
	Triangle(const uint16_t id, const Point& p1, const Point& p2, const Point& p3, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef) :
			p1(p1), p2(p2), p3(p3), Figure(id, diffuse, reflex, refract, light, nCoef),
			n(cross(distance(p2, p1), distance(p3, p1))),
			d(dot(n, distance(p1, Point(0, 0, 0)))) {}

	// Returns the normal of the triangle at the intersection point p.
	Direction getNormal(const Point& p) const {
		return n;
	}

	// Checks if the ray intersects the triangle and returns the distance to the
	// intersection point.
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
		// If not, then it's false (because tray and plane are parallel).
		return false;
	}

	// Checks if the plane in which the triangle is contained is between the
	// camera and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		return Plane(id, n, d, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
	}

};

// Class that represents a disc.
class Disc : public Figure {

	Point c;		// Center.
	Direction n;	// Nornaml.
	float r, d;		// Radius and distance of the plane to the origin.

public:

	// Constructor.
	Disc (const uint16_t id, const Point& center, const Direction& normal, float radius, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef)
		: c(center), n(normal.normalize()), r(radius), d(dot(n, distance(Point(0, 0, 0), center))), Figure(id, diffuse, reflex, refract, light, nCoef) {}

	// Returns the normal of the disc at the intersection point p.
	Direction getNormal(const Point& p) const {
		return n;
	}

	// Checks if the ray intersects the disc and returns the distance to the
	// intersection point.
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
		// If not, then it's false (because tray and plane are parallel).
		return false;
	}

	// Checks if the plane in which the disc is contained is between the
	// camera and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		return Plane(id, n, d, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
	}

};

// Class that represents a perfored disc.
class PerforedDisc : public Figure {

	Point c;		// Center.
	Direction n;	// Normal of the plane.
	float r, rp, d;	// Radius, radius of the perforation and distance of the
					// plane to the origin.

public:

	// Constructor.
	PerforedDisc (const uint16_t id, const Point& center, const Direction& normal, float radius, float radiusPerforation, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef)
		: c(center), n(normal.normalize()), r(radius), rp(radiusPerforation), d(dot(n, distance(Point(0, 0, 0), center))), Figure(id, diffuse, reflex, refract, light, nCoef) {}

	// Returns the normal of the perfored disc at the intersection point p.
	Direction getNormal(const Point& p) const {
		return n;
	}

	// Checks if the ray intersects the perfored disc and returns the distance to the
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
		// If not, then it's false (because tray and plane are parallel).
		return false;
	}

	// Checks if the plane in which the perfored disc is contained is between
	// the camera and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		return Plane(id, n, d, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
	}

};

// Class that represents a cylinder.
class Cylinder : public Figure {

	Point c;		// Center.
	Direction ax;	// Cilinder axis direction.
	float r;		// Cilinder radius.
	float h;		// Height.

public:

	// Constructor.
	Cylinder (const uint16_t id, const Point& center, const Direction& axis, float radius, float height, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef)
		: c(center), ax(axis.normalize()), r(radius), h(height/2), Figure(id, diffuse, reflex, refract, light, nCoef) {}

	// Returns the normal of the cylinder at the intersection point p.
	Direction getNormal(const Point& p) const {
		return (distance(p, c) - dot(distance(p, c), ax) * ax).normalize();
	}

	// Checks if the ray intersects the cylinder and returns the distance to the
	// intersection point.
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
		}
	}

	// Checks if the ray intersects the cylinder and returns the distance to the
	// intersection point.
	bool intersect(const Ray& ray, float& t) const {
		return intersect(ray, t, false);
	}

	// Checks if the plane in which the cylinder is contained is between
	// the camera and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		Direction planeNormal = distance(p, c) - dot(distance(p, c), ax) * ax;
		return Plane(id, planeNormal, p, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
	}

};

class Cone : public Figure {

	Point c;		// Center.
	Direction ax;	// Cone axis direction.
	float r;		// Base radius.
	float h;		// Cone height.
	Point c2;		// Base center.
	float h2;		// Distance from the center to one point of the base.

public:

	// Constructor.
	Cone (const uint16_t id, const Point& center, const Direction& axis, float radius, float height, const Color& diffuse,
			const Color& reflex, const Color& refract, const Color& light,
			const float nCoef)
		: c(center), ax(axis.normalize()), r(radius), h(height), c2 (center+ax*height),
		  h2(sqrt(r*r+h*h)), Figure(id, diffuse, reflex, refract, light, nCoef) {}

	// Returns the normal of the cone at the intersection point p.
	Direction getNormal(const Point& p) const {
		return (distance(p, c) - dot(distance(p, c), ax) * ax).normalize();
	}

	// Checks if the ray intersects the cone and returns the distance to the
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

	// Checks if the ray intersects the cone and returns the distance to the
	// intersection point.
	bool intersect(const Ray& ray, float& t) const {
		return intersect(ray, t, false);
	}

	// Checks if the plane in which the cone is contained is between the camera
	// and the light source.
	bool planeAgainstLight(const Camera& camera, const LightSource& light, const Point& p) const {
		// Gets the normal of the plane tangent to the cone in which p is contained.
		Direction vertexToP = distance(p, c);
		Direction perpendicular2 = cross(vertexToP, distance(c2, c + vertexToP.normalize()*h2));
		Direction planeNormal = cross(vertexToP, perpendicular2).normalize();
		return Plane(id, planeNormal, p, Color(0), Color(0), Color(0), Color(0), 0).planeAgainstLight(camera, light, p);
	}

};

#endif // FIGURE_H