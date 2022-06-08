/**
 * sage_3dtexmap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#include <exec/types.h>

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_bitmap.h"
#include "sage_3dtexture.h"
#include "sage_3dtexmap.h"

#define SAGE_MAPPER_ASM       1

/** Mapper data */
LONG s3dm_deltas[8], s3dm_coords[9];

/*****************************************************************************/
//            DEBUG ONLY
/*****************************************************************************/

VOID SAGE_DumpS3DTriangle(S3D_Triangle * triangle)
{
  SAGE_DebugLog("Dump S3D triangle");
  SAGE_DebugLog(" => x1=%d  y1=%d  z1=%d  u1=%d  v1=%d", triangle->x1, triangle->y1, triangle->z1, triangle->u1, triangle->v1);
  SAGE_DebugLog(" => x2=%d  y2=%d  z2=%d  u2=%d  v2=%d", triangle->x2, triangle->y2, triangle->z2, triangle->u2, triangle->v2);
  SAGE_DebugLog(" => x3=%d  y3=%d  z3=%d  u3=%d  v3=%d", triangle->x3, triangle->y3, triangle->z3, triangle->u3, triangle->v3);
}

VOID SAGE_DebugArrays(VOID)
{
  SAGE_TraceLog("-- Deltas");
  SAGE_TraceLog(" => dxdyl=%d  dudyl=%d  dvdyl=%d", s3dm_deltas[DELTA_DXDYL], s3dm_deltas[DELTA_DUDYL], s3dm_deltas[DELTA_DVDYL]);
  SAGE_TraceLog(" => dxdyr=%d  dudyr=%d  dvdyr=%d", s3dm_deltas[DELTA_DXDYR], s3dm_deltas[DELTA_DUDYR], s3dm_deltas[DELTA_DVDYR]);
  SAGE_TraceLog("-- Coords");
  SAGE_TraceLog(" => xl=%d  xr=%d", s3dm_coords[CRD_XL], s3dm_coords[CRD_XR]);
  SAGE_TraceLog(" => ul=%d  ul=%d", s3dm_coords[CRD_UL], s3dm_coords[CRD_UR]);
  SAGE_TraceLog(" => vl=%d  vr=%d", s3dm_coords[CRD_VL], s3dm_coords[CRD_VR]);
  SAGE_TraceLog(" => screen=%d", s3dm_coords[CRD_LINE]);
  SAGE_TraceLog(" => lclip=%d  rclip=%d", s3dm_coords[CRD_LCLIP], s3dm_coords[CRD_RCLIP]);
}

/*****************************************************************************/

/**
 * Map a 8bits texture
 *
 * @param nblines       Number of lines to map
 * @param texture       Texture buffer address
 * @param screen_buffer Screen buffer address
 * @param screen_width  Screen width in pixels
 */
VOID SAGE_TextureMapper8Bits(LONG nblines, UBYTE * texture, ULONG texture_width, UBYTE * screen_buffer, ULONG screen_width)
{
  LONG dx;
  LONG ui, vi, xs, xe, du, dv;
  LONG screen_pixel, texture_pixel;
  
  //SD(SAGE_TraceLog("SAGE_TextureMapper8Bits %d lines", nblines));

  while (nblines--) {

    SD(SAGE_TraceLog("-- Line %d", nblines));

    // Calcul edge coords
    xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe));

    if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {

      // Calcul texture interpolation
      du = s3dm_coords[CRD_UR] - s3dm_coords[CRD_UL];
      dv = s3dm_coords[CRD_VR] - s3dm_coords[CRD_VL];
      dx = xe - xs;
      if (dx > 0) {
        du /= dx;
        dv /= dx;
      }
      SD(SAGE_TraceLog(" => du=%d  dv=%d", du, dv));

      // Calcul texture coords
      ui = s3dm_coords[CRD_UL] + FIXP16_ROUND_UP;
      vi = s3dm_coords[CRD_VL] + FIXP16_ROUND_UP;

      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        dx = s3dm_coords[CRD_LCLIP] - xs;
        ui += dx * du;
        vi += dx * dv;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => ui=%d  vi=%d", ui, vi));

      // Pixel offset
      screen_pixel = s3dm_coords[CRD_LINE] + xs;
      SD(SAGE_TraceLog(" => pixel=%d", screen_pixel));

      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx));
      while (dx--) {

        // Write the texel
        texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
        screen_buffer[screen_pixel++] = texture[texture_pixel];

        // Interpolate u & v
        ui += du;
        vi += dv;
      }
    }

    // Interpolate next points
    s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
    s3dm_coords[CRD_UL] += s3dm_deltas[DELTA_DUDYL];
    s3dm_coords[CRD_VL] += s3dm_deltas[DELTA_DVDYL];
    s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
    s3dm_coords[CRD_UR] += s3dm_deltas[DELTA_DUDYR];
    s3dm_coords[CRD_VR] += s3dm_deltas[DELTA_DVDYR];

    // Next line address
    s3dm_coords[CRD_LINE] += screen_width;
  }
}

/**
 * Map a 16bits texture
 *
 * @param nblines       Number of lines to map
 * @param texture       Texture buffer address
 * @param screen_buffer Screen buffer address
 * @param screen_width  Screen width in pixels
 */
VOID SAGE_TextureMapper16Bits(LONG nblines, UWORD * texture, ULONG texture_width, UWORD * screen_buffer, ULONG screen_width)
{
  LONG dx;
  LONG ui, vi, xs, xe, du, dv;
  LONG screen_pixel, texture_pixel;

  SD(SAGE_TraceLog("SAGE_TextureMapper16Bits %d lines", nblines));

  while (nblines--) {

    SD(SAGE_TraceLog("-- Line %d", nblines));

    // Calcul edge coords
    xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe));

    if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {

      // Calcul texture interpolation
      du = s3dm_coords[CRD_UR] - s3dm_coords[CRD_UL];
      dv = s3dm_coords[CRD_VR] - s3dm_coords[CRD_VL];
      dx = xe - xs;
      if (dx > 0) {
        du /= dx;
        dv /= dx;
      }
      SD(SAGE_TraceLog(" => du=%d  dv=%d", du, dv));

      // Calcul texture coords
      ui = s3dm_coords[CRD_UL] + FIXP16_ROUND_UP;
      vi = s3dm_coords[CRD_VL] + FIXP16_ROUND_UP;

      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        dx = s3dm_coords[CRD_LCLIP] - xs;
        ui += dx * du;
        vi += dx * dv;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => ui=%d  vi=%d", ui, vi));

      // Pixel offset
      screen_pixel = s3dm_coords[CRD_LINE] + xs;
      SD(SAGE_TraceLog(" => pixel=%d", screen_pixel));

      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx));
      while (dx--) {

        // Write the texel
        texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
        screen_buffer[screen_pixel++] = texture[texture_pixel];

        // Interpolate u & v
        ui += du;
        vi += dv;
      }
    }

    // Interpolate next points
    s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
    s3dm_coords[CRD_UL] += s3dm_deltas[DELTA_DUDYL];
    s3dm_coords[CRD_VL] += s3dm_deltas[DELTA_DVDYL];
    s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
    s3dm_coords[CRD_UR] += s3dm_deltas[DELTA_DUDYR];
    s3dm_coords[CRD_VR] += s3dm_deltas[DELTA_DVDYR];

    // Next line address
    s3dm_coords[CRD_LINE] += screen_width;
  }
}

/**
 * Draw a textured flat top triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawFlatTop(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatTop"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d", triangle->x1, triangle->y1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d", triangle->x2, triangle->y2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d", triangle->x3, triangle->y3));

  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }

  // Left side delta
  s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy;

  // Right side delta
  s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy;

  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;

  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy + (triangle->x2 << FIXP16_SHIFT);
    s3dm_coords[CRD_UL] = s3dm_deltas[DELTA_DUDYL] * dy + (triangle->u1 << FIXP16_SHIFT);
    s3dm_coords[CRD_VL] = s3dm_deltas[DELTA_DVDYL] * dy + (triangle->v1 << FIXP16_SHIFT);
    s3dm_coords[CRD_UR] = s3dm_deltas[DELTA_DUDYR] * dy + (triangle->u2 << FIXP16_SHIFT);
    s3dm_coords[CRD_VR] = s3dm_deltas[DELTA_DVDYR] * dy + (triangle->v2 << FIXP16_SHIFT);
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_coords[CRD_XL] = triangle->x1 << FIXP16_SHIFT;
    s3dm_coords[CRD_XR] = triangle->x2 << FIXP16_SHIFT;
    s3dm_coords[CRD_UL] = triangle->u1 << FIXP16_SHIFT;
    s3dm_coords[CRD_VL] = triangle->v1 << FIXP16_SHIFT;
    s3dm_coords[CRD_UR] = triangle->u2 << FIXP16_SHIFT;
    s3dm_coords[CRD_VR] = triangle->v2 << FIXP16_SHIFT;
    // Screen start address
    s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
  }

  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }

  // Go for mapping
  SD(SAGE_DebugArrays());
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_FastMap8BitsTexture(
        dy+1,
        (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->bpr,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_FastMap16BitsTexture(
        dy+1,
        (UWORD *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->bpr,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_TextureMapper8Bits(
        dy+1,
        (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->width
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_TextureMapper16Bits(
        dy+1,
        (UWORD *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->width
    );
  }
#endif
  SD(SAGE_DebugArrays());

}

/**
 * Draw a textured flat bottom triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawFlatBottom(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatBottom"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d", triangle->x1, triangle->y1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d", triangle->x2, triangle->y2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d", triangle->x3, triangle->y3));

  // Delta height
  dy = triangle->y2 - triangle->y1;
  if (dy <= 0) {
    return;
  }

  // Left side delta
  s3dm_deltas[DELTA_DXDYL] = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYL] = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy;  
  s3dm_deltas[DELTA_DVDYL] = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy;

  // Right side delta
  s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy;  
  s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy;

  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;

  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_UL] = s3dm_deltas[DELTA_DUDYL] * dy + (triangle->u1 << FIXP16_SHIFT);
    s3dm_coords[CRD_VL] = s3dm_deltas[DELTA_DVDYL] * dy + (triangle->v1 << FIXP16_SHIFT);
    s3dm_coords[CRD_UR] = s3dm_deltas[DELTA_DUDYR] * dy + (triangle->u1 << FIXP16_SHIFT);
    s3dm_coords[CRD_VR] = s3dm_deltas[DELTA_DVDYR] * dy + (triangle->v1 << FIXP16_SHIFT);
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_coords[CRD_XL] = triangle->x1 << FIXP16_SHIFT;
    s3dm_coords[CRD_XR] = triangle->x1 << FIXP16_SHIFT;
    s3dm_coords[CRD_UL] = triangle->u1 << FIXP16_SHIFT;
    s3dm_coords[CRD_VL] = triangle->v1 << FIXP16_SHIFT;
    s3dm_coords[CRD_UR] = triangle->u1 << FIXP16_SHIFT;
    s3dm_coords[CRD_VR] = triangle->v1 << FIXP16_SHIFT;
    // Screen start address
    s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
  }

  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }

  // Go for mapping
  SD(SAGE_DebugArrays());
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_FastMap8BitsTexture(
        dy+1,
        (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->bpr,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_FastMap16BitsTexture(
        dy+1,
        (UWORD *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->bpr,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_TextureMapper8Bits(
        dy,
        (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->width
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_TextureMapper16Bits(
        dy,
        (UWORD *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->width
    );
  }
#endif

}

/**
 * Draw a textured generic triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawGeneric(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping)
{
  LONG dy1, dy2, dy3, dxdy1, dxdy2, slope;

  SD(SAGE_TraceLog("-- SAGE_DrawGeneric"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d", triangle->x1, triangle->y1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d", triangle->x2, triangle->y2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d", triangle->x3, triangle->y3));

  // Delta height
  dy1 = triangle->y2 - triangle->y1;
  dy2 = triangle->y3 - triangle->y1;
  if (dy1 <= 0 || dy2 <= 0) {
    return;
  }

  // Delta 1
  dxdy1 = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy1;
  // Delta 2
  dxdy2 = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy2;
  if (dxdy1 > dxdy2) {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = dxdy2;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = dxdy1;
    // Left side texture delta
    s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy2;
    s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy2;
    // Right side texture delta
    s3dm_deltas[DELTA_DUDYR] = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy1;
    s3dm_deltas[DELTA_DVDYR] = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy1;
    // Slope, left long
    slope = 0;
  } else {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = dxdy1;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = dxdy2;
    // Left side texture delta
    s3dm_deltas[DELTA_DUDYL] = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy1;
    s3dm_deltas[DELTA_DVDYL] = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy1;
    // Right side texture delta
    s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy2;
    s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy2;
    // Slope, right long
    slope = 1;
  }

  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;

  // y2 top clipping
  if (triangle->y2 < clipping->top) {
    dy1 = clipping->top - triangle->y1;
    dy2 = clipping->top - triangle->y2;
    dy3 = triangle->y3 - triangle->y2;
    if (dy3 <= 0) {
      return;
    }
    // Calcul new deltas and coords
    if (slope == 1) {
      s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_UL] = s3dm_deltas[DELTA_DUDYL] * dy2 + (triangle->u2 << FIXP16_SHIFT);
      s3dm_coords[CRD_VL] = s3dm_deltas[DELTA_DVDYL] * dy2 + (triangle->v2 << FIXP16_SHIFT);
      s3dm_coords[CRD_UR] = s3dm_deltas[DELTA_DUDYR] * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_coords[CRD_VR] = s3dm_deltas[DELTA_DVDYR] * dy1 + (triangle->v1 << FIXP16_SHIFT);
    } else {
      s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_coords[CRD_UL] = s3dm_deltas[DELTA_DUDYL] * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_coords[CRD_VL] = s3dm_deltas[DELTA_DVDYL] * dy1 + (triangle->v1 << FIXP16_SHIFT);
      s3dm_coords[CRD_UR] = s3dm_deltas[DELTA_DUDYR] * dy2 + (triangle->u2 << FIXP16_SHIFT);
      s3dm_coords[CRD_VR] = s3dm_deltas[DELTA_DVDYR] * dy2 + (triangle->v2 << FIXP16_SHIFT);
    }
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy3 = triangle->y3 - clipping->top;
    // Bottom clipping
    if (triangle->y3 >= clipping->bottom) {
      dy3 -= (triangle->y3 - clipping->bottom);
    }
    // Go for mapping
    SD(SAGE_DebugArrays());
#if SAGE_MAPPER_ASM == 1
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_FastMap8BitsTexture(
          dy3,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_FastMap16BitsTexture(
          dy3,
          (UWORD *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    }
#else
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_TextureMapper8Bits(
          dy3,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->width,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->width
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_TextureMapper16Bits(
          dy3,
          (UWORD *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->width,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->width
      );
    }
#endif
  } else {
    // y1 top clipping
    if (triangle->y1 < clipping->top) {
      dy1 = clipping->top - triangle->y1;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_UL] = s3dm_deltas[DELTA_DUDYL] * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_coords[CRD_VL] = s3dm_deltas[DELTA_DVDYL] * dy1 + (triangle->v1 << FIXP16_SHIFT);
      s3dm_coords[CRD_UR] = s3dm_deltas[DELTA_DUDYR] * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_coords[CRD_VR] = s3dm_deltas[DELTA_DVDYR] * dy1 + (triangle->v1 << FIXP16_SHIFT);
      // Screen start address
      s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
      dy1 = triangle->y2 - clipping->top;
    } else {
      s3dm_coords[CRD_XL] = triangle->x1 << FIXP16_SHIFT;
      s3dm_coords[CRD_XR] = triangle->x1 << FIXP16_SHIFT;
      s3dm_coords[CRD_UL] = triangle->u1 << FIXP16_SHIFT;
      s3dm_coords[CRD_VL] = triangle->v1 << FIXP16_SHIFT;
      s3dm_coords[CRD_UR] = triangle->u1 << FIXP16_SHIFT;
      s3dm_coords[CRD_VR] = triangle->v1 << FIXP16_SHIFT;
      // Screen start address
      s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
    }
    if (triangle->y2 >= clipping->bottom) {
      dy1 -= (triangle->y2 - clipping->bottom);
      // Go for mapping
      SD(SAGE_DebugArrays());
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_FastMap8BitsTexture(
            dy1,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_FastMap16BitsTexture(
            dy1,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits(
            dy1,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits(
            dy1,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width
        );
      }
#endif
    } else {
      // Go for mapping
      SD(SAGE_DebugArrays());
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_FastMap8BitsTexture(
            dy1,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_FastMap16BitsTexture(
            dy1,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits(
            dy1,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits(
            dy1,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width
        );
      }
#endif
      dy3 = triangle->y3 - triangle->y2;
      if (dy3 <= 0) {
        return;
      }
      // Calcul new deltas
      if (slope == 1) {
        s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      } else {
        s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      }
      // Screen start address
      s3dm_coords[CRD_LINE] = triangle->y2 * bitmap->width;
      // Bottom clipping
      if (triangle->y3 >= clipping->bottom) {
        dy3 -= (triangle->y3 - clipping->bottom);
      }
      // Go for mapping
      SD(SAGE_DebugArrays());
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_FastMap8BitsTexture(
            dy3,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_FastMap16BitsTexture(
            dy3,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits(
            dy3,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits(
            dy3,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width
        );
      }
#endif
    }
  }
}

/**
 * Draw a textured triangle
 */
BOOL SAGE_DrawTexturedTriangle(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping)
{
  LONG tx, ty, tu, tv;

  SD(SAGE_TraceLog("---- SAGE_DrawTexturedTriangle"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d u1=%d v1=%d", triangle->x1, triangle->y1, triangle->u1, triangle->v1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d u2=%d v2=%d", triangle->x2, triangle->y2, triangle->u2, triangle->v2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d u3=%d v3=%d", triangle->x3, triangle->y3, triangle->u3, triangle->v3));

  // Degenerated triangle elimination
  if (triangle->x1 == triangle->x2 && triangle->x2 == triangle->x3) {
    return TRUE;
  }
  if (triangle->y1 == triangle->y2 && triangle->y2 == triangle->y3) {
    return TRUE;
  }
  SD(SAGE_TraceLog(" => not degenerated"));
  // Order triangle vertices
  if (triangle->y1 > triangle->y3) {
    tx = triangle->x1; ty = triangle->y1; tu = triangle->u1; tv = triangle->v1;
    triangle->x1 = triangle->x3; triangle->y1 = triangle->y3; triangle->u1 = triangle->u3; triangle->v1 = triangle->v3;
    triangle->x3 = tx; triangle->y3 = ty; triangle->u3 = tu; triangle->v3 = tv;
  }
  if (triangle->y1 > triangle->y2) {
    tx = triangle->x1; ty = triangle->y1; tu = triangle->u1; tv = triangle->v1;
    triangle->x1 = triangle->x2; triangle->y1 = triangle->y2; triangle->u1 = triangle->u2; triangle->v1 = triangle->v2;
    triangle->x2 = tx; triangle->y2 = ty; triangle->u2 = tu; triangle->v2 = tv;
  }
  if (triangle->y2 > triangle->y3) {
    tx = triangle->x2; ty = triangle->y2; tu = triangle->u2; tv = triangle->v2;
    triangle->x2 = triangle->x3; triangle->y2 = triangle->y3; triangle->u2 = triangle->u3; triangle->v2 = triangle->v3;
    triangle->x3 = tx; triangle->y3 = ty; triangle->u3 = tu; triangle->v3 = tv;
  }
  // Trivial rejection
  if (triangle->y3 < clipping->top || triangle->y1 >= clipping->bottom) {
    return TRUE;
  }
  if (triangle->x1 < clipping->left && triangle->x2 < clipping->left && triangle->x3 < clipping->left) {
    return TRUE;
  }
  if (triangle->x1 >= clipping->right && triangle->x2 >= clipping->right && triangle->x3 >= clipping->right) {
    return TRUE;
  }
  SD(SAGE_TraceLog(" => not rejected"));
  // Render triangle depending on his type
  if (triangle->y1 == triangle->y2) {
    if (triangle->x1 > triangle->x2) {
      tx = triangle->x1; ty = triangle->y1; tu = triangle->u1; tv = triangle->v1;
      triangle->x1 = triangle->x2; triangle->y1 = triangle->y2; triangle->u1 = triangle->u2; triangle->v1 = triangle->v2;
      triangle->x2 = tx; triangle->y2 = ty; triangle->u2 = tu; triangle->v2 = tv;
    }
    SAGE_DrawFlatTop(triangle, bitmap, clipping);
  } else if (triangle->y2 == triangle->y3) {
    if (triangle->x2 > triangle->x3) {
      tx = triangle->x2; ty = triangle->y2; tu = triangle->u2; tv = triangle->v2;
      triangle->x2 = triangle->x3; triangle->y2 = triangle->y3; triangle->u2 = triangle->u3; triangle->v2 = triangle->v3;
      triangle->x3 = tx; triangle->y3 = ty; triangle->u3 = tu; triangle->v3 = tv;
    }
    SAGE_DrawFlatBottom(triangle, bitmap, clipping);
  } else {
    SAGE_DrawGeneric(triangle, bitmap, clipping);
  }
  return TRUE;
}
