#include "vec4.h"
#include "matrix.h"
#include "planet.h"

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
}