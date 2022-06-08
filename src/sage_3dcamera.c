/**
 * sage_3dcamera.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D camera management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#include <string.h>
#include <math.h>

#include <proto/exec.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_3dcamera.h"
#include "sage_3dengine.h"

/** Engine data */
extern SAGE_3DWorld sage_world;

/*****************************************************************************/
//            DEBUG ONLY
/*****************************************************************************/

VOID SAGE_DumpCamera(SAGE_Camera * camera)
{
  SAGE_DebugLog("** Dump camera **");
  if (camera == NULL) {
    SAGE_DebugLog("camera is NULL !");
    return;
  }
  SAGE_DebugLog(" => ax=%d  ay=%d  az=%d  fov=%f", camera->anglex, camera->angley, camera->anglez, camera->fov);
  SAGE_DebugLog(" => px=%f  py=%f  pz=%f", camera->posx, camera->posy, camera->posz);
  SAGE_DebugLog(" => near_plane=%f  far_plane=%f", camera->near_plane, camera->far_plane);
  SAGE_DebugLog(" => view_dist=%f  centerx=%f  centery=%f", camera->view_dist, camera->centerx, camera->centery);
  SAGE_DebugLog(" => view_left=%d  view_top=%d  view_width=%d  view_height=%d", camera->view_left, camera->view_top, camera->view_width, camera->view_height);
}

/*****************************************************************************/

/**
 * Add a camera to the world
 */
BOOL SAGE_AddCamera(ULONG index, LONG left, LONG top, LONG width, LONG height)
{
  SAGE_Camera * camera;
  
  SD(SAGE_DebugLog("Add camera #%d", index));
  if (index >= S3DE_MAX_CAMERAS) {
    SAGE_SetError(SERR_CAMERA_INDEX);
    return FALSE;
  }
  // Clean the place
  SAGE_RemoveCamera(index);
  camera = (SAGE_Camera *)SAGE_AllocMem(sizeof(SAGE_Camera));
  if (camera != NULL) {
    camera->anglex = camera->angley = camera->anglez = 0;
    camera->posx = camera->posy = camera->posz = (FLOAT)0.0;
    camera->near_plane = S3DE_MIN_DISTANCE;
    camera->far_plane = S3DE_MAX_DISTANCE;
    camera->view_left = left;
    camera->view_top = top;
    camera->view_width = width;
    camera->view_height = height;
    camera->fov = S3DE_FOV*SMTH_PRECISION;
    camera->centerx = (FLOAT)(left + (width / 2.0));
    camera->centery = (FLOAT)(top + (height / 2.0));
    camera->view_dist = (width / 2.0) / SAGE_FastTangent(camera->fov/2);
    sage_world.cameras[index] = camera;
    return TRUE;
  }
  return FALSE;
}

/**
 * Remove the camera from the world
 */
VOID SAGE_RemoveCamera(ULONG index)
{
  SAGE_Camera * camera;

  SD(SAGE_DebugLog("Remove camera #%d", index));
  if (index < S3DE_MAX_CAMERAS) {
    camera = sage_world.cameras[index];
    if (camera != NULL) {
      sage_world.cameras[index] = NULL;
      SAGE_FreeMem(camera);
    }
  } else {
    SAGE_SetError(SERR_CAMERA_INDEX);
  }
}

/**
 * Remove all cameras
 */
VOID SAGE_FlushCameras()
{
  ULONG index;
  SAGE_Camera * camera;
  
  SD(SAGE_DebugLog("Flush cameras (%d)", S3DE_MAX_CAMERAS));
  for (index = 0;index < S3DE_MAX_CAMERAS;index++) {
    camera = sage_world.cameras[index];
    if (camera != NULL) {
      sage_world.cameras[index] = NULL;
      SAGE_FreeMem(camera);
    }
  }
}

/**
 * Get the camera from her index
 */
SAGE_Camera * SAGE_GetCamera(ULONG index)
{
  if (index < S3DE_MAX_CAMERAS) {
    if (sage_world.cameras[index] == NULL) {
      SAGE_SetError(SERR_NO_CAMERA);
    }
    return sage_world.cameras[index];
  }
  SAGE_SetError(SERR_CAMERA_INDEX);
  return NULL;
}

/**
 * Set the active camera
 */
BOOL SAGE_SetActiveCamera(ULONG index)
{
  if (index < S3DE_MAX_CAMERAS) {
    sage_world.active_camera = index;
    return TRUE;
  }
  SAGE_SetError(SERR_CAMERA_INDEX);
  return FALSE;
}

/**
 * Get the active camera
 */
SAGE_Camera * SAGE_GetActiveCamera()
{
  if (sage_world.cameras[sage_world.active_camera] == NULL) {
    SAGE_SetError(SERR_NO_CAMERA);
  }
  return sage_world.cameras[sage_world.active_camera];
}

/**
 * Keep camera angle between 0 and SMTH_ANGLE_360
 *
 * @param camera Camera
 */
VOID SAGE_ClampCameraAngle(SAGE_Camera * camera)
{
  while (camera->anglex < 0) camera->anglex += SMTH_ANGLE_360;
  while (camera->anglex >= SMTH_ANGLE_360) camera->anglex-= SMTH_ANGLE_360;
  while (camera->angley < 0) camera->angley += SMTH_ANGLE_360;
  while (camera->angley >= SMTH_ANGLE_360) camera->angley-= SMTH_ANGLE_360;
  while (camera->anglez < 0) camera->anglez += SMTH_ANGLE_360;
  while (camera->anglez >= SMTH_ANGLE_360) camera->anglez-= SMTH_ANGLE_360;
}
/**
 * Set the camera angles
 */
BOOL SAGE_SetCameraAngle(ULONG index, WORD ax, WORD ay, WORD az)
{
  SAGE_Camera * camera;
  
  camera = SAGE_GetCamera(index);
  if (camera != NULL) {
    camera->anglex = ax;
    camera->angley = ay;
    camera->anglez = az;
    SAGE_ClampCameraAngle(camera);
    return TRUE;
  }
  return FALSE;
}

/**
 * Rotate the camera
 */
BOOL SAGE_RotateCamera(ULONG index, WORD dax, WORD day, WORD daz)
{
  SAGE_Camera * camera;
  
  camera = SAGE_GetCamera(index);
  if (camera != NULL) {
    camera->anglex += dax;
    camera->angley += day;
    camera->anglez += daz;
    SAGE_ClampCameraAngle(camera);
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the camera position
 */
BOOL SAGE_SetCameraPosition(ULONG index, FLOAT posx, FLOAT posy, FLOAT posz)
{
  SAGE_Camera * camera;
  
  camera = SAGE_GetCamera(index);
  if (camera != NULL) {
    camera->posx = posx;
    camera->posy = posy;
    camera->posz = posz;
    return TRUE;
  }
  return FALSE;
}

/**
 * Move the camera
 */
BOOL SAGE_MoveCamera(ULONG index, FLOAT dx, FLOAT dy, FLOAT dz)
{
  SAGE_Camera * camera;
  
  camera = SAGE_GetCamera(index);
  if (camera != NULL) {
    camera->posx += dx;
    camera->posy += dy;
    camera->posz += dz;
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the camera view
 */
BOOL SAGE_SetCameraView(ULONG index, LONG left, LONG top, LONG width, LONG height, WORD fov)
{
  SAGE_Camera * camera;
  
  camera = SAGE_GetCamera(index);
  if (camera != NULL) {
    camera->view_left = left;
    camera->view_top = top;
    camera->view_width = width;
    camera->view_height = height;
    camera->fov = fov;
    camera->centerx = (FLOAT)(left + (width / 2.0));
    camera->centery = (FLOAT)(top + (height / 2.0));
    camera->view_dist = (width / 2.0) / SAGE_FastTangent(camera->fov/2);
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the camera planes
 */
BOOL SAGE_SetCameraPlane(ULONG index, FLOAT np, FLOAT fp)
{
  SAGE_Camera * camera;
  
  camera = SAGE_GetCamera(index);
  if (camera != NULL) {
    camera->near_plane = np;
    camera->far_plane = fp;
    return TRUE;
  }
  return FALSE;
}
