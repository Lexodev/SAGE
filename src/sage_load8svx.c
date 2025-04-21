/**
 * sage_load8svx.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 8SVX sound loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <stdlib.h>
#include <stdio.h>

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_memory.h>
#include <sage/sage_load8svx.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/ahi.h>

/**
 * Load a 8SVX sound
 * 
 * @param file_handle Handler on the 8SVX file
 * 
 * @return SAGE sound pointer
 */
SAGE_Sound *SAGE_Load8SVXSound(BPTR file_handle)
{
  SAGE_Sound *sound = NULL;
  SAGE_8SVXHeader file_header;
  LONG bytes_read, chunk_id, data_size;
  BOOL body;

  SD(SAGE_DebugLog("Loading 8SVX sound");)
  // Skip unused data
  bytes_read = Seek(file_handle, 12, OFFSET_BEGINING);
  // Allocate structure
  if ((sound = (SAGE_Sound *)SAGE_AllocMem(sizeof(SAGE_Sound))) == NULL) {
    return NULL;
  }
  sound->sample_buffer = NULL;
  // Get header chunk
  bytes_read = Read(file_handle, &chunk_id, sizeof(chunk_id));
  if (bytes_read != sizeof(chunk_id)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound);
    return NULL;
  }
  if (chunk_id != SSND_VHDRTAG) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_FreeMem(sound);
    return NULL;
  }
  // Skip unused data
  bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
  // Get file header
  bytes_read = Read(file_handle, &file_header, sizeof(file_header));
  if (bytes_read != sizeof(file_header)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(sound);
    return NULL;
  }
  sound->type = 1;
  sound->channel = 1;
  sound->sample = 8;
  sound->size = file_header.oneShotHiSamples + file_header.repeatHiSamples;
  sound->frequency = file_header.samplesPerSec;
  sound->bitrate = file_header.samplesPerSec;
  sound->volume = file_header.vol;
  sound->pan = 32 * 1024;
  SD(SAGE_DebugLog(
    "Audio type=%d, Nb channels=%d, Frequency=%d, Bitrate=%d, Sample size=%d",
    sound->type, sound->channel, sound->frequency, sound->bitrate, sound->sample
  );)
  SD(SAGE_DebugLog("Loading sample data");)
  // Search the BODY tag
  body = FALSE;
  while (!body) {
    bytes_read = Read(file_handle, &chunk_id, sizeof(chunk_id));
    if (bytes_read != sizeof(chunk_id)) {
      SAGE_SetError(SERR_READFILE);
      SAGE_FreeMem(sound);
      return NULL;
    }
    bytes_read = Read(file_handle, &data_size, sizeof(data_size));
    if (bytes_read != sizeof(data_size)) {
      SAGE_SetError(SERR_READFILE);
      SAGE_FreeMem(sound);
      return NULL;
    }
    if (chunk_id == SSND_BODYTAG) {
      body = TRUE;
    } else {
      bytes_read = Seek(file_handle, data_size, OFFSET_CURRENT);
    }
  }
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
  sound->sample_info.ahisi_Type = SSND_SAMPLE8M;
  sound->sample_info.ahisi_Address = sound->sample_buffer;
  sound->sample_info.ahisi_Length = sound->size / AHI_SampleFrameSize(sound->sample_info.ahisi_Type);
  return sound;
}
