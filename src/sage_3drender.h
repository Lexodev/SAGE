/**
 * sage_3drender.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D rendering management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2021
 */

#ifndef _SAGE_3DRENDER_H_
#define _SAGE_3DRENDER_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>

#define S3DR_W3DMODE          1
#define S3DR_S3DMODE          2

#define S3DR_MAX_TRIANGLES    1024

typedef struct {
  FLOAT x1, y1, u1, v1;
  DOUBLE z1;
  FLOAT x2, y2, u2, v2;
  DOUBLE z2;
  FLOAT x3, y3, u3, v3;
  DOUBLE z3;
  UWORD texture;
} SAGE_3DTriangle;

typedef struct {
  SAGE_3DTriangle * triangle;
  DOUBLE avgz;
} SAGE_SortedTriangle;

/** DEBUG !!! */
VOID SAGE_Dump3DTriangle(SAGE_3DTriangle *);

/** Initialize the 3D renderer */
BOOL SAGE_Init3DRender(VOID);

/** Define the rendering mode */
BOOL SAGE_Set3DRenderMode(UWORD);

/** Add a triangle to the rendering queue */
BOOL SAGE_Push3DTriangle(SAGE_3DTriangle *);

/** Sort the triangles in the rendering queue */
VOID SAGE_Sort3DTriangles(VOID);

/** Render all triangles in the queue */
BOOL SAGE_Render3DTriangles(VOID);

#endif
