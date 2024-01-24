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

// Class that represents a transformation matrix.
class Matrix {

	// Matrix represented as a 4x4 array.
	double m[DIM][DIM];

public:

	// Constructor.
	Matrix () {
		for (int i = 0; i < DIM; i++) {
			for (int j = 0; j < DIM; j++)
				m[i][j] = 0;
		}
	}

	// Constructor.
	Matrix (double m[DIM][DIM]) {
		for (int i = 0; i < DIM; i++) {
			for (int j = 0; j < DIM; j++)
				this->m[i][j] = m[i][j];
		}
	}

	// Converts the transformation matrix to a translation matrix.
	void translationMatrix(double x, double y, double z){
	   for (int i = 0; i < DIM; i++)
				m[i][i] = 1;
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
	}

	// Converts the transformation matrix to a scale matrix.
	void scaleMatrix(double x, double y, double z){
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
		m[3][3] = 1;
	}

	// Converts the transformation matrix to a rotation matrix around the X
	// axis.
	void rotateXMatrix(double angle){
		angle = angle * M_PI / 180;
		m[0][0] = 1;
		m[1][1] = cos(angle);
		m[1][2] = -sin(angle);
		m[2][1] = sin(angle);
		m[2][2] = cos(angle);
		m[3][3] = 1;
	}

	// Converts the transformation matrix to a rotation matrix around the Y
	// axis.
	void rotateYMatrix(double angle){
		angle = angle * M_PI / 180;
		m[0][0] = cos(angle);
		m[0][2] = sin(angle);
		m[1][1] = 1;
		m[2][0] = -sin(angle);
		m[2][2] = cos(angle);
		m[3][3] = 1;
	}

	// Converts the transformation matrix to a rotation matrix around the Z
	// axis.
	void rotateZMatrix(double angle){
		angle = angle * M_PI / 180;
		m[0][0] = cos(angle);
		m[0][1] = -sin(angle);
		m[1][0] = sin(angle);
		m[1][1] = cos(angle);
		m[2][2] = 1;
		m[3][3] = 1;
	}
	
	// Swap two rows in a matrix
	void swapRows(Matrix matrix, int row1, int row2) {
		for (int i = 0; i < DIM; i++) {
			swap(matrix.m[row1][i], matrix.m[row2][i]);
		}
	}

	// Invert the matrix using Gauss-Jordan method.
	bool invertMatrix() {
		Matrix identity = Matrix();
		for (int i = 0; i < DIM; i++) {
			identity.m[i][i] = 1.0;
		}

		for (int i = 0; i < DIM; i++) {
			// Search for the pivot element in column i.
			int pivotRow = i;
			for (int j = i + 1; j < DIM; j++) {
				if (abs(m[j][i]) > abs(m[pivotRow][i])) {
					pivotRow = j;
				}
			}

			// Swap rows if needed.
			if (pivotRow != i) {
				swapRows(m, i, pivotRow);
				swapRows(identity, i, pivotRow);
			}

			// Diagonal element will be 1.
			double pivot = m[i][i];
			if (pivot == 0.0) {
				cout << "Matrix cannt be inverted." << endl;
				return false;
			}

			for (int j = 0; j < DIM; j++) {
				m[i][j] /= pivot;
				identity.m[i][j] /= pivot;
			}

			// Delete non-zero elements in column i.
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

		// The original matrix now contains the identity matrix and the inverse
		// in 'identity'.
		for(int i = 0; i < DIM; i++){
			for(int j = 0; j < DIM; j++){
				m[i][j] = identity.m[i][j];
			}
		}
		return true;

	}

	// Converts the transformation matrix to a image matrix.
	void imageOnes () {
		for (int i = 0; i < DIM; i++)
				m[i][i] = 1;
	}

	// Sets the value of the matrix at position (i, j).
	void set (int i, int j, double value) {
		m[i][j] = value;
	}

	// Gets the value of the matrix at position (i, j).
	double get (int i, int j) const {
		return m[i][j];
	}

	// Writes the matrix to the output stream.
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