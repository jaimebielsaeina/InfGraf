#include "vec4.h"
#include "matrix.h"
#include "planet.h"
#include "figure.h"

int main () {
    Vec4 c1 = Point(1, 4, 5);
    Vec4 c2 = Point(2, 3, 6);
    Vec4 c3 = Point(1, 2, 3);

    c1.translate(3, 4, 1);
    cout << c2 << endl;
    c2.baseChange(Point(2, 3, 6), Direction(1, 0, 0), Direction(0, 1, 0), Direction(0, 0, 1));
    cout << c2 << endl;
    c2.baseChangeInverted(Point(2, 3, 6), Direction(1, 0, 0), Direction(0, 1, 0), Direction(0, 0, 1));
    cout << c2 << endl;

    Planet p1 = Planet(Point(0, 0, 0), Direction(0, 0, 2), Point(1, 0, 0));
    Planet p2 = Planet(Point(3, 0, 0), Direction(0, 0, 2), Point(2, 0, 0));
    cout << p1.city(0, 0) << endl;
    cout << p2.city(0, 0) << endl;
    cout << crossed(p1, 0, 0, p2, 180, 0) << endl;

    Ray r = Ray(Point(0, 0, 0), Direction(1, 0, 0));
    Plane pl = Plane(Point(4, 0, 0), Direction(-1, 0, 0));
    float t;
    cout << pl.intersect(r, t) << " " << t << endl;
    Sphere sp = Sphere(Point(4, 1.001, 0), 1);
    cout << sp.intersect(r, t) << " " << t << endl;

}