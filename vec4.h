#pragma once
#ifndef VEC4_H
#define VEC4_H
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <ostream>
#include <array>
#include "matrix.h"
#include "randomGenerator.h"
using namespace std;

randomGenerator randPhi2(0, 2*M_PI);
randomGenerator randTheta2(-1, 1);

class Vec4 {

protected:
    float x, y, z;
    uint8_t isPoint;

public:
    Vec4 () : x(0), y(0), z(0), isPoint(0) {}
    Vec4 (float x, float y, float z, uint8_t isPoint) : x(x), y(y), z(z), isPoint(isPoint) {}

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

    void setX (double x) {
        this->x = x;
    }

    void setY (double y) {
        this->y = y;
    }

    void setZ (double z) {
        this->z = z;
    }

    friend Vec4 operator+ (const Vec4 &c1, const Vec4 &c2) {
        /*if(c2.isPoint){
            cout << "Error: second operand must be a direction.\n";
        }*/
        return Vec4(c1.x + c2.x, c1.y + c2.y, c1.z + c2.z, c1.isPoint);
    }

    friend Vec4 operator- (const Vec4 &c1, const Vec4 &c2) {
        /*if(c2.isPoint){
            cout << "Error: second operand must be a direction.\n";
        }*/
        return Vec4(c1.x - c2.x, c1.y - c2.y, c1.z - c2.z, c1.isPoint);
    }

    friend Vec4 operator- (const Vec4 &c) {
        return Vec4(-c.x, -c.y, -c.z, c.isPoint);
    }

    friend Vec4 distance (Vec4 p1, Vec4 p2) {
        /*if (!p1.isPoint || !p2.isPoint) {
            cout << "Error: both operands must be points.\n";
        }*/
        return Vec4(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, 0);
    }

    friend Vec4 operator* (const Vec4 &c, const double &s) {
        /*if(c.isPoint){
            cout << "Error: Vec4 operand must be a direction.\n";
        }*/
        return Vec4(c.x * s, c.y * s, c.z * s, 0);
    }

    friend Vec4 operator* (const double &s, const Vec4 &c) {
        return c * s;
    }

    friend Vec4 operator/ (const Vec4 &c, const double &s) {
        /*if(c.isPoint){
            cout << "Error: Vec4 operand must be a direction.\n";
        }*/
        return Vec4(c.x / s, c.y / s, c.z / s, 0);
    }

    friend Vec4 operator/ (const double &s, const Vec4 &c) {
        return c / s;
    }

    float mod() const {
        /*if (isPoint) {
            cout << "Error: Vec4 operand must be a direction.\n";
        }*/
        return sqrt(x * x + y * y + z * z);
    }

    Vec4 normalize() const {
        /*if (isPoint) {
            cout << "Error: Vec4 operand must be a direction.\n";
        }*/
        return *this / mod();
    }

    friend float dot (const Vec4 &c1, const Vec4 &c2) {
        /*if(c1.isPoint || c2.isPoint){
            cout << "Error: both operands must be directions.\n";
        }*/
        return c1.x * c2.x + c1.y * c2.y + c1.z * c2.z;
    }

    friend Vec4 cross (const Vec4 &c1, const Vec4 &c2) {
        /*if(c1.isPoint || c2.isPoint){
            cout << "Error: both operands must be directions.\n";
        }*/
        return Vec4 (c1.y * c2.z - c1.z * c2.y, 
                     c1.z * c2.x - c1.x * c2.z, 
                     c1.x * c2.y - c1.y * c2.x, 
                     0);
    }

    void applyMatrix (Matrix m) {
        Vec4 aux = Vec4(0, 0, 0, 0);
        aux.x = m.get(0, 0) * x + m.get(0, 1) * y + m.get(0, 2) * z + m.get(0, 3) * isPoint;
        aux.y = m.get(1, 0) * x + m.get(1, 1) * y + m.get(1, 2) * z + m.get(1, 3) * isPoint;
        aux.z = m.get(2, 0) * x + m.get(2, 1) * y + m.get(2, 2) * z + m.get(2, 3) * isPoint;
        aux.isPoint = m.get(3, 0) * x + m.get(3, 1) * y + m.get(3, 2) * z + m.get(3, 3) * isPoint;

        *this = aux;
    }

    void translate (double x, double y, double z) {
        Matrix m = Matrix();
        m.translationMatrix(x, y, z);
        this->applyMatrix(m);
    }

    void scale(double x, double y, double ){
        Matrix m = Matrix();
        m.scaleMatrix(x, y, z);
        this->applyMatrix(m);
    }

    void rotateX(double degrees)
    {
        Matrix m = Matrix();
        m.rotateXMatrix(degrees);
        this->applyMatrix(m);
    }

    void rotateY(double degrees)
    {
        Matrix m = Matrix();
        m.rotateYMatrix(degrees);
        this->applyMatrix(m);
    }

    void rotateZ(double degrees)
    {
        Matrix m = Matrix();
        m.rotateZMatrix(degrees);
        this->applyMatrix(m);
    }

    void baseChange (Vec4 o, Vec4 u, Vec4 v, Vec4 w) {
        /*if (!o.isPoint || u.isPoint || v.isPoint || w.isPoint) {
            cout << "Error.\n";
        }*/
        Matrix m = Matrix();
        m.set(0, 0, u.x);
        m.set(1, 0, u.y);
        m.set(2, 0, u.z);
        m.set(0, 1, v.x);
        m.set(1, 1, v.y);
        m.set(2, 1, v.z);
        m.set(0, 2, w.x);
        m.set(1, 2, w.y);
        m.set(2, 2, w.z);
        m.set(0, 3, o.x);
        m.set(1, 3, o.y);
        m.set(2, 3, o.z);
        m.set(3, 3, 1);
        
        this->applyMatrix(m);
    }

    void baseChangeInverted (Vec4 o, Vec4 u, Vec4 v, Vec4 w) {
        /*if (!o.isPoint || u.isPoint || v.isPoint || w.isPoint) {
            cout << "Error.\n";
        }*/
        Matrix m = Matrix();
        m.set(0, 0, u.x);
        m.set(1, 0, u.y);
        m.set(2, 0, u.z);
        m.set(0, 1, v.x);
        m.set(1, 1, v.y);
        m.set(2, 1, v.z);
        m.set(0, 2, w.x);
        m.set(1, 2, w.y);
        m.set(2, 2, w.z);
        m.set(0, 3, o.x);
        m.set(1, 3, o.y);
        m.set(2, 3, o.z);
        m.set(3, 3, 1);
        m.invertMatrix();

        //cout << m << endl;
        this->applyMatrix(m);
    }

    friend bool operator== (const Vec4 &c1, const Vec4 &c2) {
        return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z && c1.isPoint == c2.isPoint;
    }

    const float& operator[](int i) const {
        switch (i%3) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
        return x;
    }

    array<float, 3> asThree() const {
        return {x, y, z};
    }

    // devuelve el angulo entre dos direcciones en radianes
    friend double angleBetweenDirections (const Vec4 &c1, const Vec4 &c2) {
        /*if(c1.isPoint || c2.isPoint){
            cout << "Error: both operands must be directions.\n";
        }*/
        return acos(dot(c1, c2) / (c1.mod() * c2.mod()));
    }

    friend ostream &operator<< (ostream &o, const Vec4 &c){
        o << (c.isPoint?"(":"[") << c.x << ", " << c.y << ", " << c.z << (c.isPoint?")":"]");
        return o;
    }
};

class Point : public Vec4 {
public:
    Point () : Vec4(0, 0, 0, 1) {}
    Point (float x, float y, float z) : Vec4(x, y, z, 1) {}
    Point (Vec4 v) : Vec4(v.getX(), v.getY(), v.getZ(), 1) {}
    friend Vec4 toVec4 (Point p) {
        return Vec4(p.getX(), p.getY(), p.getZ(), 1);
    }
    friend istream &operator>> (istream &i, Point &p) {
        i >> p.x >> p.y >> p.z;
        return i;
    }
};

class Direction : public Vec4 {
public:
    Direction () : Vec4(0, 0, 0, 0) {}
    Direction (float x, float y, float z) : Vec4(x, y, z, 0) {}
    Direction (Vec4 v) : Vec4(v.getX(), v.getY(), v.getZ(), 0) {}
    friend Vec4 toVec4 (Direction d) {
        return Vec4(d.getX(), d.getY(), d.getZ(), 0);
    }
    Direction randomDirection() {
        /*double phi = randPhi2.get(), theta = acos(randTheta2.get());
        return Direction(sin(theta) * sin(phi), sin(theta) * cos(phi), cos(theta));*/
        double x = randTheta2.get(), y = randTheta2.get(), z = randTheta2.get();
        while (x*x + y*y + z*z > 1) {
            x = randTheta2.get();
            y = randTheta2.get();
            z = randTheta2.get();
        }
        return Direction(x, y, z).normalize();
    }
    friend istream &operator>> (istream &i, Direction &d) {
        i >> d.x >> d.y >> d.z;
        return i;
    }
};

#endif // VEC4_H