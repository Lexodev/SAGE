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
#include "sage_3dmaggie.h"
#include "sage_context.h"

#include <proto/graphics.h>

/** SAGE context */
extern SAGE_Context SageContext;

/** Mapper data */
extern LONG s3dm_deltas[11], s3dm_coords[12];

static volatile SAGE_MaggieRegs * const maggie = (SAGE_MaggieRegs *)0xdff250;

/**
 * Map a 16bits texture with Maggie
 *
 * @param nblines       Number of lines to map
 * @param texture       Texture buffer address
 * @param screen_buffer Screen buffer address
 * @param screen_width  Screen width in pixels
 */
VOID SAGE_TextureMapperMaggie(LONG nblines, APTR texture, WORD texture_width, APTR screen_buffer, ULONG screen_width, APTR zbuffer, WORD mode, WORD texmod)
{
  LONG dx, du, dv, dz;
  LONG ui, vi, xs, xe, zi, pixel;

  SD(SAGE_TraceLog("SAGE_TextureMapperMaggie %d lines", nblines));

  // Setup Maggie
  WaitBlit();
  maggie->texture = texture;
  maggie->depthDest = zbuffer;
  maggie->texSize = texture_width;
  maggie->mode = mode;
  maggie->modulo = texmod;
  maggie->lightRGBA = ~0;
  maggie->light = ~0;
  maggie->lightDelta = 0;

  s3dm_coords[CRD_LINE] *= texmod;             // 16 or 32 bits
  s3dm_coords[CRD_LINE] += (LONG)screen_buffer;

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
      dz = s3dm_coords[CRD_ZR] - s3dm_coords[CRD_ZL];
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        du /= dx;
        dv /= dx;
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => du=0x%X  dv=0x%X  dz=0x%X", du, dv, dz));
      // Calcul texture coords
      ui = s3dm_coords[CRD_UL] + FIXP16_ROUND_UP;
      vi = s3dm_coords[CRD_VL] + FIXP16_ROUND_UP;
      // Calcul Z value
      zi = s3dm_coords[CRD_ZL] + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_coords[CRD_LCLIP]) {
        SD(SAGE_TraceLog(" => left clipping"));
        dx = s3dm_coords[CRD_LCLIP] - xs;
        ui += dx * du;
        vi += dx * dv;
        zi += dx * dz;
        xs = s3dm_coords[CRD_LCLIP];
        dx = xe - xs;
      }
      if (xe >= s3dm_coords[CRD_RCLIP]) {
        SD(SAGE_TraceLog(" => right clipping"));
        dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  ui=0x%X  vi=0x%X  zi=0x%X", xs, ui, vi, zi));
      // Destination adr
      pixel = s3dm_coords[CRD_LINE] + (xs * texmod);
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx));
      // Start drawing
      WaitBlit();
      maggie->pixDest = (APTR)pixel;
      maggie->uCoord = ui;
      maggie->vCoord = vi;
      maggie->uDelta = du;
      maggie->vDelta = dv;
      maggie->depthStart = zi;
      maggie->depthDelta = dz;
      maggie->startLength = dx;

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
 * Draw a textured flat top triangle with Maggie
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawMaggieFlatTop(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping, SAGE_MaggieData * magdata)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawMaggieFlatTop"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3));
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Color or texture mapping
  if (triangle->tex == NULL) {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DUDYL] = 0;
    s3dm_deltas[DELTA_DVDYL] = 0;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DUDYR] = 0;
    s3dm_deltas[DELTA_DVDYR] = 0;
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
      // Screen start address
      s3dm_coords[CRD_LINE] = triangle->y1 * bitmap->width;
    }
    s3dm_coords[CRD_UL] = 0;
    s3dm_coords[CRD_VL] = 0;
    s3dm_coords[CRD_UR] = 0;
    s3dm_coords[CRD_VR] = 0;
  } else {
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
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;
  // Go for mapping
  if (bitmap->depth == SBMP_DEPTH16) {
#if SAGE_MAPPER_ASM == 1
    SAGE_MaggieMap16BitsTexture(
        dy+1,
        magdata->texture,
        magdata->mipsize,
        bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords,
        magdata->zbuffer,
        magdata->mode,
        magdata->texmod
    );
#else
    SAGE_TextureMapperMaggie(
        dy+1,
        magdata->texture,
        magdata->mipsize,
        bitmap->bitmap_buffer,
        bitmap->bpr,
        magdata->zbuffer,
        magdata->mode,
        magdata->texmod
    );
#endif
  }
}

/**
 * Draw a textured flat bottom triangle with Maggie
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawMaggieFlatBottom(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping, SAGE_MaggieData * magdata)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawMaggieFlatBottom"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3));
  // Delta height
  dy = triangle->y2 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Color or texture mapping
  if (triangle->tex == NULL) {
    // Left side delta
    s3dm_deltas[DELTA_DXDYL] = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DZDYL] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DUDYL] = 0;
    s3dm_deltas[DELTA_DVDYL] = 0;
    // Right side delta
    s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
    s3dm_deltas[DELTA_DUDYR] = 0;
    s3dm_deltas[DELTA_DVDYR] = 0;
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
    s3dm_coords[CRD_UL] = 0;
    s3dm_coords[CRD_VL] = 0;
    s3dm_coords[CRD_UR] = 0;
    s3dm_coords[CRD_VR] = 0;
  } else {
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
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  s3dm_coords[CRD_LCLIP] = clipping->left;
  s3dm_coords[CRD_RCLIP] = clipping->right;
  // Go for mapping
  if (bitmap->depth == SBMP_DEPTH16) {
#if SAGE_MAPPER_ASM == 1
    SAGE_MaggieMap16BitsTexture(
        dy+1,
        magdata->texture,
        magdata->mipsize,
        bitmap->bitmap_buffer,
        bitmap->bpr,
        s3dm_deltas,
        s3dm_coords,
        magdata->zbuffer,
        magdata->mode,
        magdata->texmod
    );
#else
    SAGE_TextureMapperMaggie(
        dy+1,
        magdata->texture,
        magdata->mipsize,
        bitmap->bitmap_buffer,
        bitmap->bpr,
        magdata->zbuffer,
        magdata->mode,
        magdata->texmod
    );
#endif
  }
}

/**
 * Draw a textured generic triangle with Maggie
 *
 * @param triangle Triangle to draw
 *
 */
VOID SAGE_DrawMaggieGeneric(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping, SAGE_MaggieData * magdata)
{
  LONG dy1, dy2, dy3, dxdy1, dxdy2, slope;

  SD(SAGE_TraceLog("-- SAGE_DrawMaggieGeneric"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3));
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
  // Color or texture mapping
  if (triangle->tex == NULL) {
    if (dxdy1 > dxdy2) {
      // Left side delta
      s3dm_deltas[DELTA_DXDYL] = dxdy2;
      s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
      // Right side delta
      s3dm_deltas[DELTA_DXDYR] = dxdy1;
      s3dm_deltas[DELTA_DZDYR] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
      // Slope, left long
      SD(SAGE_TraceLog(" => Left long slope"));
      slope = 0;
    } else {
      // Left side delta
      s3dm_deltas[DELTA_DXDYL] = dxdy1;
      s3dm_deltas[DELTA_DZDYL] = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
      // Right side delta
      s3dm_deltas[DELTA_DXDYR] = dxdy2;
      s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
       // Slope, right long
      SD(SAGE_TraceLog(" => Right long slope"));
      slope = 1;
    }
    s3dm_deltas[DELTA_DUDYL] = 0;
    s3dm_deltas[DELTA_DVDYL] = 0;
    s3dm_deltas[DELTA_DUDYR] = 0;
    s3dm_deltas[DELTA_DVDYR] = 0;
  } else {
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
      slope = 1;
    }
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
    if (triangle->tex == NULL) {
      if (slope == 1) {
        s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy2 + (triangle->x2 << FIXP16_SHIFT);
        s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy1 + (triangle->x1 << FIXP16_SHIFT);
      } else {
        s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_coords[CRD_XL] = s3dm_deltas[DELTA_DXDYL] * dy1 + (triangle->x1 << FIXP16_SHIFT);
        s3dm_coords[CRD_XR] = s3dm_deltas[DELTA_DXDYR] * dy2 + (triangle->x2 << FIXP16_SHIFT);
      }
      s3dm_coords[CRD_UL] = 0;
      s3dm_coords[CRD_VL] = 0;
      s3dm_coords[CRD_UR] = 0;
      s3dm_coords[CRD_VR] = 0;
    } else {
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
    }
    // Screen start address
    s3dm_coords[CRD_LINE] = clipping->top * bitmap->width;
    dy3 = triangle->y3 - clipping->top;
    // Bottom clipping
    if (triangle->y3 >= clipping->bottom) {
      dy3 -= (triangle->y3 - clipping->bottom);
    }
    // Go for mapping
    if (bitmap->depth == SBMP_DEPTH16) {
#if SAGE_MAPPER_ASM == 1
      SAGE_MaggieMap16BitsTexture(
          dy3,
          magdata->texture,
          magdata->mipsize,
          bitmap->bitmap_buffer,
          bitmap->bpr,
          s3dm_deltas,
          s3dm_coords,
          magdata->zbuffer,
          magdata->mode,
          magdata->texmod
      );
#else
      SAGE_TextureMapperMaggie(
          dy3,
          magdata->texture,
          magdata->mipsize,
          bitmap->bitmap_buffer,
          bitmap->bpr,
          magdata->zbuffer,
          magdata->mode,
          magdata->texmod
      );
#endif
    }
  } else {
    // y1 top clipping
    if (triangle->tex == NULL) {
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
      s3dm_coords[CRD_UL] = 0;
      s3dm_coords[CRD_VL] = 0;
      s3dm_coords[CRD_UR] = 0;
      s3dm_coords[CRD_VR] = 0;
    } else {
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
    }
    if (triangle->y2 >= clipping->bottom) {
      dy1 -= (triangle->y2 - clipping->bottom);
      // Go for mapping
      if (bitmap->depth == SBMP_DEPTH16) {
#if SAGE_MAPPER_ASM == 1
        SAGE_MaggieMap16BitsTexture(
            dy1,
            magdata->texture,
            magdata->mipsize,
            bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords,
            magdata->zbuffer,
            magdata->mode,
            magdata->texmod
        );
#else
        SAGE_TextureMapperMaggie(
            dy1,
            magdata->texture,
            magdata->mipsize,
            bitmap->bitmap_buffer,
            bitmap->bpr,
            magdata->zbuffer,
            magdata->mode,
            magdata->texmod
        );
#endif
      }
    } else {
      // Go for mapping
      if (bitmap->depth == SBMP_DEPTH16) {
#if SAGE_MAPPER_ASM == 1
        SAGE_MaggieMap16BitsTexture(
            dy1,
            magdata->texture,
            magdata->mipsize,
            bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords,
            magdata->zbuffer,
            magdata->mode,
            magdata->texmod
        );
#else
        SAGE_TextureMapperMaggie(
            dy1,
            magdata->texture,
            magdata->mipsize,
            bitmap->bitmap_buffer,
            bitmap->bpr,
            magdata->zbuffer,
            magdata->mode,
            magdata->texmod
        );
#endif
      }
      dy3 = triangle->y3 - triangle->y2;
      if (dy3 <= 0) {
        return;
      }
      // Calcul new deltas
      if (triangle->tex == NULL) {
        if (slope == 1) {
          s3dm_deltas[DELTA_DXDYL] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
          s3dm_deltas[DELTA_DZDYL] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        } else {
          s3dm_deltas[DELTA_DXDYR] = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
          s3dm_deltas[DELTA_DZDYR] = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        }
      } else {
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
      }
      // Screen start address
      s3dm_coords[CRD_LINE] = triangle->y2 * bitmap->width;
      // Bottom clipping
      if (triangle->y3 >= clipping->bottom) {
        dy3 -= (triangle->y3 - clipping->bottom);
      }
      // Go for mapping
      if (bitmap->depth == SBMP_DEPTH16) {
#if SAGE_MAPPER_ASM == 1
        SAGE_MaggieMap16BitsTexture(
            dy3,
            magdata->texture,
            magdata->mipsize,
            bitmap->bitmap_buffer,
            bitmap->bpr,
            s3dm_deltas,
            s3dm_coords,
            magdata->zbuffer,
            magdata->mode,
            magdata->texmod
        );
#else
        SAGE_TextureMapperMaggie(
            dy3,
            magdata->texture,
            magdata->mipsize,
            bitmap->bitmap_buffer,
            bitmap->bpr,
            magdata->zbuffer,
            magdata->mode,
            magdata->texmod
        );
#endif
      }
    }
  }
}

/**
 * Draw a textured triangle with Maggie
 */
BOOL SAGE_DrawMaggieTriangle(S3D_Triangle * triangle, SAGE_Bitmap * bitmap, SAGE_Clipping * clipping, SAGE_MaggieData * magdata)
{
  ULONG type;

  SD(SAGE_TraceLog("---- SAGE_DrawMaggieTriangle"));
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d u1=%d v1=%d", triangle->x1, triangle->y1, triangle->z1, triangle->u1, triangle->v1));
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d u2=%d v2=%d", triangle->x2, triangle->y2, triangle->z2, triangle->u2, triangle->v2));
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d u3=%d v3=%d", triangle->x3, triangle->y3, triangle->z3, triangle->u3, triangle->v3));

  SD(SAGE_TraceLog("-- MaggieData"));
  SD(SAGE_TraceLog(" => texture=%d  mipsize=%d  zbuffer=%d  mode=%d  texmod=%d", magdata->texture, magdata->mipsize, magdata->zbuffer, magdata->mode, magdata->texmod));

  // Check for triangle type
  type = SAGE_CheckTriangleType(triangle, clipping);
  // Render triangle depending on his type
  if (type == TRI_FLATTOP) {
    SAGE_DrawMaggieFlatTop(triangle, bitmap, clipping, magdata);
  } else if (type == TRI_FLATBOTTOM) {
    SAGE_DrawMaggieFlatBottom(triangle, bitmap, clipping, magdata);
  } else if (type == TRI_GENERIC) {
    SAGE_DrawMaggieGeneric(triangle, bitmap, clipping, magdata);
  }
  return TRUE;
}
