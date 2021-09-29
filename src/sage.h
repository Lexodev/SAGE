/**
 * sage.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#ifndef _SAGE_H_
#define _SAGE_H_

#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_timer.h"
#include "sage_thread.h"
#include "sage_vampire.h"
#include "sage_video.h"
#include "sage_bitmap.h"
#include "sage_blitter.h"
#include "sage_event.h"
#include "sage_screen.h"
#include "sage_picture.h"
#include "sage_draw.h"
#include "sage_layer.h"
#include "sage_sprite.h"
#include "sage_tile.h"
#include "sage_video.h"
#include "sage_audio.h"
#include "sage_input.h"
#include "sage_interrupt.h"
#include "sage_vblint.h"
#include "sage_3d.h"
#include "sage_network.h"

// Modules constants
#define SMOD_NONE             0L        // No module, only core functions
#define SMOD_VIDEO            1L        // Need P96 lib
#define SMOD_AUDIO            2L        // Need AHI lib
#define SMOD_INPUT            4L        // Need lowlevel lib
#define SMOD_INTERRUPTION     8L        // Also need lowlevel lib
#define SMOD_3DENGINE         16L       // Need Warp3D lib (not yet available)
#define SMOD_NETWORK          32L       // Need ixemul lib
#define SMOD_ALL              255L

/** Initialize SAGE library */
BOOL SAGE_Init(LONG);

/** Close et clean SAGE library */
BOOL SAGE_Exit(VOID);

/** Tell which modules are loaded */
BOOL SAGE_LoadedModule(LONG);

/** Get SAGE library version */
STRPTR SAGE_GetVersion(VOID);

/** Enable/disable AMMX use if available */
VOID SAGE_UseAMMX(BOOL);

/** Pause the program for some 1/50 of seconds */
VOID SAGE_Pause(ULONG);

#endif
