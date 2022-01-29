/**
 * sage_music.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Music management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <datatypes/datatypes.h>
#include <datatypes/soundclass.h>

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_loadaiff.h"
#include "sage_loadtracker.h"
#include "sage_music.h"

#include <proto/ahi.h>
#include <proto/dos.h>
#include <proto/exec.h>

/** @var Protracker module data */
struct PTData pt_data;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Allocate music structure
 *
 * @return SAGE music structure
 */
SAGE_Music * SAGE_AllocMusic()
{
  SAGE_Music * music;

  SD(SAGE_DebugLog("Allocate music"));
  music = (SAGE_Music *) SAGE_AllocMem(sizeof(SAGE_Music));
  if (music != NULL) {
    music->buffer = NULL;
    music->size = 0;
    music->type = SMUS_UNDEFINED;
    music->status = SMUS_STOPPED;
    music->chan0 = FALSE;
    music->chan1 = FALSE;
    music->chan2 = FALSE;
    music->chan3 = FALSE;
    return music;
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return NULL;
}

/**
 * Release sage music structure
 * 
 * @param music SAGE music structure
 */
VOID SAGE_ReleaseMusic(SAGE_Music * music)
{
  SD(SAGE_DebugLog("Release music"));
  if (music != NULL) {
    if (music->buffer != NULL) {
      SAGE_FreeMem(music->buffer);
    }
    SAGE_FreeMem(music);
  }
}

/** Dump music structure */
VOID SAGE_DumpMusic(SAGE_Music * music)
{
  if (music != NULL) {
    SAGE_DebugLog("** Music structure **");
    SAGE_DebugLog(". Size = %d", music->size);
    SAGE_DebugLog(". Type = %d", music->type);
    SAGE_DebugLog(". Channel = %d", music->channel);
    SAGE_DebugLog(". Sample = %d", music->sample);
    SAGE_DebugLog(". Frequency = %d", music->frequency);
    SAGE_DebugLog(". Bitrate = %d", music->bitrate);
  }
}

/**
 *  Get the type of a music file
 * 
 *  @param file_handle Handle on a file
 * 
 *  @return Type of music file
 */
UWORD SAGE_GetMusicFileType(BPTR file_handle)
{
  LONG bytes_read, music_tag;

  // Check for AIFF music
  bytes_read = Seek(file_handle, SMUS_AIFFOFFSET, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &music_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return SMUS_UNDEFINED;
  }
  if (music_tag == SMUS_AIFFTAG) {
    SD(SAGE_DebugLog("This is an AIFF music"));
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return SMUS_AIFF;
  }
  // Check for tracker music
  bytes_read = Seek(file_handle, SMUS_TRACKOFFSET, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &music_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return SMUS_UNDEFINED;
  }
  if (music_tag == SMUS_TRACKTAG) {
    SD(SAGE_DebugLog("This is a tracker module"));
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return SMUS_MODULE;
  }
  return SMUS_UNDEFINED;
}

/**
 * Load a music
 * 
 * @param file_name Music file name
 * 
 * @return SAGE music structure
 */
SAGE_Music * SAGE_LoadMusic(STRPTR file_name)
{
  SAGE_Music * music;
  BPTR file_handle;
  UWORD type;

  SD(SAGE_DebugLog("Load music %s", file_name));
  music = NULL;
  file_handle = Open(file_name, MODE_OLDFILE);
  if (file_handle != 0) {
    type = SAGE_GetMusicFileType(file_handle);
    if (type == SMUS_MODULE) {
      music = SAGE_LoadPTModule(file_handle);
    } else if (type == SMUS_AIFF) {
      music = SAGE_LoadAIFFMusic(file_handle);
    } else {
      SAGE_SetError(SERR_FILEFORMAT);
    }
    Close(file_handle);
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  SD(SAGE_DumpMusic(music));
  return music;
}

/**
 * Add a music to the music bank
 * 
 * @param index Music bank index
 * @param music SAGE music
 * 
 * @return Allocated music slot
 */
BOOL SAGE_AddMusic(UWORD index, SAGE_Music * music)
{
  SAGE_AudioDevice * audio;

  // Check for audio device
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  }
  if (index >= SMUS_MAX_MUSICS) {
    SAGE_SetError(SERR_MUSIC_INDEX);
    return FALSE;
  }
  if (audio->musics[index] != NULL) {
    SAGE_FreeMusic(index);
  }
  if (music != NULL) {
    audio->musics[index] = music;
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Remove a music from the music bank
 * 
 * @param index Music bank index
 * 
 * @return SAGE music
 */
SAGE_Music * SAGE_RemoveMusic(UWORD index)
{
  SAGE_AudioDevice * audio;
  SAGE_Music * music;

  // Check for audio device
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return NULL;
  }
  if (index >= SMUS_MAX_MUSICS) {
    SAGE_SetError(SERR_MUSIC_INDEX);
    return NULL;
  }
  music = audio->musics[index];
  audio->musics[index] = NULL;
  return music;
}

/**
 * Remove and free a music from the music bank
 * 
 * @param index Music bank index
 * 
 * @return Operation success
 */
BOOL SAGE_FreeMusic(UWORD index)
{
  SAGE_Music * music;

  music = SAGE_RemoveMusic(index);
  if (music == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  SAGE_ReleaseMusic(music);
  return TRUE;
}

/**
 * Remove and free all musics
 * 
 * @return Operation success
 */
BOOL SAGE_ClearMusic()
{
  UWORD index;

  for (index = 0;index < SMUS_MAX_MUSICS;index++) {
    SAGE_FreeMusic(index);
  }
  return TRUE;
}

/**
 * Play a music
 * 
 * @param index Music bank index
 * 
 * @return Operation success
 */
BOOL SAGE_PlayMusic(UWORD index)
{
  SAGE_AudioDevice * audio;
  SAGE_Music * music;

  // Check for audio device
  audio = SageContext.SageAudio;
  SAFE(if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  })
  if (index >= SMUS_MAX_MUSICS) {
    SAGE_SetError(SERR_MUSIC_INDEX);
    return FALSE;
  }
  music = audio->musics[index];
  if (music != NULL && music->buffer != NULL) {
    SAGE_StopMusic();
    if (music->type == SMUS_MODULE) {
      pt_data.ptd_AHIBase = (struct AHIBase *) AHIBase;
      pt_data.ptd_AudioCtrl = audio->actrl;
      pt_data.ptd_ModuleAddress = music->buffer;
      if (mt_init(&pt_data)) {
        audio->mod_player_hook.h_Data = (APTR) &pt_data;
        mt_start(&pt_data);
        music->status = SMUS_PLAYING;
        audio->music_played = index;
        return TRUE;
      }
    } else if (music->type == SMUS_AIFF) {
      if (AHI_LoadSound(0, AHIST_SAMPLE, &(music->music_info), audio->actrl) == AHIE_OK) {
        audio->sounds[0] = NULL;
        if (!AHI_ControlAudio(audio->actrl, AHIC_Play, TRUE, TAG_DONE)) {
          AHI_Play(
            audio->actrl,
            AHIP_BeginChannel, SMUS_CHANNEL,
            AHIP_Freq, music->frequency,
            AHIP_Vol, 64 * 1024,
            AHIP_Pan, 32 * 1024,
            AHIP_Sound, 0,
            AHIP_LoopSound, AHI_NOSOUND,
            AHIP_EndChannel, NULL,
            TAG_DONE
          );
          music->status = SMUS_PLAYING;
          audio->music_played = index;
          return TRUE;
        }
      }
    }
    SAGE_SetError(SERR_PLAYMUSIC);
    return FALSE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Stop playing music
 * 
 * @return Operation success
 */
BOOL SAGE_StopMusic()
{
  SAGE_AudioDevice * audio;
  SAGE_Music * music;

  // Check for audio device
  audio = SageContext.SageAudio;
  SAFE(if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  })
  if (audio->music_played == SMUS_MAX_MUSICS) {
    return FALSE;
  }
  music = audio->musics[audio->music_played];
  if (music != NULL) {
    if (music->status != SMUS_STOPPED) {
      if (music->type == SMUS_MODULE) {
        audio->mod_player_hook.h_Data = NULL;
        mt_stop(&pt_data);
        mt_end(&pt_data);
      } else if (music->type == SMUS_AIFF) {
        AHI_SetSound(SMUS_CHANNEL, AHI_NOSOUND, NULL, NULL, audio->actrl, NULL);
        AHI_UnloadSound(0, audio->actrl);
      }
      music->status = SMUS_STOPPED;
      audio->music_played = SMUS_MAX_MUSICS;
    }
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Pause the current played music
 * 
 * @return Operation success
 */
BOOL SAGE_PauseMusic()
{
  SAGE_AudioDevice * audio;
  SAGE_Music * music;

  // Check for audio device
  audio = SageContext.SageAudio;
  SAFE(if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  })
  if (audio->music_played == SMUS_MAX_MUSICS) {
    return FALSE;
  }
  music = audio->musics[audio->music_played];
  if (music != NULL) {
    if (music->status == SMUS_PLAYING) {
      if (music->type == SMUS_MODULE) {
        audio->mod_player_hook.h_Data = NULL;
        mt_stop(&pt_data);
      }
      music->status = SMUS_PAUSED;
    }
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Resume the current paused music
 * 
 * @return Operation success
 */
BOOL SAGE_ResumeMusic()
{
  SAGE_AudioDevice * audio;
  SAGE_Music * music;

  // Check for audio device
  audio = SageContext.SageAudio;
  SAFE(if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  })
  if (audio->music_played == SMUS_MAX_MUSICS) {
    return FALSE;
  }
  music = audio->musics[audio->music_played];
  if (music != NULL) {
    if (music->status == SMUS_PAUSED) {
      if (music->type == SMUS_MODULE) {
        audio->mod_player_hook.h_Data = (APTR) &pt_data;
        mt_resume(&pt_data);
      }
      music->status = SMUS_PLAYING;
    }
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}
