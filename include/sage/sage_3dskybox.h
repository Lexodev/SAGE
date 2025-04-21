/**
 * sage_3dskybox.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D skybox management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#ifndef _SAGE_3DSKYBOX_H_
#define _SAGE_3DSKYBOX_H_

#include <exec/types.h>

#include <sage/sage_maths.h>
#include <sage/sage_3dstruct.h>

#define S3DE_SKYBOX_VERTICES      (9*6)+(5*4)+(3*8)
#define S3DE_SKYBOX_FACEBYPLANE   16
#define S3DE_SKYBOX_PLANES        6
#define S3DE_SKYBOX_PLANEEDGES    4

#define S3DE_SKYBOX_FRONT         0
#define S3DE_SKYBOX_BACK          1
#define S3DE_SKYBOX_LEFT          2
#define S3DE_SKYBOX_RIGHT         3
#define S3DE_SKYBOX_TOP           4
#define S3DE_SKYBOX_BOTTOM        5

/** Skybox definition */
typedef struct {
  BOOL culled;
  UWORD edges[S3DE_SKYBOX_PLANEEDGES];
  SAGE_Face *faces;
} SAGE_SkyboxPlane;

typedef struct {
  SAGE_Vertex *vertices;
  ULONG textures[S3DE_SKYBOX_PLANES];
  SAGE_SkyboxPlane *planes;
} SAGE_Skybox;

/** Set the skybox textures */
VOID SAGE_SetSkyboxTextures(ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);

/** Enable/disable the skybox */
VOID SAGE_EnableSkybox(BOOL);

#endif
