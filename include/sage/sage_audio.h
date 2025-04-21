/**
 * sage_audio.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Audio module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#ifndef _SAGE_AUDIO_H_
#define _SAGE_AUDIO_H_

#include <exec/exec.h>
#include <dos/dos.h> 
#include <devices/ahi.h>

#include <sage/sage_sound.h>
#include <sage/sage_music.h>

#define SAUD_MAX_CHANNELS     8
#define SAUD_CHANNEL0         0
#define SAUD_CHANNEL1         1
#define SAUD_CHANNEL2         2
#define SAUD_CHANNEL3         3
#define SAUD_CHANNEL4         4
#define SAUD_CHANNEL5         5
#define SAUD_CHANNEL6         3
#define SAUD_CHANNEL7         7

/** SAGE Audio device structure */
typedef struct {
  /** Audio control for AHI */
  struct AHIAudioCtrl *actrl;
  /** Hook for music player */
  struct Hook mod_player_hook;
  /** Mixing frequency */
  LONG mixfreq;
  /** Current played music */
  UWORD music_played;
  /** Sound bank */
  SAGE_Sound *sounds[SSND_MAX_SOUNDS+1]; // Reserve sound 0 for music
  /** Musics */
  SAGE_Music *musics[SMUS_MAX_MUSICS];
} SAGE_AudioDevice;

/** Init the audio module */
BOOL SAGE_InitAudioModule(VOID);

/** Release the audio module */
BOOL SAGE_ReleaseAudioModule(VOID);

/** Allocate audio device */
BOOL SAGE_AllocAudioDevice(VOID);

/** Release audio device */
BOOL SAGE_FreeAudioDevice(VOID);

#endif
