/**
 * sage_loadtracker.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Module tracker loading
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
#include <sage/sage_loadtracker.h>

#include <proto/dos.h>
#include <proto/exec.h>

/**
 * Load a protracker module
 * 
 * @param file_handle Music file handle
 * 
 * @return SAGE music structure
 */
SAGE_Music *SAGE_LoadPTModule(BPTR file_handle)
{
  SAGE_Music *music;
  LONG bytes_read;

  SD(SAGE_DebugLog("Loading Protracker module");)
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
