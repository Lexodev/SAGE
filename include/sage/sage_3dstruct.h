/**
 * sage_3dstruct.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D base structures
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#ifndef _SAGE_3DSTRUCT_H_
#define _SAGE_3DSTRUCT_H_

#include <exec/types.h>

/** Vertex definition */
typedef struct {
  FLOAT x, y, z;
} SAGE_Vertex;

/** Transformed vertex definition */
typedef struct {
  BOOL calculated;            // Vertex has been calculated
  BOOL visible;               // Vertex is on a visible face
  FLOAT wx, wy, wz;           // World coordinates
  FLOAT cx, cy, cz;           // Camera coordinates
  FLOAT px, py, pz;           // Projected coordinates
  FLOAT iz;                   // Z inverse (for z-buffer)
} SAGE_TransformedVertex;

/** Face definiton */
typedef struct {
  BOOL is_quad, culled;
  WORD clipped, texture;      // texture = -1 mean use only color
  UWORD p1, p2, p3, p4;
  ULONG color;
  FLOAT u1, v1, u2, v2, u3, v3, u4, v4;
} SAGE_Face;

#endif
