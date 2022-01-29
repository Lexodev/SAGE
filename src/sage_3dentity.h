/**
 * sage_3dentity.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D entity management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DENTITY_H_
#define _SAGE_3DENTITY_H_

#include <exec/types.h>

#include "sage_maths.h"

#define S3DE_MAX_ENTITIES     64                    // Max entities in the world

#define S3DE_RENDER_WIRE      0                     // Wireframe entity rendering
#define S3DE_RENDER_FLAT      1                     // Flat entity rendering
#define S3DE_RENDER_TEXT      2                     // Textured entity rendering

#define S3DE_UNDEFINED        0                     // Undefined file type
#define S3DE_LWOB             1                     // Lightwave object file

/** Entity definition */
typedef struct {
  FLOAT x, y ,z;
} SAGE_EntityVertex;

typedef struct {
  BOOL is_quad, culled;
  WORD clipped;
  ULONG p1, p2, p3, p4, color, texture;
  FLOAT u1, v1, u2, v2, u3, v3, u4, v4;
  SAGE_Vector normal;
} SAGE_EntityFace;

typedef struct {
  WORD anglex, angley, anglez;
  FLOAT posx, posy, posz, radius;
  BOOL disabled, clipped;
  UWORD nb_vertices, nb_faces, rendering;
  SAGE_EntityVertex * vertices;
  SAGE_EntityVertex * trans_vertices;
  SAGE_EntityFace * faces;
} SAGE_Entity;

/** DEBUG */
VOID SAGE_DumpEntity(SAGE_Entity *);

/** Create an empty entity */
SAGE_Entity * SAGE_CreateEntity(WORD, WORD);

/** Release an entity */
VOID SAGE_ReleaseEntity(SAGE_Entity *);

/** Load an entity from a file */
SAGE_Entity * SAGE_LoadEntity(STRPTR);

/** Calculate the entity radius */
VOID SAGE_SetEntityRadius(SAGE_Entity *);

/** Calculate entity faces normal */
VOID SAGE_SetEntityNormals(SAGE_Entity *);

/** Add an entity to the world */
BOOL SAGE_AddEntity(ULONG, SAGE_Entity *);

/** Remove entity from the world */
VOID SAGE_RemoveEntity(ULONG);

/** Remove all entities */
VOID SAGE_FlushEntities(VOID);

/** Get an entity from her index */
SAGE_Entity * SAGE_GetEntity(ULONG);

/** Set the entity angle */
BOOL SAGE_SetEntityAngle(ULONG, WORD, WORD, WORD);

/** Rotate the entity */
BOOL SAGE_RotateEntity(ULONG, WORD, WORD, WORD);

/** Set the entity position */
BOOL SAGE_SetEntityPosition(ULONG, FLOAT, FLOAT, FLOAT);

/** Move the entity */
BOOL SAGE_MoveEntity(ULONG, FLOAT, FLOAT, FLOAT);

/** Set the entity rendering mode */
BOOL SAGE_SetEntityRenderMode(ULONG, UWORD);

#endif
