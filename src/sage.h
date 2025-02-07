/**
 * sage.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
 */

#ifndef _SAGE_H_
#define _SAGE_H_

#include <sage/sage_compiler.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_memory.h>
#include <sage/sage_timer.h>
#include <sage/sage_thread.h>
#include <sage/sage_maths.h>
#include <sage/sage_vampire.h>
#include <sage/sage_configfile.h>
#include <sage/sage_video.h>
#include <sage/sage_bitmap.h>
#include <sage/sage_blitter.h>
#include <sage/sage_event.h>
#include <sage/sage_screen.h>
#include <sage/sage_picture.h>
#include <sage/sage_draw.h>
#include <sage/sage_layer.h>
#include <sage/sage_sprite.h>
#include <sage/sage_tile.h>
#include <sage/sage_video.h>
#include <sage/sage_audio.h>
#include <sage/sage_input.h>
#include <sage/sage_interrupt.h>
#include <sage/sage_vblint.h>
#include <sage/sage_3d.h>
#include <sage/sage_network.h>

// Modules constants
#define SMOD_NONE             0L        // No module, only core functions
#define SMOD_VIDEO            1L        // Need P96 lib
#define SMOD_AUDIO            2L        // Need AHI lib
#define SMOD_INPUT            4L        // Need lowlevel lib
#define SMOD_INTERRUPTION     8L        // Also need lowlevel lib
#define SMOD_3D               16L       // Need Warp3D lib (not yet available)
#define SMOD_NETWORK          32L       // Need ixemul lib
#define SMOD_ALL              255L

/** Initialize SAGE library */
BOOL SAGE_Init(LONGBITS);

/** Close et clean SAGE library */
VOID SAGE_Exit(VOID);

/** Tell if a module has been loaded */
BOOL SAGE_LoadedModule(LONG);

/** Get SAGE library version */
STRPTR SAGE_GetVersion(VOID);

/** Enable/disable AMMX use if available */
VOID SAGE_UseAMMX(BOOL);

/** Pause the program for some 1/50 of seconds */
VOID SAGE_Pause(ULONG);

/** Activate/disable trace debug */
VOID SAGE_SetTraceDebug(BOOL);

#endif
