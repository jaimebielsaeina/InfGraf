#pragma once
#ifndef COLOR_H
#define COLOR_H
#include <cstdint>

class Color {
public:
    double r, g, b;
    Color(double r, double g, double b) : r(r), g(g), b(b) {}
    Color() : r(0), g(0), b(0) {}
    ~Color() {}

    friend Color operator+(const Color& c1, const Color& c2) {
        return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
    }

    friend Color operator-(const Color& c1, const Color& c2) {
        return Color(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b);
    }

    friend Color operator*(const Color& c, double k) {
        return Color(c.r * k, c.g * k, c.b * k);
    }

    friend Color operator*(const Color& c1, const Color& c2) {
        return Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b);
    }
    
    friend Color operator/(const Color& c, double k) {
        return Color(c.r / k, c.g / k, c.b / k);
    }
    
    
};

#endif // COLOR_H