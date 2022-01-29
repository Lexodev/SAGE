/**
 * sage_3dskybox.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D skybox management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DSKYBOX_H_
#define _SAGE_3DSKYBOX_H_

#include <exec/types.h>

#define S3DE_SKYBOX_FRONT         0
#define S3DE_SKYBOX_BACK          1
#define S3DE_SKYBOX_LEFT          2
#define S3DE_SKYBOX_RIGHT         3
#define S3DE_SKYBOX_TOP           4
#define S3DE_SKYBOX_BOTTOM        5

/** Set the world skybox radius */
VOID SAGE_SetSkyboxRadius(FLOAT);

/** Set the world skybox textures */
VOID SAGE_SetSkyboxTextures(ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);

/** Enable/disable the world skybox */
VOID SAGE_EnableSkybox(BOOL);

#endif
