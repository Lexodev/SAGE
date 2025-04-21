/**
 * sage_draw.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Graphics primitive drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 April 2025 (updated: 01/04/2025)
 */

#include <exec/exec.h>

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_context.h>
#include <sage/sage_bitmap.h>
#include <sage/sage_draw.h>

/** SAGE context */
extern SAGE_Context SageContext;

VOID SAGE_DumpEdgeCoords(LONG starty, LONG *left_crd, LONG *right_crd, ULONG nb)
{
  LONG index;
  
  SAGE_TraceLog("***   Dump %d edge coords   ***", nb);
  index = 0;
  while (index < nb) {
    SAGE_TraceLog("- point %d = %d, %d => %d, %d", index, left_crd[index], starty, right_crd[index], starty);
    index++;
    starty++;
  }
  SAGE_TraceLog("***************************");
}

/**
 * Draw a pixel with clipping
 *
 * @param x     Pixel X coord
 * @param y     Pixel Y coord
 * @param color Pixel color (in the same format as the bitmap)
 *
 * @return Operation success
 */
BOOL SAGE_DrawClippedPixel(LONG x, LONG y, LONG color)
{
  SAGE_Screen *screen;
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;

  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  if (x < screen->clipping.left || x > screen->clipping.right) {
    return TRUE;
  }
  if (y < screen->clipping.top || y > screen->clipping.bottom) {
    return TRUE;
  }
  bitmap = screen->back_bitmap;
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  if (bitmap->depth == SBMP_DEPTH8) {
    buffer8 = (UBYTE *)bitmap->bitmap_buffer;
    buffer8 += (y * bitmap->width) + x;
    *buffer8 = (UBYTE)color;
  } else if (bitmap->depth == SBMP_DEPTH16) {
    buffer16 = (UWORD *)bitmap->bitmap_buffer;
    buffer16 += (y * bitmap->width) + x;
    *buffer16 = (UWORD)color;
  } else if (bitmap->depth == SBMP_DEPTH24) {
    buffer8 = (UBYTE *)bitmap->bitmap_buffer;
    buffer8 += (y * (bitmap->width * 3)) + (x * 3);
    *buffer8++ = (UBYTE)(color>>16) & 0xFF;
    *buffer8++ = (UBYTE)(color>>8) & 0xFF;
    *buffer8 = (UBYTE)color & 0xFF;
  } else if (bitmap->depth == SBMP_DEPTH32) {
    buffer32 = (ULONG *)bitmap->bitmap_buffer;
    buffer32 += (y * bitmap->width) + x;
    *buffer32 = (ULONG)color;
  }
  return TRUE;
}

/**
 * Draw a pixel without clipping
 *
 * @param x     Pixel X coord
 * @param y     Pixel Y coord
 * @param color Pixel color (in the same format as the bitmap)
 *
 * @return Operation success
 */
BOOL SAGE_DrawPixel(LONG x, LONG y, LONG color)
{
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;

  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  if (bitmap->depth == SBMP_DEPTH8) {
    buffer8 = (UBYTE *)bitmap->bitmap_buffer;
    buffer8 += (y * bitmap->width) + x;
    *buffer8 = (UBYTE)color;
  } else if (bitmap->depth == SBMP_DEPTH16) {
    buffer16 = (UWORD *)bitmap->bitmap_buffer;
    buffer16 += (y * bitmap->width) + x;
    *buffer16 = (UWORD)color;
  } else if (bitmap->depth == SBMP_DEPTH24) {
    buffer8 = (UBYTE *)bitmap->bitmap_buffer;
    buffer8 += (y * (bitmap->width * 3)) + (x * 3);
    *buffer8++ = (UBYTE)(color>>16) & 0xFF;
    *buffer8++ = (UBYTE)(color>>8) & 0xFF;
    *buffer8 = (UBYTE)color & 0xFF;
  } else if (bitmap->depth == SBMP_DEPTH32) {
    buffer32 = (ULONG *)bitmap->bitmap_buffer;
    buffer32 += (y * bitmap->width) + x;
    *buffer32 = (ULONG)color;
  }
  return TRUE;
}

/**
 * Draw an array of pixels without clipping
 *
 * @param pixels   Array of SAGE_Pixel
 * @param nbpixels Number of pixels to draw
 *
 * @return Operation success
 */
BOOL SAGE_DrawPixelArray(SAGE_Pixel *pixels, ULONG nbpixels)
{
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32, index;

  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  index = 0;
  if (bitmap->depth == SBMP_DEPTH8) {
    while (nbpixels--) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += (pixels[index].y * bitmap->width) + pixels[index].x;
      *buffer8 = (UBYTE)pixels[index++].color;
    }
  } else if (bitmap->depth == SBMP_DEPTH16) {
    while (nbpixels--) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += (pixels[index].y * bitmap->width) + pixels[index].x;
      *buffer16 = (UWORD)pixels[index++].color;
    }
  } else if (bitmap->depth == SBMP_DEPTH24) {
    while (nbpixels--) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += (pixels[index].y * (bitmap->width * 3)) + (pixels[index].x * 3);
      *buffer8++ = (UBYTE)(pixels[index].color>>16) & 0xFF;
      *buffer8++ = (UBYTE)(pixels[index].color>>8) & 0xFF;
      *buffer8 = (UBYTE)pixels[index++].color & 0xFF;
    }
  } else if (bitmap->depth == SBMP_DEPTH32) {
    while (nbpixels--) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += (pixels[index].y * bitmap->width) + pixels[index].x;
      *buffer32 = (ULONG)pixels[index++].color;
    }
  }
  return TRUE;
}

/**
 * Draw a line with clipping
 *
 * @param x1    Start line X coord
 * @param y1    Start line Y coord
 * @param x2    End line X coord
 * @param y2    End line Y coord
 * @param color Line color in ARGB/CLUT format
 *
 * @return Operation success
 */
BOOL SAGE_DrawClippedLine(LONG x1, LONG y1, LONG x2, LONG y2, LONG color)
{
  SAGE_Screen *screen;
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG dx, dy;

  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  // Always draw from top to bottom
  if (y1 > y2) {
    dx = x2;
    dy = y2;
    x2 = x1;
    y2 = y1;
    x1 = dx;
    y1 = dy;
  }
  // Vertical rejection
  if (y1 >= screen->clipping.bottom || y2 < screen->clipping.top) {
    return TRUE;
  }
  // Clipping top
  if (y1 < screen->clipping.top) {
    // X1' = X1 + ((CLIP_TOP - Y1) * (X2 - X1)) / (Y2 - Y1)
    x1 = x1 + ((screen->clipping.top - y1) * (x2 - x1) / (y2 - y1));
    y1 = screen->clipping.top;
  }
  // Clipping bottom
  if (y2 >= screen->clipping.bottom) {
    // X2' = X2 + ((Y2 - CLIP_BOTTOM) * (X1 - X2)) / (Y2 - Y1)
    x2 = x2 + ((y2 - screen->clipping.bottom) * (x1 - x2)) / (y2 - y1);
    y2 = screen->clipping.bottom;
  }
  // Horizontal rejection
  if (x1 < screen->clipping.left && x2 < screen->clipping.left) {
    return TRUE;
  }
  if (x1 >= screen->clipping.right && x2 >= screen->clipping.right) {
    return TRUE;
  }
  // Horizontal clipping
  if (x1 < x2) {
    if (x1 < screen->clipping.left) {
      // Y1' = Y1 + ((CLIP_LEFT - X1) * (Y2 - Y1)) / (X2 - X1)
      y1 = y1 + ((screen->clipping.left - x1) * (y2 - y1)) / (x2 - x1);
      x1 = screen->clipping.left;
    }
    if (x2 >= screen->clipping.right) {
      // Y2' = Y2 - ((X2 - CLIP_RIGHT) * (Y2 - Y1)) / (X2 - X1)
      y2 = y2 - ((x2 - screen->clipping.right) * (y2 - y1)) / (x2 - x1);
      x2 = screen->clipping.right;
    }
  } else {
    if (x1 >= screen->clipping.right) {
      // Y1' = Y1 + ((X1 - CLIP_RIGHT) * (Y2 - Y1)) / (X1 - X2)
      y1 = y1 + ((x1 - screen->clipping.right) * (y2 -y1)) / (x1 - x2);
      x1 = screen->clipping.right;
    }
    if (x2 < screen->clipping.left) {
      // Y2' = Y2 + ((CLIP_LEFT - X2) * (Y1 - Y2)) / (X1 - X2)
      y2 = y2 + ((screen->clipping.left - x2) * (y1 -y2)) / (x1 - x2);
      x2 = screen->clipping.left;
    }
  }
  // Calculate delta
  dx = x2 - x1;
  dy = y2 - y1;
  // Let's draw
  color = SAGE_RemapColorToPixFormat(color, bitmap->pixformat);
  if (bitmap->depth == SBMP_DEPTH8) {
    buffer8 = (UBYTE *)bitmap->bitmap_buffer;
    buffer8 += (y1 * bitmap->width) + x1;
    SAGE_FastLine8Bits(buffer8, dx, dy, bitmap->bpr, color);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    buffer16 = (UWORD *)bitmap->bitmap_buffer;
    buffer16 += (y1 * bitmap->width) + x1;
    SAGE_FastLine16Bits(buffer16, dx, dy, bitmap->bpr, color);
  } else if (bitmap->depth == SBMP_DEPTH24) {
    SAGE_SetError(SERR_NOT_AVAILABLE);
    return FALSE;
  } else if (bitmap->depth == SBMP_DEPTH32) {
    buffer32 = (ULONG *)bitmap->bitmap_buffer;
    buffer32 += (y1 * bitmap->width) + x1;
    SAGE_FastLine32Bits(buffer32, dx, dy, bitmap->bpr, color);
  }
  return TRUE;
}

/**
 * Draw a line without clipping
 *
 * @param x1    Start line X coord
 * @param y1    Start line Y coord
 * @param x2    End line X coord
 * @param y2    End line Y coord
 * @param color Line color in ARGB/CLUT format
 *
 * @return Operation success
 */
BOOL SAGE_DrawLine(LONG x1, LONG y1, LONG x2, LONG y2, LONG color)
{
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG dx, dy;

  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  // Always draw from top to bottom
  if (y1 > y2) {
    dx = x2;
    dy = y2;
    x2 = x1;
    y2 = y1;
    x1 = dx;
    y1 = dy;
  }
  // Calculate delta
  dx = x2 - x1;
  dy = y2 - y1;
  // Let's draw
  color = SAGE_RemapColorToPixFormat(color, bitmap->pixformat);
  if (bitmap->depth == SBMP_DEPTH8) {
    buffer8 = (UBYTE *)bitmap->bitmap_buffer;
    buffer8 += (y1 * bitmap->width) + x1;
    SAGE_FastLine8Bits(buffer8, dx, dy, bitmap->bpr, color);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    buffer16 = (UWORD *)bitmap->bitmap_buffer;
    buffer16 += (y1 * bitmap->width) + x1;
    SAGE_FastLine16Bits(buffer16, dx, dy, bitmap->bpr, color);
  } else if (bitmap->depth == SBMP_DEPTH24) {
    SAGE_SetError(SERR_NOT_AVAILABLE);
    return FALSE;
  } else if (bitmap->depth == SBMP_DEPTH32) {
    buffer32 = (ULONG *)bitmap->bitmap_buffer;
    buffer32 += (y1 * bitmap->width) + x1;
    SAGE_FastLine32Bits(buffer32, dx, dy, bitmap->bpr, color);
  }
  return TRUE;
}

/**
 * Draw a line strip without clipping
 * (second pixel of previous line is first pixel of next line)
 *
 * @param lines   Array of start line coords
 * @param nblines Number of lines to draw
 *
 * @return Operation success
 */
BOOL SAGE_DrawLineStrip(SAGE_Pixel *lines, ULONG nblines)
{
  return TRUE;
}

/**
 * Draw an array of lines without clipping
 *
 * @param lines   Array of SAGE_Line
 * @param nblines Number of lines to draw
 *
 * @return Operation success
 */
BOOL SAGE_DrawLineArray(SAGE_Line *lines, ULONG nblines)
{
  return TRUE;
}

/**
 * Draw a triangle
 *
 * @param x1    First point X
 * @param y1    First point Y
 * @param x2    Second point X
 * @param y2    Second point Y
 * @param x3    Third point X
 * @param y3    Third point Y
 * @param color Triangle color in CLUT/ARGB format
 *
 * @return Operation success
 */
BOOL SAGE_DrawTriangle(LONG x1, LONG y1, LONG x2, LONG y2, LONG x3, LONG y3, LONG color)
{
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG swap, *leftcoord, *rightcoord, nb_lines;

  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  leftcoord = bitmap->first_buffer;
  rightcoord = bitmap->second_buffer;
  SAFE(if (leftcoord == NULL || rightcoord == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  // Always draw from top to bottom
  if (y1 > y2) {
    swap = x1; x1 = x2; x2 = swap;
    swap = y1; y1 = y2; y2 = swap;
  }
  if (y2 > y3) {
    swap = x2; x2 = x3; x3 = swap;
    swap = y2; y2 = y3; y3 = swap;
  }
  if (y1 > y2) {
    swap = x1; x1 = x2; x2 = swap;
    swap = y1; y1 = y2; y2 = swap;
  }
  // Calculate the triangle's edge
  if (y1 == y2) {
    if (x1 > x2) {
      swap = x1; x1 = x2; x2 = swap;
    }
    SAGE_EdgeCalc(leftcoord, x1, y1, x3, y3);
    SAGE_EdgeCalc(rightcoord, x2, y2, x3, y3);
  } else if (y2 == y3) {
    if (x2 > x3) {
      swap = x2; x2 = x3; x3 = swap;
    }
    SAGE_EdgeCalc(leftcoord, x1, y1, x2, y2);
    SAGE_EdgeCalc(rightcoord, x1, y1, x3, y3);
  } else {
    if (((x1 - x2) * (y1 - y3)) > ((x1 - x3) * (y1 - y2))) {
      SAGE_EdgeCalc(leftcoord, x1, y1, x3, y3);
      SAGE_EdgeCalc(rightcoord, x1, y1, x2, y2);
      SAGE_EdgeCalc(rightcoord + (y2 - y1), x2, y2, x3, y3);
    } else {
      SAGE_EdgeCalc(rightcoord, x1, y1, x3, y3);
      SAGE_EdgeCalc(leftcoord, x1, y1, x2, y2);
      SAGE_EdgeCalc(leftcoord + (y2 - y1), x2, y2, x3, y3);
    }
  }
  // Let's draw
  nb_lines = y3 - y1;
  if (nb_lines > 0) {
    color = SAGE_RemapColorToPixFormat(color, bitmap->pixformat);
    if (bitmap->depth == SBMP_DEPTH8) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += y1 * bitmap->width;
      SAGE_DrawFlatQuad8Bits(buffer8, leftcoord, rightcoord, nb_lines, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH16) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += y1 * bitmap->width;
      SAGE_DrawFlatQuad16Bits(buffer16, leftcoord, rightcoord, nb_lines, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH32) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += y1 * bitmap->width;
      SAGE_DrawFlatQuad32Bits(buffer32, leftcoord, rightcoord, nb_lines, bitmap->bpr, color);
    }
  }
  return TRUE;
}

/**
 * Draw a clipped triangle
 *
 * @param x1    First point X
 * @param y1    First point Y
 * @param x2    Second point X
 * @param y2    Second point Y
 * @param x3    Third point X
 * @param y3    Third point Y
 * @param color Triangle color in CLUT/ARGB format
 *
 * @return Operation success
 */
BOOL SAGE_DrawClippedTriangle(LONG x1, LONG y1, LONG x2, LONG y2, LONG x3, LONG y3, LONG color)
{
  SAGE_Screen *screen;
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG swap, *leftcoord, *rightcoord, long_points, short_points, tclip, bclip;

  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  leftcoord = bitmap->first_buffer;
  rightcoord = bitmap->second_buffer;
  SAFE(if (leftcoord == NULL || rightcoord == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  // Always draw from top to bottom
  if (y1 > y2) {
    swap = x1; x1 = x2; x2 = swap;
    swap = y1; y1 = y2; y2 = swap;
  }
  if (y2 > y3) {
    swap = x2; x2 = x3; x3 = swap;
    swap = y2; y2 = y3; y3 = swap;
  }
  if (y1 > y2) {
    swap = x1; x1 = x2; x2 = swap;
    swap = y1; y1 = y2; y2 = swap;
  }
  SD(SAGE_TraceLog("SAGE_DrawClippedTriangle %d, %d, %d, %d, %d, %d", x1, y1, x2, y2, x3, y3);)
  tclip = screen->clipping.top;
  bclip = screen->clipping.bottom;
  // Calculate the triangle's edge
  if (y1 == y2) {
    if (x1 > x2) {
      swap = x1; x1 = x2; x2 = swap;
    }
    long_points = SAGE_ClippedEdgeCalc(leftcoord, x1, y1, x3, y3, tclip, bclip);
    SAGE_ClippedEdgeCalc(rightcoord, x2, y2, x3, y3, tclip, bclip);
  } else if (y2 == y3) {
    if (x2 > x3) {
      swap = x2; x2 = x3; x3 = swap;
    }
    long_points = SAGE_ClippedEdgeCalc(leftcoord, x1, y1, x2, y2, tclip, bclip);
    SAGE_ClippedEdgeCalc(rightcoord, x1, y1, x3, y3, tclip, bclip);
  } else {
    if (((x1 - x2) * (y1 - y3)) > ((x1 - x3) * (y1 - y2))) {
      long_points = SAGE_ClippedEdgeCalc(leftcoord, x1, y1, x3, y3, tclip, bclip);
      short_points = SAGE_ClippedEdgeCalc(rightcoord, x1, y1, x2, y2, tclip, bclip);
      SAGE_ClippedEdgeCalc((rightcoord + short_points), x2, y2, x3, y3, tclip, bclip);
    } else {
      long_points = SAGE_ClippedEdgeCalc(rightcoord, x1, y1, x3, y3, tclip, bclip);
      short_points = SAGE_ClippedEdgeCalc(leftcoord, x1, y1, x2, y2, tclip, bclip);
      SAGE_ClippedEdgeCalc((leftcoord + short_points), x2, y2, x3, y3, tclip, bclip);
    }
  }
  if (long_points > 0) {
    color = SAGE_RemapColorToPixFormat(color, bitmap->pixformat);
    // Get first line y coord
    if (y1 < tclip) {
      y1 = tclip;
    }
    SD(SAGE_DumpEdgeCoords(y1, leftcoord, rightcoord, long_points);)
    // Draw and fill the polygon
    if (bitmap->depth == SBMP_DEPTH8) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += y1 * bitmap->width;
      SAGE_DrawClippedFlatQuad8Bits(
          buffer8, leftcoord, rightcoord, long_points, bitmap->bpr, color, screen->clipping.left, screen->clipping.right
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += y1 * bitmap->width;
      SAGE_DrawClippedFlatQuad16Bits(
          buffer16, leftcoord, rightcoord, long_points, bitmap->bpr, color, screen->clipping.left, screen->clipping.right
      );
    } else if (bitmap->depth == SBMP_DEPTH32) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += y1 * bitmap->width;
      SAGE_DrawClippedFlatQuad32Bits(
          buffer32, leftcoord, rightcoord, long_points, bitmap->bpr, color, screen->clipping.left, screen->clipping.right
      );
    }
  }
  return TRUE;
}

/**
 * Draw a quad with flat top and flat bottom
 *
 * @param x1    Top left point X
 * @param x2    Top right point X
 * @param yt    Top coord
 * @param x3    Top right point X
 * @param x4    Bottom right point X
 * @param yb    Bottom coord
 * @param color Quad color in CLUT/ARGB format
 *
 * @return Operation success
 */
BOOL SAGE_DrawFlatQuad(LONG x1, LONG x2, LONG yt, LONG x3, LONG x4, LONG yb, LONG color)
{
  SAGE_Screen *screen;
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG *leftcoord, *rightcoord, nb_lines;

  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  leftcoord = bitmap->first_buffer;
  rightcoord = bitmap->second_buffer;
  SAFE(if (leftcoord == NULL || rightcoord == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  SAGE_EdgeCalc(leftcoord, x1, yt, x3, yb);
  SAGE_EdgeCalc(rightcoord, x2, yt, x4, yb);
  // Let's draw
  nb_lines = yb - yt;
  if (nb_lines > 0) {
    color = SAGE_RemapColorToPixFormat(color, bitmap->pixformat);
    if (bitmap->depth == SBMP_DEPTH8) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += yt * bitmap->width;
      SAGE_DrawFlatQuad8Bits(buffer8, leftcoord, rightcoord, nb_lines, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH16) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += yt * bitmap->width;
      SAGE_DrawFlatQuad16Bits(buffer16, leftcoord, rightcoord, nb_lines, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH32) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += yt * bitmap->width;
      SAGE_DrawFlatQuad32Bits(buffer32, leftcoord, rightcoord, nb_lines, bitmap->bpr, color);
    }
  }
  return TRUE;
}

/**
 * Draw a clipped quad with flat top and flat bottom
 *
 * @param x1    Top left point X
 * @param x2    Top right point X
 * @param yt    Top coord
 * @param x3    Top right point X
 * @param x4    Bottom right point X
 * @param yb    Bottom coord
 * @param color Quad CLUT/ARGB color
 *
 * @return Operation success
 */
BOOL SAGE_DrawClippedFlatQuad(LONG x1, LONG x2, LONG yt, LONG x3, LONG x4, LONG yb, LONG color)
{
  SAGE_Screen *screen;
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG *leftcoord, *rightcoord, nb_lines, tclip, bclip;

  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  bitmap = SAGE_GetBackBitmap();
  SAFE(if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return FALSE;
  })
  leftcoord = bitmap->first_buffer;
  rightcoord = bitmap->second_buffer;
  SAFE(if (leftcoord == NULL || rightcoord == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  tclip = screen->clipping.top;
  bclip = screen->clipping.bottom;
  nb_lines = SAGE_ClippedEdgeCalc(leftcoord, x1, yt, x3, yb, tclip, bclip);
  SAGE_ClippedEdgeCalc(rightcoord, x2, yt, x4, yb, tclip, bclip);
  // Let's draw
  if (nb_lines > 0) {
    color = SAGE_RemapColorToPixFormat(color, bitmap->pixformat);
    // Get first line y coord
    if (yt < tclip) {
      yt = tclip;
    }
    if (bitmap->depth == SBMP_DEPTH8) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += yt * bitmap->width;
      SAGE_DrawClippedFlatQuad8Bits(
          buffer8, leftcoord, rightcoord, nb_lines, bitmap->bpr, color, screen->clipping.left, screen->clipping.right
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += yt * bitmap->width;
      SAGE_DrawClippedFlatQuad16Bits(
          buffer16, leftcoord, rightcoord, nb_lines, bitmap->bpr, color, screen->clipping.left, screen->clipping.right
      );
    } else if (bitmap->depth == SBMP_DEPTH32) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += yt * bitmap->width;
      SAGE_DrawClippedFlatQuad32Bits(
          buffer32, leftcoord, rightcoord, nb_lines, bitmap->bpr, color, screen->clipping.left, screen->clipping.right
      );
    }
  }
  return TRUE;
}
