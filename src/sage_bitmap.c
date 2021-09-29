/**
 * sage_bitmap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Bitmap management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <exec/exec.h>

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_blitter.h"
#include "sage_context.h"
#include "sage_bitmap.h"

#include <proto/exec.h>
#include <proto/cybergraphics.h>

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Return the full name of pixel format
 *
 * @param format Pixel format
 *
 * @return Pixel format name
 */
STRPTR SAGE_GetPixelFormatName(ULONG format)
{
  switch (format) {
    case PIXFMT_CLUT:
      return "LUT8";
    case PIXFMT_RGB15:
      return "RGB15";
    case PIXFMT_RGB15PC:
      return "RGB15PC";
    case PIXFMT_BGR15PC:
      return "BGR15PC";
    case PIXFMT_RGB16:
      return "RGB16";
    case PIXFMT_RGB16PC:
      return "RGB16PC";
    case PIXFMT_BGR16PC:
      return "BGR16PC";
    case PIXFMT_RGB24:
      return "RGB24";
    case PIXFMT_BGR24:
      return "BGR24";
    case PIXFMT_ARGB32:
      return "ARGB32";
    case PIXFMT_ABGR32:
      return "ABGR32";
    case PIXFMT_RGBA32:
      return "RGBA32";
    case PIXFMT_BGRA32:
      return "BGRA32";
  }
  return "Undefined";
}

/**
 * Return the physical bitmap address
 *
 * @var bitmap Bitmap pointer
 *
 * @return Bitmap address
 */
ULONG SAGE_GetBitmapAddress(struct BitMap * bitmap)
{
  APTR cgx_handle = NULL;
  ULONG memory, bpr;

  if (GetCyberMapAttr(bitmap, CYBRMATTR_ISCYBERGFX)) {
    cgx_handle = LockBitMapTags(
      bitmap,
      LBMI_BASEADDRESS, &memory,
      LBMI_BYTESPERROW, &bpr,
      TAG_DONE
    );
  }
  if (cgx_handle != NULL) {
    UnLockBitMap(cgx_handle);
    SD(SAGE_DebugLog(" Bitmap was locked with FB address 0x%X (BPR %d)", memory, bpr));
    SD(if ((memory & 0xffffffe0) == memory) {
      SAGE_DebugLog(" Bitmap is 32 bytes aligned");
    } else {
      SAGE_DebugLog(" Bitmap is not 32 bytes aligned");
    })
  } else {
    memory = 0;
    SAGE_SetError(SERR_LOCKBITMAP);
  }
  return memory;
}

/**
 * Return the physical bitmap BPR
 *
 * @var bitmap Bitmap pointer
 *
 * @return Bitmap BPR
 */
ULONG SAGE_GetBitmapBPR(struct BitMap * bitmap)
{
  APTR cgx_handle = NULL;
  ULONG memory, bpr;

  if (GetCyberMapAttr(bitmap, CYBRMATTR_ISCYBERGFX)) {
    cgx_handle = LockBitMapTags(
      bitmap,
      LBMI_BASEADDRESS, &memory,
      LBMI_BYTESPERROW, &bpr,
      TAG_DONE
    );
  }
  if (cgx_handle != NULL) {
    UnLockBitMap(cgx_handle);
    SD(SAGE_DebugLog(" Bitmap was locked with FB address 0x%X (BPR %d)", memory, bpr));
    SD(if ((memory & 0xffffffe0) == memory) {
      SAGE_DebugLog(" Bitmap is 32 bytes aligned");
    } else {
      SAGE_DebugLog(" Bitmap is not 32 bytes aligned");
    })
  } else {
    bpr = 0;
    SAGE_SetError(SERR_LOCKBITMAP);
  }
  return bpr;
}

/**
 * Check for bitmap size constraint
 *
 * @var width Bitmap width
 * @var depth Bitmap depth
 *
 * @return Constraint is respected
 */
BOOL SAGE_CheckSizeConstraint(ULONG width, ULONG depth)
{
  // Check for size constraint
  if ((depth == SBMP_DEPTH8) && ((width % SBMP_SIZE8BITS) == 0)) {
    return TRUE;
  }
  if ((depth == SBMP_DEPTH16) && ((width % SBMP_SIZE16BITS) == 0)) {
    return TRUE;
  }
  if ((depth == SBMP_DEPTH24) && ((width % SBMP_SIZE24BITS) == 0)) {
    return TRUE;
  }
  if ((depth == SBMP_DEPTH32) && ((width % SBMP_SIZE32BITS) == 0)) {
    return TRUE;
  }
  SAGE_SetError(SERR_BITMAP_SIZE);
  return FALSE;
}

/**
 * Allocate fast draw buffers for the bitmap
 * 
 * @param bitmap SAGE bitmap pointer
 * 
 * @return Operation success
 */
BOOL SAGE_AllocateFastDrawBuffers(SAGE_Bitmap * bitmap)
{
  if (bitmap != NULL) {
    if (bitmap->first_buffer != NULL) {
      SAGE_FreeMem(bitmap->first_buffer);
    }
    if (bitmap->second_buffer != NULL) {
      SAGE_FreeMem(bitmap->second_buffer);
    }
    bitmap->first_buffer = (LONG *) SAGE_AllocMem(SBMP_DRAWBUFSIZE);
    bitmap->second_buffer = (LONG *) SAGE_AllocMem(SBMP_DRAWBUFSIZE);
    if (bitmap->first_buffer != NULL && bitmap->second_buffer != NULL) {
      return TRUE;
    }
  } else {
    SAGE_SetError(SERR_NULL_POINTER);
  }
  return FALSE;
}

/**
 * Allocate a new bitmap structure
 *
 * @param width         Bitmap width
 * @param height        Bitmap height
 * @param depth         Bitmap depth
 * @param pixformat     Pixel format
 * @param custom_bitmap Address of a custom bitmap to use as buffer
 *
 * @return SAGE Bitmap pointer
 */
SAGE_Bitmap * SAGE_AllocBitmap(ULONG width, ULONG height, ULONG depth, ULONG pixformat, APTR custom_bitmap)
{
  SAGE_Bitmap * bitmap = NULL;

  // Check for size constraint
  if (!SAGE_CheckSizeConstraint(width, depth)) {
    return NULL;
  }
  // Allocate and init bitmap
  bitmap = (SAGE_Bitmap *) SAGE_AllocMem(sizeof(SAGE_Bitmap));
  if (bitmap != NULL) {
    bitmap->properties = SBMP_NO_PROPERTY;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->depth = depth;
    bitmap->bpr = width * (depth / 8);
    bitmap->transparency = 0;
    bitmap->pixformat = pixformat;
    bitmap->bitmap_buffer = NULL;
    bitmap->first_buffer = NULL;
    bitmap->second_buffer = NULL;
    if (custom_bitmap != NULL) {
      bitmap->properties |= SBMP_CUSTOM;
      bitmap->bitmap_buffer = custom_bitmap;
      return bitmap;
    } else {
      bitmap->bitmap_buffer = SAGE_AllocMem(width * height * (depth / 8));
      if (bitmap->bitmap_buffer != NULL) {
        return bitmap;
      }
    }
    SAGE_ReleaseBitmap(bitmap);
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return NULL;
}

/**
 * Get the bitmap buffer address
 * 
 * @param bitmap SAGE bitmap pointer
 *
 * @return Bitmap buffer address
 */
APTR * SAGE_GetBitmapBuffer(SAGE_Bitmap * bitmap)
{
  if (bitmap != NULL) {
    return bitmap->bitmap_buffer;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return NULL;
}

/**
 * Release a bitmap resource
 *
 * @param bitmap SAGE bitmap pointer
 */
VOID SAGE_ReleaseBitmap(SAGE_Bitmap * bitmap)
{
  if (bitmap != NULL) {
    if (bitmap->first_buffer != NULL) {
      SAGE_FreeMem(bitmap->first_buffer);
    }
    if (bitmap->second_buffer != NULL) {
      SAGE_FreeMem(bitmap->second_buffer);
    }
    if (!(bitmap->properties & SBMP_CUSTOM) && bitmap->bitmap_buffer != NULL) {
      SAGE_FreeMem(bitmap->bitmap_buffer);
    }
    SAGE_FreeMem(bitmap);
  }
}

/**
 * Remap the transparency color into bitmap pixel format
 *
 * @param color     Transparency color in CLUT8, RGB16, RGB24 or ARGB format
 * @param pixformat The bitmap pixel format
 *
 * @return Remapped color
 */
ULONG SAGE_RemapTransparencyColor(ULONG color, ULONG pixformat)
{
  ULONG b1, b2, b3, b4;

  b1 = (color >> 24) & 255;
  b2 = (color >> 16) & 255;
  b3 = (color >> 8) & 255;
  b4 = color & 255;
  switch (pixformat) {
    case PIXFMT_CLUT:
      return (b4 << 24) | (b4 << 16) | (b4 << 8) | b4;
    case PIXFMT_RGB16:
      return (b3 << 24) | (b4 << 16) | (b3 << 8) | b4;
    case PIXFMT_RGB16PC:
      return (b4 << 24) | (b3 << 16) | (b4 << 8) | b3;
    case PIXFMT_BGR24:
      return (b4 << 16) | (b3 << 8) | b2;
    case PIXFMT_RGBA32:
      return (b2 << 24) | (b3 << 16) | (b4 << 8) | b1;
    default:
      return color;
  }
}

/**
 * Define the bitmap transparency color
 *
 * @param bitmap SAGE bitmap pointer
 * @param color  Transparent color
 *
 * @return Operation success
 */
BOOL SAGE_SetBitmapTransparency(SAGE_Bitmap * bitmap, ULONG color)
{
  if (bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  bitmap->properties |= SBMP_TRANSPARENT;
  bitmap->transparency = SAGE_RemapTransparencyColor(color, bitmap->pixformat);
  if (bitmap->depth == SBMP_DEPTH8 && bitmap->transparency == 0x00) {
    bitmap->properties |= SBMP_FASTCOPY;        // Activate fast 8bits cookie cut AMMX operation
    SD(SAGE_DebugLog("Activate fast 8bits cookie cut AMMX operation"));
  } else if (bitmap->depth == SBMP_DEPTH16 && bitmap->transparency == 0xF81FF81F) {
    bitmap->properties |= SBMP_FASTCOPY;        // Activate fast 16bits cookie cut AMMX operation
    SD(SAGE_DebugLog("Activate fast 16bits cookie cut AMMX operation"));
  }
  return TRUE;
}

/**
 * Clear a part of a 8bits bitmap buffer
 * 
 * @param bitmap SAGE bitmap pointer
 * @param left   Area left in pixel
 * @param top    Area top in pixel
 * @param width  Area width in pixel
 * @param height Area height in pixel
 */
VOID SAGE_Clear8BitsBitmap(SAGE_Bitmap * bitmap, ULONG left, ULONG top, ULONG width, ULONG height)
{
  UBYTE * buffer;
  ULONG offset;

  buffer = (UBYTE *) bitmap->bitmap_buffer;
  buffer += (left + (bitmap->width * top));
  offset = bitmap->width - width;
  SAGE_BlitClear8Bits(
    (ULONG)buffer,
    (UWORD)height,
    (UWORD)width,
    (ULONG)offset
  );
}

/**
 * Clear a part of a 16bits bitmap buffer
 * 
 * @param bitmap SAGE bitmap pointer
 * @param left   Area left in pixel
 * @param top    Area top in pixel
 * @param width  Area width in pixel
 * @param height Area height in pixel
 */
VOID SAGE_Clear16BitsBitmap(SAGE_Bitmap * bitmap, ULONG left, ULONG top, ULONG width, ULONG height)
{
  UWORD * buffer;
  ULONG offset;

  buffer = (UWORD *) bitmap->bitmap_buffer;
  buffer += (left + (bitmap->width * top));
  offset = (bitmap->width - width) * 2;
  SAGE_BlitClear16Bits(
    (ULONG)buffer,
    (UWORD)height,
    (UWORD)width,
    (ULONG)offset
  );
}

/**
 * Clear a part of a 24bits bitmap buffer
 * 
 * @param bitmap SAGE bitmap pointer
 * @param left   Area left in pixel
 * @param top    Area top in pixel
 * @param width  Area width in pixel
 * @param height Area height in pixel
 */
VOID SAGE_Clear24BitsBitmap(SAGE_Bitmap * bitmap, ULONG left, ULONG top, ULONG width, ULONG height)
{
  UBYTE * buffer, offset;

  buffer = (UBYTE *) bitmap->bitmap_buffer;
  buffer += ((left * 3) + ((bitmap->width * 3) * top));
  offset = (bitmap->width - width) * 3;
  SAGE_BlitClear24Bits(
    (ULONG)buffer,
    (UWORD)height,
    (UWORD)width,
    (ULONG)offset
  );
}

/**
 * Clear a part of a 32bits bitmap buffer
 * 
 * @param bitmap SAGE bitmap pointer
 * @param left   Area left in pixel
 * @param top    Area top in pixel
 * @param width  Area width in pixel
 * @param height Area height in pixel
 */
VOID SAGE_Clear32BitsBitmap(SAGE_Bitmap * bitmap, ULONG left, ULONG top, ULONG width, ULONG height)
{
  ULONG * buffer, offset;

  buffer = (ULONG *) bitmap->bitmap_buffer;
  buffer += (left + (bitmap->width * top));
  offset = bitmap->width - width;
  SAGE_BlitClear32Bits(
    (ULONG)buffer,
    (UWORD)height,
    (UWORD)width,
    (ULONG)offset
  );
}

/**
 * Clear a part of a bitmap
 * 
 * @param bitmap SAGE bitmap pointer
 * @param left   Area left in pixel
 * @param top    Area top in pixel
 * @param width  Area width in pixel
 * @param height Area height in pixel
 *
 * @return Operation success
 */
BOOL SAGE_ClearBitmap(SAGE_Bitmap * bitmap, ULONG left, ULONG top, ULONG width, ULONG height)
{
  if (bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_Clear8BitsBitmap(bitmap, left, top, width, height);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_Clear16BitsBitmap(bitmap, left, top, width, height);
  } else if (bitmap->depth == SBMP_DEPTH24) {
    SAGE_Clear24BitsBitmap(bitmap, left, top, width, height);
  } else if (bitmap->depth == SBMP_DEPTH32) {
    SAGE_Clear32BitsBitmap(bitmap, left, top, width, height);
  } else {
    SAGE_SetError(SERR_UNKNOWN_DEPTH);
    return FALSE;
  }
  return TRUE;
}

/**
 * Copy a part of a 8bits bitmap buffer into another 8bits bitmap buffer
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 */
VOID SAGE_Blit8BitsBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start)
{
  UBYTE * src_buffer, * dst_buffer;
  ULONG src_offset, dst_offset;

  src_buffer = (UBYTE *) source->bitmap_buffer;
  src_buffer += (left + (source->width * top));
  src_offset = source->width - width;
  dst_buffer = (UBYTE *) destination->bitmap_buffer;
  dst_buffer += (x_start + (destination->width * y_start));
  dst_offset = destination->width - width;
  if (source->properties & SBMP_TRANSPARENT) {
    if (SageContext.AmmxReady) {
      if (source->properties & SBMP_FASTCOPY) {
        SAGE_AMMXBlitCookieCut8Bits(
          (ULONG)src_buffer,
          (ULONG)dst_buffer,
          (UWORD)height,
          (UWORD)width,
          (ULONG)src_offset,
          (ULONG)dst_offset,
          (ULONG)source->transparency
        );
      } else {
        SAGE_AMMXBlitTranspCopy8Bits(
          (ULONG)src_buffer,
          (ULONG)dst_buffer,
          (UWORD)height,
          (UWORD)width,
          (ULONG)src_offset,
          (ULONG)dst_offset,
          (ULONG)source->transparency
        );
      }
    } else {
      SAGE_BlitTransparentCopy8Bits(
        (ULONG)src_buffer,
        (ULONG)dst_buffer,
        (UWORD)height,
        (UWORD)width,
        (ULONG)src_offset,
        (ULONG)dst_offset,
        (ULONG)source->transparency
      );
    }
  } else {
    SAGE_BlitCopy8Bits(
      (ULONG)src_buffer,
      (ULONG)dst_buffer,
      (UWORD)height,
      (UWORD)width,
      (ULONG)src_offset,
      (ULONG)dst_offset
    );
  }
}

/**
 * Copy a part of a 16bits bitmap buffer into another 16bits bitmap buffer
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 */
VOID SAGE_Blit16BitsBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start)
{
  UWORD * src_buffer, * dst_buffer;
  ULONG src_offset, dst_offset;

  src_buffer = (UWORD *) source->bitmap_buffer;
  src_buffer += (left + (source->width * top));
  src_offset = (source->width - width) * 2;
  dst_buffer = (UWORD *) destination->bitmap_buffer;
  dst_buffer += (x_start + (destination->width * y_start));
  dst_offset = (destination->width - width) * 2;
  if (source->properties & SBMP_TRANSPARENT) {
    if (SageContext.AmmxReady) {
      if (source->properties & SBMP_FASTCOPY) {
        SAGE_AMMXBlitCookieCut16Bits(
          (ULONG)src_buffer,
          (ULONG)dst_buffer,
          (UWORD)height,
          (UWORD)width,
          (ULONG)src_offset,
          (ULONG)dst_offset,
          (ULONG)source->transparency
        );
      } else {
        SAGE_AMMXBlitTranspCopy16Bits(
          (ULONG)src_buffer,
          (ULONG)dst_buffer,
          (UWORD)height,
          (UWORD)width,
          (ULONG)src_offset,
          (ULONG)dst_offset,
          (ULONG)source->transparency
        );
      }
    } else {
      SAGE_BlitTransparentCopy16Bits(
        (ULONG)src_buffer,
        (ULONG)dst_buffer,
        (UWORD)height,
        (UWORD)width,
        (ULONG)src_offset,
        (ULONG)dst_offset,
        (ULONG)source->transparency
      );
    }
  } else {
    SAGE_BlitCopy16Bits(
      (ULONG)src_buffer,
      (ULONG)dst_buffer,
      (UWORD)height,
      (UWORD)width,
      (ULONG)src_offset,
      (ULONG)dst_offset
    );
  }
}

/**
 * Copy a part of a 24bits bitmap buffer into another 24bits bitmap buffer
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 */
VOID SAGE_Blit24BitsBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start)
{
  UBYTE * src_buffer, * dst_buffer;
  ULONG src_offset, dst_offset, line, color, pixel = 0;

  src_buffer = (UBYTE *) source->bitmap_buffer;
  src_buffer += ((left * 3) + ((source->width * 3) * top));
  src_offset = (source->width - width) * 3;
  dst_buffer = (UBYTE *) destination->bitmap_buffer;
  dst_buffer += ((x_start * 3) + ((destination->width * 3) * y_start));
  dst_offset = (destination->width - width) * 3;
  if (source->properties & SBMP_TRANSPARENT) {
    color = (ULONG) source->transparency;
    while (height--) {
      line = width;
      while (line--) {
        pixel = *src_buffer;
        pixel <<= 8;
        pixel |= *src_buffer + 1;
        pixel <<= 8;
        pixel |= *src_buffer + 2;
        if (pixel != color) {
          *dst_buffer++ = *src_buffer++;
          *dst_buffer++ = *src_buffer++;
          *dst_buffer++ = *src_buffer++;
        }
      }
      src_buffer += src_offset;
      dst_buffer += dst_offset;
    }
  } else {
    while (height--) {
      line = width;
      while (line--) {
        *dst_buffer++ = *src_buffer++;
        *dst_buffer++ = *src_buffer++;
        *dst_buffer++ = *src_buffer++;
      }
      src_buffer += src_offset;
      dst_buffer += dst_offset;
    }
  }
}

/**
 * Copy a part of a 32bits bitmap buffer into another 32bits bitmap buffer
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 */
VOID SAGE_Blit32BitsBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start)
{
  ULONG * src_buffer, * dst_buffer;
  ULONG src_offset, dst_offset;

  src_buffer = (ULONG *) source->bitmap_buffer;
  src_buffer += (left + (source->width * top));
  src_offset = source->width - width;
  dst_buffer = (ULONG *) destination->bitmap_buffer;
  dst_buffer += (x_start + (destination->width * y_start));
  dst_offset = destination->width - width;
  if (source->properties & SBMP_TRANSPARENT) {
    SAGE_BlitTransparentCopy32Bits(
      (ULONG)src_buffer,
      (ULONG)dst_buffer,
      (UWORD)height,
      (UWORD)width,
      (ULONG)src_offset,
      (ULONG)dst_offset,
      (ULONG)source->transparency
    );
  } else {
    SAGE_BlitCopy32Bits(
      (ULONG)src_buffer,
      (ULONG)dst_buffer,
      (UWORD)height,
      (UWORD)width,
      (ULONG)src_offset,
      (ULONG)dst_offset
    );
  }
}

/**
 * Copy a part of a bitmap into another bitmap wihout clipping
 * Bitmaps should have the same pixel format
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 *
 * @return Operation success
 */
BOOL SAGE_BlitBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start)
{
  if (source == NULL || destination == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  // Blit only if we have the same depth
  if (source->depth == destination->depth) {
    if (source->depth == SBMP_DEPTH8) {
      SAGE_Blit8BitsBitmap(source, left, top, width, height, destination, x_start, y_start);
    } else if (source->depth == SBMP_DEPTH16) {
      SAGE_Blit16BitsBitmap(source, left, top, width, height, destination, x_start, y_start);
    } else if (source->depth == SBMP_DEPTH24) {
      SAGE_Blit24BitsBitmap(source, left, top, width, height, destination, x_start, y_start);
    } else if (source->depth == SBMP_DEPTH32) {
      SAGE_Blit32BitsBitmap(source, left, top, width, height, destination, x_start, y_start);
    } else {
      SAGE_SetError(SERR_UNKNOWN_DEPTH);
      return FALSE;
    }
  } else {
    SAGE_SetError(SERR_BM_BLITFMT);
    return FALSE;
  }
  return TRUE;
}

/**
 * Copy a part of a 8bits bitmap buffer into another 8bits bitmap buffer with zoom
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 * @param z_width     Zoom width
 * @param z_height    Zoom height
 */
VOID SAGE_Blit8BitsZoomedBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start, ULONG z_width, ULONG z_height)
{
  UBYTE * src_buffer, * dst_buffer;
  ULONG src_offset, dst_offset;

  src_buffer = (UBYTE *) source->bitmap_buffer;
  src_buffer += (left + (source->width * top));
  src_offset = source->width;
  dst_buffer = (UBYTE *) destination->bitmap_buffer;
  dst_buffer += (x_start + (destination->width * y_start));
  dst_offset = destination->width - z_width;
  if (source->properties & SBMP_TRANSPARENT) {
    SAGE_BlitTranspZoomCopy8Bits(
      (ULONG)src_buffer,
      (ULONG)width,
      (ULONG)height,
      (ULONG)src_offset,
      (ULONG)dst_buffer,
      (ULONG)z_width,
      (ULONG)z_height,
      (ULONG)dst_offset,
      (ULONG)source->transparency
    );
  } else {
    SAGE_BlitZoomCopy8Bits(
      (ULONG)src_buffer,
      (ULONG)width,
      (ULONG)height,
      (ULONG)src_offset,
      (ULONG)dst_buffer,
      (ULONG)z_width,
      (ULONG)z_height,
      (ULONG)dst_offset
    );
  }
}

/**
 * Copy a part of a 16bits bitmap buffer into another 16bits bitmap buffer with zoom
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 * @param z_width     Zoom width
 * @param z_height    Zoom height
 */
VOID SAGE_Blit16BitsZoomedBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start, ULONG z_width, ULONG z_height)
{
  UWORD * src_buffer, * dst_buffer;
  ULONG src_offset, dst_offset;

  src_buffer = (UWORD *) source->bitmap_buffer;
  src_buffer += (left + (source->width * top));
  src_offset = source->width * 2;
  dst_buffer = (UWORD *) destination->bitmap_buffer;
  dst_buffer += (x_start + (destination->width * y_start));
  dst_offset = (destination->width - z_width) * 2;
  if (source->properties & SBMP_TRANSPARENT) {
    SAGE_BlitTranspZoomCopy16Bits(
      (ULONG)src_buffer,
      (ULONG)width,
      (ULONG)height,
      (ULONG)src_offset,
      (ULONG)dst_buffer,
      (ULONG)z_width,
      (ULONG)z_height,
      (ULONG)dst_offset,
      (ULONG)source->transparency
    );
  } else {
    SAGE_BlitZoomCopy16Bits(
      (ULONG)src_buffer,
      (ULONG)width,
      (ULONG)height,
      (ULONG)src_offset,
      (ULONG)dst_buffer,
      (ULONG)z_width,
      (ULONG)z_height,
      (ULONG)dst_offset
    );
  }
}

/**
 * Copy a part of a bitmap into another bitmap wihout clipping
 * Bitmaps should have the same pixel format
 * 
 * @param source      SAGE bitmap pointer
 * @param left        Source left in pixel
 * @param top         Source top in pixel
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination SAGE bitmap pointer
 * @param x_start     X position of copy
 * @param y_start     Y position of copy
 * @param z_width     Zoom width
 * @param z_height    Zoom height
 *
 * @return Operation success
 */
BOOL SAGE_BlitZoomedBitmap(SAGE_Bitmap * source, ULONG left, ULONG top, ULONG width, ULONG height, SAGE_Bitmap * destination, ULONG x_start, ULONG y_start, ULONG z_width, ULONG z_height)
{
  if (source == NULL || destination == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  // Blit only if we have the same depth
  if (source->depth == destination->depth) {
    if (source->depth == SBMP_DEPTH8) {
      SAGE_Blit8BitsZoomedBitmap(source, left, top, width, height, destination, x_start, y_start, z_width, z_height);
    } else if (source->depth == SBMP_DEPTH16) {
      SAGE_Blit16BitsZoomedBitmap(source, left, top, width, height, destination, x_start, y_start, z_width, z_height);
    } else {
      SAGE_SetError(SERR_UNKNOWN_DEPTH);
      return FALSE;
    }
  } else {
    SAGE_SetError(SERR_BM_BLITFMT);
    return FALSE;
  }
  return TRUE;
}

/**
 * Map a 8bits bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param palette     Bitmap palette
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapCLUTToARGB32(UBYTE * source, ULONG * palette, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE pixel;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      pixel = *source++;
      *destination++ = palette[pixel] >> 24;
      *destination++ = palette[pixel] >> 16;
      *destination++ = palette[pixel] >> 8;
      *destination++ = palette[pixel];
    }
  }
}

/**
 * Map a 15bits RGB bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapRGB15ToARGB32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE red, green, blue;
  UWORD pixel;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      pixel = *source++;
      pixel <<= 8;
      pixel |= *source++;
      red = (pixel >> 10) & 31;
      green = (pixel >> 5) & 31;
      blue = pixel & 31;
      *destination++ = 0;
      *destination++ = red;
      *destination++ = green;
      *destination++ = blue;
    }
  }
}

/**
 * Map a 16bits RGB bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapRGB16ToARGB32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE red, green, blue;
  UWORD pixel;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      pixel = *source++;
      pixel <<= 8;
      pixel |= *source++;
      red = (pixel >> 11) & 31;
      green = (pixel >> 5) & 63;
      blue = pixel & 31;
      *destination++ = 0;
      *destination++ = red;
      *destination++ = green;
      *destination++ = blue;
    }
  }
}

/**
 * Map a 16bits RGBPC bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapRGB16PCToARGB32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE red, green, blue;
  UWORD pixel;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      pixel = *(source + 1);
      pixel <<= 8;
      pixel |= *source;
      red = (pixel >> 11) & 31;
      green = (pixel >> 5) & 63;
      blue = pixel & 31;
      *destination++ = 0;
      *destination++ = red;
      *destination++ = green;
      *destination++ = blue;
      source += 2;
    }
  }
}

/**
 * Map a 24bits RGB bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapRGB24ToARGB32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      *destination++ = 0;
      *destination++ = *source++;
      *destination++ = *source++;
      *destination++ = *source++;
    }
  }
}

/**
 * Map a 24bits BGR bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapBGR24ToARGB32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      *destination++ = 0;
      *destination++ = *(source + 2);
      *destination++ = *(source + 1);
      *destination++ = *source;
      source += 3;
    }
  }
}

/**
 * Map a 32bits RGBA bitmap buffer to a 32bits ARGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapRGBA32ToARGB32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      *destination++ = *(source + 1);
      *destination++ = *(source + 2);
      *destination++ = *(source + 3);
      *destination++ = *source;
      source += 4;
    }
  }
}

/**
 * Remap a bitmap to a 32bits ARGB pixel format
 *
 * @param bitmap  Bitmap to remap
 * @param palette Palette of CLUT bitmap or NULL if not CLUT
 *
 * @return Operation success
 */
BOOL SAGE_RemapToARGB32(SAGE_Bitmap * bitmap, ULONG * palette)
{
  APTR remap_buffer;

  remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 4);
  if (remap_buffer == NULL) {
    SAGE_SetError(SERR_NO_MEMORY);
    return FALSE;
  }
  switch (bitmap->pixformat) {
    case PIXFMT_CLUT:
      SAGE_MapCLUTToARGB32((UBYTE *) bitmap->bitmap_buffer, palette, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    case PIXFMT_RGB15:
      SAGE_MapRGB15ToARGB32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    case PIXFMT_RGB16:
      SAGE_MapRGB16ToARGB32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    case PIXFMT_RGB16PC:
      SAGE_MapRGB16PCToARGB32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    case PIXFMT_RGB24:
      SAGE_MapRGB24ToARGB32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    case PIXFMT_BGR24:
      SAGE_MapBGR24ToARGB32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    case PIXFMT_RGBA32:
      SAGE_MapRGBA32ToARGB32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      break;
    default:
      SAGE_SetError(SERR_NOT_AVAILABLE);
      SAGE_FreeMem(remap_buffer);
      return FALSE;
  }
  SAGE_FreeMem(bitmap->bitmap_buffer);
  bitmap->depth = SBMP_DEPTH32;
  bitmap->pixformat = PIXFMT_ARGB32;
  bitmap->bitmap_buffer = remap_buffer;
  return TRUE;
}

/**
 * Map a 32bits ARGB bitmap buffer to a 15bits RGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapARGB32ToRGB15(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE red, green, blue;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      source++;
      red = *source++;
      green = *source++;
      blue = *source++;
      *destination++ = ((red >> 1) & 124) | (green >> 6);
      *destination++ = ((green << 2) & 224) | (blue >> 3);
    }
  }
}

/**
 * Map a 32bits ARGB bitmap buffer to a 16bits RGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapARGB32ToRGB16(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE red, green, blue;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      source++;
      red = *source++;
      green = *source++;
      blue = *source++;
      *destination++ = (red & 248) | (green >> 5);
      *destination++ = ((green << 3) & 224) | (blue >> 3);
    }
  }
}

/**
 * Map a 32bits ARGB bitmap buffer to a 16bits RGBPC bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapARGB32ToRGB16PC(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  UBYTE red, green, blue;
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      source++;
      red = *source++;
      green = *source++;
      blue = *source++;
      *destination++ = ((green << 3) & 224) | (blue >> 3);
      *destination++ = (red & 248) | (green >> 5);
    }
  }
}

/**
 * Map a 32bits ARGB bitmap buffer to a 24bits RGB bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapARGB32ToRGB24(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      source++;
      *destination++ = *source++;
      *destination++ = *source++;
      *destination++ = *source++;
    }
  }
}

/**
 * Map a 32bits ARGB bitmap buffer to a 24bits BGR bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapARGB32ToBGR24(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      *destination++ = *(source + 3);
      *destination++ = *(source + 2);
      *destination++ = *(source + 1);
      source += 4;
    }
  }
}

/**
 * Map a 32bits ARGB bitmap buffer to a 32bits RGBA bitmap buffer
 *
 * @param source      Buffer pointer
 * @param width       Source width in pixel
 * @param height      Source height in pixel
 * @param destination Buffer pointer
 */
VOID SAGE_MapARGB32ToRGBA32(UBYTE * source, ULONG width, ULONG height, UBYTE * destination)
{
  ULONG line;
  
  while (height--) {
    line = width;
    while (line--) {
      *destination++ = *(source + 1);
      *destination++ = *(source + 2);
      *destination++ = *(source + 3);
      *destination++ = *source;
      source += 4;
    }
  }
}

/**
 * Remap a 32bits ARGB bitmap to another pixel format
 *
 * @param bitmap    Bitmap to remap
 * @param pixformat Pixel format to remap
 *
 * @return Operation success
 */
BOOL SAGE_RemapFromARGB32(SAGE_Bitmap * bitmap, ULONG pixformat)
{
  APTR remap_buffer;

  switch (pixformat) {
    case PIXFMT_RGB15:
      remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 2);
      if (remap_buffer == NULL) {
        SAGE_SetError(SERR_NO_MEMORY);
        return FALSE;
      }
      SAGE_MapARGB32ToRGB15((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      bitmap->depth = SBMP_DEPTH16;
      break;
    case PIXFMT_RGB16:
      remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 2);
      if (remap_buffer == NULL) {
        SAGE_SetError(SERR_NO_MEMORY);
        return FALSE;
      }
      SAGE_MapARGB32ToRGB16((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      bitmap->depth = SBMP_DEPTH16;
      break;
    case PIXFMT_RGB16PC:
      remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 2);
      if (remap_buffer == NULL) {
        SAGE_SetError(SERR_NO_MEMORY);
        return FALSE;
      }
      SAGE_MapARGB32ToRGB16PC((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      bitmap->depth = SBMP_DEPTH16;
      break;
    case PIXFMT_RGB24:
      remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 3);
      if (remap_buffer == NULL) {
        SAGE_SetError(SERR_NO_MEMORY);
        return FALSE;
      }
      SAGE_MapARGB32ToRGB24((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      bitmap->depth = SBMP_DEPTH24;
      break;
    case PIXFMT_BGR24:
      remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 3);
      if (remap_buffer == NULL) {
        SAGE_SetError(SERR_NO_MEMORY);
        return FALSE;
      }
      SAGE_MapARGB32ToBGR24((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      bitmap->depth = SBMP_DEPTH24;
      break;
    case PIXFMT_RGBA32:
      remap_buffer = SAGE_AllocMem(bitmap->width * bitmap->height * 4);
      if (remap_buffer == NULL) {
        SAGE_SetError(SERR_NO_MEMORY);
        return FALSE;
      }
      SAGE_MapARGB32ToRGBA32((UBYTE *) bitmap->bitmap_buffer, bitmap->width, bitmap->height, (UBYTE *) remap_buffer);
      bitmap->depth = SBMP_DEPTH32;
      break;
    default:
      SAGE_SetError(SERR_NOT_AVAILABLE);
      return FALSE;
  }
  SAGE_FreeMem(bitmap->bitmap_buffer);
  bitmap->pixformat = pixformat;
  bitmap->bitmap_buffer = remap_buffer;
  return TRUE;
}

/**
 * Remap a bitmap to another pixel format
 *
 * @param bitmap    Bitmap to remap
 * @param palette   Palette of CLUT bitmap or NULL if not CLUT
 * @param pixformat Pixel format to remap
 *
 * @return Operation success
 */
BOOL SAGE_RemapBitmap(SAGE_Bitmap * bitmap, ULONG * palette, ULONG pixformat)
{
  // Same format, nothing to do
  if (bitmap->pixformat == pixformat) {
    return TRUE;
  }
  // Pixel format undefined
  if (pixformat == PIXFMT_UNDEFINED) {
    SAGE_SetError(SERR_UNDEF_PIXFMT);
    return FALSE;
  }
  // No remap to 8bits format
  if (pixformat == PIXFMT_CLUT) {
    SAGE_SetError(SERR_BM_MAPPING);
    return FALSE;
  }
  // Map first to ARGB32 then to final pixel format
  if (bitmap->pixformat != PIXFMT_ARGB32) {
    if (!SAGE_RemapToARGB32(bitmap, palette)) {
      return FALSE;
    }
  }
  if (pixformat != PIXFMT_ARGB32) {
    if (!SAGE_RemapFromARGB32(bitmap, pixformat)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 *    DEBUG ONLY
 */

/**
 * Display the bitmap pixel format
 *
 * @var format Pixel format
 */
VOID SAGE_DumpPixelFormat(ULONG format)
{
  switch (format) {
    case PIXFMT_CLUT:
      SAGE_DebugLog(" Pixel format is LUT8 (%d)", format);
      break;
    case PIXFMT_RGB15:
      SAGE_DebugLog(" Pixel format is RGB15 (%d)", format);
      break;
    case PIXFMT_RGB15PC:
      SAGE_DebugLog(" Pixel format is RGB15PC (%d)", format);
      break;
    case PIXFMT_BGR15PC:
      SAGE_DebugLog(" Pixel format is BGR15PC (%d)", format);
      break;
    case PIXFMT_RGB16:
      SAGE_DebugLog(" Pixel format is RGB16 (%d)", format);
      break;
    case PIXFMT_RGB16PC:
      SAGE_DebugLog(" Pixel format is RGB16PC (%d)", format);
      break;
    case PIXFMT_BGR16PC:
      SAGE_DebugLog(" Pixel format is BGR16PC (%d)", format);
      break;
    case PIXFMT_RGB24:
      SAGE_DebugLog(" Pixel format is RGB24 (%d)", format);
      break;
    case PIXFMT_BGR24:
      SAGE_DebugLog(" Pixel format is BGR24 (%d)", format);
      break;
    case PIXFMT_ARGB32:
      SAGE_DebugLog(" Pixel format is ARGB32 (%d)", format);
      break;
    case PIXFMT_ABGR32:
      SAGE_DebugLog(" Pixel format is ABGR32 (%d)", format);
      break;
   case PIXFMT_RGBA32:
      SAGE_DebugLog(" Pixel format is RGBA32 (%d)", format);
      break;
    case PIXFMT_BGRA32:
      SAGE_DebugLog(" Pixel format is BGRA32 (%d)", format);
      break;
    default:
      SAGE_DebugLog(" Pixel format %d is undefined", format);
  }
}

/**
 * Dump a system bitmap structure
 *
 * @var bitmap Bitmap pointer
 */
VOID SAGE_DumpSystemBitmap(struct BitMap * bitmap)
{
  UWORD planes;

  SAGE_DebugLog("Dumping BitMap at 0x%X", bitmap);
  SAGE_DebugLog(" BytesPerRow           %d", bitmap->BytesPerRow);
  SAGE_DebugLog(" Rows                  %d", bitmap->Rows);
  SAGE_DebugLog(" Depth                 %d", bitmap->Depth);
  for (planes = 0;planes < 8;planes++) {
    SAGE_DebugLog(" Plane %d               0x%X", planes, bitmap->Planes[planes]);
  }
  if (GetCyberMapAttr(bitmap, CYBRMATTR_ISCYBERGFX)) {
    SAGE_DebugLog(" This is a CGX BitMap");
    SAGE_DebugLog(" Width                 %d", GetCyberMapAttr(bitmap, CYBRMATTR_WIDTH));
    SAGE_DebugLog(" Height                %d", GetCyberMapAttr(bitmap, CYBRMATTR_HEIGHT));
    SAGE_DebugLog(" Depth                 %d", GetCyberMapAttr(bitmap, CYBRMATTR_DEPTH));
    SAGE_DebugLog(" Bytes per row         %d", GetCyberMapAttr(bitmap, CYBRMATTR_XMOD));
    SAGE_DebugLog(" Bytes per pixel       %d", GetCyberMapAttr(bitmap, CYBRMATTR_BPPIX));
    SAGE_DumpPixelFormat(GetCyberMapAttr(bitmap, CYBRMATTR_PIXFMT));
    SAGE_GetBitmapAddress(bitmap);
  } else {
    SAGE_DebugLog(" This is not a CGX BitMap !");
  }
}

/**
 * Dump a SAGE bitmap structure
 *
 * @var bitmap SAGE bitmap pointer
 */
VOID SAGE_DumpBitmap(SAGE_Bitmap * bitmap)
{
  SAGE_DebugLog("Dumping SAGE BitMap at 0x%X", bitmap);
  SAGE_DebugLog(" Properties            %d", bitmap->properties);
  SAGE_DebugLog(" Width                 %d", bitmap->width);
  SAGE_DebugLog(" Height                %d", bitmap->height);
  SAGE_DebugLog(" Depth                 %d", bitmap->depth);
  SAGE_DebugLog(" Transparency          %d", bitmap->transparency);
  SAGE_DebugLog(" Pixel format          %s (%d)", SAGE_GetPixelFormatName(bitmap->pixformat), bitmap->pixformat);
  SAGE_DebugLog(" Buffer                0x%X", bitmap->bitmap_buffer);
}
