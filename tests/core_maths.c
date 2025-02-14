/**
 * core_logger.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test logger functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define NB_POINTS             9

SAGE_Vector points[NB_POINTS] = {
  { -10.0,10.0,-10.0 },
  { 10.0,10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { -10.0,-10.0,-10.0 },
  { -10.0,10.0,10.0 },
  { 10.0,10.0,10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 },
  { 0.0,15.0,0.0 }
};

SAGE_Vector trpoints[NB_POINTS];

void DumpPoints(VOID)
{
  WORD idx;
  
  SAGE_DebugLog("Dump transformed points");
  for (idx = 0; idx < NB_POINTS;idx++) {
    SAGE_DebugLog(" - point %d : %f, %f, %f", idx, trpoints[idx].x, trpoints[idx].y, trpoints[idx].z);
  }
  SAGE_DebugLog("---------------------------------------------------");
}

void TransformPoints(SAGE_Matrix * matrix, SAGE_Vector * points)
{
  WORD idx;
  FLOAT x, y, z;

  for (idx = 0; idx < NB_POINTS;idx++) {
    x = points[idx].x;
    y = points[idx].y;
    z = points[idx].z;
    trpoints[idx].x = x*matrix->m11 + y*matrix->m21 + z*matrix->m31;
    trpoints[idx].y = x*matrix->m12 + y*matrix->m22 + z*matrix->m32;
    trpoints[idx].z = x*matrix->m13 + y*matrix->m23 + z*matrix->m33;
  }
  DumpPoints();
}

void RotateX(WORD ax)
{
  SAGE_Matrix rx;

  SAGE_IdentityMatrix(&rx);
  rx.m22 = SAGE_FastCosine(ax);
  rx.m23 = SAGE_FastSine(ax);
  rx.m32 = -SAGE_FastSine(ax);
  rx.m33 = SAGE_FastCosine(ax);
  TransformPoints(&rx, points);
}

void RotateY(WORD ay)
{
  SAGE_Matrix ry;

  SAGE_IdentityMatrix(&ry);
  ry.m11 = SAGE_FastCosine(ay);
  ry.m13 = SAGE_FastSine(ay);
  ry.m31 = -SAGE_FastSine(ay);
  ry.m33 = SAGE_FastCosine(ay);
  TransformPoints(&ry, trpoints);
}

void RotateXY(WORD ax, WORD ay)
{
  SAGE_Matrix rx, ry, rxy;

  SAGE_IdentityMatrix(&rx);
  rx.m22 = SAGE_FastCosine(ax);
  rx.m23 = SAGE_FastSine(ax);
  rx.m32 = -SAGE_FastSine(ax);
  rx.m33 = SAGE_FastCosine(ax);
  SAGE_IdentityMatrix(&ry);
  ry.m11 = SAGE_FastCosine(ay);
  ry.m13 = SAGE_FastSine(ay);
  ry.m31 = -SAGE_FastSine(ay);
  ry.m33 = SAGE_FastCosine(ay);
  SAGE_MultiplyMatrix(&rxy, &rx, &ry);
  TransformPoints(&rxy, points);
}

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (MATHS) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    SAGE_AppliLog("Rotate points on X");
    RotateX(15);
    SAGE_AppliLog("Rotate points on Y");
    RotateY(42);
    SAGE_AppliLog("Rotate points on X&Y");
    RotateXY(15, 42);
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
