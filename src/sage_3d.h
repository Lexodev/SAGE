/**
 * sage_3d.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * 3D module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_3DENGINE_H_
#define _SAGE_3DENGINE_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>

/** SAGE 3D device structure */
typedef struct {
  /** Warp3D last error */
  ULONG warp3d_error;
  /** Warp3D driver type */
  ULONG driver_type;
  /** Warp3D context */
  W3D_Context * context;
  /** Warp3D clipping */
  W3D_Scissor scissor;
} SAGE_3DDevice;

/** Init the 3D module */
BOOL SAGE_Init3DModule(VOID);

/** Release the 3D module */
BOOL SAGE_Release3DModule(VOID);

/** Allocate 3D device */
BOOL SAGE_Alloc3DDevice(VOID);

/** Release 3D device */
BOOL SAGE_Free3DDevice(VOID);

/** Get current 3D context */
W3D_Context * SAGE_Get3DContext(VOID);

#endif
