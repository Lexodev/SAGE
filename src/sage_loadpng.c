/**
 * sage_loadpng.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * PNG picture loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <dos/dos.h>

#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_memory.h>
#include <sage/sage_loadpng.h>

#include <proto/dos.h>
#include <clib/alib_protos.h>

/**
 * Load a PNG picture
 *
 * @param file_handle Handle on the file
 *
 * @return Picture structure pointer or NULL on error
 */
SAGE_Picture *SAGE_LoadPNGPicture(BPTR file_handle)
{
  SAGE_Picture *picture = NULL;
  SAGE_PNGHeader header;
  LONG bytes_read, size, chunk, index, nbcolors;
  UBYTE * palette, red, green, blue;

  SAGE_DebugLog("Loading PNG picture");
  // Allocate structure
  if ((picture = SAGE_AllocPicture()) == NULL) {
    return NULL;
  }
  // Skip unused data
  bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
  // Get header size
  bytes_read = Read(file_handle, &size, sizeof(size));
  if (bytes_read != sizeof(size)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  bytes_read = Read(file_handle, &chunk, sizeof(chunk));
  if (bytes_read != sizeof(chunk)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  if (chunk != SPIC_PNGIHDR) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  // Get header
  bytes_read = Read(file_handle, &header, 13);
  if (bytes_read != size) {
    SAGE_SetError(SERR_READFILE);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  // Skip CRC
  bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
  SAGE_DebugLog("PNG header_size=%d", size);
  SAGE_DebugLog(" width=%d, height=%d, depth=%d", header.width, header.height, header.depth);
  SAGE_DebugLog(" color=%d, compression=%d, filter=%d, interleave=%d", header.color, header.compression, header.filter, header.interleave);
  // Only uncompressed, not interleaved and not filtered picture
  if (header.compression != 0 || header.filter != 0 || header.interleave != 0) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  // Only indexed, true color or true color with alpha is supported
  if (header.color == SPIC_PNGGRAY || header.color == SPIC_PNGAGRAY || header.depth != 8) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  // Get the real picture depth
  if (header.color == SPIC_PNGTRUE) {
    header.depth = 24;
  } else if (header.color == SPIC_PNGATRUE) {
    header.depth = 32;
  }
  // Now allocate the picture bitmap
  SAGE_DebugLog("Allocate picture bitmap %dx%dx%d", header.width, header.height, header.depth);
  if ((picture->bitmap = SAGE_AllocBitmap(header.width, header.height, header.depth, PIXFMT_UNDEFINED, NULL)) == NULL) {
    SAGE_ReleasePicture(picture);
    return NULL;  
  }
  if (header.color == SPIC_PNGINDX) {
    // Find the picture palette
    while (chunk != SPIC_PNGPLTE) {
      // Get next chunk
      bytes_read = Read(file_handle, &size, sizeof(size));
      if (bytes_read != sizeof(size)) {
        SAGE_SetError(SERR_READFILE);
        SAGE_ReleasePicture(picture);
        return NULL;
      }
      bytes_read = Read(file_handle, &chunk, sizeof(chunk));
      if (bytes_read != sizeof(chunk)) {
        SAGE_SetError(SERR_READFILE);
        SAGE_ReleasePicture(picture);
        return NULL;
      }
      SAGE_DebugLog("Found chunk 0x%X (%d)", chunk, size);
      if (chunk == SPIC_PNGIEND) {
        SAGE_SetError(SERR_FILEFORMAT);
        SAGE_ReleasePicture(picture);
        return NULL;
      }
      if (chunk != SPIC_PNGPLTE) {
        // Skip all chunk
        bytes_read = Seek(file_handle, size+4, OFFSET_CURRENT);
      }
    }
    SAGE_DebugLog("PNG palette_size=%d", size);
    palette = SAGE_AllocMem(size);
    if (palette == NULL) {
      SAGE_SetError(SERR_NO_MEMORY);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    // Load palette
    bytes_read = Read(file_handle, palette, size);
    if (bytes_read != size) {
      SAGE_SetError(SERR_READFILE);
      SAGE_FreeMem(palette);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    // Skip CRC
    bytes_read = Seek(file_handle, 4, OFFSET_CURRENT);
    // Copy in picture structure
    nbcolors = 1 << header.depth;
    if (nbcolors > SPIC_MAXCOLORS) {
      SAGE_SetError(SERR_FILEFORMAT);
      SAGE_FreeMem(palette);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    for (index = 0;index < nbcolors;index++) {
      red = *palette++;
      green = *palette++;
      blue = *palette++;
      picture->color_map[index] = red << 16 | green << 8 | blue;
    }
    SAGE_FreeMem(palette);
  }
  // Find the picture data
  while (chunk != SPIC_PNGIDAT) {
    // Get next chunk
    bytes_read = Read(file_handle, &size, sizeof(size));
    if (bytes_read != sizeof(size)) {
      SAGE_SetError(SERR_READFILE);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    bytes_read = Read(file_handle, &chunk, sizeof(chunk));
    if (bytes_read != sizeof(chunk)) {
      SAGE_SetError(SERR_READFILE);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    if (chunk == SPIC_PNGIEND) {
      SAGE_SetError(SERR_FILEFORMAT);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    if (chunk != SPIC_PNGIDAT) {
      // Skip all chunk
      bytes_read = Seek(file_handle, size+4, OFFSET_CURRENT);
    }
  }
  SAGE_DebugLog("PNG data_size=%d", size);
  return picture;
}
