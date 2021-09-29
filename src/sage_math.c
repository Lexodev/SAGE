/**
 * sage_math.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Mathematic functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 August 2021
 */

#include "sage_math.h"

FLOAT SAGE_DotProduct(SAGE_Vector * u, SAGE_Vector * v)
{
  // u*v = xu*xv + yu*yv + zu*zv
  return (FLOAT)((u->x*v->x)+(u->y*v->y)+(u->z*v->z));
}

VOID SAGE_CrossProduct(SAGE_Vector * res, SAGE_Vector * u, SAGE_Vector * v)
{
  // x = yu*zv - zu*yv
  res->x = u->y*v->z - u->z*v->y;
  // y = - xu*vz + zu*xv
  res->y = -(u->x*v->z - u->z*v->x);
  // z = xu*vy - yu*xv
  res->z = u->x*v->y - u->y*v->x;
}

VOID SAGE_Normalize(SAGE_Vector * vec)
{
  FLOAT vlen;

  vlen = sqrt((vec->x*vec->x) + (vec->y*vec->y) + (vec->z*vec->z));
  vec->x /= vlen;
  vec->y /= vlen;
  vec->z /= vlen;
}

VOID SAGE_VectorMatrix(SAGE_Vector * res,SAGE_Vector * vec,SAGE_Matrix * mat)
{
  res->x = vec->x*mat->m11 + vec->y*mat->m21 + vec->z*mat->m31;
  res->y = vec->x*mat->m12 + vec->y*mat->m22 + vec->z*mat->m32;
  res->z = vec->x*mat->m13 + vec->y*mat->m23 + vec->z*mat->m33;
}

VOID SAGE_VectorMatrix4(SAGE_Vector * res,SAGE_Vector * vec,SAGE_Matrix * mat)
{
  res->x = vec->x*mat->m11 + vec->y*mat->m21 + vec->z*mat->m31 + mat->m41;
  res->y = vec->x*mat->m12 + vec->y*mat->m22 + vec->z*mat->m32 + mat->m42;
  res->z = vec->x*mat->m13 + vec->y*mat->m23 + vec->z*mat->m33 + mat->m43;
}

VOID SAGE_ZeroMatrix(SAGE_Matrix * mat)
{
  mat->m11 = 0.0;
  mat->m12 = 0.0;
  mat->m13 = 0.0;
  mat->m14 = 0.0;
  mat->m21 = 0.0;
  mat->m22 = 0.0;
  mat->m23 = 0.0;
  mat->m24 = 0.0;
  mat->m31 = 0.0;
  mat->m32 = 0.0;
  mat->m33 = 0.0;
  mat->m34 = 0.0;
  mat->m41 = 0.0;
  mat->m42 = 0.0;
  mat->m43 = 0.0;
  mat->m44 = 0.0;
}

VOID SAGE_IdentityMatrix(SAGE_Matrix * mat)
{
  mat->m11 = 1.0;
  mat->m12 = 0.0;
  mat->m13 = 0.0;
  mat->m14 = 0.0;
  mat->m21 = 0.0;
  mat->m22 = 1.0;
  mat->m23 = 0.0;
  mat->m24 = 0.0;
  mat->m31 = 0.0;
  mat->m32 = 0.0;
  mat->m33 = 1.0;
  mat->m34 = 0.0;
  mat->m41 = 0.0;
  mat->m42 = 0.0;
  mat->m43 = 0.0;
  mat->m44 = 1.0;
}

VOID SAGE_MultiplyMatrix(SAGE_Matrix * res,SAGE_Matrix * m1,SAGE_Matrix * m2)
{
  res->m11 = (m1->m11*m2->m11) + (m1->m12*m2->m21) + (m1->m13*m2->m31) + (m1->m14*m2->m41);
  res->m12 = (m1->m11*m2->m12) + (m1->m12*m2->m22) + (m1->m13*m2->m32) + (m1->m14*m2->m42);
  res->m13 = (m1->m11*m2->m13) + (m1->m12*m2->m23) + (m1->m13*m2->m33) + (m1->m14*m2->m43);
  res->m14 = (m1->m11*m2->m14) + (m1->m12*m2->m24) + (m1->m13*m2->m34) + (m1->m14*m2->m44);
  res->m21 = (m1->m21*m2->m11) + (m1->m22*m2->m21) + (m1->m23*m2->m31) + (m1->m24*m2->m41);
  res->m22 = (m1->m21*m2->m12) + (m1->m22*m2->m22) + (m1->m23*m2->m32) + (m1->m24*m2->m42);
  res->m23 = (m1->m21*m2->m13) + (m1->m22*m2->m23) + (m1->m23*m2->m33) + (m1->m24*m2->m43);
  res->m24 = (m1->m21*m2->m14) + (m1->m22*m2->m24) + (m1->m23*m2->m34) + (m1->m24*m2->m44);
  res->m31 = (m1->m31*m2->m11) + (m1->m32*m2->m21) + (m1->m33*m2->m31) + (m1->m24*m2->m41);
  res->m32 = (m1->m31*m2->m12) + (m1->m32*m2->m22) + (m1->m33*m2->m32) + (m1->m24*m2->m42);
  res->m33 = (m1->m31*m2->m13) + (m1->m32*m2->m23) + (m1->m33*m2->m33) + (m1->m24*m2->m43);
  res->m34 = (m1->m31*m2->m14) + (m1->m32*m2->m24) + (m1->m33*m2->m34) + (m1->m24*m2->m44);
  res->m41 = (m1->m41*m2->m11) + (m1->m42*m2->m21) + (m1->m43*m2->m31) + (m1->m44*m2->m41);
  res->m42 = (m1->m41*m2->m12) + (m1->m42*m2->m22) + (m1->m43*m2->m32) + (m1->m44*m2->m42);
  res->m43 = (m1->m41*m2->m13) + (m1->m42*m2->m23) + (m1->m43*m2->m33) + (m1->m44*m2->m43);
  res->m44 = (m1->m41*m2->m14) + (m1->m42*m2->m24) + (m1->m43*m2->m34) + (m1->m44*m2->m44);
}
