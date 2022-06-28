/**
 * sage_3dtexmap.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DTEXMAP_H_
#define _SAGE_3DTEXMAP_H_

#include <exec/types.h>

#include "sage_compiler.h"
#include "sage_bitmap.h"
#include "sage_screen.h"
#include "sage_3dtexture.h"
#include "sage_3drender.h"

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

typedef struct {
  LONG x1, y1, z1, u1, v1;
  LONG x2, y2, z2, u2, v2;
  LONG x3, y3, z3, u3, v3;
  ULONG color;
  SAGE_3DTexture * tex;
} S3D_Triangle;

/** External function for 8bits texture mapping */
extern BOOL ASM SAGE_FastMap8BitsTexture(
  REG(d0, LONG lines),
  REG(a0, UBYTE * texture),
  REG(d1, ULONG textwidth),
  REG(a1, UBYTE * bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords)
);

/** External function for 16bits texture mapping */
extern BOOL ASM SAGE_FastMap16BitsTexture(
  REG(d0, LONG lines),
  REG(a0, UWORD * texture),
  REG(d1, ULONG textwidth),
  REG(a1, UWORD * bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords)
);

/** External function for 8bits color mapping */
extern BOOL ASM SAGE_FastMap8BitsColor(
  REG(d0, LONG lines),
  REG(d1, ULONG color),
  REG(a1, UBYTE * bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords)
);

/** External function for 16bits color mapping */
extern BOOL ASM SAGE_FastMap16BitsColor(
  REG(d0, LONG lines),
  REG(d1, ULONG color),
  REG(a1, UWORD * bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords)
);

/** External function for 8bits transparent texture mapping */
extern BOOL ASM SAGE_FastMap8BitsTransparent(
  REG(d0, LONG lines),
  REG(a0, UBYTE * texture),
  REG(d1, ULONG textwidth),
  REG(a1, UBYTE * bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords)
);

/** External function for 16bits transparent texture mapping */
extern BOOL ASM SAGE_FastMap16BitsTransparent(
  REG(d0, LONG lines),
  REG(a0, UWORD * texture),
  REG(d1, ULONG textwidth),
  REG(a1, UWORD * bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords)
);

/** DEBUG */
VOID SAGE_DumpS3DTriangle(S3D_Triangle *);
/** DEBUG */

/** Draw a textured triangle */
BOOL SAGE_DrawTexturedTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *);

#endif
