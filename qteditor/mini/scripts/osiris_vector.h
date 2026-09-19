/*
* Descent 3
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OSIRIS_VECTOR_H
#define OSIRIS_VECTOR_H

#include "fix.h"
#include "vecmat_external.h"

// Disable the "possible loss of data" warning
#pragma warning(disable : 4244)

// Used for debugging.  It is used in printf's so we do not have to write out the structure 3 times
// to print all the coordinates.
#define XYZ(v) (v)->x(), (v)->y(), (v)->z()
#define HPB(v) (v)->h(), (v)->p(), (v)->b()

extern const matrix Identity_matrix;

// Given a matrix, makes it an identity matrix
extern void vm_MakeIdentity(matrix *);

// Set a vector to {0,0,0}
extern void vm_MakeZero(vector3 *v);

// Set an angvec to {0,0,0}
extern void vm_MakeZero(angvec *a);

// Rotates a vector thru a matrix
extern void vm_MatrixMulVector(vector3 *, const vector3 *, const matrix *);

// Multiply a vector times the transpose of a matrix
void vm_VectorMulTMatrix(vector3 *result, const vector3 *v, const matrix *m);

// Multiplies 2 3x3 matrixes, returning the result in first argument
extern void vm_MatrixMul(matrix *, const matrix *, const matrix *);

// Multiply a matrix times the transpose of a matrix
void vm_MatrixMulTMatrix(matrix *dest, const matrix *src0, const matrix *src1);

// Given a vector, returns the magnitude.  Uses sqrt so it's slow
extern scalar vm_GetMagnitude(const vector3 *);

// Given a vector, returns an approximation of the magnitude
extern scalar vm_GetMagnitudeFast(const vector3 *);

// Returns the dot product of the two given vectors
extern scalar vm_DotProduct(const vector3 *, const vector3 *);

// Returns a perpendicular vector to the two given vectors
extern void vm_CrossProduct(vector3 *, const vector3 *, const vector3 *);

// Returns the difference between two vectors
extern void vm_SubVectors(vector3 *, const vector3 *, const vector3 *);

// Returns adds two vectors, returns result in first arg
extern void vm_AddVectors(vector3 *, const vector3 *, const vector3 *);

// Inits vector to 0,0,0
extern void vm_CenterVector(vector3 *);

// Given a vector, divides second arg by vector components
extern void vm_AverageVector(vector3 *, int);

// Normalizes a vector
// Returns the magnitude before normalization
extern scalar vm_VectorNormalize(vector3 *);

// Scales second arg vector by 3rd arg, placing result in first arg
extern void vm_ScaleVector(vector3 *, const vector3 *, const scalar);

// Scales all components of vector v by value s adds the result to p and stores result in vector d
extern void vm_ScaleAddVector(vector3 *d, const vector3 *p, const vector3 *v, const scalar s);

// Divides second vector components by 3rd arg, placing result in first arg.  Useful for parametric lines
extern void vm_DivVector(vector3 *, const vector3 *, const scalar);

// Same as VectorNormalize, but uses approximation
extern scalar vm_VectorNormalizeFast(vector3 *);

// Clears a matrix to zero
extern void vm_ClearMatrix(matrix *);

// Transposes a matrix in place
extern void vm_TransposeMatrix(matrix *);

// Given 3 angles (p,h,b), makes a rotation matrix out of them
extern void vm_AnglesToMatrix(matrix *, const angle p, const angle h, const angle b);

// Ensure that a matrix is orthogonal
void vm_Orthogonalize(matrix *m);

// Compute a matrix from one or two vectors.  At least one and at most two vectors must/can be specified.
// Parameters:	m - filled in with the orienation matrix
//					fvec,uvec,rvec - pointers to vectors that determine the matrix.
//						One or two of these must be specified, with the other(s) set to NULL.
void vm_VectorToMatrix(matrix *m, vector3 *fvec, vector3 *uvec = NULL, vector3 *rvec = NULL);

// Computes a matrix from a vector and and angle of rotation around that vector
// Parameters:	m - filled in with the computed matrix
//					v - the forward vector of the new matrix
//					a - the angle of rotation around the forward vector
void vm_VectorAngleToMatrix(matrix *m, vector3 *v, angle a);

// Given an angle, places sin in 2nd arg, cos in 3rd.  Either can be null
extern void vm_SinCos(angle, scalar *, scalar *);

// Given x1,y1,x2,y2, returns the slope
extern scalar vm_GetSlope(scalar, scalar, scalar, scalar);

// Calculates the perpendicular vector given three points
// Parms:	n - the computed perp vector (filled in)
//			v0,v1,v2 - three clockwise vertices
void vm_GetPerp(vector3 *n, const vector3 *a, const vector3 *b, const vector3 *c);

// Calculates the (normalized) surface normal give three points
// Parms:	n - the computed surface normal (filled in)
//			v0,v1,v2 - three clockwise vertices
// Returns the magnitude of the normal before it was normalized.
// The bigger this value, the better the normal.
scalar vm_GetNormal(vector3 *n, const vector3 *v0, const vector3 *v1, const vector3 *v2);

#define vm_GetSurfaceNormal vm_GetNormal

// Gets the distances (magnitude) between two vectors. Slow.
extern scalar vm_VectorDistance(const vector3 *a, const vector3 *b);

// Gets the approx distances (magnitude) between two vectors. Faster.
extern scalar vm_VectorDistanceQuick(const vector3 *a, const vector3 *b);

// Computes a normalized direction vector between two points
// Parameters:	dest - filled in with the normalized direction vector
//					start,end - the start and end points used to calculate the vector
// Returns:		the distance between the two input points
scalar vm_GetNormalizedDir(vector3 *dest, const vector3 *end, const vector3 *start);

// Returns a normalized direction vector between two points
// Uses sloppier magnitude, less precise
scalar vm_GetNormalizedDirFast(vector3 *dest, const vector3 *end, const vector3 *start);

// extract angles from a matrix
angvec *vm_ExtractAnglesFromMatrix(angvec *a, const matrix *m);

//	returns the angle between two vectors and a forward vector
angle vm_DeltaAngVec(const vector3 *v0, const vector3 *v1, const vector3 *fvec);

//	returns the angle between two normalized vectors and a forward vector
angle vm_DeltaAngVecNorm(const vector3 *v0, const vector3 *v1, const vector3 *fvec);

// Computes the distance from a point to a plane.
// Parms:	checkp - the point to check
// Parms:	norm - the (normalized) surface normal of the plane
//				planep - a point on the plane
// Returns:	The signed distance from the plane; negative dist is on the back of the plane
scalar vm_DistToPlane(const vector3 *checkp, const vector3 *norm, const vector3 *planep);

// returns the value of a determinant
scalar calc_det_value(const matrix *det);

void vm_MakeInverseMatrix(matrix *dest);
void vm_SinCosToMatrix(matrix *m, scalar sinp, scalar cosp, scalar sinb, scalar cosb, scalar sinh, scalar cosh);

// Gets the real center of a polygon
scalar vm_GetCentroid(vector3 *centroid, const vector3 *src, int nv);

//	retrieves a random vector in values -RAND_MAX/2 to RAND_MAX/2
void vm_MakeRandomVector(vector3 *vec);

// Given a set of points, computes the minimum bounding sphere of those points
scalar vm_ComputeBoundingSphere(vector3 *center, const vector3 *vecs, int num_verts);

// Gets the real center of a polygon, but uses fast magnitude calculation
// Returns the size of the passed in stuff
scalar vm_GetCentroidFast(vector3 *centroid, const vector3 *src, int nv);

// Here are the C++ operator overloads -- they do as expected
extern matrix operator*(const matrix &src0, const matrix &src1);
extern matrix operator*=(matrix &src0, const matrix &src1);

#endif
