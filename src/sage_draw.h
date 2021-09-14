/**
 * sage_draw.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Graphics primitive drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#ifndef _SAGE_DRAW_H_
#define _SAGE_DRAW_H_

#include <exec/exec.h>

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
extern BOOL __asm SAGE_FastLine8Bits(
  register __a0 UBYTE * buffer,
  register __d0 LONG dx,
  register __d1 LONG dy,
  register __d2 ULONG offset,
  register __d3 LONG color
);

/** External function for 16bits line draw */
extern BOOL __asm SAGE_FastLine16Bits(
  register __a0 UWORD * buffer,
  register __d0 LONG dx,
  register __d1 LONG dy,
  register __d2 ULONG offset,
  register __d3 LONG color
);

/** External function for 32bits line draw */
extern BOOL __asm SAGE_FastLine32Bits(
  register __a0 ULONG * buffer,
  register __d0 LONG dx,
  register __d1 LONG dy,
  register __d2 ULONG offset,
  register __d3 LONG color
);

/** Fast left edge X coord calculation */
extern BOOL __asm SAGE_FastLeftEdgeCalculation(
  register __a0 LONG * buffer,
  register __d0 LONG x1,
  register __d1 LONG y1,
  register __d2 LONG x2,
  register __d3 LONG y2
);

/** Fast right edge X coord calculation */
extern BOOL __asm SAGE_FastRightEdgeCalculation(
  register __a0 LONG * buffer,
  register __d0 LONG x1,
  register __d1 LONG y1,
  register __d2 LONG x2,
  register __d3 LONG y2
);

/** Fast left edge X coord calculation */
extern LONG __asm SAGE_FastClippedLeftEdgeCalc(
  register __a0 LONG * buffer,
  register __d0 LONG x1,
  register __d1 LONG y1,
  register __d2 LONG x2,
  register __d3 LONG y2,
  register __a1 SAGE_Clipping *
);

/** Fast right edge X coord calculation */
extern LONG __asm SAGE_FastClippedRightEdgeCalc(
  register __a0 LONG * buffer,
  register __d0 LONG x1,
  register __d1 LONG y1,
  register __d2 LONG x2,
  register __d3 LONG y2,
  register __a1 SAGE_Clipping *
);

/** Fast 8bits flat quad draw */
extern BOOL __asm SAGE_DrawFlatQuad8Bits(
  register __a0 UBYTE * buffer,
  register __a1 LONG * leftcrd,
  register __a2 LONG * rightcrd,
  register __d0 LONG nbline,
  register __d1 LONG offset,
  register __d2 LONG color
);

/** Fast 16bits flat quad draw */
extern BOOL __asm SAGE_DrawFlatQuad16Bits(
  register __a0 UWORD * buffer,
  register __a1 LONG * leftcrd,
  register __a2 LONG * rightcrd,
  register __d0 LONG nbline,
  register __d1 LONG offset,
  register __d2 LONG color
);

/** Fast 32bits flat quad draw */
extern BOOL __asm SAGE_DrawFlatQuad32Bits(
  register __a0 ULONG * buffer,
  register __a1 LONG * leftcrd,
  register __a2 LONG * rightcrd,
  register __d0 LONG nbline,
  register __d1 LONG offset,
  register __d2 LONG color
);

VOID SAGE_DumpLineCoords(LONG, LONG, LONG, LONG);

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

#endif
