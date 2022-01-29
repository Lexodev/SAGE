/**
 * sage_maths.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Mathematic functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_MATHS_H_
#define _SAGE_MATHS_H_

#include <exec/types.h>

/** SAGE vector */
typedef struct {
  FLOAT x, y, z;
} SAGE_Vector;

/** SAGE matrix 3x3 */
typedef struct {
  FLOAT m11, m12, m13;
  FLOAT m21, m22, m23;
  FLOAT m31, m32, m33;
} SAGE_Matrix;

/** SAGE vector 4 */
typedef struct {
  FLOAT x, y, z, w;
} SAGE_Vector4;

/** SAGE matrix 4x4 */
typedef struct {
  FLOAT m11, m12, m13, m14;
  FLOAT m21, m22, m23, m24;
  FLOAT m31, m32, m33, m34;
  FLOAT m41, m42, m43, m44;
} SAGE_Matrix4;

/** Calculate vector dot product */
FLOAT SAGE_DotProduct(SAGE_Vector *, SAGE_Vector *);

/** Calculate vector cross product */
VOID SAGE_CrossProduct(SAGE_Vector *, SAGE_Vector *, SAGE_Vector *);

/** Normalize a vector */
VOID SAGE_Normalize(SAGE_Vector *);

/** Multiply a vector by a matrix */
VOID SAGE_VectorMatrix(SAGE_Vector *, SAGE_Vector *, SAGE_Matrix *);

/** Set a matrix to zero */
VOID SAGE_ZeroMatrix(SAGE_Matrix *);

/** Set a matrix4 to zero */
VOID SAGE_ZeroMatrix4(SAGE_Matrix4 *);

/** Set a matrix to identity */
VOID SAGE_IdentityMatrix(SAGE_Matrix *);

/** Set a matrix4 to identity */
VOID SAGE_IdentityMatrix4(SAGE_Matrix4 *);

/** Multiply two matrix */
VOID SAGE_MultiplyMatrix(SAGE_Matrix *, SAGE_Matrix *, SAGE_Matrix *);

/** Multiply two matrix4 */
VOID SAGE_MultiplyMatrix4(SAGE_Matrix4 *, SAGE_Matrix4 *, SAGE_Matrix4 *);

#endif
