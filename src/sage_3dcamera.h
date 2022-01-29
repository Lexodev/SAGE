/**
 * sage_3dcamera.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D camera management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DCAMERA_H_
#define _SAGE_3DCAMERA_H_

#include <exec/types.h>

#include "sage_maths.h"

#define S3DE_MAX_CAMERAS      4                     // Max cameras in the world
#define S3DE_MIN_DISTANCE     10.0                  // Minimun distance before culling entity
#define S3DE_MAX_DISTANCE     500.0                 // Maximum distance before culling entity
#define S3DE_FOV              90                    // Field of view

/** Camera definition */
typedef struct {
  WORD anglex, angley, anglez, fov;
  FLOAT posx, posy, posz;
  FLOAT near_plane, far_plane;
  FLOAT view_dist, centerx, centery;
  LONG view_left, view_top, view_width, view_height;
} SAGE_Camera;

/** Add a camera to the world */
BOOL SAGE_AddCamera(ULONG, LONG, LONG, LONG, LONG);

/** Remove the camera from the world */
VOID SAGE_RemoveCamera(ULONG);

/** Remove all cameras */
VOID SAGE_FlushCameras(VOID);

/** Get the camera from her index */
SAGE_Camera * SAGE_GetCamera(ULONG);

/** Set the active camera */
BOOL SAGE_SetActiveCamera(ULONG);

/** Set the camera angles */
BOOL SAGE_SetCameraAngle(ULONG, WORD, WORD, WORD);

/** Rotate the camera */
BOOL SAGE_RotateCamera(ULONG, WORD, WORD, WORD);

/** Set the camera position */
BOOL SAGE_SetCameraPosition(ULONG, FLOAT, FLOAT, FLOAT);

/** Move the camera */
BOOL SAGE_MoveCamera(ULONG, FLOAT, FLOAT, FLOAT);

/** Set the camera view */
BOOL SAGE_SetCameraView(ULONG, LONG, LONG, LONG, LONG, WORD);

/** Set the camera planes */
BOOL SAGE_SetCameraPlane(ULONG, FLOAT, FLOAT);

#endif
