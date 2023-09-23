/**
 * sage_3dmaggie.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D Maggie texmap
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DMAGGIE_H_
#define _SAGE_3DMAGGIE_H_

#include <exec/types.h>

#include "sage_3dtexmap.h"

#define MAGGIE_BILINEAR       1         // Bilinear filtering
#define MAGGIE_ZBUFFER        2         // Activate Z-Buffer
#define MAGGIE_16BITS         4         // Output in 16 bits (R5G6V5)

#define MAGGIE_TEXEL16        2         // 16 bits texel
#define MAGGIE_TEXEL32        4         // 32 bits texel

typedef struct
{
  APTR  texture;        /* $DFF250 32bit texture source */
  APTR  pixDest;        /* $DFF254 32bit Destination Screen Addr */
  APTR  depthDest;      /* $DFF258 32bit ZBuffer Addr */
  UWORD unused0;        /* $DFF25C */
  UWORD startLength;    /* $DFF25E 16bit LEN and START */
  UWORD texSize;        /* $DFF260 16bit MIP texture size (9=512/8=256/7=128/6=64) */
  UWORD mode;           /* $DFF262 16bit MODE (Bit0=Bilienar) (Bit1=Zbuffer) (Bit2=16bit output) */
  UWORD unused1;        /* $DFF264 */ 
  UWORD modulo;         /* $DFF266 16bit Destination Step */
  ULONG unused2;        /* $DFF268 */ 
  ULONG unused3;        /* $DFF26C */
  LONG uCoord;          /* $DFF270 32bit U (16:16 fixed) */
  LONG vCoord;          /* $DFF274 32bit V (16:16 fixed) */
  LONG uDelta;          /* $DFF278 32bit dU (16:16 fixed) */
  LONG vDelta;          /* $DFF27C 32bit dV (16:16 fixed) */
  WORD light;           /* $DFF280 16bit Light Ll (8:8 fixed) */
  WORD lightDelta;      /* $DFF282 16bit Light dLl (8:8 fixed) */
  ULONG lightRGBA;      /* $DFF284 32bit Light color (ARGB) */
  LONG depthStart;      /* $DFF288 32bit Z (16:16 fixed) */
  LONG depthDelta;      /* $DFF28C 32bit Delta (16:16 fixed) */
} SAGE_MaggieRegs;

typedef struct {
  APTR texture;     // Texture adr
  WORD mipsize;     // Texture size
  APTR zbuffer;     // Z buffer adr
  WORD mode;        // Rendering mode
  WORD texmod;      // Texel modulo
} SAGE_MaggieData;

/** External function for 16bits texture mapping with Maggie */
extern BOOL ASM SAGE_MaggieMap16BitsTexture(
  REG(d0, LONG lines),
  REG(a0, APTR texture),
  REG(d1, WORD textwidth),
  REG(a1, APTR bitmap),
  REG(d2, ULONG bitmapwidth),
  REG(a2, LONG * deltas),
  REG(a3, LONG * coords),
  REG(a4, APTR zbuffer),
  REG(d3, WORD rendermode),
  REG(d4, WORD texelmod)
);

/** Draw a textured triangle with Maggie */
BOOL SAGE_DrawMaggieTriangle(S3D_Triangle *, SAGE_Bitmap *, SAGE_Clipping *, SAGE_MaggieData *);

#endif
