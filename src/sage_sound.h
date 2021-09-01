/**
 * sage_sound.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Sound management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_SOUND_H_
#define _SAGE_SOUND_H_

#include <exec/exec.h>
#include <dos/dos.h> 
#include <devices/ahi.h>

#include <proto/ahi.h>
#include <proto/exec.h>

#define SSND_MAX_SOUNDS       128

#define SSND_UNDEFINED        0
#define SSND_WAVE             1
#define SSND_8SVX             2

#define SSND_SAMPLE8M         AHIST_M8S
#define SSND_SAMPLE8S         AHIST_S8S
#define SSND_SAMPLE16M        AHIST_M16S
#define SSND_SAMPLE16S        AHIST_S16S

/** SAGE sound structure */
typedef struct {
  /** Size of the sample */
  ULONG size;
  /** Frequency, bit rate, volume and panning of the sample */
  ULONG frequency, bitrate, volume, pan;
  /** Sample buffer */
  APTR sample_buffer;
  /** Sample info structure */
  struct AHISampleInfo sample_info;
} SAGE_Sound;

/** Load a sound */
SAGE_Sound * SAGE_LoadSound(STRPTR);

/** Load a sound using datatypes */
SAGE_Sound * SAGE_DTLoadSound(STRPTR);

/** Release a sound */
VOID SAGE_ReleaseSound(SAGE_Sound *);

/** Add a sound to the bank */
BOOL SAGE_AddSound(UWORD, SAGE_Sound *);

/** Remove a sound from the sound bank */
SAGE_Sound * SAGE_RemoveSound(UWORD);

/** Remove and release a sound from the sound bank */
BOOL SAGE_FreeSound(UWORD);

/** Clear and release all sounds from the sound bank */
BOOL SAGE_ClearSound(VOID);

/** Play a sound on a channel */
BOOL SAGE_PlaySound(UWORD, UWORD);

/** Stop a sound on a channel */
BOOL SAGE_StopSound(UWORD, UWORD);

#endif
