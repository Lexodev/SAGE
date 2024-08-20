/**
 * sage_3d.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
 */

#ifndef _SAGE_3D_H_
#define _SAGE_3D_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>
#include <Maggie3D/Maggie3D.h>

#include <sage/sage_3dtexture.h>
#include <sage/sage_3drender.h>
#include <sage/sage_3dengine.h>

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
  W3D_Context *w3d_context;
  /** Maggie3D last error */
  LONG maggie3d_error;
  /** Maggie3D context */
  M3D_Context *m3d_context;
  /** Renderer */
  UWORD render_system;
  SAGE_Render render;
  /** Textures */
  SAGE_3DTexture *textures[STEX_MAX_TEXTURES];
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

/** Get current Warp3D context */
W3D_Context *SAGE_GetW3DContext(VOID);

/** Get current Maggie3D context */
M3D_Context *SAGE_GetM3DContext(VOID);

/** Return a string describing the last 3D device error */
STRPTR SAGE_GetLast3DDeviceError(VOID);

#endif
