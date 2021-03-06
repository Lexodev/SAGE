/**
 * sage_3d.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 December 2021
 */

#ifndef _SAGE_3D_H_
#define _SAGE_3D_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>

#include "sage_3dtexture.h"
#include "sage_3drender.h"
#include "sage_3dengine.h"

#define S3DD_S3DRENDER        1
#define S3DD_W3DRENDER        2
#define S3DD_M3DRENDER        3

/** SAGE 3D device structure */
typedef struct {
  /** Warp3D last error */
  ULONG warp3d_error;
  /** Warp3D driver type */
  ULONG driver_type;
  /** Warp3D context */
  W3D_Context * context;
  /** Renderer */
  UWORD render_system;
  SAGE_Render render;
  /** Textures */
  SAGE_3DTexture * textures[STEX_MAX_TEXTURES];
} SAGE_3DDevice;

/** Init the 3D module */
BOOL SAGE_Init3DModule(VOID);

/** Release the 3D module */
BOOL SAGE_Release3DModule(VOID);

/** Allocate 3D device */
BOOL SAGE_Alloc3DDevice(VOID);

/** Release 3D device */
BOOL SAGE_Free3DDevice(VOID);

/** Define the render system */
BOOL SAGE_Set3DRenderSystem(UWORD);

/** Get current 3D context */
W3D_Context * SAGE_Get3DContext(VOID);

/** Return a string describing the last 3D device error */
STRPTR SAGE_GetLast3DDeviceError(VOID);

#endif
