#include "vec4.h"

int main () {
    Vec4 c1 = Point(1, 4, 5);
    Vec4 c2 = Point(2, 3, 6);
    Vec4 c3 = Point(1, 2, 3);

    cout << c1 << endl;
    cout << c1 + c2 << endl;
    cout << c1 - c2 << endl;
    cout << c1 * 2 << endl;
    cout << 2 * c1 << endl;
    cout << c1 / 2 << endl;
    cout << c1.mod() << endl;
    cout << c2.mod() << endl;
    cout << c1.normalize() << endl;
    cout << c2.normalize() << endl;
    cout << dot (c1, c2) << endl;
    cout << cross (c1, c2) << endl;

    cout << c1 << " " << c2 << endl << c1.mod() << " " << c2.mod() << endl;
}