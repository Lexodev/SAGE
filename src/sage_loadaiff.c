/**
 * sage_loadaiff.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * AIFF music loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_loadaiff.h"
#include "sage_music.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/ahi.h>

/**
 * Convert a 80 bits IEEE-754 number to a DOUBLE
 * 
 * @param buffer Buffer of a 80 bits number
 * 
 * @return DOUBLE value
 */
DOUBLE SAGE_Convert80Bits(UBYTE * buffer)
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
SAGE_Music * SAGE_LoadAIFFMusic(BPTR file_handle)
{
  SAGE_Music * music;
  SAGE_AiffFmt info;
  LONG bytes_read, file_tag, data;

  if ((music = SAGE_AllocMusic()) == NULL) {
    return NULL;
  }
  // Read the COMM chunk
  bytes_read = Seek(file_handle, 12, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &file_tag, 4);
  if (bytes_read != 4) {
    SAGE_ReleaseMusic(music);
    SAGE_SetError(SERR_READFILE);
    return NULL;
  }
  if (file_tag == SMUS_COMMTAG) {
    bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
    bytes_read = Read(file_handle, &info, sizeof(SAGE_AiffFmt));
    if (bytes_read != (sizeof(SAGE_AiffFmt))) {
      SAGE_ReleaseMusic(music);
      SAGE_SetError(SERR_READFILE);
      return NULL;
    }
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
    // Read the SSND chunk
    bytes_read = Read(file_handle, &file_tag, 4);
    if (bytes_read != 4) {
      SAGE_ReleaseMusic(music);
      SAGE_SetError(SERR_READFILE);
      return NULL;
    }
    if (file_tag == SMUS_SSNDTAG) {
      bytes_read = Read(file_handle, &data, 4);
      if (bytes_read != 4) {
        SAGE_ReleaseMusic(music);
        SAGE_SetError(SERR_READFILE);
        return NULL;
      }
      music->size = data;
      music->buffer = SAGE_AllocMem(music->size);
      if (music->buffer == NULL) {
        SAGE_ReleaseMusic(music);
        return NULL;
      }
      bytes_read = Read(file_handle, music->buffer, music->size);
      if (bytes_read != music->size) {
        SAGE_ReleaseMusic(music);
        SAGE_SetError(SERR_LOADMUSIC);
        return NULL;
      }
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
    }
  }
  return music;
}
