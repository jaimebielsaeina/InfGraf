#ifndef MATRIX_H
#define MATRIX_H
#pragma once

#define DIM 4

#include <iostream>
#include <ostream>
#include <iomanip>
#include <cmath>
#include <math.h>
#include "vec4.h"
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
    
    // Función para intercambiar dos filas de una matriz
    void swapRows(Matrix matrix, int row1, int row2) {
        for (int i = 0; i < DIM; i++) {
            std::swap(matrix.m[row1][i], matrix.m[row2][i]);
        }
    }

    // Función para invertir una matriz 4x4 utilizando el método de Gauss-Jordan
    bool invertMatrix() {
        Matrix identity = Matrix();
        for (int i = 0; i < DIM; i++) {
            identity.m[i][i] = 1.0;
        }

        for (int i = 0; i < DIM; i++) {
            // Buscar el elemento pivote en la columna i
            int pivotRow = i;
            for (int j = i + 1; j < DIM; j++) {
                if (std::abs(m[j][i]) > std::abs(m[pivotRow][i])) {
                    pivotRow = j;
                }
            }

            // Intercambiar filas si es necesario
            if (pivotRow != i) {
                swapRows(m, i, pivotRow);
                swapRows(identity, i, pivotRow);
            }

            // Hacer que el elemento diagonal sea 1
            double pivot = m[i][i];
            if (pivot == 0.0) {
                std::cout << "La matriz no es invertible." << std::endl;
                return false;
            }

            for (int j = 0; j < DIM; j++) {
                m[i][j] /= pivot;
                identity.m[i][j] /= pivot;
            }

            // Eliminar elementos no nulos en la columna i
            for (int j = 0; j < DIM; j++) {
                if (j != i) {
                    double factor = m[j][i];
                    for (int k = 0; k < DIM; k++) {
                        m[j][k] -= factor * m[i][k];
                        identity.m[j][k] -= factor * identity.m[i][k];
                    }
                }
            }
        }

        // La matriz original ahora contiene la matriz identidad y la inversa en 'identity'
        for(int i = 0; i < DIM; i++){
            for(int j = 0; j < DIM; j++){
                m[i][j] = identity.m[i][j];
            }
        }
        return true;
    }
    /*
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
    */

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