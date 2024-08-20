/**
 * sage_loadobj.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Wavefront object loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#ifndef _SAGE_LOADOBJ_H_
#define _SAGE_LOADOBJ_H_

#include <exec/types.h>
#include <dos/dos.h>

#include <sage/sage_3dentity.h>

#define S3DE_OBJZOOM          1.0
#define S3DE_OBJNOMATERIAL    -1

/** OBJ structures */

typedef struct {
  FLOAT x, y, z;
} SAGE_OBJVertice;

typedef struct {
  FLOAT u, v;
} SAGE_OBJVerticeTexture;

typedef struct {
  BOOL is_quad;
  WORD p1, p2, p3, p4;
  WORD t1, t2, t3, t4;
  WORD material;
} SAGE_OBJFace;

typedef struct {
  UBYTE name[256];
  UBYTE file[256];
  ULONG color;
  WORD texture;
  BOOL transparent;
  ULONG tcolor;
} SAGE_OBJMaterial;

typedef struct {
  UBYTE filedir[256];
  WORD nb_vertices;
  SAGE_OBJVertice *vertices;
  WORD nb_vertexts;
  SAGE_OBJVerticeTexture *vertexts;
  WORD nb_normals;
  SAGE_OBJVertice *normals;
  WORD nb_faces;
  SAGE_OBJFace *faces;
  UBYTE matlib[256];
  WORD nb_materials;
  SAGE_OBJMaterial * materials;
} SAGE_WavefrontObject;

/** Load a OBJ file */
SAGE_Entity *SAGE_LoadOBJ(BPTR, STRPTR);

#endif
