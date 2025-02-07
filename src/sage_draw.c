/**
 * sage_draw.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Graphics primitive drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
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

/********************************** DEBUG ONLY ********************************/

/**
 * Dump coordinates
 */
VOID SAGE_DumpCoords(STRPTR type, LONG *crd, ULONG nb)
{
  LONG index;
  
  SAGE_DebugLog("** Dump '%s' line coords (%d) **", type, nb);
  index = 0;
  nb++;
  while (nb--) {
    SAGE_DebugLog("- point %d = %d", index, crd[index]);
    index++;
  }
  SAGE_DebugLog("***************************");
}

/**
 * Dump line coords
 */
VOID SAGE_DumpLineCoords(LONG x1, LONG y1, LONG x2, LONG y2)
{
  LONG dx, dy, len, e;
  
  // Always draw from top to bottom
  if (y1 > y2) {
    dx = x2;
    dy = y2;
    x2 = x1;
    y2 = y1;
    x1 = dx;
    y1 = dy;
  }
  SAGE_DebugLog("** Dump line coords %d,%d to %d,%d", x1, y1, x2, y2);
  // Calculate delta
  dx = x2 - x1;
  dy = y2 - y1;
  // Let's draw
  if (dx > 0) {
    if (dy == 0) {
      SAGE_DebugLog("- HORIZONTAL + => DX(%d) > 0 & DY(%d) = 0", dx, dy);
      len = dx + 1;
      while (len--) {
        SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
        x1++;
      }
    } else {
      if (dx >= dy) {
        SAGE_DebugLog("- SOFT SLOPE + => DX(%d) > 0 & DX >= DY(%d)", dx, dy);
        len = dx + 1;
        e = dx;
        dx *= 2;
        dy *= 2;
        while (len--) {
          SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
          x1++;
          e -= dy;
          if (e < 0) {
            y1++;
            e += dx;
          }
        }
      } else {
        SAGE_DebugLog("- HARD SLOPE + => DX(%d) > 0 & DX <= DY(%d)", dx, dy);
        len = dy + 1;
        e = dy;
        dx *= 2;
        dy *= 2;
        while (len--) {
          SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
          y1++;
          e -= dx;
          if (e < 0) {
            x1++;
            e += dy;
          }
        }
      }
    }
  } else if (dx < 0) {
    dx *= -1;
    if (dy == 0) {
      SAGE_DebugLog("- HORIZONTAL - => DX(%d) < 0 & DY(%d) = 0", dx, dy);
      len = dx + 1;
      while (len--) {
        SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
        x1--;
      }
    } else {
      if (dx >= dy) {
        SAGE_DebugLog("- SOFT SLOPE - => DX(%d) < 0 & -DX >= DY(%d)", -dx, dy);
        len = dx + 1;
        e = dx;
        dx *= 2;
        dy *= 2;
        while (len--) {
          SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
          x1--;
          e -= dy;
          if (e < 0) {
            y1++;
            e += dx;
          }
        }
      } else {
        SAGE_DebugLog("- HARD SLOPE - => DX(%d) < 0 & -DX <= DY(%d)", -dx, dy);
        len = dy + 1;
        e = dy;
        dx *= 2;
        dy *= 2;
        while (len--) {
          SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
          y1++;
          e -= dx;
          if (e < 0) {
            x1--;
            e += dy;
          }
        }
      }
    }
  } else {
    if (dy > 0) {
      SAGE_DebugLog("- VERTICAL => DX(%d) = 0 & DY(%d) > 0", dx, dy);
      len = dy + 1;
      while (len--) {
        SAGE_DebugLog("- point %d = %d,%d", len, x1, y1);
        y1++;
      }
    }
  }
  SAGE_DebugLog("********************************************************************");
}

/********************************** DEBUG ONLY ********************************/

/**
 * Draw a pixel with clipping
 *
 * @param x     Pixel X coord
 * @param y     Pixel Y coord
 * @param color Pixel color
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
 * @param color Pixel color
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
 * @param color Line color
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
 * @param color Line color
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
 * @param color Triangle color
 *
 * @return Operation success
 */
BOOL SAGE_DrawTriangle(LONG x1, LONG y1, LONG x2, LONG y2, LONG x3, LONG y3, LONG color)
{
  SAGE_Bitmap *bitmap;
  UBYTE *buffer8;
  UWORD *buffer16;
  ULONG *buffer32;
  LONG swapx, swapy, *leftcoord, *rightcoord, *tempcoord;

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
    swapx = x1;
    swapy = y1;
    x1 = x2;
    y1 = y2;
    x2 = swapx;
    y2 = swapy;
  }
  if (y2 > y3) {
    swapx = x2;
    swapy = y2;
    x2 = x3;
    y2 = y3;
    x3 = swapx;
    y3 = swapy;
  }
  if (y1 > y2) {
    swapx = x1;
    swapy = y1;
    x1 = x2;
    y1 = y2;
    x2 = swapx;
    y2 = swapy;
  }
  // Calculate the triangle's edge
  if (y1 == y2) {
    if (x1 > x2) {
      swapx = x1;
      x1 = x2;
      x2 = swapx;
    }
    SAGE_FastLeftEdgeCalculation(leftcoord, x1, y1, x3, y3);
    SAGE_FastRightEdgeCalculation(rightcoord, x2, y2, x3, y3);
  } else if (y2 == y3) {
    if (x2 > x3) {
      swapx = x2;
      x2 = x3;
      x3 = swapx;
    }
    SAGE_FastLeftEdgeCalculation(leftcoord, x1, y1, x2, y2);
    SAGE_FastRightEdgeCalculation(rightcoord, x1, y1, x3, y3);
  } else {
    if (((x1 - x2) * (y1 - y3)) > ((x1 - x3) * (y1 - y2))) {
      SAGE_FastLeftEdgeCalculation(leftcoord, x1, y1, x3, y3);
      SAGE_FastRightEdgeCalculation(rightcoord, x1, y1, x2, y2);
      tempcoord = rightcoord + (y2 - y1);
      SAGE_FastRightEdgeCalculation(tempcoord, x2, y2, x3, y3);
    } else {
      SAGE_FastRightEdgeCalculation(rightcoord, x1, y1, x3, y3);
      SAGE_FastLeftEdgeCalculation(leftcoord, x1, y1, x2, y2);
      tempcoord = leftcoord + (y2 - y1);
      SAGE_FastLeftEdgeCalculation(tempcoord, x2, y2, x3, y3);
    }
  }
  // Let's draw
  if ((y3 - y1) > 0) {
    if (bitmap->depth == SBMP_DEPTH8) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += y1 * bitmap->width;
      SAGE_DrawFlatQuad8Bits(buffer8, leftcoord, rightcoord, y3 - y1, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH16) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += y1 * bitmap->width;
      SAGE_DrawFlatQuad16Bits(buffer16, leftcoord, rightcoord, y3 - y1, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH32) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += y1 * bitmap->width;
      SAGE_DrawFlatQuad32Bits(buffer32, leftcoord, rightcoord, y3 - y1, bitmap->bpr, color);
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
 * @param color Triangle color
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
  LONG swapx, swapy, *leftcoord, *rightcoord, *tempcoord, left_points, right_points;

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
    swapx = x1;
    swapy = y1;
    x1 = x2;
    y1 = y2;
    x2 = swapx;
    y2 = swapy;
  }
  if (y2 > y3) {
    swapx = x2;
    swapy = y2;
    x2 = x3;
    y2 = y3;
    x3 = swapx;
    y3 = swapy;
  }
  if (y1 > y2) {
    swapx = x1;
    swapy = y1;
    x1 = x2;
    y1 = y2;
    x2 = swapx;
    y2 = swapy;
  }
  // Calculate the triangle's edge
  if (y1 == y2) {
    if (x1 > x2) {
      swapx = x1;
      x1 = x2;
      x2 = swapx;
    }
    left_points = SAGE_FastClippedLeftEdgeCalc(leftcoord, x1, y1, x3, y3, &screen->clipping);
    right_points = SAGE_FastClippedRightEdgeCalc(rightcoord, x2, y2, x3, y3, &screen->clipping);
  } else if (y2 == y3) {
    if (x2 > x3) {
      swapx = x2;
      x2 = x3;
      x3 = swapx;
    }
    left_points = SAGE_FastClippedLeftEdgeCalc(leftcoord, x1, y1, x2, y2, &screen->clipping);
    right_points = SAGE_FastClippedRightEdgeCalc(rightcoord, x1, y1, x3, y3, &screen->clipping);
  } else {
    if (((x1 - x2) * (y1 - y3)) > ((x1 - x3) * (y1 - y2))) {
      left_points = SAGE_FastClippedLeftEdgeCalc(leftcoord, x1, y1, x3, y3, &screen->clipping);
      right_points = SAGE_FastClippedRightEdgeCalc(rightcoord, x1, y1, x2, y2, &screen->clipping);
      tempcoord = rightcoord + right_points;
      right_points += SAGE_FastClippedRightEdgeCalc(tempcoord, x2, y2, x3, y3, &screen->clipping);
    } else {
      right_points = SAGE_FastClippedRightEdgeCalc(rightcoord, x1, y1, x3, y3, &screen->clipping);
      left_points = SAGE_FastClippedLeftEdgeCalc(leftcoord, x1, y1, x2, y2, &screen->clipping);
      tempcoord = leftcoord + left_points;
      left_points += SAGE_FastClippedLeftEdgeCalc(tempcoord, x2, y2, x3, y3, &screen->clipping);
    }
  }
  if (left_points > 0) {
    // Get first line y coord
    if (y1 < screen->clipping.top) {
      y1 = screen->clipping.top;
    }
    // Draw and fill the polygon
    if (bitmap->depth == SBMP_DEPTH8) {
      buffer8 = (UBYTE *)bitmap->bitmap_buffer;
      buffer8 += y1 * bitmap->width;
      color &= 0xFF;
      color = (color << 24) + (color << 16) + (color << 8) + color;
      SAGE_DrawFlatQuad8Bits(buffer8, leftcoord, rightcoord, left_points, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH16) {
      buffer16 = (UWORD *)bitmap->bitmap_buffer;
      buffer16 += y1 * bitmap->width;
      color &= 0xFFFF;
      color = (color << 16) + color;
      SAGE_DrawFlatQuad16Bits(buffer16, leftcoord, rightcoord, left_points, bitmap->bpr, color);
    } else if (bitmap->depth == SBMP_DEPTH32) {
      buffer32 = (ULONG *)bitmap->bitmap_buffer;
      buffer32 += y1 * bitmap->width;
      SAGE_DrawFlatQuad32Bits(buffer32, leftcoord, rightcoord, left_points, bitmap->bpr, color);
    }
  }
  return TRUE;
}
