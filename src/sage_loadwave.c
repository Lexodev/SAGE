/**
 * sage_loadwave.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * WAVE sound loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <dos/dos.h>

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_loadwave.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/ahi.h>

/**
 * Load a wave sound
 * 
 * @param file_handle Handler on the wave file
 * 
 * @return SAGE sound pointer
 */
SAGE_Sound * SAGE_LoadWAVESound(BPTR file_handle)
{
  SAGE_Sound * sound = NULL;
  SAGE_WaveFmt audio_fmt;
  BYTE * sound_buffer8;
  WORD * sound_buffer16;
  LONG bytes_read, chunk_id, data_size;

  SD(SAGE_DebugLog("Loading WAVE sound"));
  // Skip unused data
  bytes_read = Seek(file_handle, 12, OFFSET_BEGINING);
  // Allocate structure
  if ((sound = (SAGE_Sound *) SAGE_AllocMem(sizeof(SAGE_Sound))) == NULL) {
    return NULL;
  }
  sound->sample_buffer = NULL;
  // Get format chunk
  bytes_read = Read(file_handle, &chunk_id, sizeof(chunk_id));
  if (bytes_read != sizeof(chunk_id)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound);
    return NULL;
  }
  if (chunk_id != SSND_FMTTAG) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_FreeMem(sound);
    return NULL;
  }
  // Skip unused data
  bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
  // Get audio format
  bytes_read = Read(file_handle, &audio_fmt, sizeof(audio_fmt));
  if (bytes_read != sizeof(audio_fmt)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound);
    return NULL;
  }
  audio_fmt.format = SAGE_WORDTOBE(audio_fmt.format);
  audio_fmt.channel = SAGE_WORDTOBE(audio_fmt.channel);
  sound->frequency = SAGE_LONGTOBE(audio_fmt.frequency);
  sound->bitrate = SAGE_LONGTOBE(audio_fmt.bitrate);
  audio_fmt.sample = SAGE_WORDTOBE(audio_fmt.sample);
  SD(SAGE_DebugLog(
    "Audio format=%d, Nb channels=%d, Frequency=%d, Bitrate=%d, Sample size=%d",
    audio_fmt.format, audio_fmt.channel, sound->frequency, sound->bitrate, audio_fmt.sample
  ));
  SD(SAGE_DebugLog("Loading sample data"));
  // Load only PCM sound, mono or stereo and 16bits max sample
  if (audio_fmt.format != 1 || audio_fmt.channel > 2 || audio_fmt.sample > 16) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_FreeMem(sound);
    return NULL;
  }
  // Get data chunk
  bytes_read = Read(file_handle, &chunk_id, sizeof(chunk_id));
  if (bytes_read != sizeof(chunk_id)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound);
    return NULL;
  }
  if (chunk_id != SSND_DATATAG) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_FreeMem(sound);
    return NULL;
  }
  // Get data size
  bytes_read = Read(file_handle, &data_size, sizeof(data_size));
  if (bytes_read != sizeof(data_size)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound);
    return NULL;
  }
  sound->size = SAGE_LONGTOBE(data_size);
  SD(SAGE_DebugLog("Data size=%d", sound->size));
  if ((sound->sample_buffer = SAGE_AllocMem(sound->size)) == NULL) {
    SAGE_FreeMem(sound);
    return NULL;
  }
  bytes_read = Read(file_handle, sound->sample_buffer, sound->size);
  if (bytes_read != sound->size) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound->sample_buffer);
    SAGE_FreeMem(sound);
    return NULL;
  }
  sound->volume = 64 * 1024;
  sound->pan = 32 * 1024;
  if (audio_fmt.sample == 8) {
    if (audio_fmt.channel == 1) {
      sound->sample_info.ahisi_Type = SSND_SAMPLE8M;
    } else {
      sound->sample_info.ahisi_Type = SSND_SAMPLE8S;
    }
    sound_buffer8 = (BYTE *) sound->sample_buffer;
    for (data_size =0;data_size < sound->size;data_size++) {
      sound_buffer8[data_size] -= 128;
    }
  } else {
    if (audio_fmt.channel == 1) {
      sound->sample_info.ahisi_Type = SSND_SAMPLE16M;
    } else {
      sound->sample_info.ahisi_Type = SSND_SAMPLE16S;
    }
    sound_buffer16 = (WORD *) sound->sample_buffer;
    for (data_size = 0;data_size < (sound->size / 2);data_size++) {
      sound_buffer16[data_size] = SAGE_WORDTOBE(sound_buffer16[data_size]);
    }
  }
  sound->sample_info.ahisi_Address = sound->sample_buffer;
  sound->sample_info.ahisi_Length = sound->size / AHI_SampleFrameSize(sound->sample_info.ahisi_Type);
  return sound;
}