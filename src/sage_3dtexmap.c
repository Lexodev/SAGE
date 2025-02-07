/**
 * sage_3dtexmap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
 */

#include <exec/types.h>

#include <sage/sage_logger.h>
#include <sage/sage_bitmap.h>
#include <sage/sage_3dtexture.h>
#include <sage/sage_3dtexmap.h>
#include <sage/sage_context.h>

#include <sage/sage_debug.h>

#include <proto/graphics.h>

#define SAGE_MAPPER_ASM       0

#define FIXEDTOFLOAT(x)       ((FLOAT)(x/65536)+(FLOAT)((x&0xffff)*(1.0/65536)))

/** SAGE context */
extern SAGE_Context SageContext;

/** Mapper data */
LONG s3dm_deltas[11], s3dm_coords[13];
SAGE_TextureMapping s3dm_texmap;

/*****************************************************************************
 *                   START DEBUG
 *****************************************************************************/

#if _SAGE_DEBUG_MODE_ == 1
VOID SAGE_DumpS3DTriangle(S3D_Triangle *triangle)
{
  SAGE_DebugLog("Dump S3D triangle");
  SAGE_DebugLog(" => x1=%d  y1=%d  z1=%d  u1=%d  v1=%d", triangle->x1, triangle->y1, triangle->z1, triangle->u1, triangle->v1);
  SAGE_DebugLog(" => x2=%d  y2=%d  z2=%d  u2=%d  v2=%d", triangle->x2, triangle->y2, triangle->z2, triangle->u2, triangle->v2);
  SAGE_DebugLog(" => x3=%d  y3=%d  z3=%d  u3=%d  v3=%d", triangle->x3, triangle->y3, triangle->z3, triangle->u3, triangle->v3);
}

VOID SAGE_DebugArrays(VOID)
{
  SAGE_DebugLog("-- Deltas");
  SAGE_DebugLog(" => dxdyl=%d  dzdyl=%d  dudyl=%d  dvdyl=%d", s3dm_deltas[DELTA_DXDYL], s3dm_deltas[DELTA_DZDYL], s3dm_deltas[DELTA_DUDYL], s3dm_deltas[DELTA_DVDYL]);
  SAGE_DebugLog(" => dxdyr=%d  dzdyr=%d  dudyr=%d  dvdyr=%d", s3dm_deltas[DELTA_DXDYR], s3dm_deltas[DELTA_DZDYR], s3dm_deltas[DELTA_DUDYR], s3dm_deltas[DELTA_DVDYR]);
  SAGE_DebugLog("-- Coords");
  SAGE_DebugLog(" => xl=%d  xr=%d", s3dm_coords[CRD_XL], s3dm_coords[CRD_XR]);
  SAGE_DebugLog(" => zl=%d  zr=%d", s3dm_coords[CRD_ZL], s3dm_coords[CRD_ZR]);
  SAGE_DebugLog(" => ul=%d  ur=%d", s3dm_coords[CRD_UL], s3dm_coords[CRD_UR]);
  SAGE_DebugLog(" => vl=%d  vr=%d", s3dm_coords[CRD_VL], s3dm_coords[CRD_VR]);
  SAGE_DebugLog(" => screen=%d", s3dm_coords[CRD_LINE]);
  SAGE_DebugLog(" => lclip=%d  rclip=%d", s3dm_coords[CRD_LCLIP], s3dm_coords[CRD_RCLIP]);
}
#endif

/*****************************************************************************
 *                   END DEBUG
 *****************************************************************************/

/**
 * Map a 8bits color
 *
 * @param nblines       Number of lines to map
 * @param color         Color value
 * @param screen_buffer Screen buffer address
 * @param screen_width  Screen width in pixels
 * @param z_buffer      Z buffer address
 */
VOID SAGE_ColorMapper8Bits(LONG nblines, ULONG color, UBYTE *screen_buffer, ULONG screen_width, UWORD *z_buffer)
{
  LONG dx, dz, xs, xe, zi;
  LONG screen_pixel;
  
  SD(SAGE_TraceLog("SAGE_ColorMapper8Bits %d lines", nblines);)
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_coords[CRD_XL], s3dm_coords[CRD_XR], s3dm_coords[CRD_ZL], s3dm_coords[CRD_ZR]);)
    // Calcul edge coords
    xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {
      // Calcul interpolation
      dz = s3dm_coords[CRD_ZR] - s3dm_coords[CRD_ZL];
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => dz=0x%X", dz);)
      // Calcul Z value
      zi = s3dm_coords[CRD_ZL] + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_coords[CRD_LCLIP] - xs;
        zi += dx * dz;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  zi=0x%X", xs, zi);)
      // Pixel offset
      screen_pixel = s3dm_coords[CRD_LINE] + xs;
      SD(SAGE_TraceLog(" => pixel=%d", screen_pixel);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (z_buffer != NULL) {
        SD(SAGE_TraceLog(" => zbuffer=0x%X", z_buffer[screen_pixel]);)
        while (dx--) {
          // Compare with zbuffer
          if (z_buffer[screen_pixel] > (UWORD)(zi >> FIXP16_SHIFT)) {
            z_buffer[screen_pixel] = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the color
            screen_buffer[screen_pixel] = (UBYTE)color;
          }
          screen_pixel++;
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the color
          screen_buffer[screen_pixel++] = (UBYTE)color;
        }
      }
    }
    // Interpolate next points
    s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
    s3dm_coords[CRD_ZL] += s3dm_deltas[DELTA_DZDYL];
    s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
    s3dm_coords[CRD_ZR] += s3dm_deltas[DELTA_DZDYR];
    // Next line address
    s3dm_coords[CRD_LINE] += screen_width;
  }
}

/**
 * Map a 8bits texture
 *
 * @param nblines       Number of lines to map
 * @param texture       Texture buffer address
 * @param screen_buffer Screen buffer address
 * @param screen_width  Screen width in pixels
 * @param z_buffer      Z buffer address
 */
VOID SAGE_TextureMapper8Bits(LONG nblines, UBYTE *texture, ULONG texture_width, UBYTE *screen_buffer, ULONG screen_width, UWORD *z_buffer)
{
  LONG dx, du, dv, dz;
  LONG ui, vi, xs, xe, zi;
  LONG screen_pixel, texture_pixel;
  
  SD(SAGE_TraceLog("SAGE_TextureMapper8Bits %d lines", nblines);)
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_coords[CRD_XL], s3dm_coords[CRD_XR], s3dm_coords[CRD_ZL], s3dm_coords[CRD_ZR]);)
    SD(SAGE_TraceLog(" => ul=0x%X  ur=0x%X  vl=0x%X  vr=0x%X", s3dm_coords[CRD_UL], s3dm_coords[CRD_UR], s3dm_coords[CRD_VL], s3dm_coords[CRD_VR]);)
    // Calcul edge coords
    xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {
      // Calcul interpolation
      du = s3dm_coords[CRD_UR] - s3dm_coords[CRD_UL];
      dv = s3dm_coords[CRD_VR] - s3dm_coords[CRD_VL];
      dz = s3dm_coords[CRD_ZR] - s3dm_coords[CRD_ZL];
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        du /= dx;
        dv /= dx;
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => du=0x%X  dv=0x%X  dz=0x%X", du, dv, dz);)
      // Calcul texture coords
      ui = s3dm_coords[CRD_UL] + FIXP16_ROUND_UP;
      vi = s3dm_coords[CRD_VL] + FIXP16_ROUND_UP;
      // Calcul Z value
      zi = s3dm_coords[CRD_ZL] + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_coords[CRD_LCLIP] - xs;
        ui += dx * du;
        vi += dx * dv;
        zi += dx * dz;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  ui=0x%X  vi=0x%X  zi=0x%X", xs, ui, vi, zi);)
      // Pixel offset
      screen_pixel = s3dm_coords[CRD_LINE] + xs;
      SD(SAGE_TraceLog(" => pixel=%d", screen_pixel);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (z_buffer != NULL) {
        SD(SAGE_TraceLog(" => zbuffer=0x%X", z_buffer[screen_pixel]);)
        while (dx--) {
          // Compare with zbuffer
          if (z_buffer[screen_pixel] > (UWORD)(zi >> FIXP16_SHIFT)) {
            z_buffer[screen_pixel] = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
            screen_buffer[screen_pixel] = texture[texture_pixel];
          }
          screen_pixel++;
          // Interpolate u & v
          ui += du;
          vi += dv;
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the texel
          texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
          screen_buffer[screen_pixel++] = texture[texture_pixel];
          // Interpolate u & v
          ui += du;
          vi += dv;
        }
      }
    }
    // Interpolate next points
    s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
    s3dm_coords[CRD_ZL] += s3dm_deltas[DELTA_DZDYL];
    s3dm_coords[CRD_UL] += s3dm_deltas[DELTA_DUDYL];
    s3dm_coords[CRD_VL] += s3dm_deltas[DELTA_DVDYL];
    s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
    s3dm_coords[CRD_ZR] += s3dm_deltas[DELTA_DZDYR];
    s3dm_coords[CRD_UR] += s3dm_deltas[DELTA_DUDYR];
    s3dm_coords[CRD_VR] += s3dm_deltas[DELTA_DVDYR];
    // Next line address
    s3dm_coords[CRD_LINE] += screen_width;
  }
}

/**
 * Map a 16bits color
 *
 * @param nblines       Number of lines to map
 * @param color         Color value
 * @param screen_buffer Screen buffer address
 * @param screen_width  Screen width in pixels
 * @param z_buffer      Z buffer address
 */
VOID SAGE_ColorMapper16Bits(LONG nblines, ULONG color, UWORD *screen_buffer, ULONG screen_width, UWORD *z_buffer)
{
  LONG dx, dz, xs, xe, zi;
  LONG screen_pixel;

  SD(SAGE_TraceLog("SAGE_ColorMapper16Bits %d lines", nblines);)
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_coords[CRD_XL], s3dm_coords[CRD_XR], s3dm_coords[CRD_ZL], s3dm_coords[CRD_ZR]);)
    // Calcul edge coords
    xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {
      // Calcul interpolation
      dz = s3dm_coords[CRD_ZR] - s3dm_coords[CRD_ZL];
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => dz=0x%X", dz);)
      // Calcul Z value
      zi = s3dm_coords[CRD_ZL] + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_coords[CRD_LCLIP] - xs;
        zi += dx * dz;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  zi=0x%X", xs, zi);)
      // Pixel offset
      screen_pixel = s3dm_coords[CRD_LINE] + xs;
      SD(SAGE_TraceLog(" => pixel=%d", screen_pixel);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (z_buffer != NULL) {
        SD(SAGE_TraceLog(" => zbuffer=0x%X", z_buffer[screen_pixel]);)
        while (dx--) {
          // Compare with zbuffer
          if (z_buffer[screen_pixel] > (UWORD)(zi >> FIXP16_SHIFT)) {
            z_buffer[screen_pixel] = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            screen_buffer[screen_pixel] = (UWORD)color;
          }
          screen_pixel++;
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the texel
          screen_buffer[screen_pixel++] = (UWORD)color;
        }
      }
    }
    // Interpolate next points
    s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
    s3dm_coords[CRD_ZL] += s3dm_deltas[DELTA_DZDYL];
    s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
    s3dm_coords[CRD_ZR] += s3dm_deltas[DELTA_DZDYR];
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
 * @param z_buffer      Z buffer address
 */
VOID SAGE_TextureMapper16Bits(LONG nblines, UWORD *texture, ULONG texture_width, UWORD *screen_buffer, ULONG screen_width, UWORD *z_buffer)
{
  LONG dx, du, dv, dz;
  LONG ui, vi, xs, xe, zi;
  LONG screen_pixel, texture_pixel;

  SD(SAGE_TraceLog("SAGE_TextureMapper16Bits %d lines", nblines);)
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_coords[CRD_XL], s3dm_coords[CRD_XR], s3dm_coords[CRD_ZL], s3dm_coords[CRD_ZR]);)
    SD(SAGE_TraceLog(" => ul=0x%X  ur=0x%X  vl=0x%X  vr=0x%X", s3dm_coords[CRD_UL], s3dm_coords[CRD_UR], s3dm_coords[CRD_VL], s3dm_coords[CRD_VR]);)
    // Calcul edge coords
    xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {
      // Calcul interpolation
      du = s3dm_coords[CRD_UR] - s3dm_coords[CRD_UL];
      dv = s3dm_coords[CRD_VR] - s3dm_coords[CRD_VL];
      dz = s3dm_coords[CRD_ZR] - s3dm_coords[CRD_ZL];
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        du /= dx;
        dv /= dx;
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => du=0x%X  dv=0x%X  dz=0x%X", du, dv, dz);)
      // Calcul texture coords
      ui = s3dm_coords[CRD_UL] + FIXP16_ROUND_UP;
      vi = s3dm_coords[CRD_VL] + FIXP16_ROUND_UP;
      // Calcul Z value
      zi = s3dm_coords[CRD_ZL] + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_coords[CRD_LCLIP] - xs;
        ui += dx * du;
        vi += dx * dv;
        zi += dx * dz;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  ui=0x%X  vi=0x%X  zi=0x%X", xs, ui, vi, zi);)
      // Pixel offset
      screen_pixel = s3dm_coords[CRD_LINE] + xs;
      SD(SAGE_TraceLog(" => pixel=%d", screen_pixel);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (z_buffer != NULL) {
        SD(SAGE_TraceLog(" => zbuffer=0x%X", z_buffer[screen_pixel]);)
        while (dx--) {
          // Compare with zbuffer
          if (z_buffer[screen_pixel] > (UWORD)(zi >> FIXP16_SHIFT)) {
            z_buffer[screen_pixel] = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
            screen_buffer[screen_pixel] = texture[texture_pixel];
          }
          screen_pixel++;
          // Interpolate u & v
          ui += du;
          vi += dv;
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the texel
          texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
          screen_buffer[screen_pixel++] = texture[texture_pixel];
          // Interpolate u & v
          ui += du;
          vi += dv;
        }
      }
    }
    // Interpolate next points
    s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
    s3dm_coords[CRD_ZL] += s3dm_deltas[DELTA_DZDYL];
    s3dm_coords[CRD_UL] += s3dm_deltas[DELTA_DUDYL];
    s3dm_coords[CRD_VL] += s3dm_deltas[DELTA_DVDYL];
    s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
    s3dm_coords[CRD_ZR] += s3dm_deltas[DELTA_DZDYR];
    s3dm_coords[CRD_UR] += s3dm_deltas[DELTA_DUDYR];
    s3dm_coords[CRD_VR] += s3dm_deltas[DELTA_DVDYR];
    // Next line address
    s3dm_coords[CRD_LINE] += screen_width;
  }
}

/*****************************************************************************/

/**
 * Draw a colored flat top triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawFlatTopColored(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatTopColored");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy + (triangle->x2 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy + (triangle->z2 << FIXP16_SHIFT);
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_coords[CRD_XL] = triangle->x1 << FIXP16_SHIFT;
    s3dm_coords[CRD_XR] = triangle->x2 << FIXP16_SHIFT;
    s3dm_coords[CRD_ZL] = triangle->z1 << FIXP16_SHIFT;
    s3dm_coords[CRD_ZR] = triangle->z2 << FIXP16_SHIFT;
    // Screen start address
    s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;
  // Go for mapping
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_FastMap8BitsColor(
        dy+1,
        triangle->color,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_FastMap16BitsColor(
        dy+1,
        triangle->color,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_ColorMapper8Bits(
        dy+1,
        triangle->color,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_ColorMapper16Bits(
        dy+1,
        triangle->color,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  }
#endif
}

/**
 * Draw a textured flat top triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawFlatTopTextured(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatTopTextured");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy + (triangle->x2 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy + (triangle->z2 << FIXP16_SHIFT);
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
    s3dm_coords[CRD_ZL] = triangle->z1 << FIXP16_SHIFT;
    s3dm_coords[CRD_ZR] = triangle->z2 << FIXP16_SHIFT;
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
  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;
  // Go for mapping
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
      s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
      SAGE_FastMap8BitsTransparent(
          dy+1,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    } else {
      SAGE_FastMap8BitsTexture(
          dy+1,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    }
  } else if (bitmap->depth == SBMP_DEPTH16) {
    if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
      s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
      SAGE_FastMap16BitsTransparent(
          dy+1,
          (UWORD *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    } else {
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
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_TextureMapper8Bits(
        dy+1,
        (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_TextureMapper16Bits(
        dy+1,
        (UWORD *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  }
#endif
}

/**
 * Draw a colored flat bottom triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawFlatBottomColored(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatBottomColored");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y2 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_deltas[DELTA_DXDYL] = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYL] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy + (triangle->z1 << FIXP16_SHIFT);
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_coords[CRD_XL] = triangle->x1 << FIXP16_SHIFT;
    s3dm_coords[CRD_XR] = triangle->x1 << FIXP16_SHIFT;
    s3dm_coords[CRD_ZL] = triangle->z1 << FIXP16_SHIFT;
    s3dm_coords[CRD_ZR] = triangle->z1 << FIXP16_SHIFT;
    // Screen start address
    s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;
  // Go for mapping
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_FastMap8BitsColor(
        dy+1,
        triangle->color,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_FastMap16BitsColor(
        dy+1,
        triangle->color,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords
    );
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_ColorMapper8Bits(
        dy,
        triangle->color,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_ColorMapper16Bits(
        dy,
        triangle->color,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  }
#endif
}

/**
 * Draw a textured flat bottom triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawFlatBottomTextured(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatBottomTextured");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y2 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_deltas[DELTA_DXDYL] = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYL] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYL] = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DVDYL] = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy + (triangle->z1 << FIXP16_SHIFT);
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
    s3dm_coords[CRD_ZL] = triangle->z1 << FIXP16_SHIFT;
    s3dm_coords[CRD_ZR] = triangle->z1 << FIXP16_SHIFT;
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
  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;
  // Go for mapping
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
      s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
      SAGE_FastMap8BitsTransparent(
          dy+1,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    } else {
      SAGE_FastMap8BitsTexture(
          dy+1,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    }
  } else if (bitmap->depth == SBMP_DEPTH16) {
    if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
      s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
      SAGE_FastMap16BitsTransparent(
          dy+1,
          (UWORD *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->bpr,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    } else {
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
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_TextureMapper8Bits(
        dy,
        (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UBYTE *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_TextureMapper16Bits(
        dy,
        (UWORD *) triangle->tex->bitmap->bitmap_buffer,
        triangle->tex->bitmap->width,
        (UWORD *) bitmap->bitmap_buffer,
        bitmap->width,
        (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
    );
  }
#endif
}

/**
 * Draw a colored generic triangle
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawGenericColored(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy1, dy2, dy3, dxdy1, dxdy2, slope;

  SD(SAGE_TraceLog("-- SAGE_DrawGenericColored");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
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
  // Define slope
  if (dxdy1 > dxdy2) {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = dxdy2;
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = dxdy1;
    s3dm_deltas[DELTA_DZDYR] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    // Slope, left long
    SD(SAGE_TraceLog(" => left long slope");)
    slope = 0;
  } else {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = dxdy1;
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = dxdy2;
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    // Slope, right long
    SD(SAGE_TraceLog(" => right long slope");)
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
      s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy2 + (triangle->z2 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy1 + (triangle->z1 << FIXP16_SHIFT);
    } else {
      s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy2 + (triangle->z2 << FIXP16_SHIFT);
    }
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy3 = triangle->y3 - clipping->top;
    // Bottom clipping
    if (triangle->y3 >= clipping->bottom) {
      dy3 -= (triangle->y3 - clipping->bottom);
    }
    // Go for mapping
#if SAGE_MAPPER_ASM == 1
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_FastMap8BitsColor(
          dy3,
          triangle->color,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_FastMap16BitsColor(
          dy3,
          triangle->color,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords
      );
    }
#else
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_ColorMapper8Bits(
          dy3,
          triangle->color,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->width,
          (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_ColorMapper16Bits(
          dy3,
          triangle->color,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->width,
          (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
      );
    }
#endif
  } else {
    // y1 top clipping
    if (triangle->y1 < clipping->top) {
      dy1 = clipping->top - triangle->y1;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy1 + (triangle->z1 << FIXP16_SHIFT);
      // Screen start address
      s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
      dy1 = triangle->y2 - clipping->top;
    } else {
      s3dm_coords[CRD_XL] = triangle->x1 << FIXP16_SHIFT;
      s3dm_coords[CRD_XR] = triangle->x1 << FIXP16_SHIFT;
      s3dm_coords[CRD_ZL] = triangle->z1 << FIXP16_SHIFT;
      s3dm_coords[CRD_ZR] = triangle->z1 << FIXP16_SHIFT;
      // Screen start address
      s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
    }
    if (triangle->y2 >= clipping->bottom) {
      dy1 -= (triangle->y2 - clipping->bottom);
      // Go for mapping
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_FastMap8BitsColor(
            dy1,
            triangle->color,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_FastMap16BitsColor(
            dy1,
            triangle->color,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_ColorMapper8Bits(
            dy1,
            triangle->color,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_ColorMapper16Bits(
            dy1,
            triangle->color,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      }
#endif
    } else {
      // Go for mapping
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_FastMap8BitsColor(
            dy1,
            triangle->color,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_FastMap16BitsColor(
            dy1,
            triangle->color,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_ColorMapper8Bits(
            dy1,
            triangle->color,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_ColorMapper16Bits(
            dy1,
            triangle->color,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
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
        s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      } else {
        s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      }
      // Screen start address
      s3dm_coords[CRD_LINE] = triangle->y2 * bitmap->width;
      // Bottom clipping
      if (triangle->y3 >= clipping->bottom) {
        dy3 -= (triangle->y3 - clipping->bottom);
      }
      // Go for mapping
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_FastMap8BitsColor(
            dy3,
            triangle->color,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_FastMap16BitsColor(
            dy3,
            triangle->color,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_ColorMapper8Bits(
            dy3,
            triangle->color,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_ColorMapper16Bits(
            dy3,
            triangle->color,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      }
#endif
    }
  }
}

/**
 * Draw a textured generic triangle
 *
 * @param triangle Triangle to draw
 * @param bitmap   Destination bitmap
 * @param clipping Clipping informations
 *
 */
VOID SAGE_DrawGenericTextured(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy1, dy2, dy3, dxdy1, dxdy2, slope;

  SD(SAGE_TraceLog("-- SAGE_DrawGenericTextured");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
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
  // Define slope
  if (dxdy1 > dxdy2) {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = dxdy2;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = dxdy1;
    // Left side texture delta
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy2;
    s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy2;
    // Right side texture delta
    s3dm_deltas[DELTA_DZDYR] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    s3dm_deltas[DELTA_DUDYR] = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy1;
    s3dm_deltas[DELTA_DVDYR] = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy1;
    // Slope, left long
    SD(SAGE_TraceLog(" => left long slope");)
    slope = 0;
  } else {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = dxdy1;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = dxdy2;
    // Left side texture delta
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    s3dm_deltas[DELTA_DUDYL] = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy1;
    s3dm_deltas[DELTA_DVDYL] = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy1;
    // Right side texture delta
    s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy2;
    s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy2;
    // Slope, right long
    SD(SAGE_TraceLog(" => right long slope");)
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
      s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy2 + (triangle->z2 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_coords[CRD_UL] = s3dm_deltas[DELTA_DUDYL] * dy2 + (triangle->u2 << FIXP16_SHIFT);
      s3dm_coords[CRD_VL] = s3dm_deltas[DELTA_DVDYL] * dy2 + (triangle->v2 << FIXP16_SHIFT);
      s3dm_coords[CRD_UR] = s3dm_deltas[DELTA_DUDYR] * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_coords[CRD_VR] = s3dm_deltas[DELTA_DVDYR] * dy1 + (triangle->v1 << FIXP16_SHIFT);
    } else {
      s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DUDYR] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
      s3dm_deltas[DELTA_DVDYR] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy2 + (triangle->z2 << FIXP16_SHIFT);
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
#if SAGE_MAPPER_ASM == 1
    if (bitmap->depth == SBMP_DEPTH8) {
      if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
        s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
        SAGE_FastMap8BitsTransparent(
            dy3,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else {
        SAGE_FastMap8BitsTexture(
            dy3,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      }
    } else if (bitmap->depth == SBMP_DEPTH16) {
      if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
        s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
        SAGE_FastMap16BitsTransparent(
            dy3,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->bpr,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords
        );
      } else {
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
    }
#else
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_TextureMapper8Bits(
          dy3,
          (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->width,
          (UBYTE *) bitmap->bitmap_buffer,
          bitmap->width,
          (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
      );
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_TextureMapper16Bits(
          dy3,
          (UWORD *) triangle->tex->bitmap->bitmap_buffer,
          triangle->tex->bitmap->width,
          (UWORD *) bitmap->bitmap_buffer,
          bitmap->width,
          (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
      );
    }
#endif
  } else {
    // y1 top clipping
    if (triangle->y1 < clipping->top) {
      dy1 = clipping->top - triangle->y1;
      s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZL] = s3dm_deltas[DELTA_DZDYL] * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_coords[CRD_ZR] = s3dm_deltas[DELTA_DZDYR] * dy1 + (triangle->z1 << FIXP16_SHIFT);
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
      s3dm_coords[CRD_ZL] = triangle->z1 << FIXP16_SHIFT;
      s3dm_coords[CRD_ZR] = triangle->z1 << FIXP16_SHIFT;
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
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
          s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
          SAGE_FastMap8BitsTransparent(
              dy1,
              (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UBYTE *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        } else {
          SAGE_FastMap8BitsTexture(
              dy1,
              (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UBYTE *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        }
      } else if (bitmap->depth == SBMP_DEPTH16) {
        if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
          s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
          SAGE_FastMap16BitsTransparent(
              dy1,
              (UWORD *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UWORD *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        } else {
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
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits(
            dy1,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits(
            dy1,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      }
#endif
    } else {
      // Go for mapping
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
          s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
          SAGE_FastMap8BitsTransparent(
              dy1,
              (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UBYTE *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        } else {
          SAGE_FastMap8BitsTexture(
              dy1,
              (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UBYTE *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        }
      } else if (bitmap->depth == SBMP_DEPTH16) {
        if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
          s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
          SAGE_FastMap16BitsTransparent(
              dy1,
              (UWORD *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UWORD *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        } else {
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
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits(
            dy1,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits(
            dy1,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
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
        s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DUDYL] = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DVDYL] = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      } else {
        s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
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
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
          s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
          SAGE_FastMap8BitsTransparent(
              dy3,
              (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UBYTE *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        } else {
          SAGE_FastMap8BitsTexture(
              dy3,
              (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UBYTE *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        }
      } else if (bitmap->depth == SBMP_DEPTH16) {
        if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
          s3dm_coords[CRD_TCOLOR] = triangle->tex->bitmap->transparency;
          SAGE_FastMap16BitsTransparent(
              dy3,
              (UWORD *) triangle->tex->bitmap->bitmap_buffer,
              triangle->tex->bitmap->bpr,
              (UWORD *) bitmap->bitmap_buffer,
              bitmap->bpr,
              s3dm_deltas,
              s3dm_coords
          );
        } else {
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
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits(
            dy3,
            (UBYTE *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UBYTE *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits(
            dy3,
            (UWORD *) triangle->tex->bitmap->bitmap_buffer,
            triangle->tex->bitmap->width,
            (UWORD *) bitmap->bitmap_buffer,
            bitmap->width,
            (UWORD *) SageContext.Sage3D->render.zbuffer.buffer
        );
      }
#endif
    }
  }
}

/**
 * Check the triangle type & order the vertices
 */
ULONG SAGE_CheckTriangleType(S3D_Triangle *triangle, SAGE_Clipping *clipping)
{
  LONG tx, ty, tz, tu, tv;

  // Degenerated triangle elimination
  if (triangle->x1 == triangle->x2 && triangle->x2 == triangle->x3) {
    return TRI_REJECTED;
  }
  if (triangle->y1 == triangle->y2 && triangle->y2 == triangle->y3) {
    return TRI_REJECTED;
  }
  SD(SAGE_TraceLog(" => not degenerated");)
  // Order triangle vertices from top to bottom
  if (triangle->y1 > triangle->y3) {
    tx = triangle->x1; ty = triangle->y1; tz = triangle->z1; tu = triangle->u1; tv = triangle->v1;
    triangle->x1 = triangle->x3; triangle->y1 = triangle->y3; triangle->z1 = triangle->z3;
    triangle->u1 = triangle->u3; triangle->v1 = triangle->v3;
    triangle->x3 = tx; triangle->y3 = ty; triangle->z3 = tz; triangle->u3 = tu; triangle->v3 = tv;
  }
  if (triangle->y1 > triangle->y2) {
    tx = triangle->x1; ty = triangle->y1; tz = triangle->z1; tu = triangle->u1; tv = triangle->v1;
    triangle->x1 = triangle->x2; triangle->y1 = triangle->y2; triangle->z1 = triangle->z2;
    triangle->u1 = triangle->u2; triangle->v1 = triangle->v2;
    triangle->x2 = tx; triangle->y2 = ty; triangle->z2 = tz; triangle->u2 = tu; triangle->v2 = tv;
  }
  if (triangle->y2 > triangle->y3) {
    tx = triangle->x2; ty = triangle->y2; tz = triangle->z2; tu = triangle->u2; tv = triangle->v2;
    triangle->x2 = triangle->x3; triangle->y2 = triangle->y3; triangle->z2 = triangle->z3;
    triangle->u2 = triangle->u3; triangle->v2 = triangle->v3;
    triangle->x3 = tx; triangle->y3 = ty; triangle->z3 = tz; triangle->u3 = tu; triangle->v3 = tv;
  }
  // Trivial rejection
  if (triangle->y3 < clipping->top || triangle->y1 >= clipping->bottom) {
    return TRI_REJECTED;
  }
  if (triangle->x1 < clipping->left && triangle->x2 < clipping->left && triangle->x3 < clipping->left) {
    return TRI_REJECTED;
  }
  if (triangle->x1 >= clipping->right && triangle->x2 >= clipping->right && triangle->x3 >= clipping->right) {
    return TRI_REJECTED;
  }
  SD(SAGE_TraceLog(" => not rejected");)
  // Check the type & order from left to right
  if (triangle->y1 == triangle->y2) {
    if (triangle->x1 > triangle->x2) {
      tx = triangle->x1; ty = triangle->y1; tz = triangle->z1; tu = triangle->u1; tv = triangle->v1;
      triangle->x1 = triangle->x2; triangle->y1 = triangle->y2; triangle->z1 = triangle->z2;
      triangle->u1 = triangle->u2; triangle->v1 = triangle->v2;
      triangle->x2 = tx; triangle->y2 = ty; triangle->z2 = tz; triangle->u2 = tu; triangle->v2 = tv;
    }
    return TRI_FLATTOP;
  } else if (triangle->y2 == triangle->y3) {
    if (triangle->x2 > triangle->x3) {
      tx = triangle->x2; ty = triangle->y2; tz = triangle->z2; tu = triangle->u2; tv = triangle->v2;
      triangle->x2 = triangle->x3; triangle->y2 = triangle->y3; triangle->z2 = triangle->z3;
      triangle->u2 = triangle->u3; triangle->v2 = triangle->v3;
      triangle->x3 = tx; triangle->y3 = ty; triangle->z3 = tz; triangle->u3 = tu; triangle->v3 = tv;
    }
    return TRI_FLATBOTTOM;
  }
  return TRI_GENERIC;
}

/**
 * Draw a colored triangle
 */
BOOL SAGE_DrawColoredTriangle(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  ULONG type;

  SD(SAGE_TraceLog("---- SAGE_DrawColoredTriangle");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d u1=%d v1=%d", triangle->x1, triangle->y1, triangle->z1, triangle->u1, triangle->v1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d u2=%d v2=%d", triangle->x2, triangle->y2, triangle->z2, triangle->u2, triangle->v2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d u3=%d v3=%d", triangle->x3, triangle->y3, triangle->z3, triangle->u3, triangle->v3);)

  // Check for triangle type
  type = SAGE_CheckTriangleType(triangle, clipping);
  // Render triangle depending on his type
  if (type == TRI_FLATTOP) {
    SAGE_DrawFlatTopColored(triangle, bitmap, clipping);
  } else if (type == TRI_FLATBOTTOM) {
    SAGE_DrawFlatBottomColored(triangle, bitmap, clipping);
  } else if (type == TRI_GENERIC) {
    SAGE_DrawGenericColored(triangle, bitmap, clipping);
  }
  return TRUE;
}

/**
 * Draw a textured triangle
 */
BOOL SAGE_DrawTexturedTriangle(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  ULONG type;

  SD(SAGE_TraceLog("---- SAGE_DrawTexturedTriangle");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d u1=%d v1=%d", triangle->x1, triangle->y1, triangle->z1, triangle->u1, triangle->v1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d u2=%d v2=%d", triangle->x2, triangle->y2, triangle->z2, triangle->u2, triangle->v2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d u3=%d v3=%d", triangle->x3, triangle->y3, triangle->z3, triangle->u3, triangle->v3);)

  // Check for triangle type
  type = SAGE_CheckTriangleType(triangle, clipping);
  // Render triangle depending on his type
  if (type == TRI_FLATTOP) {
    SAGE_DrawFlatTopTextured(triangle, bitmap, clipping);
  } else if (type == TRI_FLATBOTTOM) {
    SAGE_DrawFlatBottomTextured(triangle, bitmap, clipping);
  } else if (type == TRI_GENERIC) {
    SAGE_DrawGenericTextured(triangle, bitmap, clipping);
  }
  return TRUE;
}
