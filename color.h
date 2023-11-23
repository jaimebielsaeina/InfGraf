#pragma once
#ifndef COLOR_H
#define COLOR_H
#include <cstdint>

#define Light char
#define R 0
#define G 1
#define B 2

class Color {
public:
    double c[3];
    Color(double r, double g, double b) { c[0] = r; c[1] = g; c[2] = b; }
    Color(double w) : Color(w, w, w) {}
    Color() : Color(0) {}
    ~Color() {}

    double maxC() const {
        return max(max(c[0], c[1]), c[2]);
    }
    double maxC(Light& l) const {
        l = c[0] > c[1] ? (c[0] > c[2] ? 0 : 2) : (c[1] > c[2] ? 1 : 2);
        return c[l];
    }

    friend Color operator+(const Color& c1, const Color& c2) {
        return Color(c1.c[0] + c2.c[0], c1.c[1] + c2.c[1], c1.c[2] + c2.c[2]);
    }

    friend Color operator+=(Color& c1, const Color& c2) {
        c1 = c1 + c2;
        return c1;
    }

    friend Color operator-(const Color& c1, const Color& c2) {
        return Color(c1.c[0] - c2.c[0], c1.c[1] - c2.c[1], c1.c[2] - c2.c[2]);
    }

    friend Color operator-=(Color& c1, const Color& c2) {
        c1 = c1 - c2;
        return c1;
    }

    friend Color operator*(const Color& c, double k) {
        return Color(c.c[0] * k, c.c[1] * k, c.c[2] * k);
    }

    friend Color operator*=(Color& c, double k) {
        c = c * k;
        return c;
    }

    friend Color operator*(double k, const Color& c) {
        return Color(c.c[0] * k, c.c[1] * k, c.c[2] * k);
    }

    friend Color operator*(const Color& c1, const Color& c2) {
        return Color(c1.c[0] * c2.c[0], c1.c[1] * c2.c[1], c1.c[2] * c2.c[2]);
    }

    friend Color operator*=(Color& c1, const Color& c2) {
        c1 = c1 * c2;
        return c1;
    }
    
    friend Color operator/(const Color& c, double k) {
        return Color(c.c[0] / k, c.c[1] / k, c.c[2] / k);
    }

    friend Color operator/=(Color& c, double k) {
        c = c / k;
        return c;
    }

    friend ostream &operator<< (ostream &o, const Color &c){
        o << c.c[0] << " " << c.c[1] << " " << c.c[2];
        return o;
    }
    
};

#endif // COLOR_H