/**
 * sage_math.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Mathematic functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 August 2021
 */

#ifndef _SAGE_MATH_H_
#define _SAGE_MATH_H_

#include <math.h>
#include <exec/types.h>

/** SAGE 3D point */
typedef struct {
  FLOAT x, y, z;
} SAGE_Point;

/** SAGE 3D vector */
typedef struct {
  FLOAT x, y, z;
} SAGE_Vector;

/** SAGE 3D matrix */
typedef struct {
  FLOAT m11, m12, m13, m14;
  FLOAT m21, m22, m23, m24;
  FLOAT m31, m32, m33, m34;
  FLOAT m41, m42, m43, m44;
} SAGE_Matrix;

/** Calculate vector dot product */
FLOAT SAGE_DotProduct(SAGE_Vector *, SAGE_Vector *);

/** Calculate vector cross product */
VOID SAGE_CrossProduct(SAGE_Vector *, SAGE_Vector *, SAGE_Vector *);

/** Normalize a vector */
VOID SAGE_Normalize(SAGE_Vector *);

/** Multiply a vector by a matrix */
VOID SAGE_VectorMatrix(SAGE_Vector *, SAGE_Vector *, SAGE_Matrix *);

/** Multiply a vector bye a matrix with homogenous coord */
VOID SAGE_VectorMatrix4(SAGE_Vector *, SAGE_Vector *, SAGE_Matrix *);

/** Set a matrix to zero */
VOID SAGE_ZeroMatrix(SAGE_Matrix *);

/** Set a matrix to identity */
VOID SAGE_IdentityMatrix(SAGE_Matrix *);

/** Multiply two matrix */
VOID SAGE_MultiplyMatrix(SAGE_Matrix *, SAGE_Matrix *, SAGE_Matrix *);

#endif
