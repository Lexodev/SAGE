;--------------------------------------
; sage_fastdraw.asm
;
; SAGE (Simple Amiga Game Engine) project
; Fast draw primitives
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 25.1 February 2025 (updated: 26/02/2025)
;--------------------------------------

; SAGE_Clipping struct
SSCR_LEFTCLIP       EQU 0
SSCR_TOPCLIP        EQU 4
SSCR_RIGHTCLIP      EQU 8
SSCR_BOTTOMCLIP     EQU 12
SSCR_CLIPSIZE       EQU 16

  SECTION fastdraw,code

;--------------------------------------
; Draw a line on a 8bits screen
;
; @in a0.l line start address
; @in d0.l delta x
; @in d1.l delta y
; @in d2.l offset to the next line in bytes
; @in d3.l color in CLUT format
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastLine8Bits

_SAGE_FastLine8Bits:
  movem.l d1-d5/a0,-(sp)
  tst.l   d0                            ; dx > 0 ?
  blt     .DxNegative
  bne.s   .DxPositive
.DxZero:                                ; vertical line
  move.b  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; next row
  dbf     d1,.DxZero
  bra     .EndDraw
.DxPositive:
  tst.l   d1                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive
.DyZeroPositive:                        ; horizontal line
  move.b  d3,(a0)+                      ; draw pixel
  dbf     d0,.DyZeroPositive
  bra     .EndDraw
.DyNotZeroPositive:
  cmp.l   d0,d1                         ; dx >= dy ?
  bgt.s   .DySupDxPos
  move.l  d0,d4                         ; line len
  move.l  d0,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawSoftPositive:
  move.b  d3,(a0)+                      ; draw pixel
  sub.l   d1,d5                         ; e - dy
  bge.s   .NextSoftPositive
  adda.l  d2,a0                         ; next row
  add.l   d0,d5                         ; e + dx
.NextSoftPositive:
  dbf     d4,.DrawSoftPositive
  bra     .EndDraw
.DySupDxPos:
  move.l  d1,d4                         ; line len
  move.l  d1,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawHardPositive:
  move.b  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; next row
  sub.l   d0,d5                         ; e - dx
  bge.s   .NextHardPositive
  adda.l  #1,a0                         ; next column
  add.l   d1,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw
.DxNegative:
  neg.l   d0                            ; d0 * -1
  tst.l   d1                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative
  adda.l  #1,a0
.DyZeroNegative:                        ; horizontal line
  move.b  d3,-(a0)                      ; draw pixel
  dbf     d0,.DyZeroNegative
  bra     .EndDraw
.DyNotZeroNegative:
  cmp.l   d0,d1                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d0,d4                         ; line len
  move.l  d0,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
  adda.l  #1,a0
.DrawSoftNegative:
  move.b  d3,-(a0)                      ; draw pixel
  sub.l   d1,d5                         ; e - dy
  bge.s   .NextSoftNegative
  adda.l  d2,a0                         ; change line
  add.l   d0,d5                         ; e + dx
.NextSoftNegative:
  dbf     d4,.DrawSoftNegative
  bra     .EndDraw
.DySupDxNeg:
  move.l  d1,d4                         ; line len
  move.l  d1,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawHardNegative:
  move.b  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; change line
  sub.l   d0,d5                         ; e - dx
  bge.s   .NextHardNegative
  suba.l  #1,a0                         ; previous column
  add.l   d1,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative
.EndDraw:
  movem.l (sp)+,d1-d5/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Draw a line on a 16bits screen
;
; @in a0.l line start address
; @in d0.l delta x
; @in d1.l delta y
; @in d2.l offset to the next line in bytes
; @in d3.l color in RGB16 format
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastLine16Bits

_SAGE_FastLine16Bits:
  movem.l d1-d5/a0,-(sp)
  tst.l   d0                            ; dx > 0 ?
  blt     .DxNegative
  bne.s   .DxPositive
.DxZero:                                ; vertical line
  move.w  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; next row
  dbf     d1,.DxZero
  bra     .EndDraw
.DxPositive:
  tst.l   d1                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive
.DyZeroPositive:                        ; horizontal line
  move.w  d3,(a0)+                      ; draw pixel
  dbf     d0,.DyZeroPositive
  bra     .EndDraw
.DyNotZeroPositive:
  cmp.l   d0,d1                         ; dx >= dy ?
  bgt.s   .DySupDxPos
  move.l  d0,d4                         ; line len
  move.l  d0,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawSoftPositive:
  move.w  d3,(a0)+                      ; draw pixel
  sub.l   d1,d5                         ; e - dy
  bge.s   .NextSoftPositive
  adda.l  d2,a0                         ; next row
  add.l   d0,d5                         ; e + dx
.NextSoftPositive:
  dbf     d4,.DrawSoftPositive
  bra     .EndDraw
.DySupDxPos:
  move.l  d1,d4                         ; line len
  move.l  d1,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawHardPositive:
  move.w  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; next row
  sub.l   d0,d5                         ; e - dx
  bge.s   .NextHardPositive
  adda.l  #2,a0                         ; next column
  add.l   d1,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw
.DxNegative:
  neg.l   d0                            ; d0 * -1
  tst.l   d1                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative
  adda.l  #2,a0
.DyZeroNegative:                        ; horizontal line
  move.w  d3,-(a0)                      ; draw pixel
  dbf     d0,.DyZeroNegative
  bra     .EndDraw
.DyNotZeroNegative:
  cmp.l   d0,d1                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d0,d4                         ; line len
  move.l  d0,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
  adda.l  #2,a0
.DrawSoftNegative:
  move.w  d3,-(a0)                      ; draw pixel
  sub.l   d1,d5                         ; e - dy
  bge.s   .NextSoftNegative
  adda.l  d2,a0                         ; change line
  add.l   d0,d5                         ; e + dx
.NextSoftNegative:
  dbf     d4,.DrawSoftNegative
  bra     .EndDraw
.DySupDxNeg:
  move.l  d1,d4                         ; line len
  move.l  d1,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawHardNegative:
  move.w  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; change line
  sub.l   d0,d5                         ; e - dx
  bge.s   .NextHardNegative
  suba.l  #2,a0                         ; previous column
  add.l   d1,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative
.EndDraw:
  movem.l (sp)+,d1-d5/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Draw a line on a 32bits screen
;
; @in a0.l line start address
; @in d0.l delta x
; @in d1.l delta y
; @in d2.l offset to the next line in bytes
; @in d3.l color in ARGB format
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastLine32Bits

_SAGE_FastLine32Bits:
  movem.l d1-d5/a0,-(sp)
  tst.l   d0                            ; dx > 0 ?
  blt     .DxNegative
  bne.s   .DxPositive
.DxZero:                                ; vertical line
  move.l  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; next row
  dbf     d1,.DxZero
  bra     .EndDraw
.DxPositive:
  tst.l   d1                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive
.DyZeroPositive:                        ; horizontal line
  move.l  d3,(a0)+                      ; draw pixel
  dbf     d0,.DyZeroPositive
  bra     .EndDraw
.DyNotZeroPositive:
  cmp.l   d0,d1                         ; dx >= dy ?
  bgt.s   .DySupDxPos
  move.l  d0,d4                         ; line len
  move.l  d0,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawSoftPositive:
  move.l  d3,(a0)+                      ; draw pixel
  sub.l   d1,d5                         ; e - dy
  bge.s   .NextSoftPositive
  adda.l  d2,a0                         ; next row
  add.l   d0,d5                         ; e + dx
.NextSoftPositive:
  dbf     d4,.DrawSoftPositive
  bra     .EndDraw
.DySupDxPos:
  move.l  d1,d4                         ; line len
  move.l  d1,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawHardPositive:
  move.l  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; next row
  sub.l   d0,d5                         ; e - dx
  bge.s   .NextHardPositive
  adda.l  #4,a0                         ; next column
  add.l   d1,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw
.DxNegative:
  neg.l   d0                            ; d0 * -1
  tst.l   d1                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative
  adda.l  #4,a0
.DyZeroNegative:                        ; horizontal line
  move.l  d3,-(a0)                      ; draw pixel
  dbf     d0,.DyZeroNegative
  bra     .EndDraw
.DyNotZeroNegative:
  cmp.l   d0,d1                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d0,d4                         ; line len
  move.l  d0,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
  adda.l  #4,a0
.DrawSoftNegative:
  move.l  d3,-(a0)                      ; draw pixel
  sub.l   d1,d5                         ; e - dy
  bge.s   .NextSoftNegative
  adda.l  d2,a0                         ; change line
  add.l   d0,d5                         ; e + dx
.NextSoftNegative:
  dbf     d4,.DrawSoftNegative
  bra     .EndDraw
.DySupDxNeg:
  move.l  d1,d4                         ; line len
  move.l  d1,d5                         ; e
  add.l   d0,d0                         ; dx * 2
  add.l   d1,d1                         ; dy * 2
.DrawHardNegative:
  move.l  d3,(a0)                       ; draw pixel
  adda.l  d2,a0                         ; change line
  sub.l   d0,d5                         ; e - dx
  bge.s   .NextHardNegative
  suba.l  #4,a0                         ; previous column
  add.l   d1,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative
.EndDraw:
  movem.l (sp)+,d1-d5/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Calcul coordinates for an edge
;
; @in a0.l buffer for coordinates
; @in d0.l x1 coordinate
; @in d1.l y1 coordinate
; @in d2.l x2 coordinate
; @in d3.l y2 coordinate
;
; @out d0.l number of calculated coordinates
;--------------------------------------

  xdef _SAGE_EdgeCalc

_SAGE_EdgeCalc:
  movem.l d1-d3/a0,-(sp)
  moveq.l #0,d7
  cmp.l   d1,d3
  beq     .EndCalculate               ; y1 = y2
  bge.s   .CalcDelta                  ; y2 >= y1
  exg     d0,d2
  exg     d1,d3
.CalcDelta:
  fmove.l d0,fp0                      ; x1
  fmove.l d1,fp1                      ; y1
  fmove.l d2,fp2                      ; x2
  fmove.l d3,fp3                      ; y2
  fsub    fp0,fp2                     ; x2 - x1
  fsub    fp1,fp3                     ; y2 - y1
  fdiv    fp3,fp2                     ; delta => fp2(x2 - x1) / fp3(y2 - y1) 
  sub.l   d1,d3
  move.l  d3,d7
  subq.l  #1,d3                       ; real height
.NextEdge:
  fmove.l fp0,(a0)+                   ; x edge
  fadd    fp2,fp0                     ; x + delta
  dbf     d3,.NextEdge
.EndCalculate:
  move.l  d7,d0
  movem.l (sp)+,d1-d3/a0
  rts

;--------------------------------------
; Draw a flat quad for 8bits screen
; upper and lower segments are horizontal
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of lines to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color in CLUT format (extended to 32bits)
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawFlatQuad8Bits

_SAGE_DrawFlatQuad8Bits:
  movem.l d1-d5/a0-a3,-(sp)
  subq.l  #1,d0
.NextLine:
  move.l  (a1)+,d3                      ; left coord
  move.l  (a2)+,d4                      ; right coord
  move.l  a0,a3                         ; line address
  sub.l   d3,d4                         ; pixels to draw
  add.l   d3,a3                         ; start address
  addq.l  #1,d4                         ; even if start = end we should draw a pixel
  move.l  d4,d5                         ; save the value
  andi.l  #$7,d5                        ; look for a multiple of 8
  beq.s   .DrawFullLine                 ; draw only block of eight pixels
  subq.l  #1,d5                         ; extra pixels to draw
.DrawExtraPixel:
  move.b  d2,(a3)+
  dbf     d5,.DrawExtraPixel
.DrawFullLine:
  andi.l  #$fffffff8,d4                 ; clear low bits
  beq.s   .SkipLine                     ; nothing more to draw
  lsr.l   #3,d4                         ; draw 8 pixels each time
  subq.l  #1,d4                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d4,.DrawLine
.SkipLine:
  adda.l  d1,a0
  dbf     d0,.NextLine
.EndDraw:
  movem.l (sp)+,d1-d5/a0-a3
  move.l  #-1,d0
  rts

;--------------------------------------
; Draw a flat quad for 16bits screen
; upper and lower segments are horizontal
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of rows to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color in RGB16 format (extended to 32bits)
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawFlatQuad16Bits

_SAGE_DrawFlatQuad16Bits:
  movem.l d1-d5/a0-a3,-(sp)
  subq.l  #1,d0
.NextLine:
  move.l  (a1)+,d3                      ; left coord
  move.l  (a2)+,d4                      ; right coord
  move.l  a0,a3                         ; line address
  sub.l   d3,d4                         ; pixels to draw
  add.l   d3,d3                         ; 2 bytes per pixel
  add.l   d3,a3                         ; start address
  addq.l  #1,d4                         ; even if start = end we should draw a pixel
  move.l  d4,d5                         ; save the value
  andi.l  #$3,d5                        ; look for a multiple of 4
  beq.s   .DrawFullLine                 ; draw only block of eight pixels
  subq.l  #1,d5                         ; extra pixels to draw
.DrawExtraPixel:
  move.w  d2,(a3)+
  dbf     d5,.DrawExtraPixel
.DrawFullLine:
  andi.l  #$fffffffc,d4                 ; clear low bits
  beq.s   .SkipLine                     ; nothing more to draw
  lsr.l   #2,d4                         ; draw 4 pixels each time
  subq.l  #1,d4                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d4,.DrawLine
.SkipLine:
  adda.l  d1,a0
  dbf     d0,.NextLine
.EndDraw:
  movem.l (sp)+,d1-d5/a0-a3
  move.l  #-1,d0
  rts

;--------------------------------------
; Draw a flat quad for 32bits screen
; upper and lower segments are horizontal
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of rows to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color in ARGB format
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawFlatQuad32Bits

_SAGE_DrawFlatQuad32Bits:
  movem.l d1-d4/a0-a3,-(sp)
  subq.l  #1,d0
.NextLine:
  move.l  (a1)+,d3                      ; left coord
  move.l  (a2)+,d4                      ; right coord
  move.l  a0,a3                         ; line address
  sub.l   d3,d4                         ; pixels to draw
  lsl.l   #2,d3                         ; 4 bytes per pixel
  add.l   d3,a3                         ; start address
  addq.l  #1,d4                         ; even if start = end we should draw a pixel
  btst    #0,d4                         ; look for a multiple of 2
  beq.s   .DrawFullLine                 ; draw only block of eight pixels
.DrawExtraPixel:
  move.l  d2,(a3)+
.DrawFullLine:
  andi.l  #$fffffffe,d4                 ; clear low bits
  beq.s   .SkipLine                     ; nothing more to draw
  lsr.l   #1,d4                         ; draw 2 pixels each time
  subq.l  #1,d4                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d4,.DrawLine
.SkipLine:
  adda.l  d1,a0
  dbf     d0,.NextLine
.EndDraw:
  movem.l (sp)+,d1-d4/a0-a3
  move.l  #-1,d0
  rts

;--------------------------------------
; Calcul coordinates for an edge
; with clipping support
;
; @in a0.l buffer for coordinates
; @in d0.l x1 coordinate
; @in d1.l y1 coordinate
; @in d2.l x2 coordinate
; @in d3.l y2 coordinate
; @in d4.l top clip
; @in d5.l bottom clip
;
; @out d0.l number of calculated coordinates
;--------------------------------------

  xdef _SAGE_ClippedEdgeCalc

_SAGE_ClippedEdgeCalc:
  movem.l d1-d7/a0,-(sp)
  moveq.l #0,d7
  cmp.l   d1,d3
  beq     .EndCalculate               ; y1 = y2
  bge.s   .NoSwap                     ; y2 >= y1
  exg     d0,d2
  exg     d1,d3
.NoSwap:
  cmp.l   d1,d5
  blt     .EndCalculate               ; bclip < y1
  cmp.l   d3,d4
  bgt     .EndCalculate               ; tclip > y2
.CalculateDelta:
  fmove.l d0,fp0                      ; x1
  fmove.l d1,fp1                      ; y1
  fmove.l d2,fp2                      ; x2
  fmove.l d3,fp3                      ; y2
  fsub    fp0,fp2                     ; x2 - x1
  fsub    fp1,fp3                     ; y2 - y1
  fdiv    fp3,fp2                     ; delta => fp2(x2 - x1) / fp3(y2 - y1) 
.ClipCheck:
  cmp.l   d1,d4
  ble.s   .NoTopClip                  ; tclip <= y1
  fmove.l d4,fp4                      ; tclip
  fsub    fp1,fp4                     ; tclip - y1
  fmul    fp2,fp4                     ; delta * (tclip - y1)
  fadd    fp4,fp0                     ; x1 + delta * (tclip - y1)
  move.l  d4,d1                       ; y1 clipped
.NoTopClip:
  cmp.l   d3,d5
  bge.s   .NoBottomClip               ; bclip >= y2
  move.l  d5,d3                       ; y2 clipped
  addq.l  #1,d3                       ; trick to have the last line when clipped
.NoBottomClip:
  sub.l   d1,d3
  move.l  d3,d7
  subq.l  #1,d3                       ; real height
.NextEdge:
  fmove.l fp0,(a0)+                   ; x edge
  fadd    fp2,fp0                     ; x + delta
  dbf     d3,.NextEdge
.EndCalculate:
  move.l  d7,d0
  movem.l (sp)+,d1-d7/a0
  rts

;--------------------------------------
; Draw a flat quad in 8bits with clipping
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of lines to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color in CLUT format (extended to 32bits)
; @in d3.l left clip
; @in d4.l right clip
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawClippedFlatQuad8Bits

_SAGE_DrawClippedFlatQuad8Bits:
  movem.l d0-d7/a0-a3,-(sp)
  subq.l  #1,d0
.NextLine:
  move.l  (a1)+,d5                      ; left coord
  cmp.l   d4,d5
  bgt     .SkipLine                     ; x1 > rclip
  move.l  (a2)+,d6                      ; right coord
  cmp.l   d3,d6
  blt     .SkipLine                     ; x2 < lclip
  cmp.l   d3,d5
  bge.s   .NoLeftClip                   ; inside clip area
  move.l  d3,d5
.NoLeftClip:
  cmp.l   d4,d6
  ble.s   .NoRightClip                  ; inside clip area
  move.l  d4,d6
.NoRightClip:
  move.l  a0,a3                         ; line address
  sub.l   d5,d6                         ; pixels to draw
  add.l   d5,a3                         ; start address
  addq.l  #1,d6                         ; even if start = end we should draw a pixel
  move.l  d6,d7                         ; save the value
  andi.l  #$7,d7                        ; look for a multiple of 8
  beq.s   .DrawFullLine                 ; draw only block of eight pixels
  subq.l  #1,d7                         ; extra pixels to draw
.DrawExtraPixel:
  move.b  d2,(a3)+
  dbf     d7,.DrawExtraPixel
.DrawFullLine:
  andi.l  #$fffffff8,d6                 ; clear low bits
  beq.s   .SkipLine                     ; nothing more to draw
  lsr.l   #3,d6                         ; draw 4 pixels each time
  subq.l  #1,d6                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d6,.DrawLine
.SkipLine:
  adda.l  d1,a0
  dbf     d0,.NextLine
.EndDraw:
  movem.l (sp)+,d0-d7/a0-a3
  move.l  #-1,d0
  rts

;--------------------------------------
; Draw a flat quad in 16bits with clipping
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of lines to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color in RGB16 format (extended to 32bits)
; @in d3.l left clip
; @in d4.l right clip
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawClippedFlatQuad16Bits

_SAGE_DrawClippedFlatQuad16Bits:
  movem.l d0-d7/a0-a3,-(sp)
  subq.l  #1,d0
.NextLine:
  move.l  (a1)+,d5                      ; left coord
  cmp.l   d4,d5
  bgt     .SkipLine                     ; x1 > rclip
  move.l  (a2)+,d6                      ; right coord
  cmp.l   d3,d6
  blt     .SkipLine                     ; x2 < lclip
  cmp.l   d3,d5
  bge.s   .NoLeftClip                   ; inside clip area
  move.l  d3,d5
.NoLeftClip:
  cmp.l   d4,d6
  ble.s   .NoRightClip                  ; inside clip area
  move.l  d4,d6
.NoRightClip:
  move.l  a0,a3                         ; line address
  sub.l   d5,d6                         ; pixels to draw
  add.l   d5,d5                         ; 2 bytes per pixel
  add.l   d5,a3                         ; start address
  addq.l  #1,d6                         ; even if start = end we should draw a pixel
  move.l  d6,d7                         ; save the value
  andi.l  #$3,d7                        ; look for a multiple of 4
  beq.s   .DrawFullLine                 ; draw only block of eight pixels
  subq.l  #1,d7                         ; extra pixels to draw
.DrawExtraPixel:
  move.w  d2,(a3)+
  dbf     d7,.DrawExtraPixel
.DrawFullLine:
  andi.l  #$fffffffc,d6                 ; clear low bits
  beq.s   .SkipLine                     ; nothing more to draw
  lsr.l   #2,d6                         ; draw 4 pixels each time
  subq.l  #1,d6                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d6,.DrawLine
.SkipLine:
  adda.l  d1,a0
  dbf     d0,.NextLine
.EndDraw:
  movem.l (sp)+,d0-d7/a0-a3
  move.l  #-1,d0
  rts

;--------------------------------------
; Draw a flat quad in 32bits with clipping
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of lines to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color in RGB16 format (extended to 32bits)
; @in d3.l left clip
; @in d4.l right clip
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawClippedFlatQuad32Bits

_SAGE_DrawClippedFlatQuad32Bits:
  movem.l d0-d7/a0-a3,-(sp)
  subq.l  #1,d0
.NextLine:
  move.l  (a1)+,d5                      ; left coord
  cmp.l   d4,d5
  bgt     .SkipLine                     ; x1 > rclip
  move.l  (a2)+,d6                      ; right coord
  cmp.l   d3,d6
  blt     .SkipLine                     ; x2 < lclip
  cmp.l   d3,d5
  bge.s   .NoLeftClip                   ; inside clip area
  move.l  d3,d5
.NoLeftClip:
  cmp.l   d4,d6
  ble.s   .NoRightClip                  ; inside clip area
  move.l  d4,d6
.NoRightClip:
  move.l  a0,a3                         ; line address
  sub.l   d5,d6                         ; pixels to draw
  lsl.l   #2,d5                         ; 4 bytes per pixel
  add.l   d5,a3                         ; start address
  addq.l  #1,d6                         ; even if start = end we should draw a pixel
  move.l  d6,d7                         ; save the value
  btst    #0,d7                         ; look for a multiple of 2
  beq.s   .DrawFullLine                 ; draw only block of eight pixels
.DrawExtraPixel:
  move.l  d2,(a3)+
.DrawFullLine:
  andi.l  #$fffffffe,d6                 ; clear low bits
  beq.s   .SkipLine                     ; nothing more to draw
  lsr.l   #1,d6                         ; draw 4 pixels each time
  subq.l  #1,d6                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d6,.DrawLine
.SkipLine:
  adda.l  d1,a0
  dbf     d0,.NextLine
.EndDraw:
  movem.l (sp)+,d0-d7/a0-a3
  move.l  #-1,d0
  rts

  END
