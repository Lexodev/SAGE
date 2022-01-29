/**
 * sage_3dengine.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D engine functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DENGINE_H_
#define _SAGE_3DENGINE_H_

#include <exec/types.h>

#include "sage_maths.h"
#include "sage_3dcamera.h"
#include "sage_3dentity.h"
#include "sage_3dmaterial.h"
#include "sage_3dskybox.h"

#define DEGTORAD(x)           ((x)*PI/180.0)

#define S3DE_PRECISION        4                     // Degree precision (1/precision by degree)
#define S3DE_ANGLE_90         90*S3DE_PRECISION     // 90 degree
#define S3DE_ANGLE_180        180*S3DE_PRECISION    // 180 degree
#define S3DE_ANGLE_270        270*S3DE_PRECISION    // 270 degree
#define S3DE_ANGLE_360        360*S3DE_PRECISION    // 360 degree

#define S3DE_MAX_VERTICES     256                   // Max entity vertices
#define S3DE_CLIP_VERTICES    2                     // Small space for clipped vertices
#define S3DE_VERTEX_CLIP1     S3DE_MAX_VERTICES     // First clipped vertex
#define S3DE_VERTEX_CLIP2     S3DE_MAX_VERTICES+1   // Second clipped vertex

#define S3DE_NOCLIP           0
#define S3DE_P1CLIP           1L<<0
#define S3DE_P2CLIP           1L<<1
#define S3DE_P3CLIP           1L<<2
#define S3DE_P4CLIP           1L<<3
#define S3DE_MASKP4           (S3DE_P1CLIP|S3DE_P2CLIP|S3DE_P3CLIP)
#define S3DE_MASKP2           (S3DE_P1CLIP|S3DE_P4CLIP|S3DE_P3CLIP)

/** Ordered entities */
typedef struct {
  SAGE_Entity * entity;
  FLOAT posz;
} SAGE_SortedEntity;

/** World structure */
typedef struct {
  LONGBITS flags;
  ULONG nb_materials;
  SAGE_Material * materials[S3DE_MAX_MATERIALS];
  SAGE_Entity * entities[S3DE_MAX_ENTITIES];
  ULONG visible_entities;
  SAGE_SortedEntity ordering[S3DE_MAX_ENTITIES];
  ULONG active_camera;
  SAGE_Camera * cameras[S3DE_MAX_CAMERAS];
  BOOL active_skybox;
  SAGE_Entity * skybox;
} SAGE_3DWorld;

/** Init the 3D engine */
BOOL SAGE_Init3DEngine(VOID);

/** Release the 3D engine */
VOID SAGE_Release3DEngine(VOID);

/** Render the 3D world */
VOID SAGE_RenderWorld(VOID);

#endif
