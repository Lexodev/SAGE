/**
 * sage_bitmap.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Bitmap management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_BITMAP_H_
#define _SAGE_BITMAP_H_

#include <exec/types.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>

// Bitmap depth
#define SBMP_DEPTH8           8UL
#define SBMP_DEPTH16          16UL
#define SBMP_DEPTH24          24UL
#define SBMP_DEPTH32          32UL

// Bitmap properties
#define SBMP_NO_PROPERTY      0UL
#define SBMP_TRANSPARENT      1UL
#define SBMP_CUSTOM           2UL

// Bitmap size constraint (in pixels)
#define SBMP_SIZE8BITS        8UL
#define SBMP_SIZE16BITS       4UL
#define SBMP_SIZE24BITS       4UL
#define SBMP_SIZE32BITS       2UL

// Pixel format
#define PIXFMT_PLANAR         99UL
#define PIXFMT_CLUT           PIXFMT_LUT8
/*#define PIXFMT_RGB15          RGBFB_R5G5B5
#define PIXFMT_BGR15          N/A
#define PIXFMT_RGB15PC        RGBFB_R5G5B5PC
#define PIXFMT_BGR15PC        RGBFB_B5G5R5PC
#define PIXFMT_RGB16          RGBFB_R5G6B5
#define PIXFMT_BGR16          N/A
#define PIXFMT_RGB16PC        RGBFB_R5G6B5PC
#define PIXFMT_BGR16PC        RGBFB_B5G6R5PC
#define PIXFMT_RGB24          RGBFB_R8G8B8
#define PIXFMT_BGR24          RGBFB_B8G8R8
#define PIXFMT_ARGB32         RGBFB_A8R8G8B8
#define PIXFMT_RGBA32         RGBFB_R8G8B8A8
#define PIXFMT_BGRA32         RGBFB_B8G8R8A8*/
#define PIXFMT_ABGR32         14UL
#define PIXFMT_UNDEFINED      100UL

/** SAGE bitmap */
typedef struct {
  /** Bitmap properties */
  LONGBITS properties;
  /** Bitmap width, height, depth and bytes per row */
  ULONG width, height, depth, bpr;
  /** Transparency color */
  ULONG transparency;
  /** Bitmap pixel format */
  ULONG pixformat;
  /** Bitmap buffer address */
  APTR bitmap_buffer;
  /** Fast drawing buffers */
  LONG * left_coords, * right_coords;
} SAGE_Bitmap;

/** Return the full name of pixel format */
STRPTR SAGE_GetPixelFormatName(ULONG);

/** Check for bitmap size constraint */
BOOL SAGE_CheckSizeConstraint(ULONG, ULONG);

/** Allocate fast draw buffers for the bitmap */
BOOL SAGE_AllocateFastDrawBuffers(SAGE_Bitmap *);

/** Allocate a bitmap structure */
SAGE_Bitmap * SAGE_AllocBitmap(ULONG, ULONG, ULONG, ULONG, APTR);

/** Get the bitmap buffer address */
APTR * SAGE_GetBitmapBuffer(SAGE_Bitmap *);

/** Release a bitmap structure */
VOID SAGE_ReleaseBitmap(SAGE_Bitmap *);

/** Set the bitmap transparency */
BOOL SAGE_SetBitmapTransparency(SAGE_Bitmap *, ULONG);

/** Clear the bitmap */
BOOL SAGE_ClearBitmap(SAGE_Bitmap *, ULONG, ULONG, ULONG, ULONG);

/** Blit a block from a bitmap to another */
BOOL SAGE_BlitBitmap(SAGE_Bitmap *, ULONG, ULONG, ULONG, ULONG, SAGE_Bitmap *, ULONG, ULONG);

/** Remap a bitmap buffer to another pixel format */
BOOL SAGE_RemapBitmap(SAGE_Bitmap *, ULONG *, ULONG);

/** Get the system bitmap address */
ULONG SAGE_GetBitmapAddress(struct BitMap *);

/** Get the system bitmap bytes per row */
ULONG SAGE_GetBitmapBPR(struct BitMap *);

/** Only for debug purpose, don't use it in your code */

/** DEBUG : Display the pixel format */
VOID SAGE_DumpPixelFormat(ULONG);

/** DEBUG : Dump the system bitmap attributes */
VOID SAGE_DumpSystemBitmap(struct BitMap *);

/** DEBUG : Dump the SAGE bitmap attributes */
VOID SAGE_DumpBitmap(SAGE_Bitmap *);

#endif
