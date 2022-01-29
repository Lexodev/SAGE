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

typedef struct {
  LONG x1, y1, z1, u1, v1;
  LONG x2, y2, z2, u2, v2;
  LONG x3, y3, z3, u3, v3;
  SAGE_3DTexture * tex;
} S3D_Triangle;

/** External function for 8bits texture mapping */
extern BOOL __asm SAGE_FastMap8BitsTexture(
  register __d0 LONG lines,
  register __a0 UBYTE * texture,
  register __d1 ULONG textwidth,
  register __a1 UBYTE * bitmap,
  register __d2 ULONG bitmapwidth,
  register __a2 LONG * deltas,
  register __a3 LONG * coords
);

/** External function for 16bits texture mapping */
extern BOOL __asm SAGE_FastMap16BitsTexture(
  register __d0 LONG lines,
  register __a0 UWORD * texture,
  register __d1 ULONG textwidth,
  register __a1 UWORD * bitmap,
  register __d2 ULONG bitmapwidth,
  register __a2 LONG * deltas,
  register __a3 LONG * coords
);

/** DEBUG !!! */
VOID SAGE_DumpS3DTriangle(S3D_Triangle *);

/** Draw a textured triangle */
BOOL SAGE_DrawTexturedTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *);

#endif
