/**
 * sage_loadtracker.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Module tracker loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#ifndef _SAGE_LOADTRACKER_H_
#define _SAGE_LOADTRACKER_H_

#include <exec/exec.h>
#include <dos/dos.h>

#include <sage/sage_music.h>

#define SMUS_TRACKTAG         0x4D2E4B2E
#define SMUS_TRACKOFFSET      0x438

/** Load a protracker module file */
SAGE_Music *SAGE_LoadPTModule(BPTR);

#endif
