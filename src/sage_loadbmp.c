/**
 * sage_loadbmp.c
 * 
 * SAGE (Smimple Amiga Game Engine) project
 * BMP picture loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <dos/dos.h>

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_memory.h>
#include <sage/sage_bitmap.h>
#include <sage/sage_loadbmp.h>

#include <proto/dos.h>
#include <clib/alib_protos.h>

/**
 * Decode a BMP picture
 *
 * @param file_handle Handle to the BMP file
 *
 * @return Picture structure pointer or NULL on error
 */
SAGE_Picture *SAGE_DecodeBMPPicture(BPTR file_handle)
{
  SAGE_Picture *picture = NULL;
  UBYTE *picture_body = NULL, *picture_data = NULL, pixel;
  SAGE_BMPHeader dib_header;
  LONG bytes_read, image_offset, size, line_size, height, width;
  ULONG pixformat;

  // Allocate structure
  if ((picture = SAGE_AllocPicture()) == NULL) {
    return NULL;
  }
  // Get header size
  bytes_read = Read(file_handle, &image_offset, sizeof(image_offset));
  if (bytes_read != sizeof(image_offset)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  image_offset = SAGE_LONGTOBE(image_offset);
  // Get DIB header
  bytes_read = Read(file_handle, &dib_header, sizeof(dib_header));
  if (bytes_read != sizeof(dib_header)) {
    SAGE_SetError(SERR_READFILE);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  dib_header.size = SAGE_LONGTOBE(dib_header.size);
  SD(SAGE_DebugLog("BMP image_offset=%d, dib_size=%d", image_offset, dib_header.size);)
  // Load only Windows 3 or + format
  if (dib_header.size != SPIC_BMPWIN3 && dib_header.size != SPIC_BMPWIN4 && dib_header.size != SPIC_BMPWIN5) {
    SAGE_SetError(SERR_FILEFORMAT);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  dib_header.width = SAGE_LONGTOBE(dib_header.width);
  dib_header.height = SAGE_LONGTOBE(dib_header.height);
  dib_header.depth = SAGE_WORDTOBE(dib_header.depth);
  SD(SAGE_DebugLog("BMP picture dimension %dx%dx%d", dib_header.width, dib_header.height, dib_header.depth);)
  // Get the CMAP for 8bits picture
  if (dib_header.depth == SBMP_DEPTH8) {
    SD(SAGE_DebugLog("Load colormap");)
    // Reset file pointer
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    // Go to CMAP data
    bytes_read = Seek(file_handle, dib_header.size + SPIC_BMPHSIZE, OFFSET_BEGINNING);
    // Get the CMAP
    size = SPIC_MAXCOLORS * 4;
    bytes_read = Read(file_handle, picture->color_map, size);
    if (bytes_read != size) {
      SAGE_SetError(SERR_READFILE);
      SAGE_ReleasePicture(picture);
      return NULL;
    }
    for (size = 0;size < SPIC_MAXCOLORS;size++) {
      picture->color_map[size] = SAGE_LONGTOBE(picture->color_map[size]);
    }
    pixformat = PIXFMT_CLUT;
  } else if (dib_header.depth == SBMP_DEPTH16) {
    pixformat = PIXFMT_RGB16;
  } else if (dib_header.depth == SBMP_DEPTH24) {
    pixformat = PIXFMT_RGB24;
  } else if (dib_header.depth == SBMP_DEPTH32) {
    pixformat = PIXFMT_ARGB32;
  }
  // Reset file pointer
  bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
  // Go to bitmap data
  bytes_read = Seek(file_handle, image_offset, OFFSET_BEGINNING);
  // Allocate picture buffer and load it
  size = dib_header.width * dib_header.height * (dib_header.depth / 8);
  SD(SAGE_DebugLog("BMP picture buffer size %d", size);)
  picture_body = SAGE_AllocMem(size);
  if (picture_body == NULL) {
    SAGE_SetError(SERR_NO_MEMORY);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  bytes_read = Read(file_handle, picture_body, size);
  if (bytes_read != size) {
    SAGE_SetError(SERR_READFILE);
    SAGE_FreeMem(picture_body);
    SAGE_ReleasePicture(picture);
    return NULL;
  }
  // Now allocate the picture bitmap
  SD(SAGE_DebugLog("Allocate picture bitmap %dx%dx%d", dib_header.width, dib_header.height, dib_header.depth);)
  if ((picture->bitmap = SAGE_AllocBitmap(dib_header.width, dib_header.height, dib_header.depth, 0, pixformat, NULL)) == NULL) {
    SAGE_FreeMem(picture_body);
    SAGE_ReleasePicture(picture);
    return NULL;  
  }
  // Mirror picture
  SD(SAGE_DebugLog("Mirror picture");)
  picture_data = (UBYTE *)picture->bitmap->bitmap_buffer;
  line_size = picture->bitmap->width * (picture->bitmap->depth / 8);
  picture_body += line_size * (picture->bitmap->height - 1);
  height = picture->bitmap->height;
  while (height--) {
    width = picture->bitmap->width * (picture->bitmap->depth / 8);
    while (width--) {
      *picture_data++ = *picture_body++;
    }
    picture_body -= (line_size * 2);
  }
  picture_body += line_size;
  SAGE_FreeMem(picture_body);
  if (picture->bitmap->depth != SBMP_DEPTH8) {
    // Remap to big endian format
    SD(SAGE_DebugLog("Remap to Big endian format");)
    picture_data = (UBYTE *)picture->bitmap->bitmap_buffer;
    if (picture->bitmap->depth == SBMP_DEPTH16) {
      height = picture->bitmap->height;
      while (height--) {
        width = picture->bitmap->width;
        while (width--) {
          pixel = picture_data[0];
          picture_data[0] = picture_data[1];
          picture_data[1] = pixel;
          picture_data += 2;
        }
      }
    } else if (picture->bitmap->depth == SBMP_DEPTH24) {
      height = picture->bitmap->height;
      while (height--) {
        width = picture->bitmap->width;
        while (width--) {
          pixel = picture_data[0];
          picture_data[0] = picture_data[2];
          picture_data[2] = pixel;
          picture_data += 3;
        }
      }
    } else if (picture->bitmap->depth = SBMP_DEPTH32) {
      height = picture->bitmap->height;
      while (height--) {
        width = picture->bitmap->width;
        while (width--) {
          pixel = picture_data[0];
          picture_data[0] = picture_data[3];
          picture_data[3] = pixel;
          pixel = picture_data[1];
          picture_data[1] = picture_data[2];
          picture_data[2] = pixel;
          picture_data += 4;
        }
      }
    }
  }
  return picture;
}

/**
 * Load a BMP picture
 *
 * @param file_name File name
 *
 * @return Picture structure pointer or NULL on error
 */
SAGE_Picture *SAGE_LoadBMPPicture(STRPTR file_name)
{
  BPTR file_handle;
  SAGE_Picture *picture;
  WORD bmp_tag;
  LONG bytes_read, data_size;

  SD(SAGE_DebugLog("Loading BMP picture %s", file_name);)
  file_handle = Open(file_name, MODE_OLDFILE);
  if (file_handle != 0) {
    // Check if it's a BMP file
    bytes_read = Read(file_handle, &bmp_tag, sizeof(bmp_tag));
    if (bytes_read != sizeof(bmp_tag)) {
      SAGE_SetError(SERR_READFILE);
      Close(file_handle);
      return NULL;
    }
    if (bmp_tag != SPIC_BMPTAG) {
      SAGE_SetError(SERR_FILEFORMAT);
      Close(file_handle);
      return NULL;
    }
    bytes_read = Read(file_handle, &data_size, sizeof(data_size));
    if (bytes_read != sizeof(data_size)) {
      SAGE_SetError(SERR_READFILE);
      Close(file_handle);
      return NULL;
    }
    picture = SAGE_DecodeBMPPicture(file_handle);
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  Close(file_handle);
  return picture;
}
