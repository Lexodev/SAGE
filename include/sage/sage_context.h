/**
 * sage_context.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Context definition
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#ifndef _SAGE_CONTEXT_H_
#define _SAGE_CONTEXT_H_

#include <sage/sage_video.h>
#include <sage/sage_audio.h>
#include <sage/sage_input.h>
#include <sage/sage_network.h>
#include <sage/sage_interrupt.h>
#include <sage/sage_thread.h>
#include <sage/sage_3d.h>

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
  /** SAGE interruptions */
  SAGE_Interruption Interruptions[SINT_MAX_INTERRUPT];
  /** SAGE threads */
  SAGE_Thread *Threads[STHD_MAX_THREAD];
  /** SAGE video */
  SAGE_VideoDevice *SageVideo;
  /** SAGE audio */
  SAGE_AudioDevice *SageAudio;
  /** SAGE input */
  SAGE_InputDevice *SageInput;
  /** SAGE network */
  SAGE_NetworkDevice *SageNetwork;
  /** SAGE 3D */
  SAGE_3DDevice *Sage3D;
} SAGE_Context;

#endif
