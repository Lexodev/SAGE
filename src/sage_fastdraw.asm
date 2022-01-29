;--------------------------------------
; sage_fastdraw.asm
;
; SAGE (Simple Amiga Game Engine) project
; Fast draw primitives
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 1.0 February 2021
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
; @in d3.l color
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
; @in d3.l color
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
; @in d3.l color
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
; Calculate the X coords for a left
; edge line (one point per row)
;
; @in a0.l buffer for coords
; @in d0.l x1
; @in d1.l y1
; @in d2.l x2
; @in d3.l y2
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastLeftEdgeCalculation

_SAGE_FastLeftEdgeCalculation:
  movem.l d1-d5/a0,-(sp)

  cmp.l   d1,d3
  bge.s   .NoSwap
  exg.l   d0,d2
  exg.l   d1,d3                         ; always draw from top to bottom
.NoSwap:

  sub.l   d1,d3                         ; dy
  move.l  d2,d1                         ; save last x
  sub.l   d0,d2                         ; dx
  blt     .DxNegative                   ; dx < 0 ?
  bne.s   .DxPositive                   ; dx > 0 ?

.DxZero:                                ; vertical line
  move.l  d0,(a0)+                      ; save x coord
  dbf     d3,.DxZero
  bra     .EndDraw

.DxPositive:
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive

.DyZeroPositive:                        ; horizontal line
  move.l  d0,(a0)                       ; save X coord
  bra     .EndDraw

.DyNotZeroPositive:
  cmp.l   d2,d3                         ; dx >= dy ?
  bgt.s   .DySupDxPos

.DxSupDyPos:                            ; soft slope +
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
  move.l  d0,(a0)+                      ; save X coord
.DrawSoftPositive:
  addq.l  #1,d0                         ; next pixel
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftPositive
  move.l  d0,(a0)+                      ; save X coord
  add.l   d2,d5                         ; e + dx
.NextSoftPositive:
  dbf     d4,.DrawSoftPositive
;  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw

.DySupDxPos:                            ; hard slope +
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardPositive:
  move.l  d0,(a0)+                      ; save X coord
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardPositive
  addq.l  #1,d0                         ; next pixel
  add.l   d3,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw

.DxNegative:
  neg.l   d2                            ; dx * -1
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative

.DyZeroNegative:                        ; horizontal line
  move.l  d0,(a0)                       ; save X coord
  bra     .EndDraw

.DyNotZeroNegative:                     ; soft slope -
  cmp.l   d2,d3                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawSoftNegative:
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftNegative
  move.l  d0,(a0)+                      ; save X coord
  add.l   d2,d5                         ; e + dx
.NextSoftNegative:
  subq.l  #1,d0                         ; previous pixel
  dbf     d4,.DrawSoftNegative
  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw

.DySupDxNeg:                            ; hard slope -
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardNegative:
  move.l  d0,(a0)+                      ; save X coord
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardNegative
  subq.l  #1,d0                         ; previous pixel
  add.l   d3,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative

.EndDraw:
  movem.l (sp)+,d1-d5/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Calculate the X coords for a right
; edge line (one point per row)
;
; @in a0.l buffer for coords
; @in d0.l x1
; @in d1.l y1
; @in d2.l x2
; @in d3.l y2
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastRightEdgeCalculation

_SAGE_FastRightEdgeCalculation:
  movem.l d1-d5/a0,-(sp)
  cmp.l   d1,d3
  bge.s   .NoSwap
  exg.l   d0,d2
  exg.l   d1,d3                         ; always draw from top to bottom
.NoSwap:
  sub.l   d1,d3                         ; dy
  move.l  d2,d1                         ; save last x
  sub.l   d0,d2                         ; dx
  blt     .DxNegative                   ; dx < 0 ?
  bne.s   .DxPositive                   ; dx > 0 ?
.DxZero:                                ; vertical line
  move.l  d0,(a0)+                      ; save x coord
  dbf     d3,.DxZero
  bra     .EndDraw
.DxPositive:
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive
.DyZeroPositive:                        ; horizontal line
  move.l  d0,(a0)                       ; save X coord
  bra     .EndDraw
.DyNotZeroPositive:
  cmp.l   d2,d3                         ; dx >= dy ?
  bgt.s   .DySupDxPos
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawSoftPositive:
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftPositive
  move.l  d0,(a0)+                      ; save X coord
  add.l   d2,d5                         ; e + dx
.NextSoftPositive:
  addq.l  #1,d0                         ; next pixel
  dbf     d4,.DrawSoftPositive
  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw
.DySupDxPos:
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardPositive:
  move.l  d0,(a0)+                      ; save X coord
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardPositive
  addq.l  #1,d0                         ; next pixel
  add.l   d3,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw
.DxNegative:
  neg.l   d2                            ; dx * -1
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative
.DyZeroNegative:                        ; horizontal line
  move.l  d0,(a0)                       ; save X coord
  bra     .EndDraw
.DyNotZeroNegative:
  cmp.l   d2,d3                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
  move.l  d0,(a0)+                      ; save X coord
.DrawSoftNegative:
  subq.l  #1,d0                         ; previous pixel
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftNegative
  move.l  d0,(a0)+                      ; save X coord
  add.l   d2,d5                         ; e + dx
.NextSoftNegative:
  dbf     d4,.DrawSoftNegative
  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw
.DySupDxNeg:
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardNegative:
  move.l  d0,(a0)+                      ; save X coord
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardNegative
  subq.l  #1,d0                         ; previous pixel
  add.l   d3,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative
.EndDraw:
  movem.l (sp)+,d1-d5/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Calculate the X coords for a clipped
; left edge line (one point per row)
;
; @in a0.l buffer for coords
; @in d0.l x1
; @in d1.l y1
; @in d2.l x2
; @in d3.l y2
; @in a1.l clipping structure
;
; @out d0.l number of visible points
;--------------------------------------
  xdef _SAGE_FastClippedLeftEdgeCalc

_SAGE_FastClippedLeftEdgeCalc:
  movem.l d1-d7/a0-a1,-(sp)
  
  cmp.l   d1,d3
  bge.s   .NoSwap
  exg.l   d0,d2
  exg.l   d1,d3                         ; Always draw from top to bottom
.NoSwap:

  cmp.l   SSCR_TOPCLIP(a1),d3           ; top clip
  blt.s   .Outside                      ; totally outside
  cmp.l   SSCR_BOTTOMCLIP(a1),d1        ; bottom clip
  ble.s   .TopClip
.Outside:
  moveq.l #0,d6
  bra     .EndDraw

.TopClip:
  cmp.l   SSCR_TOPCLIP(a1),d1           ; clip y1 ?
  bge.s   .BottomClip
; do the calculation of new x1' with y1'=TOPCLIP
; x1' = x1 + ((y1' - y1) * (x2 - x1) / (y2 - y1))
  move.l  SSCR_TOPCLIP(a1),d4           ; y1'
  sub.l   d1,d4                         ; y1' - y1
  move.l  d2,d5                         ; x2
  sub.l   d0,d5                         ; x2 - x1
  muls.w  d5,d4                         ; (x2 - x1) * (y1' - y1)
  move.l  d3,d5                         ; y2
  sub.l   d1,d5                         ; y2 - y1
  divs.w  d5,d4                         ; (y1' - y1) * (x2 - x1) / (y2 - y1)
  ext.l   d4
  add.l   d4,d0                         ; x1'
  move.l  SSCR_TOPCLIP(a1),d1           ; y1'
.BottomClip:
  cmp.l   SSCR_BOTTOMCLIP(a1),d3        ; clip y2 ?
  ble.s   .Calculate
; do the calculation of new x2' with y2'=BOTTOMCLIP
; x'2 = x2 + ((y2 - y2') * (x1 - x2)) / (y2 - y1)
  move.l  d3,d5; y2
  sub.l   SSCR_BOTTOMCLIP(a1),d5        ; y2 - y2'
  move.l  d0,d4                         ; x1
  sub.l   d2,d4                         ; x1 - x2
  muls.w  d5,d4                         ; (y2 - y2()) * (x1 - x2)
  move.l  d3,d5                         ; y2
  sub.l   d1,d5                         ; y2 - y1
  divs.w  d5,d4                         ; ((y2 - y2') * (x1 - x2)) / (y2 - y1)
  ext.l   d4
  add.l   d4,d2                         ; x2'
  move.l  SSCR_BOTTOMCLIP(a1),d3        ; y2'

.Calculate:
  move.l  #-1,d7                        ; Clipping indicator
  sub.l   d1,d3                         ; dy
  move.l  d3,d6                         ; save it
  move.l  d2,d1                         ; save last x
  sub.l   d0,d2                         ; dx
  blt     .DxNegative
  bne.s   .DxPositive

.DxZero:                                ; vertical line
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  blt.s   .VerticalLeftClip
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .DrawVertical
  move.l  d7,d0                         ; clamp right value
  bra.s   .DrawVertical
.VerticalLeftClip:
  move.l  SSCR_LEFTCLIP(a1),d0          ; clip left value
.DrawVertical:
  move.l  d0,(a0)+                      ; save x coord
  dbf     d3,.DrawVertical
  bra     .EndDraw

.DxPositive:                            ; soft slope +
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive

.DyZeroPositive:                        ; horizontal line
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  blt.s   .PositiveHorizontalLeftClip
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .DrawPositiveHorizontal
  move.l  d7,d0                         ; clamp right value
  bra.s   .DrawPositiveHorizontal
.PositiveHorizontalLeftClip:
  move.l  SSCR_LEFTCLIP(a1),d0          ; clip left value
.DrawPositiveHorizontal:
  move.l  d0,(a0)                       ; save X coord
  moveq.l #1,d6                         ; number of points
  bra     .EndDraw

.DyNotZeroPositive:
  cmp.l   d2,d3                         ; dx >= dy ?
  bgt.s   .DySupDxPos

.DxSupDyPos:                            ; soft slope +
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; E
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .P1RightClipSoftPositive
  move.l  SSCR_LEFTCLIP(a1),(a0)+       ; clip left value
  bra.s   .DrawSoftPositive
.P1RightClipSoftPositive:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .P1SaveSoftPositive
  move.l  d7,(a0)+                      ; clamp right value
  bra.s   .DrawSoftPositive
.P1SaveSoftPositive:
  move.l  d0,(a0)+                      ; save X coord
.DrawSoftPositive:
  addq.l  #1,d0                         ; next pixel
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftPositive 
.LeftClipSoftPositive:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipSoftPositive
  move.l  SSCR_LEFTCLIP(a1),(a0)+       ; clip left value
  bra.s   .ErrorSoftPositive
.RightClipSoftPositive:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveSoftPositive
  move.l  d7,(a0)+                      ; clamp right value
  bra.s   .ErrorSoftPositive
.SaveSoftPositive:
  move.l  d0,(a0)+                      ; save X coord
.ErrorSoftPositive:
  add.l   d2,d5                         ; e + dx
.NextSoftPositive:
  dbf     d4,.DrawSoftPositive
  bra     .EndDraw

.DySupDxPos:                            ; hard slope +
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; E
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardPositive:
.LeftClipHardPositive:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipHardPositive
  move.l  SSCR_LEFTCLIP(a1),(a0)+       ; clip left value
  bra.s   .ErrorHardPositive
.RightClipHardPositive:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveHardPositive
  move.l  d7,(a0)+                      ; clamp right value
  bra.s   .ErrorHardPositive
.SaveHardPositive:
  move.l  d0,(a0)+                      ; save X coord
.ErrorHardPositive:
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardPositive
  addq.l  #1,d0                         ; next pixel
  add.l   d3,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw

.DxNegative:
  neg.l   d2                            ; dx * -1
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative

.DyZeroNegative:                        ; horizontal line
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  blt.s   .NegativeHorizontalLeftClip
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .DrawNegativeHorizontal
  move.l  d7,d0                         ; clamp right value
  bra.s   .DrawNegativeHorizontal
.NegativeHorizontalLeftClip:
  move.l  SSCR_LEFTCLIP(a1),d0          ; clip left value
.DrawNegativeHorizontal:
  move.l  d0,(a0)                       ; save X coord
  moveq.l #1,d6                         ; number of points
  bra     .EndDraw

.DyNotZeroNegative:                     ; soft slope -
  cmp.l   d2,d3                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; E
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawSoftNegative:
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftNegative
.LeftClipSoftNegative:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipSoftNegative
  move.l  SSCR_LEFTCLIP(a1),(a0)+       ; clip left value
  bra.s   .ErrorSoftNegative
.RightClipSoftNegative:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveSoftNegative
  move.l  d7,(a0)+                      ; clamp right value
  bra.s   .ErrorSoftNegative
.SaveSoftNegative:
  move.l  d0,(a0)+                      ; save X coord
.ErrorSoftNegative:
  add.l   d2,d5                         ; e + dx
.NextSoftNegative:
  subq.l  #1,d0                         ; previous pixel
  dbf     d4,.DrawSoftNegative
  cmp.l   SSCR_LEFTCLIP(a1),d1          ; left clip
  bge.s   .P1RightClipSoftNegative
  move.l  SSCR_LEFTCLIP(a1),(a0)+       ; clip left value
  bra.s   .EndDraw
.P1RightClipSoftNegative:
  cmp.l   SSCR_RIGHTCLIP(a1),d1         ; right clip
  ble.s   .P1SaveSoftNegative
  move.l  d7,(a0)+                      ; clamp right value
  bra.s   .EndDraw
.P1SaveSoftNegative:
  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw

.DySupDxNeg:                            ; hard slope -
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; E
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardNegative:
.LeftClipHardNegative:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipHardNegative
  move.l  SSCR_LEFTCLIP(a1),(a0)+       ; clip left value
  bra.s   .ErrorHardNegative
.RightClipHardNegative:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveHardNegative
  move.l  d7,(a0)+                      ; clamp right value
  bra.s   .ErrorHardNegative
.SaveHardNegative:
  move.l  d0,(a0)+                      ; save X coord
.ErrorHardNegative:
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardNegative
  subq.l  #1,d0                         ; previous pixel
  add.l   d3,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative

.EndDraw:
  move.l  d6,d0                         ; number of points
  movem.l (sp)+,d1-d7/a0-a1
  rts

;--------------------------------------
; Calculate the X coords for a
; right clipped line
;
; @in a0.l buffer for coords
; @in d0.l x1
; @in d1.l y1
; @in d2.l x2
; @in d3.l y2
; @in a1.l clipping structure
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastClippedRightEdgeCalc

_SAGE_FastClippedRightEdgeCalc:
  movem.l d1-d7/a0-a1,-(sp)

  cmp.l   d1,d3
  bge.s   .NoSwap
  exg.l   d0,d2
  exg.l   d1,d3                         ; always draw from top to bottom
.NoSwap:

  cmp.l   SSCR_TOPCLIP(a1),d3           ; top clip
  blt.s   .Outside                      ; totally outside
  cmp.l   SSCR_BOTTOMCLIP(a1),d1        ; bottom clip
  ble.s   .TopClip
.Outside:
  moveq.l #0,d6
  bra     .EndDraw

.TopClip:
  cmp.l   SSCR_TOPCLIP(a1),d1           ; clip y1 ?
  bge.s   .BottomClip
; do the calculation of new x1' with y1'=TOPCLIP
; x1' = x1 + ((y1' - y1) * (x2 - x1) / (y2 - y1))
  move.l  SSCR_TOPCLIP(a1),d4           ; y1'
  sub.l   d1,d4                         ; y1' - y1
  move.l  d2,d5                         ; x2
  sub.l   d0,d5                         ; x2 - x1
  muls.w  d5,d4                         ; (x2 - x1) * (y1' - y1)
  move.l  d3,d5                         ; y2
  sub.l   d1,d5                         ; y2 - y1
  divs.w  d5,d4                         ; (y1' - y1) * (x2 - x1) / (y2 - y1)
  ext.l   d4
  add.l   d4,d0                         ; x1'
  move.l  SSCR_TOPCLIP(a1),d1           ; y1'
.BottomClip:
  cmp.l   SSCR_BOTTOMCLIP(a1),d3        ; clip y2 ?
  ble.s   .Calculate
; do the calculation of new x2' with y2'=BOTTOMCLIP
; x'2 = x2 + ((y2 - y2') * (x1 - x2)) / (y2 - y1)
  move.l  d3,d5; y2
  sub.l   SSCR_BOTTOMCLIP(a1),d5        ; y2 - y2'
  move.l  d0,d4                         ; x1
  sub.l   d2,d4                         ; x1 - x2
  muls.w  d5,d4                         ; (y2 - y2()) * (x1 - x2)
  move.l  d3,d5                         ; y2
  sub.l   d1,d5                         ; y2 - y1
  divs.w  d5,d4                         ; ((y2 - y2') * (x1 - x2)) / (y2 - y1)
  ext.l   d4
  add.l   d4,d2                         ; x2'
  move.l  SSCR_BOTTOMCLIP(a1),d3        ; y2'

.Calculate:
  move.l  #-1,d7                        ; Clipping indicator
  sub.l   d1,d3                         ; dy
  move.l  d3,d6                         ; save it
  move.l  d2,d1                         ; save last x
  sub.l   d0,d2                         ; dx
  blt     .DxNegative                   ; dx < 0 ?
  bne.s   .DxPositive                   ; dx > 0 ?

.DxZero:                                ; vertical line
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  blt.s   .VerticalLeftClip
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .DrawVertical
  move.l  SSCR_RIGHTCLIP(a1),d0         ; clip right value
  bra.s   .DrawVertical
.VerticalLeftClip:
  move.l  d7,d0                         ; clamp left value
.DrawVertical:
  move.l  d0,(a0)+                      ; save x coord
  dbf     d3,.DrawVertical
  bra     .EndDraw

.DxPositive:                            ; soft slope +
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroPositive

.DyZeroPositive:                        ; horizontal line
  cmp.l   SSCR_LEFTCLIP(a1),d1          ; left clip
  blt.s   .PositiveHorizontalLeftClip
  cmp.l   SSCR_RIGHTCLIP(a1),d1         ; right clip
  ble.s   .DrawPositiveHorizontal
  move.l  SSCR_RIGHTCLIP(a1),d1         ; clip right value
  bra.s   .DrawPositiveHorizontal
.PositiveHorizontalLeftClip:
  move.l  d7,d1                         ; clamp left value
.DrawPositiveHorizontal:
  move.l  d1,(a0)                       ; save X coord
  moveq.l #1,d6                         ; number of points
  bra     .EndDraw

.DyNotZeroPositive:
  cmp.l   d2,d3                         ; dx >= dy ?
  bgt.s   .DySupDxPos

  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawSoftPositive:
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftPositive
.LeftClipSoftPositive:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipSoftPositive
  move.l  d7,(a0)+                      ; clamp left value
  bra.s   .ErrorSoftPositive
.RightClipSoftPositive:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveSoftPositive
  move.l  SSCR_RIGHTCLIP(a1),(a0)+      ; clip right value
  bra.s   .ErrorSoftPositive
.SaveSoftPositive:
  move.l  d0,(a0)+                      ; save X coord
.ErrorSoftPositive:
  add.l   d2,d5                         ; e + dx
.NextSoftPositive:
  addq.l  #1,d0                         ; next pixel
  dbf     d4,.DrawSoftPositive
  cmp.l   SSCR_LEFTCLIP(a1),d1          ; left clip
  bge.s   .P1RightClipSoftPositive
  move.l  d7,(a0)+                      ; clamp left value
  bra     .EndDraw
.P1RightClipSoftPositive:
  cmp.l   SSCR_RIGHTCLIP(a1),d1         ; right clip
  ble.s   .P1SaveSoftPositive
  move.l  SSCR_RIGHTCLIP(a1),(a0)+      ; clip right value
  bra     .EndDraw
.P1SaveSoftPositive:
  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw

.DySupDxPos:                            ; hard slope +
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardPositive:
.LeftClipHardPositive:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipHardPositive
  move.l  d7,(a0)+                      ; clamp left value
  bra.s   .ErrorHardPositive
.RightClipHardPositive:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveHardPositive
  move.l  SSCR_RIGHTCLIP(a1),(a0)+      ; clip right value
  bra.s   .ErrorHardPositive
.SaveHardPositive:
  move.l  d0,(a0)+                      ; save X coord
.ErrorHardPositive:
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardPositive
  addq.l  #1,d0                         ; next pixel
  add.l   d3,d5                         ; e + dy
.NextHardPositive:
  dbf     d4,.DrawHardPositive
  bra     .EndDraw

.DxNegative:
  neg.l   d2                            ; dx * -1
  tst.l   d3                            ; dy == 0 ?
  bne.s   .DyNotZeroNegative

.DyZeroNegative:                        ; horizontal line
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  blt.s   .NegativeHorizontalLeftClip
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .DrawNegativeHorizontal
  move.l  SSCR_RIGHTCLIP(a1),d0         ; clip right value
  bra.s   .DrawNegativeHorizontal
.NegativeHorizontalLeftClip:
  move.l  d7,d0                         ; clamp left value
.DrawNegativeHorizontal:
  move.l  d0,(a0)                       ; save X coord
  moveq.l #1,d6                         ; number of points
  bra     .EndDraw

.DyNotZeroNegative:                     ; soft slope -
  cmp.l   d2,d3                         ; dx >= dy
  bgt.s   .DySupDxNeg
  move.l  d2,d4                         ; line len
  move.l  d2,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .P1RightClipSoftNegative
  move.l  d7,(a0)+                      ; clamp left value
  bra.s   .DrawSoftNegative
.P1RightClipSoftNegative:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .P1SaveSoftNegative
  move.l  SSCR_RIGHTCLIP(a1),(a0)+      ; clip right value
  bra.s   .DrawSoftNegative
.P1SaveSoftNegative:
  move.l  d0,(a0)+                      ; save X coord
.DrawSoftNegative:
  subq.l  #1,d0                         ; previous pixel
  sub.l   d3,d5                         ; e - dy
  bge.s   .NextSoftNegative
.LeftClipSoftNegative:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipSoftNegative
  move.l  d7,(a0)+                      ; clamp left value
  bra.s   .ErrorSoftNegative
.RightClipSoftNegative:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveSoftNegative
  move.l  SSCR_RIGHTCLIP(a1),(a0)+      ; clip right value
  bra.s   .ErrorSoftNegative
.SaveSoftNegative:
  move.l  d0,(a0)+                      ; save X coord
.ErrorSoftNegative:
  add.l   d2,d5                         ; e + dx
.NextSoftNegative:
  dbf     d4,.DrawSoftNegative
  move.l  d1,(a0)                       ; last pixel
  bra     .EndDraw

.DySupDxNeg:                            ; hard slope -
  move.l  d3,d4                         ; line len
  move.l  d3,d5                         ; e
  add.l   d2,d2                         ; dx * 2
  add.l   d3,d3                         ; dy * 2
.DrawHardNegative:
.LeftClipHardNegative:
  cmp.l   SSCR_LEFTCLIP(a1),d0          ; left clip
  bge.s   .RightClipHardNegative
  move.l  d7,(a0)+                      ; clamp left value
  bra.s   .ErrorHardNegative
.RightClipHardNegative:
  cmp.l   SSCR_RIGHTCLIP(a1),d0         ; right clip
  ble.s   .SaveHardNegative
  move.l  SSCR_RIGHTCLIP(a1),(a0)+      ; clip right value
  bra.s   .ErrorHardNegative
.SaveHardNegative:
  move.l  d0,(a0)+                      ; save X coord
.ErrorHardNegative:
  sub.l   d2,d5                         ; e - dx
  bge.s   .NextHardNegative
  subq.l  #1,d0                         ; previous pixel
  add.l   d3,d5                         ; e + dy
.NextHardNegative:
  dbf     d4,.DrawHardNegative

.EndDraw:
  move.l  d6,d0                         ; number of points
  movem.l (sp)+,d1-d7/a0-a1
  rts

;--------------------------------------
; Draw a flat quad for 8bits screen
; upper and lower segments are horizontal
;
; @in a0.l first line address
; @in a1.l array of left coords
; @in a2.l array of right coords
; @in d0.l number of rows to draw
; @in d1.l offset to the next line in bytes
; @in d2.l color (extended to 32bits)
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawFlatQuad8Bits

_SAGE_DrawFlatQuad8Bits:
  movem.l d1-d5/a0-a3,-(sp)
.NextRow:
  move.l  a0,a3                         ; line address
  move.l  (a1)+,d3                      ; left coord
  bge.s   .RightCoord                   ; inside of clip area
  adda.l  #4,a2                         ; next right coord
  bra.s   .SkipRow                      ; next row
.RightCoord:
  move.l  (a2)+,d4                      ; right coord
  blt.s   .SkipRow                      ; outside of clip area
  cmp.l   d3,d4                         ; sometimes d4 is lower than d3
  bge.s   .NoSwap
  exg     d3,d4                         ; set in good order
.NoSwap:
  sub.l   d3,d4                         ; pixels to draw
  add.l   d3,a3                         ; start address
  move.l  d4,d5                         ; save the value
  andi.l  #$7,d5                        ; look for a multiple of 8
  beq.s   .DrawFastLine                 ; draw only block of eight pixels
  subq.l  #1,d5                         ; extra pixels to draw
.DrawExtraPixel:
  move.b  d2,(a3)+
  dbf     d5,.DrawExtraPixel
.DrawFastLine:
  andi.l  #$fffffff8,d4                 ; clear low bits
  beq.s   .SkipRow                      ; nothing more to draw
  lsr.l   #3,d4                         ; draw 8 pixels each time
  subq.l  #1,d4                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d4,.DrawLine
.SkipRow:
  adda.l  d1,a0
  dbf     d0,.NextRow
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
; @in d2.l color (extended to 32bits)
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawFlatQuad16Bits

_SAGE_DrawFlatQuad16Bits:
  movem.l d1-d5/a0-a3,-(sp)
.NextRow:
  move.l  a0,a3                         ; line address
  move.l  (a1)+,d3                      ; left coord
  bge.s   .RightCoord                   ; inside of clip area
  adda.l  #4,a2                         ; next right coord
  bra.s   .SkipRow                      ; next row
.RightCoord:
  move.l  (a2)+,d4                      ; right coord
  blt.s   .SkipRow                      ; outside of clip area
  sub.l   d3,d4                         ; pixels to draw
  add.l   d3,d3                         ; 2 bytes per pixel
  add.l   d3,a3                         ; start address
  move.l  d4,d5                         ; save the value
  andi.l  #$3,d5                        ; look for a multiple of 4
  beq.s   .DrawFastLine                 ; draw only block of eight pixels
  subq.l  #1,d5                         ; extra pixels to draw
.DrawExtraPixel:
  move.w  d2,(a3)+
  dbf     d5,.DrawExtraPixel
.DrawFastLine:
  andi.l  #$fffffffc,d4                 ; clear low bits
  beq.s   .SkipRow                      ; nothing more to draw
  lsr.l   #2,d4                         ; draw 4 pixels each time
  subq.l  #1,d4                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d4,.DrawLine
.SkipRow:
  adda.l  d1,a0
  dbf     d0,.NextRow
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
; @in d2.l color (extended to 32bits)
;
; @out d0.l Operation success
;--------------------------------------

  xdef _SAGE_DrawFlatQuad32Bits

_SAGE_DrawFlatQuad32Bits:
  movem.l d1-d5/a0-a3,-(sp)
.NextRow:
  move.l  a0,a3                         ; line address
  move.l  (a1)+,d3                      ; left coord
  bge.s   .RightCoord                   ; inside of clip area
  adda.l  #4,a2                         ; next right coord
  bra.s   .SkipRow                      ; next row
.RightCoord:
  move.l  (a2)+,d4                      ; right coord
  blt.s   .SkipRow                      ; outside of clip area
  sub.l   d3,d4                         ; pixels to draw
  lsl.l   #2,d3                         ; 4 bytes per pixel
  add.l   d3,a3                         ; start address
  btst    #0,d4                         ; look for a multiple of 2
  beq.s   .DrawFastLine                 ; draw only block of eight pixels
  move.l  d2,(a3)+
.DrawFastLine:
  andi.l  #$fffffffe,d4                 ; clear low bits
  beq.s   .SkipRow                      ; nothing more to draw
  lsr.l   #1,d4                         ; draw 2 pixels each time
  subq.l  #1,d4                         ; pixels to draw
.DrawLine:
  move.l  d2,(a3)+
  move.l  d2,(a3)+
  dbf     d4,.DrawLine
.SkipRow:
  adda.l  d1,a0
  dbf     d0,.NextRow
.EndDraw:
  movem.l (sp)+,d1-d5/a0-a3
  move.l  #-1,d0
  rts

  END
