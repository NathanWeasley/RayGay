/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright 2003 by Jesper Christensen <jesper@kalliope.org>
 *
 * Added Thu Apr 17 2003
 */

#ifndef MATRIX_H
#define MATRIX_H

#include "math/vector.h"
#include <iosfwd>

/// Some basic linear algebra
class Matrix {

    friend std::ostream &operator<<(std::ostream &os,const Matrix &m);

    public:
        /// The default constructor creates an identity
	Matrix();

        /// Reset matrix to the identity
	void identity();

        /// Same as reset()
	void reset();

	/// Fills matrix with 0's
	void clear();
	
	/// Says whether this matrix is the identity
	bool isIdentity() const;
	
        /// Returns the inverse to this matrix
	Matrix inverse() const;

	/// Returns a matrix with the translation part stripped
	Matrix extractRotation() const;

	/// Multiply this matrix with a vector.
	Vector operator*(const Vector & v) const;

	/// Multiply with another Matrix
	Matrix operator*(const Matrix &) const;
	
	/// Multiply with another Matrix
	Matrix &operator*=(const Matrix &);

	/// Comparator
	bool operator==(const Matrix &m) const;

	/// Comparator
	bool operator!=(const Matrix &m) const;

	/// Rotate angle degrees around axis
	static Matrix matrixRotate(const Vector axis, const double angle);

	/// Rotate around the three axises
	static Matrix matrixRotate(const Vector angles);

	/// Translate along a vector
	static Matrix matrixTranslate(const Vector trans);
	
	/// Scale
	static Matrix matrixScale(const Vector& scale);

	/// Rotate v onto the positive z-axis
	static Matrix matrixOrient(const Vector& v);

	/// Orientation transformation matrix
	static Matrix matrixOrient(const Vector &x,const Vector &y,const Vector &z);
	
	/// Orientation transformation matrix
        static Matrix matrixOrient(const Vector &direction,const Vector &up);

	inline void set(const int col,const int row,const double val) {
	    _matrix[col*4+row] = val;
	}

    private:
	double _matrix[16];
	static double _identity[16];

	static void invertMatrixGeneral(const double *in, double *out);


	inline double get(const int col,const int row) const {
	    return _matrix[col*4+row];
	}

	inline double &element(const int col,const int row) {
	    return _matrix[col*4+row];
	}
};

#endif
