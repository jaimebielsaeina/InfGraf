#pragma once
#ifndef Vec4_H
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <ostream>
using namespace std;

class Vec4 {
    double x, y, z;
    uint8_t isPoint;

public:
    Vec4 () : x(0), y(0), z(0), isPoint(0) {}
    Vec4 (double x, double y, double z, uint8_t isPoint) : x(x), y(y), z(z), isPoint(isPoint) {}

public:
    // static Vec4 Point (double x, double y, double z) {
    //     return Vec4(x, y, z, 1);
    // }

    // static Vec4 Direction (double x, double y, double z) {
    //     return Vec4(x, y, z, 0);
    // }

    double getX () const {
        return x;
    }

    double getY () const {
        return y;
    }

    double getZ () const {
        return z;
    }

    uint8_t getIsPoint () const {
        return isPoint;
    }

    uint8_t getIsDirection () const {
        return 1-isPoint;
    }

    friend Vec4 operator+ (const Vec4 &c1, const Vec4 &c2) {
        if(c2.isPoint){
            cout << "Error: second operand must be a direction.\n";
        }
        return Vec4(c1.x + c2.x, c1.y + c2.y, c1.z + c2.z, c1.isPoint);
    }

    friend Vec4 operator- (const Vec4 &c1, const Vec4 &c2) {
        if(c2.isPoint){
            cout << "Error: second operand must be a direction.\n";
        }
        return Vec4(c1.x - c2.x, c1.y - c2.y, c1.z - c2.z, c1.isPoint);
    }

    friend Vec4 operator* (const Vec4 &c, const double &s) {
        if(c.isPoint){
            cout << "Error: Vec4 operand must be a direction.\n";
        }
        return Vec4(c.x * s, c.y * s, c.z * s, 0);
    }

    friend Vec4 operator* (const double &s, const Vec4 &c) {
        return c * s;
    }

    friend Vec4 operator/ (const Vec4 &c, const double &s) {
        if(c.isPoint){
            cout << "Error: Vec4 operand must be a direction.\n";
        }
        return Vec4(c.x / s, c.y / s, c.z / s, 0);
    }

    friend Vec4 operator/ (const double &s, const Vec4 &c) {
        return c / s;
    }

    double mod() {
        if (isPoint) {
            cout << "Error: Vec4 operand must be a direction.\n";
        }
        return sqrt(x * x + y * y + z * z);
    }

    Vec4 normalize() {
        if (isPoint) {
            cout << "Error: Vec4 operand must be a direction.\n";
        }
        return *this / mod();
    }

    friend double dot (const Vec4 &c1, const Vec4 &c2) {
        if(c1.isPoint || c2.isPoint){
            cout << "Error: both operands must be directions.\n";
        }
        return c1.x * c2.x + c1.y * c2.y + c1.z * c2.z;
    }

    friend Vec4 cross (const Vec4 &c1, const Vec4 &c2) {
        if(c1.isPoint || c2.isPoint){
            cout << "Error: both operands must be directions.\n";
        }
        return Vec4 (c1.y * c2.z - c1.z * c2.y, 
                     c1.z * c2.x - c1.x * c2.z, 
                     c1.x * c2.y - c1.y * c2.x, 
                     0);
    }

    friend ostream &operator<< (ostream &o, const Vec4 &c){
        o << (c.isPoint?"(":"[") << c.x << ", " << c.y << ", " << c.z << (c.isPoint?")":"]");
        return o;
    }
};

class Point : public Vec4 {
public:
    Point () : Vec4(0, 0, 0, 1) {}
    Point (double x, double y, double z) : Vec4(x, y, z, 1) {}
};

class Direction : public Vec4 {
public:
    Direction () : Vec4(0, 0, 0, 0) {}
    Direction (double x, double y, double z) : Vec4(x, y, z, 0) {}
};

#endif // Vec4_H