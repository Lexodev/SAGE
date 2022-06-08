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

#define S3DR_ZBUFFER          1
#define S3DR_PERSPECTIVE      2
#define S3DR_GOURAUD          4
#define S3DR_BILINEAR         8
#define S3DR_FOGGING          16

#define S3DR_RENDER_WIRE      0                     // Wireframe rendering
#define S3DR_RENDER_FLAT      1                     // Flat rendering
#define S3DR_RENDER_TEXT      2                     // Textured rendering

#define S3DR_MAX_TRIANGLES    8192                  // Maximum number of triangles to render

typedef struct {
  FLOAT x1, y1, u1, v1;
  DOUBLE z1;
  FLOAT x2, y2, u2, v2;
  DOUBLE z2;
  FLOAT x3, y3, u3, v3;
  DOUBLE z3;
  WORD texture;
  ULONG color;
} SAGE_3DTriangle;

typedef struct {
  SAGE_3DTriangle * triangle;
  DOUBLE avgz;
} SAGE_SortedTriangle;

typedef struct {
  LONGBITS options;
  UWORD render_triangles, render_mode;
  SAGE_3DTriangle s3d_triangles[S3DR_MAX_TRIANGLES];
  SAGE_SortedTriangle ordered_triangles[S3DR_MAX_TRIANGLES];
} SAGE_Render;

/** DEBUG */
VOID SAGE_Dump3DTriangle(SAGE_3DTriangle *);
/** DEBUG */

/** Initialize the 3D renderer */
BOOL SAGE_Init3DRender(VOID);

/** Enable/disable Z buffer */
BOOL SAGE_EnableZBuffer(BOOL);

/** Tell if a render option is active */
BOOL SAGE_Get3DRenderOption(LONGBITS);

/** Set the rendering mode */
BOOL SAGE_Set3DRenderMode(UWORD);

/** Add a triangle to the rendering queue */
BOOL SAGE_Push3DTriangle(SAGE_3DTriangle *);

/** Sort the triangles in the rendering queue */
BOOL SAGE_Sort3DTriangles(VOID);

/** Render all triangles in the queue */
BOOL SAGE_Render3DTriangles(VOID);

#endif
