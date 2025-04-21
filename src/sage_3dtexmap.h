/**
 * sage_3dtexmap.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 10/03/2025)
 */

#ifndef _SAGE_3DTEXMAP_H_
#define _SAGE_3DTEXMAP_H_

#include <exec/types.h>

#include <sage/sage_compiler.h>
#include <sage/sage_bitmap.h>
#include <sage/sage_screen.h>
#include <sage/sage_3dtexture.h>
#include <sage/sage_3drender.h>

#define TRI_REJECTED          0
#define TRI_FLATTOP           1
#define TRI_FLATBOTTOM        2
#define TRI_GENERIC           3

#define FIXP16_SHIFT          16
#define FIXP16_ROUND_UP       0x8000

#define NO_TRANSP_COLOR       0xBADCBADC

typedef signed long FIXED;

typedef struct {
// All rendering process
  UBYTE *frame_buffer;              // Frame buffer address
  ULONG fb_bpr;                     // Frame buffer bytes per row
  UBYTE *z_buffer;                  // Z buffer address
  ULONG zb_bpr;                     // Z buffer bytes per row
  LONG lclip, rclip;                // Left & right clipping
// Triangle rendering
  LONG start_y, nb_line;            // Y start coordinate and number of lines to draw
  FIXED dxdyl, dxdyr;               // Delta X pente gauche et droite
  FIXED dzdyl, dzdyr;               // Delta Z pente gauche et droite
  FIXED dudyl, dudyr;               // Delta U pente gauche et droite
  FIXED dvdyl, dvdyr;               // Delta V pente gauche et droite
  UBYTE *tex_buffer;                // Texture buffer address
  ULONG tb_bpr;                     // Texture buffer bytes per row
  ULONG color;                      // Color or transparent color for texture
// Line rendering
  FIXED du, dv;                     // Texture U&V interpolation
  FIXED dz;                         // Z interpolation
  FIXED xl, xr;                     // X left & right coordinates
  FIXED zl, zr;                     // Z left & right coordinates
  FIXED ul, ur, vl, vr;             // U&V texture coordinates
} SAGE_TextureMapping;

/** Internal triangle structure */
typedef struct {
  LONG x1, y1, z1, u1, v1;
  LONG x2, y2, z2, u2, v2;
  LONG x3, y3, z3, u3, v3;
  ULONG color;
  SAGE_3DTexture *tex;
} S3D_Triangle;

/** Draw a Colored triangle */
BOOL SAGE_DrawColoredTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *);

/** Draw a textured triangle */
BOOL SAGE_DrawTexturedTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *);

#endif
