/**
 * sage_music.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Music management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_MUSIC_H_
#define _SAGE_MUSIC_H_

#include <exec/exec.h>
#include <dos/dos.h> 
#include <devices/ahi.h>

#include <proto/ahi.h>
#include <proto/exec.h>

#include "ext/PT-AHIPlay.h"

#define SMUS_MAX_MUSICS       4

#define SMUS_UNDEFINED        0
#define SMUS_MODULE           1
#define SMUS_AIFF             2

#define SMUS_STOPPED          0
#define SMUS_PLAYING          1
#define SMUS_PAUSED           2

#define SMUS_CHANNEL          0

#define SMUS_SAMPLE8M         AHIST_M8S
#define SMUS_SAMPLE8S         AHIST_S8S
#define SMUS_SAMPLE16M        AHIST_M16S
#define SMUS_SAMPLE16S        AHIST_S16S

/** SAGE music structure */
typedef struct {
  /** Music buffer */
  APTR buffer;
  /** Music size */
  LONG size;
  /** Music type, nb channels, samples size */
  UWORD type, channel, sample;
  /** Music frequency and bit rate */
  LONG frequency, bitrate;
  /** Current music status */
  UWORD status;
  /** Channels in use */
  BOOL chan0, chan1, chan2, chan3, chan4, chan5, chan6, chan7;
  /** Sample info structure */
  struct AHISampleInfo music_info;
} SAGE_Music;

/** Get the type of a music file */
UWORD SAGE_GetMusicFileType(BPTR);

/** Allocate and init a music structure */
SAGE_Music * SAGE_AllocMusic(VOID);

/** Release a music structure */
VOID SAGE_ReleaseMusic(SAGE_Music *);

/** Load a music */
SAGE_Music * SAGE_LoadMusic(STRPTR);

/** Add a music to the bank */
BOOL SAGE_AddMusic(UWORD, SAGE_Music *);

/** Remove a music from the music bank */
SAGE_Music * SAGE_RemoveMusic(UWORD);

/** Remove and release a music from the music bank */
BOOL SAGE_FreeMusic(UWORD);

/** Remove and release all musics from the music bank */
BOOL SAGE_ClearMusic(VOID);

/** Play a music */
BOOL SAGE_PlayMusic(UWORD);

/** Stop the current music */
BOOL SAGE_StopMusic(VOID);

/** Pause the current music */
BOOL SAGE_PauseMusic(VOID);

/** Restart the current music */
BOOL SAGE_ResumeMusic(VOID);

#endif
