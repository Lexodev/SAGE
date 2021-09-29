/**
 * sage_3d.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <exec/execbase.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuitionbase.h>

#include <proto/exec.h>
#include <proto/Warp3D.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_3d.h"
#include "sage_context.h"

#define WARP3DVERSION         2L

/** @var Warp3D library */
struct Library * Warp3DBase = NULL;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Initialize 3D module
 * Open librairies
 *
 * @return Operation success
 */
BOOL SAGE_Init3DModule(VOID)
{
  SD(SAGE_DebugLog("Init 3D module"));
  if ((Warp3DBase = OpenLibrary("Warp3D.library", WARP3DVERSION)) == NULL) {
    SAGE_SetError(SERR_WARP3D_LIB);
    return FALSE;
  }
  if (!SAGE_Alloc3DDevice()) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Release 3D module
 * Close librairies
 *
 * @return Operation success
 */
BOOL SAGE_Release3DModule(VOID)
{
  SD(SAGE_DebugLog("Release 3D module"));
  if (SageContext.Sage3D != NULL) {
    SAGE_Free3DDevice();
  }
  if (Warp3DBase != NULL) {
    CloseLibrary(Warp3DBase);
    Warp3DBase = NULL;
  }
  return TRUE;
}

/**
 * Allocate 3D device
 *
 * @return Operation success
 */
BOOL SAGE_Alloc3DDevice(VOID)
{
  struct Window * window;
  SAGE_3DDevice * device;

  SD(SAGE_InfoLog("Allocate 3D device"));
  if (SageContext.SageVideo == NULL || SageContext.SageVideo->screen == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  if ((device = SAGE_AllocMem(sizeof(SAGE_3DDevice))) == NULL) {
    return FALSE;
  }
  device->driver_type = W3D_CheckDriver();
  if (device->driver_type & W3D_DRIVER_UNAVAILABLE) {
    SAGE_FreeMem(device);
    SAGE_SetError(SERR_NO_3DDRIVER);
    return FALSE;
  }
  window = SageContext.SageVideo->screen->system_window;
  device->context = W3D_CreateContextTags(
    &(device->warp3d_error),
    W3D_CC_BITMAP, window->RPort->BitMap,
    W3D_CC_YOFFSET, 0,
    W3D_CC_DRIVERTYPE, W3D_DRIVER_BEST,
    W3D_CC_FAST, TRUE,
    TAG_DONE
  );
  if (device->context == NULL) {
    SAGE_FreeMem(device);
    SAGE_SetError(SERR_NO_3DCONTEXT);
    return FALSE;
  }
  SageContext.Sage3D = device;
  return TRUE;
}

/**
 * Release 3D device
 *
 * @return Operation success
 */
BOOL SAGE_Free3DDevice()
{
  SAGE_3DDevice * device;
  
  SD(SAGE_InfoLog("Release 3D device"));
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return FALSE;
  }
  if (device->context != NULL) {
    W3D_DestroyContext(device->context);
  }
  SAGE_FreeMem(device);
  SageContext.Sage3D = NULL;
  return TRUE;
}

/**
 * Get current 3D context
 *
 * @return 3D context
 */
W3D_Context * SAGE_Get3DContext(VOID)
{
  SAGE_3DDevice * device;
  
  SD(SAGE_InfoLog("Get 3D context"));
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return NULL;
  }
  return device->context;
}
