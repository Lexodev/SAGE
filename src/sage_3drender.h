/**
 * sage_3drender.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D rendering management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 10/03/2025)
 */

#ifndef _SAGE_3DRENDER_H_
#define _SAGE_3DRENDER_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>
#include <Maggie3D/Maggie3D.h>

#define S3DR_ZBUFFER          1
#define S3DR_PERSPECTIVE      2
#define S3DR_GOURAUD          4
#define S3DR_BILINEAR         8
#define S3DR_FOGGING          16

#define S3DR_RENDER_WIRE      0                     // Wireframe rendering
#define S3DR_RENDER_FLAT      1                     // Flat rendering
#define S3DR_RENDER_TEXT      2                     // Textured rendering

#define S3DR_ELEM_POINT       0                     // Point element
#define S3DR_ELEM_LINE        1                     // Line element
#define S3DR_ELEM_TRIANGLE    2                     // Triangle element
#define S3DR_ELEM_QUAD        3                     // Quad element

#define S3DR_MAX_ELEMENTS     8192                  // Maximum number of elements to render

typedef struct {
  FLOAT x1, y1, z1, u1, v1;
  FLOAT x2, y2, z2, u2, v2;
  FLOAT x3, y3, z3, u3, v3;
  FLOAT x4, y4, z4, u4, v4;
  WORD texture, type;
  ULONG color;
} SAGE_3DElement;

typedef struct {
  SAGE_3DElement *element;
  DOUBLE avgz;
} SAGE_SortedElement;

typedef struct {
  UWORD width, height, bpr, bpp;
  APTR buffer;
} SAGE_ZBuffer;

typedef struct {
  LONGBITS options;
  UWORD render_elements, render_mode;
  SAGE_3DElement s3d_elements[S3DR_MAX_ELEMENTS];
  SAGE_SortedElement ordered_elements[S3DR_MAX_ELEMENTS];
  SAGE_ZBuffer zbuffer;
} SAGE_Render;

/** Initialize the 3D renderer */
BOOL SAGE_Init3DRender(VOID);

/** Enable/disable Z buffer */
BOOL SAGE_EnableZBuffer(BOOL);

/** Enable/disable filtering */
BOOL SAGE_EnableFiltering(BOOL);

/** Tell if a render option is active */
BOOL SAGE_Get3DRenderOption(LONGBITS);

/** Set the rendering mode */
BOOL SAGE_Set3DRenderMode(UWORD);

/** Allocate Z buffer */
BOOL SAGE_AllocateZBuffer(VOID);

/** Release Z buffer */
VOID SAGE_ReleaseZBuffer(VOID);

/** Clear Z buffer */
BOOL SAGE_ClearZBuffer(VOID);

/** Add an element to the rendering queue */
BOOL SAGE_Push3DElement(SAGE_3DElement *);

/** Sort the elements in the rendering queue */
BOOL SAGE_Sort3DElements(BOOL);

/** Render all elements in the queue */
BOOL SAGE_Render3DElements(VOID);

#endif
