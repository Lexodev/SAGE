/**
 * sage_draw.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Graphics primitive drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#ifndef _SAGE_DRAW_H_
#define _SAGE_DRAW_H_

#include <exec/exec.h>

#include <sage/sage_screen.h>

/** Pixel structure */
typedef struct {
  LONG x, y, color;
} SAGE_Pixel;

/** Line structure */
typedef struct {
  LONG x1, y1, x2, y2, color;
} SAGE_Line;

/** Triangle structure */
typedef struct {
  LONG x1, y1, x2, y2, x3, y3, color;
} SAGE_Triangle;

/** External function for 8bits line draw */
extern BOOL ASM SAGE_FastLine8Bits(
  REG(a0, UBYTE *buffer),
  REG(d0, LONG dx),
  REG(d1, LONG dy),
  REG(d2, ULONG offset),
  REG(d3, LONG color)
);

/** External function for 16bits line draw */
extern BOOL ASM SAGE_FastLine16Bits(
  REG(a0, UWORD *buffer),
  REG(d0, LONG dx),
  REG(d1, LONG dy),
  REG(d2, ULONG offset),
  REG(d3, LONG color)
);

/** External function for 32bits line draw */
extern BOOL ASM SAGE_FastLine32Bits(
  REG(a0, ULONG *buffer),
  REG(d0, LONG dx),
  REG(d1, LONG dy),
  REG(d2, ULONG offset),
  REG(d3, LONG color)
);

/** Fast left edge X coord calculation */
extern BOOL ASM SAGE_FastLeftEdgeCalculation(
  REG(a0, LONG *buffer),
  REG(d0, LONG x1),
  REG(d1, LONG y1),
  REG(d2, LONG x2),
  REG(d3, LONG y2)
);

/** Fast right edge X coord calculation */
extern BOOL ASM SAGE_FastRightEdgeCalculation(
  REG(a0, LONG *buffer),
  REG(d0, LONG x1),
  REG(d1, LONG y1),
  REG(d2, LONG x2),
  REG(d3, LONG y2)
);

/** Fast left edge X coord calculation */
extern LONG ASM SAGE_FastClippedLeftEdgeCalc(
  REG(a0, LONG *buffer),
  REG(d0, LONG x1),
  REG(d1, LONG y1),
  REG(d2, LONG x2),
  REG(d3, LONG y2),
  REG(a1, SAGE_Clipping *clip)
);

/** Fast right edge X coord calculation */
extern LONG ASM SAGE_FastClippedRightEdgeCalc(
  REG(a0, LONG *buffer),
  REG(d0, LONG x1),
  REG(d1, LONG y1),
  REG(d2, LONG x2),
  REG(d3, LONG y2),
  REG(a1, SAGE_Clipping *clip)
);

/** Fast 8bits flat quad draw */
extern BOOL ASM SAGE_DrawFlatQuad8Bits(
  REG(a0, UBYTE *buffer),
  REG(a1, LONG *leftcrd),
  REG(a2, LONG *rightcrd),
  REG(d0, LONG nbline),
  REG(d1, LONG offset),
  REG(d2, LONG color)
);

/** Fast 16bits flat quad draw */
extern BOOL ASM SAGE_DrawFlatQuad16Bits(
  REG(a0, UWORD *buffer),
  REG(a1, LONG *leftcrd),
  REG(a2, LONG *rightcrd),
  REG(d0, LONG nbline),
  REG(d1, LONG offset),
  REG(d2, LONG color)
);

/** Fast 32bits flat quad draw */
extern BOOL ASM SAGE_DrawFlatQuad32Bits(
  REG(a0, ULONG *buffer),
  REG(a1, LONG *leftcrd),
  REG(a2, LONG *rightcrd),
  REG(d0, LONG nbline),
  REG(d1, LONG offset),
  REG(d2, LONG color)
);

/** Draw a pixel with clipping */
BOOL SAGE_DrawClippedPixel(LONG, LONG, LONG);

/** Draw a pixel */
BOOL SAGE_DrawPixel(LONG, LONG, LONG);

/** Draw an array of pixels */
BOOL SAGE_DrawPixelArray(SAGE_Pixel *, ULONG);

/** Draw a line with clipping */
BOOL SAGE_DrawClippedLine(LONG, LONG, LONG, LONG, LONG);

/** Draw a line */
BOOL SAGE_DrawLine(LONG, LONG, LONG, LONG, LONG);

/** Draw a line strip */
BOOL SAGE_DrawLineStrip(SAGE_Pixel *, ULONG);

/** Draw an array of lines */
BOOL SAGE_DrawLineArray(SAGE_Line *, ULONG);

/** Draw a triangle */
BOOL SAGE_DrawTriangle(LONG, LONG, LONG, LONG, LONG, LONG, LONG);

/** Draw a clipped triangle */
BOOL SAGE_DrawClippedTriangle(LONG, LONG, LONG, LONG, LONG, LONG, LONG);

/** Only for debug purpose, don't use it in your code */

/** DEBUG : Dump line coordinates */
VOID SAGE_DumpLineCoords(LONG, LONG, LONG, LONG);

#endif
