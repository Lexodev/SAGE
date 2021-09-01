/**
 * sage_blitter.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Blitting functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_BLITTER_H_
#define _SAGE_BLITTER_H_

#include <exec/types.h>
#include "sage_picture.h"
#include "sage_layer.h"

/** External function for screen copy */
extern BOOL __asm SAGE_FastCopyScreen(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD bytes,
  register __d2 ULONG offset
);

/** External function for screen clear */
extern BOOL __asm SAGE_FastClearScreen(
  register __a0 ULONG source,
  register __d0 UWORD lines,
  register __d1 UWORD bytes
);

/** External function for 8 bits bitmap clear */
extern BOOL __asm SAGE_BlitClear8Bits(
  register __a0 ULONG source,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset
);

/** External function for 8 bits bitmap copy */
extern BOOL __asm SAGE_BlitCopy8Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset
);

/** External function for 8 bits bitmap transparent copy */
extern BOOL __asm SAGE_BlitTransparentCopy8Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

/** External function for 8 bits bitmap transparent copy using AMMX */
extern BOOL __asm SAGE_AMMXBlitTranspCopy8Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

/** External function for 8 bits bitmap transparent copy using AMMX cookie cut instruction */
extern BOOL __asm SAGE_AMMXBlitCookieCut8Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

/** External function for 16 bits bitmap clear */
extern BOOL __asm SAGE_BlitClear16Bits(
  register __a0 ULONG source,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset
);

/** External function for 16 bits bitmap copy */
extern BOOL __asm SAGE_BlitCopy16Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset
);

/** External function for 16 bits bitmap transparent copy */
extern BOOL __asm SAGE_BlitTransparentCopy16Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

/** External function for 16 bits bitmap transparent copy using AMMX */
extern BOOL __asm SAGE_AMMXBlitTranspCopy16Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

/** External function for 16 bits bitmap transparent copy using AMMX cookie cut instruction */
extern BOOL __asm SAGE_AMMXBlitCookieCut16Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

/** External function for 24 bits bitmap clear */
extern BOOL __asm SAGE_BlitClear24Bits(
  register __a0 ULONG source,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset
);

/** External function for 32 bits bitmap clear */
extern BOOL __asm SAGE_BlitClear32Bits(
  register __a0 ULONG source,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset
);

/** External function for 32 bits bitmap copy */
extern BOOL __asm SAGE_BlitCopy32Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset
);

/** External function for 32 bits bitmap transparent copy */
extern BOOL __asm SAGE_BlitTransparentCopy32Bits(
  register __a0 ULONG source,
  register __a1 ULONG destination,
  register __d0 UWORD lines,
  register __d1 UWORD pixels,
  register __d2 ULONG src_offset,
  register __d3 ULONG dst_offset,
  register __d4 ULONG color
);

#endif
