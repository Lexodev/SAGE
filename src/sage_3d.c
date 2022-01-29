/**
 * sage_3d.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 December 2021
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
 * Display available 3D drivers
 */
VOID SAGE_Display3DDrivers(VOID)
{
  W3D_Driver ** drivers, * driver;
  ULONG index;
  
  drivers = W3D_GetDrivers();
  index = 0;
  while (*drivers != NULL) {
    driver = *drivers;
    SAGE_InfoLog("W3D driver %d :", index);
    // 3D chip
    switch (driver->ChipID) {
      case W3D_CHIP_UNKNOWN:
        SAGE_InfoLog(" - Unknown chip");
        break;
      case W3D_CHIP_VIRGE:
        SAGE_InfoLog(" - S3 Virge");
        break;
      case W3D_CHIP_PERMEDIA2:
        SAGE_InfoLog(" - Permedia2");
        break;
      case W3D_CHIP_VOODOO1:
        SAGE_InfoLog(" - Voodoo1");
        break;
      case W3D_CHIP_AVENGER_LE:
        SAGE_InfoLog(" - Voodoo3 LE");
        break;
      case W3D_CHIP_AVENGER_BE:
        SAGE_InfoLog(" - Voodoo3 BE");
        break;
      case W3D_CHIP_PERMEDIA3:
        SAGE_InfoLog(" - Permedia3");
        break;
      case W3D_CHIP_RADEON:
        SAGE_InfoLog(" - Radeon");
        break;
      case W3D_CHIP_RADEON2:
        SAGE_InfoLog(" - Radeon2");
        break;
    }
    SAGE_InfoLog(" - %s", driver->name);
//  @todo : display driver->formats
    if (driver->swdriver == W3D_TRUE) {
      SAGE_InfoLog(" - this is a software driver !");
    }
    drivers++;
    index++;
  }
}

/**
 * Allocate 3D device
 *
 * @return Operation success
 */
BOOL SAGE_Alloc3DDevice(VOID)
{
  SAGE_3DDevice * device;

  SD(SAGE_InfoLog("Allocate 3D device"));
  if ((device = SAGE_AllocMem(sizeof(SAGE_3DDevice))) == NULL) {
    return FALSE;
  }
  // Show available drivers
  SD(SAGE_Display3DDrivers());
  device->render_mode = S3DR_W3DMODE;
  device->driver_type = W3D_CheckDriver();
  if (device->driver_type & W3D_DRIVER_UNAVAILABLE) {
    SAGE_FreeMem(device);
    SAGE_SetError(SERR_NO_3DDRIVER);
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
  UWORD index;
  
  SD(SAGE_InfoLog("Release 3D device"));
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return FALSE;
  }
  // Release all textures
  for (index = 0;index < STEX_MAX_TEXTURES;index++) {
    if (device->textures[index] != NULL) {
      SAGE_ReleaseTexture(index);
      device->textures[index] = NULL;
    }
  }
  if (device->context != NULL) {
    W3D_DestroyContext(device->context);
  }
  SAGE_FreeMem(device);
  SageContext.Sage3D = NULL;
  return TRUE;
}

/**
 * Allocate the context for 3D rendering
 *
 * @return Operation success
 */
BOOL SAGE_Allocate3DContext(VOID)
{
  struct Window * window;
  SAGE_3DDevice * device;
  
  SD(SAGE_InfoLog("Allocate 3D context"));
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return FALSE;
  }
  if (SageContext.SageVideo == NULL || SageContext.SageVideo->screen == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
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
    SD(SAGE_InfoLog("Error : %s", SAGE_GetLast3DDeviceError()));
    SAGE_FreeMem(device);
    SAGE_SetError(SERR_NO_3DCONTEXT);
    return FALSE;
  }
  SD(SAGE_InfoLog("Set context states"));
  W3D_SetState(device->context, W3D_GOURAUD, W3D_ENABLE);
  W3D_SetState(device->context, W3D_TEXMAPPING, W3D_ENABLE);
  W3D_SetState(device->context, W3D_AUTOTEXMANAGEMENT, W3D_ENABLE);
  W3D_SetState(device->context, W3D_PERSPECTIVE, W3D_ENABLE);
  W3D_SetState(device->context, W3D_INDIRECT, W3D_DISABLE);
  W3D_SetState(device->context, W3D_SYNCHRON, W3D_DISABLE);
  W3D_SetState(device->context, W3D_FAST, W3D_ENABLE);
  W3D_SetState(device->context, W3D_ZBUFFER, W3D_DISABLE);
  W3D_SetState(device->context, W3D_FOGGING, W3D_DISABLE);
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

/**
 * Return a string describing the last 3D device error
 *
 * @return 3D device error string
 */
STRPTR SAGE_GetLast3DDeviceError(VOID)
{
  SAGE_3DDevice * device;
  
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return "No device";
  }
  switch (device->warp3d_error) {
    case W3D_ILLEGALINPUT:
      return "Illegal input";
    case W3D_NOMEMORY:
      return "Out of memory";
    case W3D_NODRIVER:
      return "No suitable driver found";
    case W3D_UNSUPPORTEDFMT:
      return "Supplied bitmap cannot be handled by Warp3D";
    case W3D_ILLEGALBITMAP:
      return "Supplied bitmap not properly initialized";
    case W3D_UNSUPPORTEDTEXSIZE:
      return "Unsupported texture size";
    case W3D_NOPALETTE:
      return "Chunky texture without palette specified";
    case W3D_UNSUPPORTEDTEXFMT:
      return "Texture format not supported";
  }
  return "Unknown error";
}
