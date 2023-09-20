#pragma once
#ifndef MATRIX_H
#define DIM 4
#include <iostream>
#include <ostream>
#include <iomanip>
#include <cmath>
#include <math.h>
using namespace std;

class Matrix {
    double m[DIM][DIM];

public:
    Matrix () {
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++)
                m[i][j] = 0;
        }
    }

    Matrix (double m[DIM][DIM]) {
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++)
                this->m[i][j] = m[i][j];
        }
    }

    void translationMatrix(double x, double y, double z){
       for (int i = 0; i < DIM; i++)
                m[i][i] = 1;
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z; 
    }

    void scaleMatrix(double x, double y, double z){
        m[0][0] = x;
        m[1][1] = y;
        m[2][2] = z;
        m[3][3] = 1;
    }

    void rotateXMatrix(double angle){
        angle = angle * M_PI / 180;
        m[0][0] = 1;
        m[1][1] = cos(angle);
        m[1][2] = -sin(angle);
        m[2][1] = sin(angle);
        m[2][2] = cos(angle);
        m[3][3] = 1;
    }

    void rotateYMatrix(double angle){
        angle = angle * M_PI / 180;
        m[0][0] = cos(angle);
        m[0][2] = sin(angle);
        m[1][1] = 1;
        m[2][0] = -sin(angle);
        m[2][2] = cos(angle);
        m[3][3] = 1;
    }

    void rotateZMatrix(double angle){
        angle = angle * M_PI / 180;
        m[0][0] = cos(angle);
        m[0][1] = -sin(angle);
        m[1][0] = sin(angle);
        m[1][1] = cos(angle);
        m[2][2] = 1;
        m[3][3] = 1;
    }

    void invertMatrix () {
        double det = 0;
        for (int i = 0; i < DIM; i++) {
            det += m[0][i] * (m[1][(i+1)%DIM] * m[2][(i+2)%DIM] * m[3][(i+3)%DIM] - m[1][(i+3)%DIM] * m[2][(i+2)%DIM] * m[3][(i+1)%DIM]);
        }
        if (det == 0) {
            cout << "Error.\n";
            return;
        }
        Matrix inv = Matrix();
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++)
                inv.m[i][j] = (m[(j+1)%DIM][(i+1)%DIM] * m[(j+2)%DIM][(i+2)%DIM] * m[(j+3)%DIM][(i+3)%DIM] - m[(j+1)%DIM][(i+3)%DIM] * m[(j+2)%DIM][(i+2)%DIM] * m[(j+3)%DIM][(i+1)%DIM]) / det;
        }
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++)
                m[i][j] = inv.m[i][j];
        }
    }

    void imageOnes () {
        for (int i = 0; i < DIM; i++)
                m[i][i] = 1;
    }

    void set (int i, int j, double value) {
        m[i][j] = value;
    }

    double get (int i, int j) const {
        return m[i][j];
    }

    friend ostream &operator<< (ostream &o, const Matrix &m) {
        for (int i = 0; i < DIM; i++) {
            o << (i==0?"/ ":i==3?"\\ ":"| ");
            for (int j = 0; j < DIM; j++) {
                o << fixed << setprecision(2) << m.m[i][j] << " ";
            }
            o << (i==0?"\\\n":i==3?"/\n ":"|\n");
        }
        return o;
    }
};

#endif // MATRIX_H