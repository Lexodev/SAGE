/**
 * sage_context.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Conext definition
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#ifndef _SAGE_CONTEXT_H_
#define _SAGE_CONTEXT_H_

#include "sage_video.h"
#include "sage_audio.h"
#include "sage_input.h"
#include "sage_network.h"
#include "sage_interrupt.h"
#include "sage_thread.h"
#include "sage_3d.h"

/** SAGE context */
typedef struct {
  /** Loaded modules */
  LONGBITS LoadedModules;
  /** Trace debug flag */
  BOOL TraceDebug;
  /** AMMX available */
  BOOL AmmxReady;
  /** Auto remap picture */
  BOOL AutoRemap;
  /** SAGE video */
  SAGE_VideoDevice * SageVideo;
  /** SAGE audio */
  SAGE_AudioDevice * SageAudio;
  /** SAGE input */
  SAGE_InputDevice * SageInput;
  /** SAGE network */
  SAGE_NetworkDevice * SageNetwork;
  /** SAGE interruptions */
  SAGE_Interruption Interruptions[SINT_MAX_INTERRUPT];
  /** SAGE threads */
  SAGE_Thread * Threads[STHD_MAX_THREAD];
  /** SAGE 3D */
  SAGE_3DDevice * Sage3D;
} SAGE_Context;

#endif
