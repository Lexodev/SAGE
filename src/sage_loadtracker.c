/**
 * sage_loadtracker.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Module tracker loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdlib.h>
#include <stdio.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_loadtracker.h"
#include "sage_music.h"

#include <proto/dos.h>
#include <proto/exec.h>

/**
 * Load a protracker module
 * 
 * @param file_handle Music file handle
 * 
 * @return SAGE music structure
 */
SAGE_Music * SAGE_LoadPTModule(BPTR file_handle)
{
  SAGE_Music * music;
  LONG bytes_read;

  if ((music = SAGE_AllocMusic()) == NULL) {
    return NULL;
  }
  // Get the file size
  bytes_read = Seek(file_handle, 0, OFFSET_END);
  bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
  music->size = bytes_read;
  music->buffer = SAGE_AllocChipMem(bytes_read);
  if (music->buffer == NULL) {
    SAGE_ReleaseMusic(music);
    SAGE_SetError(SERR_LOADMUSIC);
    return NULL;
  }
  bytes_read = Read(file_handle, music->buffer, music->size);
  if (bytes_read != music->size) {
    SAGE_ReleaseMusic(music);
    SAGE_SetError(SERR_LOADMUSIC);
    return NULL;
  }
  music->type = SMUS_MODULE;
  return music;
}
