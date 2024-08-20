/**
 * sage_3dtexmap.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
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

#define DELTA_DXDYL           0
#define DELTA_DUDYL           1
#define DELTA_DVDYL           2
#define DELTA_DXDYR           3
#define DELTA_DUDYR           4
#define DELTA_DVDYR           5
#define DELTA_DU              6
#define DELTA_DV              7
#define DELTA_DZDYL           8
#define DELTA_DZDYR           9
#define DELTA_DZ              10

#define CRD_XL                0
#define CRD_XR                1
#define CRD_UL                2
#define CRD_VL                3
#define CRD_UR                4
#define CRD_VR                5
#define CRD_LINE              6
#define CRD_LCLIP             7
#define CRD_RCLIP             8
#define CRD_TCOLOR            9
#define CRD_ZL                10
#define CRD_ZR                11
#define CRD_ZBUF              13

/*
  STRUCTURE POUR MAPPING
  
 Pour tout le rendu
  SCRBUF  Adresse du frame buffer
  SCRBPR  Bytes per row de l'écran
  ZBUFFER Adresse du ZBuffer
  ZBUBPR  Bytes per row du ZBuffer
  LCLIP   Clipping gauche
  RCLIP   Clipping droit

 Pour chaque triangle
  YTOP    Coordonnée Y de départ
  DY      Nombre de lignes
  DXDYL   Delta X sur pente gauche
  DXDYR   Delta X sur pente droite
  DZDYL   Delta Z sur pente gauche
  DZDYR   Delta Z sur pente droite
  DUDYL   Delta U texture sur pente gauche
  DUDYR   Delta U texture sur pente droite
  DVDYL   Delta V texture sur pente gauche
  DVDYR   Delta V texture sur pendre droite
  TEXBUF  Adresse de la texture
  TEXBPR  Bytes per row de la texture
  TCOLOR  Couleur transparente

 Pour chaque ligne
  DU      Interpolation texture U
  DV      Interpolation texture V
  DZ      Interpolation Z
  XL      Coordonnée X gauche
  XR      Coordonnée X droite
  ZL      Coordonnée Z gauche
  ZR      Coordonnée Z droite
  UL      Coordonnée texture U gauche
  UR      Coordonnée texture U droite
  VL      Coordonnée texture V gauche
  VR      Coordonnée texture V droite
*/

typedef signed long FIXED;

typedef struct {
// All rendering process
  APTR frame_buffer;                // Frame buffer address
  ULONG fb_bpr;                     // Frame buffer bytes per row
  APTR z_buffer;                    // Z buffer address
  ULONG zb_bpr;                     // Z buffer bytes per row
  LONG lclip, rclip;                // Left & right clipping
// Triangle rendering
  LONG start_y, nb_line;            // Y start coordinate and number of lines to draw
  FIXED dxdyl, dxdyr;               // Delta X pente gauche et droite
  FIXED dzdyl, dzdyr;               // Delta Z pente gauche et droite
  FIXED dudyl, dudyr;               // Delta U pente gauche et droite
  FIXED dvdyl, dvdyr;               // Delta V pente gauche et droite
  APTR tex_buffer;                  // Texture buffer address
  ULONG tb_bpr;                     // Texture buffer bytes per row
  ULONG trans_color;                // Transparent color
// Line rendering
  FIXED du, dv;                     // Texture U&V interpolation
  FIXED dz;                         // Z interpolation
  FIXED xl, xr;                     // X left & right coordinates
  FIXED zl, zr;                     // Z left & right coordinates
  FIXED ul, ur, vl, vr;             // U&V texture coordinates
} SAGE_TextureMapping;

typedef struct {
  LONG x1, y1, z1, u1, v1;
  LONG x2, y2, z2, u2, v2;
  LONG x3, y3, z3, u3, v3;
  ULONG color;
  SAGE_3DTexture *tex;
} S3D_Triangle;

/** External function for z buffer clear */
extern BOOL ASM SAGE_FastClearZBuffer(
  REG(a0, ULONG source),
  REG(d0, UWORD lines),
  REG(d1, UWORD bytes)
);

/** External function for 8bits texture mapping */
extern BOOL ASM SAGE_FastMap8BitsTexture(
  REG(d0, LONG lines),
  REG(a0, UBYTE *texture),
  REG(d1, ULONG textwidth),
  REG(a1, UBYTE *bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG *deltas),
  REG(a3, LONG *coords)
);

/** External function for 16bits texture mapping */
extern BOOL ASM SAGE_FastMap16BitsTexture(
  REG(d0, LONG lines),
  REG(a0, UWORD *texture),
  REG(d1, ULONG textwidth),
  REG(a1, UWORD *bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG *deltas),
  REG(a3, LONG *coords)
);

/** External function for 8bits color mapping */
extern BOOL ASM SAGE_FastMap8BitsColor(
  REG(d0, LONG lines),
  REG(d1, ULONG color),
  REG(a1, UBYTE *bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG *deltas),
  REG(a3, LONG *coords)
);

/** External function for 16bits color mapping */
extern BOOL ASM SAGE_FastMap16BitsColor(
  REG(d0, LONG lines),
  REG(d1, ULONG color),
  REG(a1, UWORD *bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG *deltas),
  REG(a3, LONG *coords)
);

/** External function for 8bits transparent texture mapping */
extern BOOL ASM SAGE_FastMap8BitsTransparent(
  REG(d0, LONG lines),
  REG(a0, UBYTE *texture),
  REG(d1, ULONG textwidth),
  REG(a1, UBYTE *bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG *deltas),
  REG(a3, LONG *coords)
);

/** External function for 16bits transparent texture mapping */
extern BOOL ASM SAGE_FastMap16BitsTransparent(
  REG(d0, LONG lines),
  REG(a0, UWORD *texture),
  REG(d1, ULONG textwidth),
  REG(a1, UWORD *bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG *deltas),
  REG(a3, LONG *coords)
);

/** Check the triangle type & order the vertices */
ULONG SAGE_CheckTriangleType(S3D_Triangle *, SAGE_Clipping *);

/** Draw a Colored triangle */
BOOL SAGE_DrawColoredTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *);

/** Draw a textured triangle */
BOOL SAGE_DrawTexturedTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *);

#endif
