/**
 * sage_sound.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Sound management
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
#include "sage_loadwave.h"
#include "sage_load8svx.h"
#include "sage_sound.h"

#include <proto/ahi.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <clib/datatypes_protos.h>

/** SAGE context */
extern SAGE_Context SageContext;

/**
 *  Get the type of a sound file
 * 
 *  @param file_handle Handle on a file
 * 
 *  @return Type of sound music
 */
UWORD SAGE_GetSoundFileType(BPTR file_handle)
{
  LONG bytes_read, sound_tag;

  // Check for WAVE sound
  bytes_read = Seek(file_handle, SSND_WAVEOFFSET, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &sound_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return SSND_UNDEFINED;
  }
  if (sound_tag == SSND_WAVETAG) {
    SD(SAGE_DebugLog("This is a WAVE sound"));
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return SSND_WAVE;
  }
  // Check for 8SVX sound
  bytes_read = Seek(file_handle, SSND_8SVXOFFSET, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &sound_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return SMUS_UNDEFINED;
  }
  if (sound_tag == SSND_8SVXTAG) {
    SD(SAGE_DebugLog("This is a 8SVX sound"));
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return SSND_8SVX;
  }
  return SSND_UNDEFINED;
}

/**
 * Load a sound
 *
 * @param file_name Sound file name
 *
 * @return Sound structure pointer
 */
SAGE_Sound * SAGE_LoadSound(STRPTR file_name)
{
  SAGE_Sound * sound = NULL;
  BPTR file_handle;
  UWORD type;

  SD(SAGE_DebugLog("Load sound %s", file_name));
  sound = NULL;
  file_handle = Open(file_name, MODE_OLDFILE);
  if (file_handle != 0) {
    type = SAGE_GetSoundFileType(file_handle);
    if (type == SSND_8SVX) {
      sound = SAGE_Load8SVXSound(file_handle);
    } else if (type == SSND_WAVE) {
      sound = SAGE_LoadWAVESound(file_handle);
    } else {
      SAGE_SetError(SERR_FILEFORMAT);
    }
    Close(file_handle);
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  return sound;
}

/**
 * Load a sound using datatypes
 *
 * @param file_name Sound file name
 *
 * @return Sound structure pointer
 */
SAGE_Sound * SAGE_DTLoadSound(STRPTR file_name)
{
  SAGE_Sound * sound = NULL;
  APTR object = NULL;
  UBYTE * sample;
  ULONG length;
  UWORD period, volume;

  SD(SAGE_DebugLog("Load sound %s using datatypes", file_name));
  if (object = NewDTObject(file_name, DTA_GroupID, GID_SOUND, TAG_END)) {
    // Allocate sound structure
    sound = (SAGE_Sound *) SAGE_AllocMem(sizeof(SAGE_Sound));
    if (sound == NULL) {
      DisposeDTObject(object);
      SAGE_SetError(SERR_NO_MEMORY);
      return NULL;
    }
    GetDTAttrs(object, SDTA_Sample, &sample, TAG_END);
    GetDTAttrs(object, SDTA_SampleLength , &length, TAG_END);
    GetDTAttrs(object, SDTA_Period, &period, TAG_END);
    GetDTAttrs(object, SDTA_Volume, &volume, TAG_END);
    SAGE_DebugLog("Sound loaded 0x%X %d %d %d", sample, length, period, volume);
    DisposeDTObject(object);
    return sound;
  }
  SAGE_SetError(SERR_SOUNDLOAD);
  return NULL;
}

/**
 * Release a sound
 *
 * @param sound Sound structure pointer
 */
VOID SAGE_ReleaseSound(SAGE_Sound * sound)
{
  if (sound != NULL) {
    if (sound->sample_buffer) {
      SAGE_FreeMem(sound->sample_buffer);
    }
    SAGE_FreeMem(sound);
  }
}

/**
 * Add a sound to the sound bank
 * 
 * @param index Sound bank index
 * @param sound SAGE sound pointer
 * 
 * @return Operation success
 */
BOOL SAGE_AddSound(UWORD index, SAGE_Sound * sound)
{
  SAGE_AudioDevice * audio;

  // Check for audio device
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  }
  if (index > SSND_MAX_SOUNDS) {
    SAGE_SetError(SERR_SOUND_INDEX);
    return FALSE;
  }
  if (audio->sounds[index] != NULL) {
    SAGE_FreeSound(index);
  }
  if (sound != NULL) {
    if (AHI_LoadSound(index+1, AHIST_SAMPLE, &(sound->sample_info), audio->actrl) == AHIE_OK) {
      audio->sounds[index+1] = sound;
      return TRUE;
    }
    SAGE_SetError(SERR_SOUNDADD);
    return FALSE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Remove a sound from the sound bank
 * 
 * @param index Sound bank index
 * 
 * @return SAGE sound removed
 */
SAGE_Sound * SAGE_RemoveSound(UWORD index)
{
  SAGE_AudioDevice * audio;
  SAGE_Sound * sound;

  // Check for audio device
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return NULL;
  }
  if (index > SSND_MAX_SOUNDS) {
    SAGE_SetError(SERR_SOUND_INDEX);
    return NULL;
  }
  AHI_UnloadSound(index+1, audio->actrl);
  sound = audio->sounds[index+1];
  audio->sounds[index+1] = NULL;
  return sound;
}

/**
 * Remove and free a sound from the sound bank
 * 
 * @param index Sound bank index
 * 
 * @return Operation success
 */
BOOL SAGE_FreeSound(UWORD index)
{
  SAGE_Sound * sound;

  sound = SAGE_RemoveSound(index);
  if (sound == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  SAGE_ReleaseSound(sound);
  return TRUE;
}

/**
 * Clear all sounds from the bank and release sound resources
 * 
 * @return Operation success
 */
BOOL SAGE_ClearSound()
{
  UWORD index;

  for (index = 0;index < SSND_MAX_SOUNDS;index++) {
    SAGE_FreeSound(index);
  }
  return TRUE;
}

/**
 * Play a sound on a channel
 * 
 * @param index   Sound bank index
 * @param channel Channel to play the sound
 * 
 * @return Operation success
 */
BOOL SAGE_PlaySound(UWORD index, UWORD channel)
{
  SAGE_AudioDevice * audio;
  SAGE_Sound * sound;

  // Check for audio device
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  }
  if (index > SSND_MAX_SOUNDS) {
    SAGE_SetError(SERR_SOUND_INDEX);
    return FALSE;
  }
  sound = audio->sounds[index+1];
  if (sound != NULL && channel < SAUD_MAX_CHANNELS) {
    if (!AHI_ControlAudio(audio->actrl, AHIC_Play, TRUE, TAG_DONE)) {
      AHI_Play(
        audio->actrl,
        AHIP_BeginChannel, channel,
        AHIP_Freq, sound->frequency,
        AHIP_Vol, sound->volume,
        AHIP_Pan, sound->pan,
        AHIP_Sound, index+1,
        AHIP_LoopSound, AHI_NOSOUND,
        AHIP_EndChannel, NULL,
        TAG_DONE
      );
      return TRUE;
    }
  }
  SAGE_SetError(SERR_SOUNDPLAY);
  return FALSE;
}

/**
 * Stop a sound on a channel
 * 
 * @param index   Sound bank index
 * @param channel Channel to play the sound
 * 
 * @return Operation success
 */
BOOL SAGE_StopSound(UWORD index, UWORD channel)
{
  SAGE_AudioDevice * audio;
  SAGE_Sound * sound;

  // Check for audio device
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  }
  if (index > SSND_MAX_SOUNDS) {
    SAGE_SetError(SERR_SOUND_INDEX);
    return FALSE;
  }
  sound = audio->sounds[index+1];
  if (sound != NULL && channel < SAUD_MAX_CHANNELS) {
    AHI_SetSound(channel, AHI_NOSOUND, NULL, NULL, audio->actrl, NULL);
    return TRUE;
  }
  SAGE_SetError(SERR_SOUNDPLAY);
  return FALSE;
}
