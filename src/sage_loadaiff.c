/**
 * sage_loadaiff.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * AIFF music loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_loadaiff.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/ahi.h>

/**
 * Load the COMM chunk of an AIFF file
 *
 * @param file_handle File handle
 * @param info        File info structure
 * 
 * @return Operation success
 */
BOOL SAGE_LoadAIFFInfo(BPTR file_handle, SAGE_AiffFmt *info)
{
  LONG bytes_read, file_tag;
  
  // Read the COMM chunk
  bytes_read = Seek(file_handle, 12, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &file_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return FALSE;
  }
  if (file_tag == SMUS_COMMTAG) {
    bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
    bytes_read = Read(file_handle, info, sizeof(SAGE_AiffFmt));
    if (bytes_read != (sizeof(SAGE_AiffFmt))) {
      SAGE_SetError(SERR_READFILE);
      return FALSE;
    }
    return TRUE;
  }
  SAGE_SetError(SERR_FILEFORMAT);
  return FALSE;
}

/**
 * Load the SSND chunk of an AIFF file
 *
 * @param file_handle File handle
 * @param size        Data size
 * 
 * @return Sound data buffer
 */
APTR SAGE_LoadAIFFData(BPTR file_handle, ULONG *size)
{
  LONG bytes_read, file_tag, data;
  APTR buffer;
  BOOL data_chunk;
  
  buffer = NULL;
  data_chunk = FALSE;
  while (!data_chunk) {
    bytes_read = Read(file_handle, &file_tag, 4);
    if (bytes_read != 4) {
      SAGE_SetError(SERR_READFILE);
      return NULL;
    }
    bytes_read = Read(file_handle, &data, 4);
    if (bytes_read != 4) {
      SAGE_SetError(SERR_READFILE);
      return NULL;
    }
    SD(SAGE_DebugLog("Chunk found 0X%d of size %d", file_tag, data);)
    if (file_tag == SMUS_SSNDTAG) {
      SD(SAGE_DebugLog("SSND chunk, skip offset and blocksize");)
      bytes_read = Seek(file_handle, 8, OFFSET_CURRENT);
      *size = data - 8;
      buffer = SAGE_AllocMem(data);
      if (buffer == NULL) {
        return NULL;
      }
      bytes_read = Read(file_handle, buffer, *size);
      if (bytes_read != *size) {
        SAGE_FreeMem(buffer);
        SAGE_SetError(SERR_READFILE);
        return NULL;
      }
      data_chunk = TRUE;
    } else {
      // Data size should be always even
      if (data & 1) {
        data += 1;
      }
      bytes_read = Seek(file_handle, data, OFFSET_CURRENT);
    }
  }
  return buffer;
}

/**
 * Convert a 80 bits IEEE-754 number to a DOUBLE
 * 
 * @param buffer Buffer of a 80 bits number
 * 
 * @return DOUBLE value
 */
DOUBLE SAGE_Convert80Bits(UBYTE *buffer)
{
  DOUBLE sign, normalize, result;
  UWORD exponent;
  ULONG mantissa, mantissa2;

  // 80 bit floating point value according to the IEEE-754 specification and the Standard Apple Numeric Environment specification:
  // 1 bit sign, 15 bit exponent, 1 bit normalization indication, 63 bit mantissa
  if ((buffer[0] & 0x80) == 0x00)
    sign = 1;
  else
    sign = -1;
  exponent = (((UWORD)buffer[0] & 0x7F) << 8) | (UWORD)buffer[1];
  if ((buffer[2] & 0x80) != 0x00)
    normalize = 1;
  else
    normalize = 0;
  mantissa = (((ULONG)buffer[2] & 0x7F) << 24) | ((ULONG)buffer[3] << 16) | ((ULONG)buffer[4] << 8) | (ULONG)buffer[5];
  mantissa2 = (((ULONG)buffer[6] & 0x7F) << 24) | ((ULONG)buffer[7] << 16) | ((ULONG)buffer[8] << 8) | (ULONG)buffer[9];
  result = sign * (normalize + (DOUBLE)mantissa / ((ULONG)1 << 31)) * pow2((DOUBLE)(exponent - 16383));
  return result;
}

/**
 * Load an AIFF music
 * 
 * @param file_handle Music file handle
 * 
 * @return SAGE music structure
 */
SAGE_Music *SAGE_LoadAIFFMusic(BPTR file_handle)
{
  SAGE_Music *music;
  SAGE_AiffFmt info;

  SD(SAGE_DebugLog("Loading AIFF music");)
  if ((music = SAGE_AllocMusic()) == NULL) {
    return NULL;
  }
  if (SAGE_LoadAIFFInfo(file_handle, &info)) {
    // Play only mono or stereo and 8 or 16bits sample
    if (info.channel > 2 || info.size > 16) {
      SAGE_ReleaseMusic(music);
      SAGE_SetError(SERR_FILEFORMAT);
      return NULL;
    }
    music->channel = info.channel;
    music->sample = info.size;
    music->frequency = SAGE_Convert80Bits(info.rate_ieee);
    music->bitrate = music->channel * (music->sample / 8) * music->frequency;
    SD(SAGE_DebugLog(
      "Audio Nb channels=%d, Frequency=%d, Bitrate=%d, Sample size=%d",
      info.channel, music->frequency, music->bitrate, info.size
    );)
    SD(SAGE_DebugLog("Loading sample data");)
    music->buffer = SAGE_LoadAIFFData(file_handle, &(music->size));
    if (music->buffer != NULL) {
      SD(SAGE_DebugLog("Data size=%d", music->size);)
      // Setup music info for AHI
      if (music->sample == 8) {
        if (music->channel == 1) {
          music->music_info.ahisi_Type = SMUS_SAMPLE8M;
        } else {
          music->music_info.ahisi_Type = SMUS_SAMPLE8S;
        }
      } else {
        if (music->channel == 1) {
          music->music_info.ahisi_Type = SMUS_SAMPLE16M;
        } else {
          music->music_info.ahisi_Type = SMUS_SAMPLE16S;
        }
      }
      music->music_info.ahisi_Address = music->buffer;
      music->music_info.ahisi_Length = music->size / AHI_SampleFrameSize(music->music_info.ahisi_Type);
      music->type = SMUS_AIFF;
      return music;
    }
  }
  SAGE_ReleaseMusic(music);
  return NULL;
}

/**
 * Load an AIFF sound
 * 
 * @param file_handle Music file handle
 * 
 * @return SAGE sound structure
 */
SAGE_Sound *SAGE_LoadAIFFSound(BPTR file_handle)
{
  SAGE_Sound *sound = NULL;
  SAGE_AiffFmt info;

  SD(SAGE_DebugLog("Loading AIFF sound");)
  // Allocate structure
  if ((sound = (SAGE_Sound *)SAGE_AllocMem(sizeof(SAGE_Sound))) == NULL) {
    return NULL;
  }
  if (SAGE_LoadAIFFInfo(file_handle, &info)) {
    // Play only mono or stereo and 8 or 16bits sample
    if (info.channel > 2 || info.size > 16) {
      SAGE_FreeMem(sound);
      SAGE_SetError(SERR_FILEFORMAT);
      return NULL;
    }
    sound->type = 1;
    sound->channel = info.channel;
    sound->sample = info.size;
    sound->frequency = SAGE_Convert80Bits(info.rate_ieee);
    sound->bitrate = sound->channel * (sound->sample / 8) * sound->frequency;
    sound->volume = 64 * 1024;
    sound->pan = 32 * 1024;
    SD(SAGE_DebugLog(
      "Audio type=%d, Nb channels=%d, Frequency=%d, Bitrate=%d, Sample size=%d",
      sound->type, sound->channel, sound->frequency, sound->bitrate, sound->sample
    );)
    SD(SAGE_DebugLog("Loading sample data");)
    sound->sample_buffer = SAGE_LoadAIFFData(file_handle, &(sound->size));
    if (sound->sample_buffer != NULL) {
      SD(SAGE_DebugLog("Data size=%d", sound->size);)
      // Setup sound info for AHI
      if (sound->sample == 8) {
        if (sound->channel == 1) {
          sound->sample_info.ahisi_Type = SMUS_SAMPLE8M;
        } else {
          sound->sample_info.ahisi_Type = SMUS_SAMPLE8S;
        }
      } else {
        if (sound->channel == 1) {
          sound->sample_info.ahisi_Type = SMUS_SAMPLE16M;
        } else {
          sound->sample_info.ahisi_Type = SMUS_SAMPLE16S;
        }
      }
      sound->sample_info.ahisi_Address = sound->sample_buffer;
      sound->sample_info.ahisi_Length = sound->size / AHI_SampleFrameSize(sound->sample_info.ahisi_Type);
      return sound;
    }
  }
  SAGE_FreeMem(sound);
  return NULL;
}
