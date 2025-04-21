/**
 * sage_blitter.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Blitting functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#ifndef _SAGE_BLITTER_H_
#define _SAGE_BLITTER_H_

#include <exec/types.h>

#include <sage/sage_compiler.h>

/** External function for screen copy */
extern BOOL ASM SAGE_FastCopyScreen(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD bytes),
  REG(d2, ULONG offset)
);

/** External function for screen clear */
extern BOOL ASM SAGE_FastClearScreen(
  REG(a0, ULONG source),
  REG(d0, UWORD lines),
  REG(d1, UWORD bytes)
);

/** External function for 8 bits bitmap fill */
extern BOOL ASM SAGE_BlitFill8Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG color)
);

/** External function for 8 bits bitmap copy */
extern BOOL ASM SAGE_BlitCopy8Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset)
);

/** External function for 8 bits bitmap zoom copy */
extern BOOL ASM SAGE_BlitZoomCopy8Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD src_width),
  REG(d1, UWORD src_height),
  REG(d2, ULONG src_offset),
  REG(a1, ULONG destination),
  REG(d3, ULONG dst_width),
  REG(d4, ULONG dst_height),
  REG(d5, ULONG dst_offset)
);

/** External function for 8 bits bitmap transparent copy */
extern BOOL ASM SAGE_BlitTransparentCopy8Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 8 bits bitmap transparent zoom copy */
extern BOOL ASM SAGE_BlitTranspZoomCopy8Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD src_width),
  REG(d1, UWORD src_height),
  REG(d2, ULONG src_offset),
  REG(a1, ULONG destination),
  REG(d3, ULONG dst_width),
  REG(d4, ULONG dst_height),
  REG(d5, ULONG dst_offset),
  REG(d6, ULONG color)
);

/** External function for 8 bits bitmap transparent copy using AMMX */
extern BOOL ASM SAGE_AMMXBlitTranspCopy8Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 8 bits bitmap transparent copy using AMMX cookie cut instruction */
extern BOOL ASM SAGE_AMMXBlitCookieCut8Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 16 bits bitmap fill */
extern BOOL ASM SAGE_BlitFill16Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG color)
);

/** External function for 16 bits bitmap copy */
extern BOOL ASM SAGE_BlitCopy16Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset)
);

/** External function for 16 bits bitmap zoom copy */
extern BOOL ASM SAGE_BlitZoomCopy16Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD src_width),
  REG(d1, UWORD src_height),
  REG(d2, ULONG src_offset),
  REG(a1, ULONG destination),
  REG(d3, ULONG dst_width),
  REG(d4, ULONG dst_height),
  REG(d5, ULONG dst_offset)
);

/** External function for 16 bits bitmap transparent copy */
extern BOOL ASM SAGE_BlitTransparentCopy16Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 16 bits bitmap transparent zoom copy */
extern BOOL ASM SAGE_BlitTranspZoomCopy16Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD src_width),
  REG(d1, UWORD src_height),
  REG(d2, ULONG src_offset),
  REG(a1, ULONG destination),
  REG(d3, ULONG dst_width),
  REG(d4, ULONG dst_height),
  REG(d5, ULONG dst_offset),
  REG(d6, ULONG color)
);

/** External function for 16 bits bitmap transparent copy using AMMX */
extern BOOL ASM SAGE_AMMXBlitTranspCopy16Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 16 bits bitmap transparent copy using AMMX cookie cut instruction */
extern BOOL ASM SAGE_AMMXBlitCookieCut16Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset)
);

/** External function for 16 bits bitmap transparent zoom copy using AMMX cookie cut instruction */
extern BOOL ASM SAGE_AMMXBlitCookieCutZoom16Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD src_width),
  REG(d1, UWORD src_height),
  REG(d2, ULONG src_offset),
  REG(a1, ULONG destination),
  REG(d3, ULONG dst_width),
  REG(d4, ULONG dst_height),
  REG(d5, ULONG dst_offset)
);

/** External function for 24 bits bitmap fill */
extern BOOL ASM SAGE_BlitFill24Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG color)
);

/** External function for 32 bits bitmap fill */
extern BOOL ASM SAGE_BlitFill32Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG color)
);

/** External function for 32 bits bitmap copy */
extern BOOL ASM SAGE_BlitCopy32Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset)
);

/** External function for 32 bits bitmap transparent copy */
extern BOOL ASM SAGE_BlitTransparentCopy32Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 32 bits bitmap transparent copy using AMMX */
extern BOOL ASM SAGE_AMMXBlitTranspCopy32Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset),
  REG(d4, ULONG color)
);

/** External function for 32 bits bitmap transparent copy using AMMX cookie cut instruction */
extern BOOL ASM SAGE_AMMXBlitCookieCut32Bits(
  REG(a0, ULONG source),
  REG(a1, ULONG destination),
  REG(d0, UWORD lines),
  REG(d1, UWORD pixels),
  REG(d2, ULONG src_offset),
  REG(d3, ULONG dst_offset)
);

/** External function for 32 bits bitmap transparent zoom copy using AMMX cookie cut instruction */
extern BOOL ASM SAGE_AMMXBlitCookieCutZoom32Bits(
  REG(a0, ULONG source),
  REG(d0, UWORD src_width),
  REG(d1, UWORD src_height),
  REG(d2, ULONG src_offset),
  REG(a1, ULONG destination),
  REG(d3, ULONG dst_width),
  REG(d4, ULONG dst_height),
  REG(d5, ULONG dst_offset)
);

#endif
