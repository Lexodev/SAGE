/**
 * sage_picture.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Picture management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 November 2020
 */

#include <dos/dos.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_picture.h"

#include <proto/dos.h>
#include <clib/alib_protos.h>
#include <clib/datatypes_protos.h>
#include <proto/cybergraphics.h>

/* V43 extensions (attributes) */

/* Set the app datatype interface mode (LONG); see "Interface modes" below */
#define PDTA_DestMode   (DTA_Dummy + 251)

/* Interface modes */
#define PMODE_V42 (0)   /* Compatibility mode */
#define PMODE_V43 (1)   /* Extended mode */

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Allocate a picture structure
 *
 * @return SAGE Picture structure
 */
SAGE_Picture * SAGE_AllocPicture()
{
  SAGE_Picture * picture;

  picture = (SAGE_Picture *) SAGE_AllocMem(sizeof(SAGE_Picture));
  if (picture != NULL) {
    picture->bitmap = NULL;
  }
  return picture;
}

/**
 * Release a picture
 *
 * @param picture Picture structure pointer
 */
VOID SAGE_ReleasePicture(SAGE_Picture * picture)
{
  if (picture != NULL) {
    SAGE_ReleaseBitmap(picture->bitmap);
    SAGE_FreeMem(picture);
  }
}

/**
 * Map a planar bitmap to a chunky bitmap
 * Totally un-optimized method
 * 
 * @param src_bitmap  Source bitmap
 * @param dest_bitmap Destination bitmap
 */
VOID SAGE_PlanarToChunky(struct BitMap * src_bitmap, SAGE_Bitmap * dest_bitmap)
{
  UWORD width, height, po;
  UBYTE  * p0, * p1, * p2, * p3, * p4, * p5, * p6, * p7, pixel, mask, * picture_data;

  SD(SAGE_DebugLog("Remapping picture data to chunky bitmap"));
  p0 = (UBYTE *) src_bitmap->Planes[0];
  p1 = (UBYTE *) src_bitmap->Planes[1];
  p2 = (UBYTE *) src_bitmap->Planes[2];
  p3 = (UBYTE *) src_bitmap->Planes[3];
  p4 = (UBYTE *) src_bitmap->Planes[4];
  p5 = (UBYTE *) src_bitmap->Planes[5];
  p6 = (UBYTE *) src_bitmap->Planes[6];
  p7 = (UBYTE *) src_bitmap->Planes[7];
  po = src_bitmap->BytesPerRow - (dest_bitmap->width / 8);
  picture_data = (UBYTE *) dest_bitmap->bitmap_buffer;
  height = dest_bitmap->height;
  while (height--) {
    width = dest_bitmap->width / 8;
    while (width--) {
      mask = 128;
      pixel = ((*p0) & mask) >> 7 | ((*p1) & mask) >> 6 | ((*p2) & mask) >> 5 | ((*p3) & mask) >> 4 | ((*p4) & mask) >> 3 | ((*p5) & mask) >> 2 | ((*p6) & mask) >> 1 | ((*p7) & mask);
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) >> 6 | ((*p1) & mask) >> 5 | ((*p2) & mask) >> 4 | ((*p3) & mask) >> 3 | ((*p4) & mask) >> 2 | ((*p5) & mask) >> 1 | ((*p6) & mask) | ((*p7) & mask) << 1;
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) >> 5 | ((*p1) & mask) >> 4 | ((*p2) & mask) >> 3 | ((*p3) & mask) >> 2 | ((*p4) & mask) >> 1 | ((*p5) & mask) | ((*p6) & mask) << 1 | ((*p7) & mask) << 2;
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) >> 4 | ((*p1) & mask) >> 3 | ((*p2) & mask) >> 2 | ((*p3) & mask) >> 1 | ((*p4) & mask) | ((*p5) & mask) << 1 | ((*p6) & mask) << 2 | ((*p7) & mask) << 3;
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) >> 3 | ((*p1) & mask) >> 2 | ((*p2) & mask) >> 1 | ((*p3) & mask) | ((*p4) & mask) << 1 | ((*p5) & mask) << 2 | ((*p6) & mask) << 3 | ((*p7) & mask) << 4;
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) >> 2 | ((*p1) & mask) >> 1 | ((*p2) & mask) | ((*p3) & mask) << 1 | ((*p4) & mask) << 2 | ((*p5) & mask) << 3 | ((*p6) & mask) << 4 | ((*p7) & mask) << 5;
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) >> 1 | ((*p1) & mask) | ((*p2) & mask) << 1 | ((*p3) & mask) << 2 | ((*p4) & mask) << 3 | ((*p5) & mask) << 4 | ((*p6) & mask) << 5 | ((*p7) & mask) << 6;
      *picture_data++ = pixel;
      mask >>= 1;
      pixel = ((*p0) & mask) | ((*p1) & mask) << 1 | ((*p2) & mask) << 2 | ((*p3) & mask) << 3 | ((*p4) & mask) << 4 | ((*p5) & mask) << 5 | ((*p6) & mask) << 6 | ((*p7) & mask) << 7;
      *picture_data++ = pixel;
      p0++;
      p1++;
      p2++;
      p3++;
      p4++;
      p5++;
      p6++;
      p7++;
    }
    p0 += po;
    p1 += po;
    p2 += po;
    p3 += po;
    p4 += po;
    p5 += po;
    p6 += po;
    p7 += po;
  }
}

/**
 * Load a picture using datatypes
 *
 * @param file_name Picture file name
 *
 * @return Picture structure pointer
 */
SAGE_Picture * SAGE_LoadPicture(STRPTR file_name)
{
  SAGE_Picture * picture = NULL;
  APTR object = NULL, bm_handle;
  struct BitMapHeader * bmhd = NULL;
  struct gpLayout layout;
  ULONG * palette = NULL, red, green, blue, pixformat, bm_address, bm_bpr;
  struct BitMap * bitmap = NULL;
  UWORD index, width, height;
  UBYTE * source_data = NULL, * picture_data = NULL;

  SD(SAGE_DebugLog("Load picture %s using datatypes", file_name));
  // Ask datatypes to open the file and decode it
  if (object = NewDTObject(file_name, PDTA_Remap, FALSE, DTA_GroupID, GID_PICTURE, PDTA_DestMode, PMODE_V43, TAG_END)) {
    GetDTAttrs(object, PDTA_BitMapHeader, &bmhd, TAG_END);
    SD(SAGE_DebugLog("Picture size %dx%dx%d", bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth));
    if (bmhd->bmh_Depth < SBMP_DEPTH8) {
      SAGE_SetError(SERR_FILEFORMAT);
      DisposeDTObject(object);
      return NULL;
    }
    // Remap the picture
    layout.MethodID = DTM_PROCLAYOUT;
    layout.gpl_GInfo = NULL;
    layout.gpl_Initial= 1L;
    if (DoMethodA(object, (Msg) &layout)) {
      // Allocate structure
      if ((picture = SAGE_AllocPicture()) == NULL) {
        DisposeDTObject(object);
        return NULL;
      }
      // Get the picture colors if we are in palette mode
      if (bmhd->bmh_Depth == SBMP_DEPTH8) {
        SD(SAGE_DebugLog("Getting picture palette"));
        GetDTAttrs(object, PDTA_CRegs, &palette, TAG_END);
        for (index = 0;index < SPIC_MAXCOLORS;index++) {
          red = palette[index * 3 + 0] & 0xFF000000;
          green = palette[index * 3 + 1] & 0xFF000000;;
          blue = palette[index * 3 + 2] & 0xFF000000;;
          picture->color_map[index] = red >> 8 | green >> 16 | blue >> 24;
        }
      }
      // Get the picture bitmap
      GetDTAttrs(object, PDTA_BitMap, &bitmap, TAG_END);
      SD(SAGE_DumpSystemBitmap(bitmap));
      if (GetCyberMapAttr(bitmap, CYBRMATTR_ISCYBERGFX)) {
        // Check if pixel format is supported
        pixformat = GetCyberMapAttr(bitmap, CYBRMATTR_PIXFMT);
        if (pixformat != PIXFMT_CLUT && pixformat != PIXFMT_RGB16 && pixformat != PIXFMT_RGB24 && pixformat != PIXFMT_ARGB32) {
          SAGE_SetError(SERR_PIXFORMAT);
          DisposeDTObject(object);
          SAGE_ReleasePicture(picture);
          return NULL;  
        }
        // Allocate the picture bitmap
        if ((picture->bitmap = SAGE_AllocBitmap(bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth, pixformat, NULL)) == NULL) {
          DisposeDTObject(object);
          SAGE_ReleasePicture(picture);
          return NULL;  
        }
        // Get the bitmap base address
        bm_handle = LockBitMapTags(bitmap, LBMI_BASEADDRESS, &bm_address, LBMI_BYTESPERROW, &bm_bpr, TAG_DONE);
        if (bm_handle != NULL) {
          UnLockBitMap(bm_handle);
        } else {
          SAGE_SetError(SERR_LOCKBITMAP);
          DisposeDTObject(object);
          SAGE_ReleasePicture(picture);
          return NULL;  
        }
        source_data = (UBYTE *) bm_address;
        picture_data = (UBYTE *) picture->bitmap->bitmap_buffer;
        if (picture->bitmap->depth == SBMP_DEPTH8) {
          SD(SAGE_DebugLog("Getting 8bits picture data"));
          bm_bpr -= picture->bitmap->width;
          height = picture->bitmap->height;
          while (height--) {
            width = picture->bitmap->width;
            while (width--) {
              *picture_data++ = *source_data++;
            }
            source_data += bm_bpr;
          }
        } else if (picture->bitmap->depth == SBMP_DEPTH16) {
          SD(SAGE_DebugLog("Getting 16bits picture data"));
          bm_bpr -= (picture->bitmap->width * 2);
          height = picture->bitmap->height;
          while (height--) {
            width = picture->bitmap->width;
            while (width--) {
              *picture_data++ = *source_data++;
              *picture_data++ = *source_data++;
            }
            source_data += bm_bpr;
          }
        } else if (picture->bitmap->depth == SBMP_DEPTH24) {
          SD(SAGE_DebugLog("Getting 24bits picture data"));
          bm_bpr -= (picture->bitmap->width * 3);
          height = picture->bitmap->height;
          while (height--) {
            width = picture->bitmap->width;
            while (width--) {
              *picture_data++ = *source_data++;
              *picture_data++ = *source_data++;
              *picture_data++ = *source_data++;
            }
            source_data += bm_bpr;
          }
        } else if (picture->bitmap->depth == SBMP_DEPTH32) {
          SD(SAGE_DebugLog("Getting 32bits picture data"));
          bm_bpr -= (picture->bitmap->width * 4);
          height = picture->bitmap->height;
          while (height--) {
            width = picture->bitmap->width;
            while (width--) {
              *picture_data++ = *source_data++;
              *picture_data++ = *source_data++;
              *picture_data++ = *source_data++;
              *picture_data++ = *source_data++;
            }
            source_data += bm_bpr;
          }
        }
      } else {
        // Allocate the picture bitmap
        if ((picture->bitmap = SAGE_AllocBitmap(bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth, PIXFMT_CLUT, NULL)) == NULL) {
          DisposeDTObject(object);
          SAGE_ReleasePicture(picture);
          return NULL;  
        }
        SAGE_PlanarToChunky(bitmap, picture->bitmap);
      }
      DisposeDTObject(object);
      if (SageContext.AutoRemap) {
        SAGE_RemapPicture(picture);
      }
      return picture;
    }
    SAGE_SetError(SERR_PICMAPPING);
    DisposeDTObject(object);
    return NULL;
  }
  SAGE_SetError(SERR_OPENFILE);
  return NULL;
}

/**
 * Set the picture auto remap feature
 */
VOID SAGE_AutoRemapPicture(BOOL flag)
{
  SageContext.AutoRemap = flag;
}

/**
 * Set the picture transparency
 * 
 * @param picture SAGE Picture pointer
 * @param color   Transparent color
 * 
 * @return Operation success
 */
BOOL SAGE_SetPictureTransparency(SAGE_Picture * picture, ULONG color)
{
  if (picture != NULL) {
    return SAGE_SetBitmapTransparency(picture->bitmap, color);
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Set the screen color map from a picture color map
 *
 * @param picture SAGE Picture pointer
 *
 * @return Operation success
 */
BOOL SAGE_LoadPictureColorMap(SAGE_Picture * picture)
{
  UWORD index;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (picture == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  for (index = 0;index < SSCR_MAXCOLORS;index++) {
    screen->color_map[index] = picture->color_map[index];
  }
  return TRUE;
}

/**
 * Remap a picture to the screen pixel format
 *
 * @param picture SAGE Picture pointer
 *
 * @return Operation success
 */
BOOL SAGE_RemapPicture(SAGE_Picture * picture)
{
  SD(SAGE_DebugLog(
    "Remap picture from %s to %s",
    SAGE_GetPixelFormatName(picture->bitmap->pixformat),
    SAGE_GetPixelFormatName(SAGE_GetPixelFormat())
  ));
  if (picture == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  return SAGE_RemapBitmap(picture->bitmap, picture->color_map, SAGE_GetPixelFormat());
}

/**
 * Blit a picture part to a bitmap
 *
 * @param picture SAGE Picture pointer
 * @param left    Picture left in pixel
 * @param top     Picture top in pixel
 * @param width   Picture width in pixel
 * @param height  Picture height in pixel
 * @param bitmap  SAGE Bitmap pointer
 * @param x_pos   Horizontal position
 * @param y_pos   Vertical position
 *
 * @return Operation success
 */
BOOL SAGE_BlitPictureToBitmap(SAGE_Picture * picture, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * bitmap, ULONG x_pos, ULONG y_pos)
{
  if (picture == NULL || bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if ((left + width) <= picture->bitmap->width && (top + height) <= picture->bitmap->height) {
    if (bitmap->width >= (width + x_pos) && bitmap->height >= (height + y_pos)) {
      return SAGE_BlitBitmap(picture->bitmap, left, top, width, height, bitmap, x_pos, y_pos);
    }
  }
  SAGE_SetError(SERR_PICTURE_SIZE);
  return FALSE;
}

/**
 * Blit a picture or a part of it to the screen
 *
 * @param picture SAGE Picture pointer
 * @param left    Picture left in pixel
 * @param top     Picture top in pixel
 * @param width   Picture width in pixel
 * @param height  Picture height in pixel
 * @param x_pos   Horizontal position
 * @param y_pos   Vertical position
 *
 * @return Operation success
 */
BOOL SAGE_BlitPictureToScreen(SAGE_Picture * picture, ULONG left, ULONG top, ULONG width, ULONG height, ULONG x_pos, ULONG y_pos)
{
  SAGE_Screen * screen;

  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (picture == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if ((left + width) <= picture->bitmap->width && (top + height) <= picture->bitmap->height) {
    if (screen->width >= (width + x_pos) && screen->height >= (height + y_pos)) {
      // Copy picture bitmap into screen backbuffer
      return SAGE_BlitBitmap(picture->bitmap, left, top, width, height, SAGE_GetBackBitmap(), x_pos, y_pos);
    }
  }
  SAGE_SetError(SERR_PICTURE_SIZE);
  return FALSE;
}
