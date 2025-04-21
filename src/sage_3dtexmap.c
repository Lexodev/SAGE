/**
 * sage_3dtexmap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 10/03/2025)
 */

#include <exec/types.h>

#include <sage/sage_logger.h>
#include <sage/sage_bitmap.h>
#include <sage/sage_3dtexture.h>
#include <sage/sage_3dtexmap.h>
#include <sage/sage_context.h>

#include <sage/sage_debug.h>

#include <proto/graphics.h>

#define SAGE_MAPPER_ASM       1

#if SAGE_MAPPER_ASM == 1

/** External function for 8bits color mapping */
extern BOOL ASM SAGE_FastMap8BitsColor(
  REG(a0, SAGE_TextureMapping *texmap)
);

/** External function for 8bits texture mapping */
extern BOOL ASM SAGE_FastMap8BitsTexture(
  REG(a0, SAGE_TextureMapping *texmap)
);

/** External function for 8bits transparent texture mapping */
extern BOOL ASM SAGE_FastMap8BitsTransparent(
  REG(a0, SAGE_TextureMapping *texmap)
);

/** External function for 16bits color mapping */
extern BOOL ASM SAGE_FastMap16BitsColor(
  REG(a0, SAGE_TextureMapping *texmap)
);

/** External function for 16bits texture mapping */
extern BOOL ASM SAGE_FastMap16BitsTexture(
  REG(a0, SAGE_TextureMapping *texmap)
);

/** External function for 16bits transparent texture mapping */
extern BOOL ASM SAGE_FastMap16BitsTransparent(
  REG(a0, SAGE_TextureMapping *texmap)
);

#endif

/** SAGE context */
extern SAGE_Context SageContext;

/** Mapper data */
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

VOID SAGE_DebugTexMap(VOID)
{
  SAGE_TraceLog("-- Texture mapping data");
  SAGE_TraceLog("-- Render data");
  SAGE_TraceLog(" => frame_buffer=%d (0x%X)  BPR=%d", s3dm_texmap.frame_buffer, s3dm_texmap.frame_buffer, s3dm_texmap.fb_bpr);
  SAGE_TraceLog(" => z_buffer=%d (0x%X)  BPR=%d", s3dm_texmap.z_buffer, s3dm_texmap.z_buffer, s3dm_texmap.zb_bpr);
  SAGE_TraceLog(" => lclip=%d  rclip=%d", s3dm_texmap.lclip, s3dm_texmap.rclip);
  SAGE_TraceLog("-- Triangle data");
  SAGE_TraceLog(" => start_y=%d  nb_line=%d", s3dm_texmap.start_y, s3dm_texmap.nb_line);
  SAGE_TraceLog(" => dxdyl=%d (0x%X)  dxdyr=%d (0x%X)", s3dm_texmap.dxdyl, s3dm_texmap.dxdyl, s3dm_texmap.dxdyr, s3dm_texmap.dxdyr);
  SAGE_TraceLog(" => dzdyl=%d (0x%X)  dzdyr=%d (0x%X)", s3dm_texmap.dzdyl, s3dm_texmap.dzdyl, s3dm_texmap.dzdyr, s3dm_texmap.dzdyr);
  SAGE_TraceLog(" => dudyl=%d (0x%X)  dudyr=%d (0x%X)", s3dm_texmap.dudyl, s3dm_texmap.dudyl, s3dm_texmap.dudyr, s3dm_texmap.dudyr);
  SAGE_TraceLog(" => dvdyl=%d (0x%X)  dvdyr=%d (0x%X)", s3dm_texmap.dvdyl, s3dm_texmap.dvdyl, s3dm_texmap.dvdyr, s3dm_texmap.dvdyr);
  SAGE_TraceLog(" => tex_buffer=%d (0x%X)  BPR=%d  color=0x%X", s3dm_texmap.tex_buffer, s3dm_texmap.tex_buffer, s3dm_texmap.tb_bpr, s3dm_texmap.color);
  SAGE_TraceLog("-- Line data");
  SAGE_TraceLog(" => du=%d (0x%X)  dv=%d (0x%X)  dz=%d (0x%X)", s3dm_texmap.du, s3dm_texmap.du, s3dm_texmap.dv, s3dm_texmap.dv, s3dm_texmap.dz, s3dm_texmap.dz);
  SAGE_TraceLog(" => xl=%d (0x%X)  xr=%d (0x%X)", s3dm_texmap.xl, s3dm_texmap.xl, s3dm_texmap.xr, s3dm_texmap.xr);
  SAGE_TraceLog(" => zl=%d (0x%X)  zr=%d (0x%X)", s3dm_texmap.zl, s3dm_texmap.zl, s3dm_texmap.zr, s3dm_texmap.zr);
  SAGE_TraceLog(" => ul=%d (0x%X)  ur=%d (0x%X)  vl=%d (0x%X)  vr=%d (0x%X)", s3dm_texmap.ul, s3dm_texmap.ul, s3dm_texmap.ur, s3dm_texmap.ur, s3dm_texmap.vl, s3dm_texmap.vl, s3dm_texmap.vr, s3dm_texmap.vr);
}
#endif

/*****************************************************************************
 *                   END DEBUG
 *****************************************************************************/

#if SAGE_MAPPER_ASM == 0

/**
 * Map a 8bits color
 */
VOID SAGE_ColorMapper8Bits(VOID)
{
  LONG nblines, dx, dz, xs, xe, zi;
  UBYTE *fb_line, *zb_line, *screen, col;
  UWORD *zbuffer;

  fb_line = s3dm_texmap.frame_buffer + (s3dm_texmap.start_y * s3dm_texmap.fb_bpr);  // a1
  zb_line = s3dm_texmap.z_buffer + (s3dm_texmap.start_y * s3dm_texmap.zb_bpr);      // a2
  nblines = s3dm_texmap.nb_line;    // d0
  SD(SAGE_TraceLog("SAGE_ColorMapper8Bits %d lines", nblines);)
  col = s3dm_texmap.color & 0xFF;
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_texmap.xl, s3dm_texmap.xr, s3dm_texmap.zl, s3dm_texmap.zr);)
    // Calcul edge coords
    xs = (s3dm_texmap.xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_texmap.xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_texmap.rclip && xe >= s3dm_texmap.lclip) {
      // Calcul interpolation
      dz = s3dm_texmap.zr - s3dm_texmap.zl;
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => dz=0x%X", dz);)
      // Calcul Z value
      zi = s3dm_texmap.zl + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_texmap.lclip) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_texmap.lclip - xs;
        zi += dx * dz;
        xs = s3dm_texmap.lclip;
        dx = xe - xs;
      }
      if (xe >= s3dm_texmap.rclip) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_texmap.rclip - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  zi=0x%X", xs, zi);)
      // Start address
      screen = fb_line + xs;    // Because screen is 8bits
      SD(SAGE_TraceLog(" => screen=0x%X", screen);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (s3dm_texmap.z_buffer != NULL) {
        zbuffer = (UWORD *)(zb_line + (xs * 2)); // Because zbuffer is always 16bits
        SD(SAGE_TraceLog(" => zbuffer=0x%X", zbuffer);)
        while (dx--) {
          // Compare with zbuffer
          if (*zbuffer > (UWORD)(zi >> FIXP16_SHIFT)) {
            *zbuffer = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            *screen++ = col;
          } else {
            screen += 2;
          }
          zbuffer++;
          // Interpolate z
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the texel
          *screen++ = col;
        }
      }
    }
    // Interpolate next points
    s3dm_texmap.xl += s3dm_texmap.dxdyl;
    s3dm_texmap.xr += s3dm_texmap.dxdyr;
    s3dm_texmap.zl += s3dm_texmap.dzdyl;
    s3dm_texmap.zr += s3dm_texmap.dzdyr;
    // Next line address
    fb_line += s3dm_texmap.fb_bpr;
    zb_line += s3dm_texmap.zb_bpr;
  }
}

/**
 * Map a 8bits texture
 */
VOID SAGE_TextureMapper8Bits(VOID)
{
  LONG nblines, dx, du, dv, dz;
  LONG ui, vi, xs, xe, zi;
  UBYTE *fb_line, *zb_line, *screen, *texture;
  UWORD *zbuffer;

  fb_line = s3dm_texmap.frame_buffer + (s3dm_texmap.start_y * s3dm_texmap.fb_bpr);  // a1
  zb_line = s3dm_texmap.z_buffer + (s3dm_texmap.start_y * s3dm_texmap.zb_bpr);      // a2
  nblines = s3dm_texmap.nb_line;    // d0
  SD(SAGE_TraceLog("SAGE_TextureMapper8Bits %d lines", nblines);)
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_texmap.xl, s3dm_texmap.xr, s3dm_texmap.zl, s3dm_texmap.zr);)
    SD(SAGE_TraceLog(" => ul=0x%X  ur=0x%X  vl=0x%X  vr=0x%X", s3dm_texmap.ul, s3dm_texmap.ur, s3dm_texmap.vl, s3dm_texmap.vr);)
    // Calcul edge coords
    xs = (s3dm_texmap.xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_texmap.xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_texmap.rclip && xe >= s3dm_texmap.lclip) {
      // Calcul interpolation
      du = s3dm_texmap.ur - s3dm_texmap.ul;
      dv = s3dm_texmap.vr - s3dm_texmap.vl;
      dz = s3dm_texmap.zr - s3dm_texmap.zl;
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        du /= dx;
        dv /= dx;
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => du=0x%X  dv=0x%X  dz=0x%X", du, dv, dz);)
      // Calcul texture coords
      ui = s3dm_texmap.ul + FIXP16_ROUND_UP;
      vi = s3dm_texmap.vl + FIXP16_ROUND_UP;
      // Calcul Z value
      zi = s3dm_texmap.zl + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_texmap.lclip) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_texmap.lclip - xs;
        ui += dx * du;
        vi += dx * dv;
        zi += dx * dz;
        xs = s3dm_texmap.lclip;
        dx = xe - xs;
      }
      if (xe >= s3dm_texmap.rclip) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_texmap.rclip - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  ui=0x%X  vi=0x%X  zi=0x%X", xs, ui, vi, zi);)
      // Start address
      screen = fb_line + xs;        // Because screen is 8bits
      SD(SAGE_TraceLog(" => screen=0x%X", screen);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (s3dm_texmap.z_buffer != NULL) {
        zbuffer = (UWORD *)(zb_line + (xs * 2)); // Because zbuffer is always 16bits
        SD(SAGE_TraceLog(" => zbuffer=0x%X", zbuffer);)
        while (dx--) {
          // Compare with zbuffer
          if (*zbuffer > (UWORD)(zi >> FIXP16_SHIFT)) {
            *zbuffer = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            texture = (UBYTE *)s3dm_texmap.tex_buffer + (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr);
            *screen++ = *texture;
          } else {
            screen++;
          }
          zbuffer++;
          // Interpolate u, v & z
          ui += du;
          vi += dv;
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the texel
          texture = (UBYTE *)s3dm_texmap.tex_buffer + (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr);
          *screen++ = *texture;
          // Interpolate u & v
          ui += du;
          vi += dv;
        }
      }
    }
    // Interpolate next points
    s3dm_texmap.xl += s3dm_texmap.dxdyl;
    s3dm_texmap.xr += s3dm_texmap.dxdyr;
    s3dm_texmap.zl += s3dm_texmap.dzdyl;
    s3dm_texmap.zr += s3dm_texmap.dzdyr;
    s3dm_texmap.ul += s3dm_texmap.dudyl;
    s3dm_texmap.ur += s3dm_texmap.dudyr;
    s3dm_texmap.vl += s3dm_texmap.dvdyl;
    s3dm_texmap.vr += s3dm_texmap.dvdyr;
    // Next line address
    fb_line += s3dm_texmap.fb_bpr;
    zb_line += s3dm_texmap.zb_bpr;
  }
}

/**
 * Map a 16bits color
 */
VOID SAGE_ColorMapper16Bits(VOID)
{
  LONG nblines, dx, dz, xs, xe, zi;
  UBYTE *fb_line, *zb_line, *screen, ch, cl;
  UWORD *zbuffer;

  fb_line = s3dm_texmap.frame_buffer + (s3dm_texmap.start_y * s3dm_texmap.fb_bpr);  // a1
  zb_line = s3dm_texmap.z_buffer + (s3dm_texmap.start_y * s3dm_texmap.zb_bpr);      // a2
  nblines = s3dm_texmap.nb_line;    // d0
  SD(SAGE_TraceLog("SAGE_ColorMapper16Bits %d lines", nblines);)
  ch = (s3dm_texmap.color >> 8) & 0xFF;
  cl = s3dm_texmap.color & 0xFF;
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_texmap.xl, s3dm_texmap.xr, s3dm_texmap.zl, s3dm_texmap.zr);)
    // Calcul edge coords
    xs = (s3dm_texmap.xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_texmap.xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_texmap.rclip && xe >= s3dm_texmap.lclip) {
      // Calcul interpolation
      dz = s3dm_texmap.zr - s3dm_texmap.zl;
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => dz=0x%X", dz);)
      // Calcul Z value
      zi = s3dm_texmap.zl + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_texmap.lclip) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_texmap.lclip - xs;
        zi += dx * dz;
        xs = s3dm_texmap.lclip;
        dx = xe - xs;
      }
      if (xe >= s3dm_texmap.rclip) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_texmap.rclip - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  zi=0x%X", xs, zi);)
      // Start address
      screen = fb_line + (xs * 2);    // Because screen is 16bits
      SD(SAGE_TraceLog(" => screen=0x%X", screen);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (s3dm_texmap.z_buffer != NULL) {
        zbuffer = (UWORD *)(zb_line + (xs * 2)); // Because zbuffer is always 16bits
        SD(SAGE_TraceLog(" => zbuffer=0x%X", zbuffer);)
        while (dx--) {
          // Compare with zbuffer
          if (*zbuffer > (UWORD)(zi >> FIXP16_SHIFT)) {
            *zbuffer = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            *screen++ = ch;
            *screen++ = cl;
          } else {
            screen += 2;
          }
          zbuffer++;
          // Interpolate z
          zi += dz;
        }
      } else {
        while (dx--) {
          // Write the texel
          *screen++ = ch;
          *screen++ = cl;
        }
      }
    }
    // Interpolate next points
    s3dm_texmap.xl += s3dm_texmap.dxdyl;
    s3dm_texmap.xr += s3dm_texmap.dxdyr;
    s3dm_texmap.zl += s3dm_texmap.dzdyl;
    s3dm_texmap.zr += s3dm_texmap.dzdyr;
    // Next line address
    fb_line += s3dm_texmap.fb_bpr;
    zb_line += s3dm_texmap.zb_bpr;
  }
}

/**
 * Map a 16bits texture
 */
VOID SAGE_TextureMapper16Bits(VOID)
{
  LONG nblines, dx, du, dv, dz;
  LONG ui, vi, xs, xe, zi;
  UBYTE *fb_line, *zb_line, *screen, *texture;
  UWORD *zbuffer;

  fb_line = s3dm_texmap.frame_buffer + (s3dm_texmap.start_y * s3dm_texmap.fb_bpr);  // a1
  zb_line = s3dm_texmap.z_buffer + (s3dm_texmap.start_y * s3dm_texmap.zb_bpr);      // a2
  nblines = s3dm_texmap.nb_line;    // d0
  SD(SAGE_TraceLog("SAGE_TextureMapper16Bits %d lines", nblines);)
  while (nblines--) {
    SD(SAGE_TraceLog("-- Line %d", nblines);)
    SD(SAGE_TraceLog(" => xl=0x%X  xr=0x%X  zl=0x%X  zr=0x%X", s3dm_texmap.xl, s3dm_texmap.xr, s3dm_texmap.zl, s3dm_texmap.zr);)
    SD(SAGE_TraceLog(" => ul=0x%X  ur=0x%X  vl=0x%X  vr=0x%X", s3dm_texmap.ul, s3dm_texmap.ur, s3dm_texmap.vl, s3dm_texmap.vr);)
    SD(SAGE_TraceLog(" => fb_line=%d (0x%X)  zb_line=%d (0x%X)", fb_line, fb_line, zb_line, zb_line);)
    // Calcul edge coords
    xs = (s3dm_texmap.xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    xe = (s3dm_texmap.xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
    SD(SAGE_TraceLog(" => xs=%d  xe=%d", xs, xe);)
    if (xs < s3dm_texmap.rclip && xe >= s3dm_texmap.lclip) {
      // Calcul interpolation
      du = s3dm_texmap.ur - s3dm_texmap.ul;
      dv = s3dm_texmap.vr - s3dm_texmap.vl;
      dz = s3dm_texmap.zr - s3dm_texmap.zl;
      dx = xe - xs;
      // DX could be 0 in some situations
      if (dx > 0) {
        du /= dx;
        dv /= dx;
        dz /= dx;
      }
      SD(SAGE_TraceLog(" => du=0x%X  dv=0x%X  dz=0x%X", du, dv, dz);)
      // Calcul texture coords
      ui = s3dm_texmap.ul + FIXP16_ROUND_UP;
      vi = s3dm_texmap.vl + FIXP16_ROUND_UP;
      // Calcul Z value
      zi = s3dm_texmap.zl + FIXP16_ROUND_UP;
      // Horizontal clipping
      if (xs < s3dm_texmap.lclip) {
        SD(SAGE_TraceLog(" => left clipping");)
        dx = s3dm_texmap.lclip - xs;
        ui += dx * du;
        vi += dx * dv;
        zi += dx * dz;
        xs = s3dm_texmap.lclip;
        dx = xe - xs;
      }
      if (xe >= s3dm_texmap.rclip) {
        SD(SAGE_TraceLog(" => right clipping");)
        dx = (s3dm_texmap.rclip - 1) - xs;
      }
      SD(SAGE_TraceLog(" => xs=%d  ui=0x%X  vi=0x%X  zi=0x%X", xs, ui, vi, zi);)
      // Start address
      screen = fb_line + (xs * 2);    // Because screen is 16bits
      SD(SAGE_TraceLog(" => screen=%d (0x%X)", screen, screen);)
      // Draw the line
      dx++;    // Real number of points to draw
      SD(SAGE_TraceLog(" => dx=%d", dx);)
      if (s3dm_texmap.z_buffer != NULL) {
        zbuffer = (UWORD *)(zb_line + (xs * 2)); // Because zbuffer is always 16bits
        SD(SAGE_TraceLog(" => zbuffer=0x%X", zbuffer);)
        while (dx--) {
          // Compare with zbuffer
          if (*zbuffer > (UWORD)(zi >> FIXP16_SHIFT)) {
            *zbuffer = (UWORD)(zi >> FIXP16_SHIFT);
            // Write the texel
            texture = (UBYTE *)s3dm_texmap.tex_buffer + ((ui >> FIXP16_SHIFT) * 2) + ((vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr);
            *screen++ = *texture++;
            *screen++ = *texture;
          } else {
            screen += 2;
          }
          zbuffer++;
          // Interpolate u, v & z
          ui += du;
          vi += dv;
          zi += dz;
        }
      } else {
        //SD(SAGE_TraceLog(" -- Start inner loop");)
        while (dx--) {
          //SD(SAGE_TraceLog(" => texel #%d : ui=%d  vi=%d  screen=%d (0x%X)", dx, ui, vi, screen, screen);)
          // Write the texel
          texture = (UBYTE *)s3dm_texmap.tex_buffer + ((ui >> FIXP16_SHIFT) * 2) + ((vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr);
          *screen++ = *texture++;
          *screen++ = *texture;
          // Interpolate u & v
          ui += du;
          vi += dv;
        }
      }
    }
    // Interpolate next points
    s3dm_texmap.xl += s3dm_texmap.dxdyl;
    s3dm_texmap.xr += s3dm_texmap.dxdyr;
    s3dm_texmap.zl += s3dm_texmap.dzdyl;
    s3dm_texmap.zr += s3dm_texmap.dzdyr;
    s3dm_texmap.ul += s3dm_texmap.dudyl;
    s3dm_texmap.ur += s3dm_texmap.dudyr;
    s3dm_texmap.vl += s3dm_texmap.dvdyl;
    s3dm_texmap.vr += s3dm_texmap.dvdyr;
    // Next line address
    fb_line += s3dm_texmap.fb_bpr;
    zb_line += s3dm_texmap.zb_bpr;
  }
}

#endif

/*****************************************************************************/

/**
 * Draw a colored flat top triangle
 *
 * @param triangle Triangle to draw
 * @param bitmap   Bitmap to render
 * @param clipping Screen clipping
 *
 */
VOID SAGE_DrawFlatTopColored(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatTopColored (new version)");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_texmap.dxdyl = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_texmap.xl = s3dm_texmap.dxdyl * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_texmap.xr = s3dm_texmap.dxdyr * dy + (triangle->x2 << FIXP16_SHIFT);
    s3dm_texmap.zl = s3dm_texmap.dzdyl * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_texmap.zr = s3dm_texmap.dzdyr * dy + (triangle->z2 << FIXP16_SHIFT);
    // Start Y coord
    s3dm_texmap.start_y = clipping->top;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_texmap.xl = triangle->x1 << FIXP16_SHIFT;
    s3dm_texmap.xr = triangle->x2 << FIXP16_SHIFT;
    s3dm_texmap.zl = triangle->z1 << FIXP16_SHIFT;
    s3dm_texmap.zr = triangle->z2 << FIXP16_SHIFT;
    // Start Y coord
    s3dm_texmap.start_y = triangle->y1;
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  // Lines to draw
  s3dm_texmap.nb_line = dy;
  // Go for mapping
  SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SD(SAGE_TraceLog("SAGE_FastMap8BitsColor %d lines", dy);)
    SAGE_FastMap8BitsColor(&s3dm_texmap);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SD(SAGE_TraceLog("SAGE_FastMap16BitsColor %d lines", dy);)
    SAGE_FastMap16BitsColor(&s3dm_texmap);
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_ColorMapper8Bits();
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_ColorMapper16Bits();
  }
#endif
}

/**
 * Draw a textured flat top triangle
 *
 * @param triangle Triangle to draw
 * @param bitmap   Bitmap to render
 * @param clipping Screen clipping
 *
 */
VOID SAGE_DrawFlatTopTextured(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatTopTextured (new version)");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_texmap.dxdyl = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dudyl = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dvdyl = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dudyr = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dvdyr = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_texmap.xl = s3dm_texmap.dxdyl * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_texmap.xr = s3dm_texmap.dxdyr * dy + (triangle->x2 << FIXP16_SHIFT);
    s3dm_texmap.zl = s3dm_texmap.dzdyl * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_texmap.zr = s3dm_texmap.dzdyr * dy + (triangle->z2 << FIXP16_SHIFT);
    s3dm_texmap.ul = s3dm_texmap.dudyl * dy + (triangle->u1 << FIXP16_SHIFT);
    s3dm_texmap.ur = s3dm_texmap.dudyr * dy + (triangle->u2 << FIXP16_SHIFT);
    s3dm_texmap.vl = s3dm_texmap.dvdyl * dy + (triangle->v1 << FIXP16_SHIFT);
    s3dm_texmap.vr = s3dm_texmap.dvdyr * dy + (triangle->v2 << FIXP16_SHIFT);
    // Start Y coord
    s3dm_texmap.start_y = clipping->top;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_texmap.xl = triangle->x1 << FIXP16_SHIFT;
    s3dm_texmap.xr = triangle->x2 << FIXP16_SHIFT;
    s3dm_texmap.zl = triangle->z1 << FIXP16_SHIFT;
    s3dm_texmap.zr = triangle->z2 << FIXP16_SHIFT;
    s3dm_texmap.ul = triangle->u1 << FIXP16_SHIFT;
    s3dm_texmap.ur = triangle->u2 << FIXP16_SHIFT;
    s3dm_texmap.vl = triangle->v1 << FIXP16_SHIFT;
    s3dm_texmap.vr = triangle->v2 << FIXP16_SHIFT;
    // Start Y coord
    s3dm_texmap.start_y = triangle->y1;
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  // Lines to draw
  s3dm_texmap.nb_line = dy;
  // Go for mapping
  SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SD(SAGE_TraceLog("SAGE_FastMap8BitsTexture %d lines", dy);)
    SAGE_FastMap8BitsTexture(&s3dm_texmap);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SD(SAGE_TraceLog("SAGE_FastMap16BitsTexture %d lines", dy);)
    SAGE_FastMap16BitsTexture(&s3dm_texmap);
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_TextureMapper8Bits();
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_TextureMapper16Bits();
  }
#endif
}

/**
 * Draw a colored flat bottom triangle
 *
 * @param triangle Triangle to draw
 * @param bitmap   Bitmap to render
 * @param clipping Screen clipping
 *
 */
VOID SAGE_DrawFlatBottomColored(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatBottomColored (new version)");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_texmap.dxdyl = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyl = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_texmap.xl = s3dm_texmap.dxdyl * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_texmap.xr = s3dm_texmap.dxdyr * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_texmap.zl = s3dm_texmap.dzdyl * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_texmap.zr = s3dm_texmap.dzdyr * dy + (triangle->z1 << FIXP16_SHIFT);
    // Start Y coord
    s3dm_texmap.start_y = clipping->top;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_texmap.xl = triangle->x1 << FIXP16_SHIFT;
    s3dm_texmap.xr = s3dm_texmap.xl;
    s3dm_texmap.zl = triangle->z1 << FIXP16_SHIFT;
    s3dm_texmap.zr = s3dm_texmap.zl;
    // Start Y coord
    s3dm_texmap.start_y = triangle->y1;
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  // Lines to draw
  s3dm_texmap.nb_line = dy;
  // Go for mapping
  SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SD(SAGE_TraceLog("SAGE_FastMap8BitsColor %d lines", dy);)
    SAGE_FastMap8BitsColor(&s3dm_texmap);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SD(SAGE_TraceLog("SAGE_FastMap16BitsColor %d lines", dy);)
    SAGE_FastMap16BitsColor(&s3dm_texmap);
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_ColorMapper8Bits();
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_ColorMapper16Bits();
  }
#endif
}

/**
 * Draw a textured flat bottom triangle
 *
 * @param triangle Triangle to draw
 * @param bitmap   Bitmap to render
 * @param clipping Screen clipping
 *
 */
VOID SAGE_DrawFlatBottomTextured(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy;

  SD(SAGE_TraceLog("-- SAGE_DrawFlatBottomTextured (new version)");)
  SD(SAGE_TraceLog(" => x1=%d y1=%d z1=%d", triangle->x1, triangle->y1, triangle->z1);)
  SD(SAGE_TraceLog(" => x2=%d y2=%d z2=%d", triangle->x2, triangle->y2, triangle->z2);)
  SD(SAGE_TraceLog(" => x3=%d y3=%d z3=%d", triangle->x3, triangle->y3, triangle->z3);)
  // Delta height
  dy = triangle->y3 - triangle->y1;
  if (dy <= 0) {
    return;
  }
  // Left side delta
  s3dm_texmap.dxdyl = ((triangle->x2 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyl = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dudyl = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dvdyl = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy;
  // Right side delta
  s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dudyr = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy;
  s3dm_texmap.dvdyr = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy;
  // Start coords & clipping
  if (triangle->y1 < clipping->top) {
    dy = clipping->top - triangle->y1;
    s3dm_texmap.xl = s3dm_texmap.dxdyl * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_texmap.xr = s3dm_texmap.dxdyr * dy + (triangle->x1 << FIXP16_SHIFT);
    s3dm_texmap.zl = s3dm_texmap.dzdyl * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_texmap.zr = s3dm_texmap.dzdyr * dy + (triangle->z1 << FIXP16_SHIFT);
    s3dm_texmap.ul = s3dm_texmap.dudyl * dy + (triangle->u1 << FIXP16_SHIFT);
    s3dm_texmap.ur = s3dm_texmap.dudyr * dy + (triangle->u1 << FIXP16_SHIFT);
    s3dm_texmap.vl = s3dm_texmap.dvdyl * dy + (triangle->v1 << FIXP16_SHIFT);
    s3dm_texmap.vr = s3dm_texmap.dvdyr * dy + (triangle->v1 << FIXP16_SHIFT);
    // Start Y coord
    s3dm_texmap.start_y = clipping->top;
    dy = triangle->y3 - clipping->top;
  } else {
    s3dm_texmap.xl = triangle->x1 << FIXP16_SHIFT;
    s3dm_texmap.xr = s3dm_texmap.xl;
    s3dm_texmap.zl = triangle->z1 << FIXP16_SHIFT;
    s3dm_texmap.zr = s3dm_texmap.zl;
    s3dm_texmap.ul = triangle->u1 << FIXP16_SHIFT;
    s3dm_texmap.ur = s3dm_texmap.ul;
    s3dm_texmap.vl = triangle->v1 << FIXP16_SHIFT;
    s3dm_texmap.vr = s3dm_texmap.vl;
    // Start Y coord
    s3dm_texmap.start_y = triangle->y1;
  }
  // Bottom clipping
  if (triangle->y3 >= clipping->bottom) {
    dy -= (triangle->y3 - clipping->bottom);
  }
  // Lines to draw
  s3dm_texmap.nb_line = dy;
  // Go for mapping
  SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
  if (bitmap->depth == SBMP_DEPTH8) {
    SD(SAGE_TraceLog("SAGE_FastMap8BitsTexture %d lines", dy);)
    SAGE_FastMap8BitsTexture(&s3dm_texmap);
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SD(SAGE_TraceLog("SAGE_FastMap16BitsTexture %d lines", dy);)
    SAGE_FastMap16BitsTexture(&s3dm_texmap);
  }
#else
  if (bitmap->depth == SBMP_DEPTH8) {
    SAGE_TextureMapper8Bits();
  } else if (bitmap->depth == SBMP_DEPTH16) {
    SAGE_TextureMapper16Bits();
  }
#endif
}

/**
 * Draw a colored generic triangle
 *
 * @param triangle Triangle to draw
 * @param bitmap   Bitmap to render
 * @param clipping Screen clipping
 *
 */
VOID SAGE_DrawGenericColored(S3D_Triangle *triangle, SAGE_Bitmap *bitmap, SAGE_Clipping *clipping)
{
  LONG dy1, dy2, dy3, dxdy1, dxdy2, slope;

  SD(SAGE_TraceLog("-- SAGE_DrawGenericColored (new version)");)
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
    s3dm_texmap.dxdyl = dxdy2;
    s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    // Right side delta
    s3dm_texmap.dxdyr = dxdy1;
    s3dm_texmap.dzdyr = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    // Slope, left long
    SD(SAGE_TraceLog(" => left long slope (0)");)
    slope = 0;
  } else {
    // Left side delta
    s3dm_texmap.dxdyl = dxdy1;
    s3dm_texmap.dzdyl = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    // Right side delta
    s3dm_texmap.dxdyr = dxdy2;
    s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    // Slope, right long
    SD(SAGE_TraceLog(" => right long slope (1)");)
    slope = 1;
  }
  // y2 top clipping
  if (triangle->y2 < clipping->top) {
    SD(SAGE_TraceLog(" => y2 top clipping, only draw second sub-triangle");)
    dy1 = clipping->top - triangle->y1;
    dy2 = clipping->top - triangle->y2;
    dy3 = triangle->y3 - triangle->y2;
    if (dy3 <= 0) {
      return;
    }
    // Calcul new deltas and coords
    if (slope == 1) {
      s3dm_texmap.dxdyl = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.xl = s3dm_texmap.dxdyl * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_texmap.xr = s3dm_texmap.dxdyr * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.zl = s3dm_texmap.dzdyl * dy2 + (triangle->z2 << FIXP16_SHIFT);
      s3dm_texmap.zr = s3dm_texmap.dzdyr * dy1 + (triangle->z1 << FIXP16_SHIFT);
    } else {
      s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.xl = s3dm_texmap.dxdyl * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.xr = s3dm_texmap.dxdyr * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_texmap.zl = s3dm_texmap.dzdyl * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_texmap.zr = s3dm_texmap.dzdyr * dy2 + (triangle->z2 << FIXP16_SHIFT);
    }
    // Start Y coord
    s3dm_texmap.start_y = clipping->top;
    dy3 = triangle->y3 - clipping->top;
    // Bottom clipping
    if (triangle->y3 >= clipping->bottom) {
      SD(SAGE_TraceLog(" => y3 bottom clipping");)
      dy3 -= (triangle->y3 - clipping->bottom);
    }
    // Lines to draw
    s3dm_texmap.nb_line = dy3;
    // Go for mapping
    SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
    if (bitmap->depth == SBMP_DEPTH8) {
      SD(SAGE_TraceLog("SAGE_FastMap8BitsColor %d lines", dy3);)
      SAGE_FastMap8BitsColor(&s3dm_texmap);
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SD(SAGE_TraceLog("SAGE_FastMap16BitsColor %d lines", dy3);)
      SAGE_FastMap16BitsColor(&s3dm_texmap);
    }
#else
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_ColorMapper8Bits();
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_ColorMapper16Bits();
    }
#endif
  } else {
    SD(SAGE_TraceLog(" => draw first sub-triangle");)
    // y1 top clipping
    if (triangle->y1 < clipping->top) {
      SD(SAGE_TraceLog(" => y1 top clipping");)
      dy1 = clipping->top - triangle->y1;
      s3dm_texmap.xl = s3dm_texmap.dxdyl * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.xr = s3dm_texmap.dxdyr * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.zl = s3dm_texmap.dzdyl * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_texmap.zr = s3dm_texmap.dzdyr * dy1 + (triangle->z1 << FIXP16_SHIFT);
      // Start Y coord
      s3dm_texmap.start_y = clipping->top;
      dy1 = triangle->y2 - clipping->top;
    } else {
      s3dm_texmap.xl = triangle->x1 << FIXP16_SHIFT;
      s3dm_texmap.xr = s3dm_texmap.xl;
      s3dm_texmap.zl = triangle->z1 << FIXP16_SHIFT;
      s3dm_texmap.zr = s3dm_texmap.zl;
      // Start Y coord
      s3dm_texmap.start_y = triangle->y1;
    }
    if (triangle->y2 >= clipping->bottom) {
      SD(SAGE_TraceLog(" => y2 bottom clipping, draw only first sub-triangle");)
      dy1 -= (triangle->y2 - clipping->bottom);
      // Lines to draw
      s3dm_texmap.nb_line = dy1;
      // Go for mapping
      SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SD(SAGE_TraceLog("SAGE_FastMap8BitsColor %d lines", dy1);)
        SAGE_FastMap8BitsColor(&s3dm_texmap);
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SD(SAGE_TraceLog("SAGE_FastMap16BitsColor %d lines", dy1);)
        SAGE_FastMap16BitsColor(&s3dm_texmap);
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_ColorMapper8Bits();
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_ColorMapper16Bits();
      }
#endif
    } else {
      // Lines to draw
      s3dm_texmap.nb_line = dy1;
      // Go for mapping
      SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SD(SAGE_TraceLog("SAGE_FastMap8BitsColor %d lines", dy1);)
        SAGE_FastMap8BitsColor(&s3dm_texmap);
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SD(SAGE_TraceLog("SAGE_FastMap16BitsColor %d lines", dy1);)
        SAGE_FastMap16BitsColor(&s3dm_texmap);
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_ColorMapper8Bits();
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_ColorMapper16Bits();
      }
#endif
      SD(SAGE_TraceLog(" => draw second sub-triangle");)
      dy3 = triangle->y3 - triangle->y2;
      if (dy3 <= 0) {
        return;
      }
      // Calcul new deltas
      if (slope == 1) {
        s3dm_texmap.dxdyl = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.xl = triangle->x2 << FIXP16_SHIFT;
        s3dm_texmap.zl = triangle->z2 << FIXP16_SHIFT;
      } else {
        s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.xr = triangle->x2 << FIXP16_SHIFT;
        s3dm_texmap.zr = triangle->z2 << FIXP16_SHIFT;
      }
      // Start Y coord
      s3dm_texmap.start_y = triangle->y2;
      // Bottom clipping
      if (triangle->y3 >= clipping->bottom) {
        dy3 -= (triangle->y3 - clipping->bottom);
      }
      // Lines to draw
      s3dm_texmap.nb_line = dy3;
      // Go for mapping
      SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SD(SAGE_TraceLog("SAGE_FastMap8BitsColor %d lines", dy3);)
        SAGE_FastMap8BitsColor(&s3dm_texmap);
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SD(SAGE_TraceLog("SAGE_FastMap16BitsColor %d lines", dy3);)
        SAGE_FastMap16BitsColor(&s3dm_texmap);
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_ColorMapper8Bits();
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_ColorMapper16Bits();
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

  SD(SAGE_TraceLog("-- SAGE_DrawGenericTextured (new version)");)
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
    s3dm_texmap.dxdyl = dxdy2;
    // Right side delta
    s3dm_texmap.dxdyr = dxdy1;
    // Left side texture delta
    s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    s3dm_texmap.dudyl = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy2;
    s3dm_texmap.dvdyl = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy2;
    // Right side texture delta
    s3dm_texmap.dzdyr = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    s3dm_texmap.dudyr = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy1;
    s3dm_texmap.dvdyr = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy1;
    // Slope, left long
    SD(SAGE_TraceLog(" => left long slope (0)");)
    slope = 0;
  } else {
    // Left side delta
    s3dm_texmap.dxdyl = dxdy1;
    // Right side delta
    s3dm_texmap.dxdyr = dxdy2;
    // Left side texture delta
    s3dm_texmap.dzdyl = ((triangle->z2 - triangle->z1) << FIXP16_SHIFT) / dy1;
    s3dm_texmap.dudyl = ((triangle->u2 - triangle->u1) << FIXP16_SHIFT) / dy1;
    s3dm_texmap.dvdyl = ((triangle->v2 - triangle->v1) << FIXP16_SHIFT) / dy1;
    // Right side texture delta
    s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z1) << FIXP16_SHIFT) / dy2;
    s3dm_texmap.dudyr = ((triangle->u3 - triangle->u1) << FIXP16_SHIFT) / dy2;
    s3dm_texmap.dvdyr = ((triangle->v3 - triangle->v1) << FIXP16_SHIFT) / dy2;
    // Slope, right long
    SD(SAGE_TraceLog(" => right long slope (1)");)
    slope = 1;
  }
  // y2 top clipping
  if (triangle->y2 < clipping->top) {
    SD(SAGE_TraceLog(" => y2 top clipping, only draw second sub-triangle");)
    dy1 = clipping->top - triangle->y1;
    dy2 = clipping->top - triangle->y2;
    dy3 = triangle->y3 - triangle->y2;
    if (dy3 <= 0) {
      return;
    }
    // Calcul new deltas and coords
    if (slope == 1) {
      s3dm_texmap.dxdyl = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dudyl = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dvdyl = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.xl = s3dm_texmap.dxdyl * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_texmap.xr = s3dm_texmap.dxdyr * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.zl = s3dm_texmap.dzdyl * dy2 + (triangle->z2 << FIXP16_SHIFT);
      s3dm_texmap.zr = s3dm_texmap.dzdyr * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_texmap.ul = s3dm_texmap.dudyl * dy2 + (triangle->u2 << FIXP16_SHIFT);
      s3dm_texmap.ur = s3dm_texmap.dudyr * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_texmap.vl = s3dm_texmap.dvdyl * dy2 + (triangle->v2 << FIXP16_SHIFT);
      s3dm_texmap.vr = s3dm_texmap.dvdyr * dy1 + (triangle->v1 << FIXP16_SHIFT);
    } else {
      s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dudyr = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.dvdyr = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
      s3dm_texmap.xl = s3dm_texmap.dxdyl * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.xr = s3dm_texmap.dxdyr * dy2 + (triangle->x2 << FIXP16_SHIFT);
      s3dm_texmap.zl = s3dm_texmap.dzdyl * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_texmap.zr = s3dm_texmap.dzdyr * dy2 + (triangle->z2 << FIXP16_SHIFT);
      s3dm_texmap.ul = s3dm_texmap.dudyl * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_texmap.ur = s3dm_texmap.dudyr * dy2 + (triangle->u2 << FIXP16_SHIFT);
      s3dm_texmap.vl = s3dm_texmap.dvdyl * dy1 + (triangle->v1 << FIXP16_SHIFT);
      s3dm_texmap.vr = s3dm_texmap.dvdyr * dy2 + (triangle->v2 << FIXP16_SHIFT);
    }
    // Start Y coord
    s3dm_texmap.start_y = clipping->top;
    dy3 = triangle->y3 - clipping->top;
    // Bottom clipping
    if (triangle->y3 >= clipping->bottom) {
      SD(SAGE_TraceLog(" => y3 bottom clipping");)
      dy3 -= (triangle->y3 - clipping->bottom);
    }
    // Lines to draw
    s3dm_texmap.nb_line = dy3;
    // Go for mapping
    SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
    if (bitmap->depth == SBMP_DEPTH8) {
      SD(SAGE_TraceLog("SAGE_FastMap8BitsTexture %d lines", dy3);)
      SAGE_FastMap8BitsTexture(&s3dm_texmap);
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SD(SAGE_TraceLog("SAGE_FastMap16BitsTexture %d lines", dy3);)
      SAGE_FastMap16BitsTexture(&s3dm_texmap);
    }
#else
    if (bitmap->depth == SBMP_DEPTH8) {
      SAGE_TextureMapper8Bits();
    } else if (bitmap->depth == SBMP_DEPTH16) {
      SAGE_TextureMapper16Bits();
    }
#endif
  } else {
    SD(SAGE_TraceLog(" => draw first sub-triangle");)
    // y1 top clipping
    if (triangle->y1 < clipping->top) {
      SD(SAGE_TraceLog(" => y1 top clipping");)
      dy1 = clipping->top - triangle->y1;
      s3dm_texmap.xl = s3dm_texmap.dxdyl * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.xr = s3dm_texmap.dxdyr * dy1 + (triangle->x1 << FIXP16_SHIFT);
      s3dm_texmap.zl = s3dm_texmap.dzdyl * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_texmap.zr = s3dm_texmap.dzdyr * dy1 + (triangle->z1 << FIXP16_SHIFT);
      s3dm_texmap.ul = s3dm_texmap.dudyl * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_texmap.ur = s3dm_texmap.dudyr * dy1 + (triangle->u1 << FIXP16_SHIFT);
      s3dm_texmap.vl = s3dm_texmap.dvdyl * dy1 + (triangle->v1 << FIXP16_SHIFT);
      s3dm_texmap.vr = s3dm_texmap.dvdyr * dy1 + (triangle->v1 << FIXP16_SHIFT);
      // Start Y coord
      s3dm_texmap.start_y = clipping->top;
      dy1 = triangle->y2 - clipping->top;
    } else {
      s3dm_texmap.xl = triangle->x1 << FIXP16_SHIFT;
      s3dm_texmap.xr = s3dm_texmap.xl;
      s3dm_texmap.zl = triangle->z1 << FIXP16_SHIFT;
      s3dm_texmap.zr = s3dm_texmap.zl;
      s3dm_texmap.ul = triangle->u1 << FIXP16_SHIFT;
      s3dm_texmap.ur = s3dm_texmap.ul;
      s3dm_texmap.vl = triangle->v1 << FIXP16_SHIFT;
      s3dm_texmap.vr = s3dm_texmap.vl;
      // Start Y coord
      s3dm_texmap.start_y = triangle->y1;
    }
    if (triangle->y2 >= clipping->bottom) {
      SD(SAGE_TraceLog(" => y2 bottom clipping, draw only first sub-triangle");)
      dy1 -= (triangle->y2 - clipping->bottom);
      // Lines to draw
      s3dm_texmap.nb_line = dy1;
      // Go for mapping
      SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SD(SAGE_TraceLog("SAGE_FastMap8BitsTexture %d lines", dy1);)
        SAGE_FastMap8BitsTexture(&s3dm_texmap);
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SD(SAGE_TraceLog("SAGE_FastMap16BitsTexture %d lines", dy1);)
        SAGE_FastMap16BitsTexture(&s3dm_texmap);
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits();
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits();
      }
#endif
    } else {
      // Lines to draw
      s3dm_texmap.nb_line = dy1;
      SD(SAGE_DebugTexMap();)
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SD(SAGE_TraceLog("SAGE_FastMap8BitsTexture %d lines", dy1);)
        SAGE_FastMap8BitsTexture(&s3dm_texmap);
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SD(SAGE_TraceLog("SAGE_FastMap16BitsTexture %d lines", dy1);)
        SAGE_FastMap16BitsTexture(&s3dm_texmap);
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits();
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits();
      }
#endif
      SD(SAGE_TraceLog(" => draw second sub-triangle");)
      dy3 = triangle->y3 - triangle->y2;
      if (dy3 <= 0) {
        return;
      }
      // Calcul new deltas
      if (slope == 1) {
        s3dm_texmap.dxdyl = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dzdyl = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dudyl = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dvdyl = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.xl = triangle->x2 << FIXP16_SHIFT;
        s3dm_texmap.zl = triangle->z2 << FIXP16_SHIFT;
        s3dm_texmap.ul = triangle->u2 << FIXP16_SHIFT;
        s3dm_texmap.vl = triangle->v2 << FIXP16_SHIFT;
      } else {
        s3dm_texmap.dxdyr = ((triangle->x3 - triangle->x2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dzdyr = ((triangle->z3 - triangle->z2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dudyr = ((triangle->u3 - triangle->u2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.dvdyr = ((triangle->v3 - triangle->v2) << FIXP16_SHIFT) / dy3;
        s3dm_texmap.xr = triangle->x2 << FIXP16_SHIFT;
        s3dm_texmap.zr = triangle->z2 << FIXP16_SHIFT;
        s3dm_texmap.ur = triangle->u2 << FIXP16_SHIFT;
        s3dm_texmap.vr = triangle->v2 << FIXP16_SHIFT;
      }
      // Start Y coord
      s3dm_texmap.start_y = triangle->y2;
      // Bottom clipping
      if (triangle->y3 >= clipping->bottom) {
        dy3 -= (triangle->y3 - clipping->bottom);
      }
      // Lines to draw
      s3dm_texmap.nb_line = dy3;
      // Go for mapping
#if SAGE_MAPPER_ASM == 1
      if (bitmap->depth == SBMP_DEPTH8) {
        SD(SAGE_TraceLog("SAGE_FastMap8BitsTexture %d lines", dy3);)
        SAGE_FastMap8BitsTexture(&s3dm_texmap);
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SD(SAGE_TraceLog("SAGE_FastMap16BitsTexture %d lines", dy3);)
        SAGE_FastMap16BitsTexture(&s3dm_texmap);
      }
#else
      if (bitmap->depth == SBMP_DEPTH8) {
        SAGE_TextureMapper8Bits();
      } else if (bitmap->depth == SBMP_DEPTH16) {
        SAGE_TextureMapper16Bits();
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
  // Frame buffer
  s3dm_texmap.frame_buffer = bitmap->bitmap_buffer;
  s3dm_texmap.fb_bpr = bitmap->bpr;
  s3dm_texmap.lclip = clipping->left;
  s3dm_texmap.rclip = clipping->right;
  // Zbuffer
  if (SAGE_Get3DRenderOption(S3DR_ZBUFFER)) {
    s3dm_texmap.z_buffer = SageContext.Sage3D->render.zbuffer.buffer;
    s3dm_texmap.zb_bpr = SageContext.Sage3D->render.zbuffer.bpr;
  } else {
    s3dm_texmap.z_buffer = NULL;
    s3dm_texmap.zb_bpr = 0;
  }
  // Color
  s3dm_texmap.color = triangle->color;
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
  // Frame buffer
  s3dm_texmap.frame_buffer = bitmap->bitmap_buffer;
  s3dm_texmap.fb_bpr = bitmap->bpr;
  s3dm_texmap.lclip = clipping->left;
  s3dm_texmap.rclip = clipping->right;
  // Zbuffer
  if (SAGE_Get3DRenderOption(S3DR_ZBUFFER)) {
    s3dm_texmap.z_buffer = SageContext.Sage3D->render.zbuffer.buffer;
    s3dm_texmap.zb_bpr = SageContext.Sage3D->render.zbuffer.bpr;
  } else {
    s3dm_texmap.z_buffer = NULL;
    s3dm_texmap.zb_bpr = 0;
  }
  // Texture
  s3dm_texmap.tex_buffer = triangle->tex->bitmap->bitmap_buffer;
  s3dm_texmap.tb_bpr = triangle->tex->bitmap->bpr;
  if (triangle->tex->bitmap->properties & SBMP_TRANSPARENT) {
    s3dm_texmap.color = triangle->tex->bitmap->transparency;
  } else {
    s3dm_texmap.color = NO_TRANSP_COLOR;
  }
  // Check for triangle type
  type = SAGE_CheckTriangleType(triangle, clipping);
  // Render triangle depending on his type
  if (type == TRI_FLATTOP) {
    SAGE_DrawFlatTopTextured(triangle, bitmap, clipping);       // New version
  } else if (type == TRI_FLATBOTTOM) {
    SAGE_DrawFlatBottomTextured(triangle, bitmap, clipping);    // New version
  } else if (type == TRI_GENERIC) {
    SAGE_DrawGenericTextured(triangle, bitmap, clipping);
  }
  return TRUE;
}
