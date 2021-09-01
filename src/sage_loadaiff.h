/**
 * sage_loadaiff.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * AIFF music loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_LOADAIFF_H_
#define _SAGE_LOADAIFF_H_

#include <exec/exec.h>
#include <dos/dos.h>

#include "sage_music.h"

#define SMUS_AIFFTAG          0x41494646
#define SMUS_AIFFOFFSET       0x8
#define SMUS_COMMTAG          0x434F4D4D
#define SMUS_SSNDTAG          0x53534E44

/** AIFF file structure */
typedef struct {
  /** Sound channels */
  WORD channel;
  /** Sample frames */
  ULONG frame;
  /** Sample size */
  WORD size;
  /** Sample rate (80 bit IEEE Standard 754) */
  UBYTE rate_ieee[10];
} SAGE_AiffFmt;

/** Load an AIFF file */
SAGE_Music * SAGE_LoadAIFFMusic(BPTR);

#endif
