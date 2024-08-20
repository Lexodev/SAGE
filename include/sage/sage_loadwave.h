/**
 * sage_loadwave.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * WAVE sound loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#ifndef _SAGE_LOADWAVE_H_
#define _SAGE_LOADWAVE_H_

#include <exec/exec.h>
#include <dos/dos.h>

#include <sage/sage_sound.h>

#define SSND_RIFFTAG          0x52494646
#define SSND_WAVETAG          0x57415645
#define SSND_WAVEOFFSET       8
#define SSND_FMTTAG           0x666D7420
#define SSND_DATATAG          0x64617461

/** Wave file structure */
typedef struct {
  /** Wave format and sound channels */
  WORD format, channel;
  /** Wave frequency and bit rate */
  LONG frequency, bitrate;
  /** Wave byte bloc and sample size */
  WORD bytebloc, sample;
} SAGE_WaveFmt;

/** Load a wave file */
SAGE_Sound *SAGE_LoadWAVESound(BPTR);

#endif
