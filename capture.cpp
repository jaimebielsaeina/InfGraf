#include "vec4.h"
#include "figure.h"
#include "camera.h"
#include "randomGenerator.h"
#include "lightSource.h"
#include "planet.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>




/*
	
*/
Color getColorOfHit (const Figure* figure, list<Figure*> figures, const Point& hit,
					 const Point& prevHit, list<LightSource*> lightSources, 
					 const Camera& camera, float& cosine, Direction& Wi) {
	
	Color totalColor;
	for (LightSource* light : lightSources) {

		Vec4 distanceToLight = distance(light->center, hit);
		float modDistanceToLight = distanceToLight.mod();

		bool shadow = false;
		float t;

		// Check if the plane is against the light.
		if (figure->planeAgainstLight(camera, *light, hit)) {
			shadow = true;
			break;
		} else

		// Check if the point is in shadow.
		for (Figure* fig : figures)
				if (fig->intersect(Ray(light->center, -distanceToLight), t)
				&&	(1 - t) * modDistanceToLight > 1e-6
				&&	(  fig != figure
					|| distance(hit, light->center - t*(distanceToLight)).mod() > 1e-4)) {
						shadow = true;
						break;
		}

		if (shadow) continue;

		Color Li = light->power / (modDistanceToLight*modDistanceToLight);
		Color Fr = figure->color / M_PI;
		
		// revisar bien como se calcula el coseno, aqui se acumula para todas las luces y no deberia ser asi
		
		totalColor = totalColor + (Li * Fr * cosine * abs(dot(figure->getNormal(hit), distanceToLight.normalize())));
	}

	Wi = figure -> nextDirection(hit);
	cosine = cosine * abs(dot(figure->getNormal(hit), distance(hit, prevHit)));
	return totalColor;
}



// Capture the scene from the camera's point of view
void capture(Camera& camera, list<Figure*> figures, list<LightSource*> lightSources, int raysPerPixel, int maxBounces, string fileName) {
	std::ofstream output(fileName);
	output << "P3" << endl;
	//output << "# MAX" << endl;
	output << camera.width << " " << camera.height << endl;
	output << "255" << endl;

	Vec4 modL = camera.l.normalize(); 
    Vec4 modU = camera.u.normalize();
	Vec4 sightOrigin = camera.f + camera.l + camera.u;
	randomGenerator rand(0, 1);
	Color pxColor;
	for (int i = 0; i < camera.height; i++) {
		//cout << i << endl;
		for (int j = 0; j < camera.width; j++) {
			pxColor = Color();
			for (int k = 0; k < raysPerPixel; k++) {

				Direction rayDirection = Direction(sightOrigin - (j+rand.get())*camera.widthPerPixel*modL - (i+rand.get())*camera.heightPerPixel*modU);
				Ray ray = Ray(camera.o, rayDirection);

				float t, minT, cosine = 1;
				Point hit, prevHit = camera.o;

				for (int n = 0; n < maxBounces; ++n) {

					minT = 1e6;
					Figure* closestFigure = nullptr;
					for (Figure* figure : figures)
						if (figure->intersect(ray, t) && t < minT) {
							minT = t;
							closestFigure = figure;
					}

					if (closestFigure == nullptr) break;
					hit = ray.getPoint() + minT*ray.getDirection();

					pxColor += getColorOfHit(closestFigure, figures, hit, prevHit, lightSources, camera, cosine, rayDirection);	

					//ray = ;
					prevHit = hit;

				}
			}
			output << pxColor.r/raysPerPixel << " " << pxColor.g/raysPerPixel << " " << pxColor.b/raysPerPixel << "    ";
		}
		output << endl;
	}
	output.close();
}

int main() {
	Direction l = Direction(-1, 0, 0); // l.rotateY(-15);
	Direction u = Direction(0, 1, 0); // u.rotateY(-15);
	Direction f = Direction(0, 0, 3); // f.rotateY(-15);
    Camera camera = Camera(Point(0, 0, -3.5), l, u, f, 256, 256);
	
    list<Figure*> listFigures = {};

	Plane* leftPlane = new Plane(Direction(1, 0, 0), 1, Color(255, 0, 0));
	Plane* rightPlane = new Plane(Direction(-1, 0, 0), 1, Color(0, 255, 0));
	Plane* floorPlane = new Plane(Direction(0, 1, 0), 1, Color(200, 200, 200));
	Plane* ceilingPlane = new Plane(Direction(0, -1, 0), 1, Color(200, 200, 200));
	Plane* backPlane = new Plane(Direction(0, 0, -1), 1, Color(200, 200, 200));

	Sphere* leftSphere = new Sphere(Point(-0.5, -0.7, 0.25), 0.3, Color(255, 0, 255));
	Sphere* rightSphere = new Sphere(Point(0.5, -0.7, -0.25), 0.3, Color(0, 255, 255));

    Sphere* centerSphere = new Sphere(Point(0, -0.5, 0), 0.5, Color(255, 255, 0));

	Triangle* triangle = new Triangle(Point(-0.25, -0.5, -0.5), Point(1.5, 0, 1), Point(-0.25, 1, 0), Color(255, 127, 0));

	Disc* disc = new Disc(Point(-0.6, 0.6, 0), Direction(-2, 1, 1), 0.2, Color(0, 255, 255));
	PerforedDisc* perforedDisc = new PerforedDisc(Point(-0.6, 0.2, -0.2), Direction(1, 1, 1), 0.3, 0.2, Color(0, 255, 255));

	Cylinder* cylinder = new Cylinder(Point(0.5, 0, 0.4), Direction(0, 1, 1), 0.2, 1.5, Color(127, 0, 255));

	Cone* cone = new Cone (Point(0.5, -0.5, 0), Direction(0, 1, -0.2), 0.5, 0.5, Color(128, 0, 0));

    listFigures.push_back(leftPlane);
    listFigures.push_back(rightPlane);
    listFigures.push_back(floorPlane);
    listFigures.push_back(ceilingPlane);
    listFigures.push_back(backPlane);

    listFigures.push_back(leftSphere);
    listFigures.push_back(rightSphere);
	
	// listFigures.push_back(centerSphere);

	// listFigures.push_back(triangle);
	// listFigures.push_back(disc);
	// listFigures.push_back(perforedDisc);
	// listFigures.push_back(cylinder);
	// listFigures.push_back(cone);
	
	list<LightSource*> lightSources = {};

	lightSources.push_back(new LightSource(Point(0.5, 0.5, -1), Color(1, 1, 1)));
	lightSources.push_back(new LightSource(Point(-0.5, 0, 0.4), Color(1, 1, 1)));

    capture(camera, listFigures, lightSources, 16, 1, "output.ppm");
}