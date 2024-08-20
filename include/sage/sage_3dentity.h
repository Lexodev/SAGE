/**
 * sage_3dentity.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D entity management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
 */

#ifndef _SAGE_3DENTITY_H_
#define _SAGE_3DENTITY_H_

#include <exec/types.h>

#include <sage/sage_maths.h>
#include <sage/sage_3dstruct.h>

#define S3DE_MAX_ENTITIES     1024                  // Max entities in the world

#define S3DE_UNDEFINED        0                     // Undefined file type
#define S3DE_LWOB             1                     // Lightwave object file
#define S3DE_WFOB             2                     // Wavefront object file

#define S3DE_TEXT_NOCALC      0                     // Do not recalcul entity texture coordinates
#define S3DE_TEXT_RECALC      1                     // Recalcul entity texture coordinates (0.0 -> 1.0)

/** Entity definition */
typedef struct {
  WORD anglex, angley, anglez;
  FLOAT posx, posy, posz, radius;
  BOOL disabled, culled, clipped;
  UWORD nb_vertices, nb_faces, lod;
  SAGE_Vertex *vertices;
  SAGE_Face *faces;
  SAGE_Vector *normals;
} SAGE_Entity;

/** Create an empty entity */
SAGE_Entity *SAGE_CreateEntity(UWORD, UWORD);

/** Initialize an entity */
VOID SAGE_InitEntity(SAGE_Entity *);

/** Clone an entity */
SAGE_Entity *SAGE_CloneEntity(SAGE_Entity *);

/** Release an entity */
VOID SAGE_ReleaseEntity(SAGE_Entity *);

/** Load an entity from a file */
SAGE_Entity *SAGE_LoadEntity(STRPTR);

/** Calculate the entity radius */
VOID SAGE_SetEntityRadius(SAGE_Entity *);

/** Calculate entity faces normal */
VOID SAGE_SetEntityNormals(SAGE_Entity *);

/** Add an entity to the world */
BOOL SAGE_AddEntity(UWORD, SAGE_Entity *);

/** Remove entity from the world */
VOID SAGE_RemoveEntity(UWORD);

/** Remove all entities */
VOID SAGE_FlushEntities(VOID);

/** Get an entity from her index */
SAGE_Entity *SAGE_GetEntity(UWORD);

/** Set the entity angle */
BOOL SAGE_SetEntityAngle(UWORD, WORD, WORD, WORD);

/** Rotate the entity */
BOOL SAGE_RotateEntity(UWORD, WORD, WORD, WORD);

/** Set the entity position */
BOOL SAGE_SetEntityPosition(UWORD, FLOAT, FLOAT, FLOAT);

/** Move the entity */
BOOL SAGE_MoveEntity(UWORD, FLOAT, FLOAT, FLOAT);

/** Hide the entity */
BOOL SAGE_HideEntity(UWORD);

/** Show the entity */
BOOL SAGE_ShowEntity(UWORD);

/** Set the entity texture */
BOOL SAGE_SetEntityTexture(UWORD, UWORD, UWORD, UWORD);

#endif
