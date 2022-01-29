;--------------------------------------
; sage_blitter.asm
;
; SAGE (Simple Amiga Game Engine) project
; Bitmap copy functions
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 1.0 April 2020
;--------------------------------------

  SECTION blitter,code

;--------------------------------------
; Copy the frame buffer to the screen
;
; @in a0.l frame buffer address
; @in a1.l screen buffer address
; @in d0.w number of lines to copy
; @in d1.w number of bytes per line
; @in d2.l offset to the next line in bytes
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_FastCopyScreen

_SAGE_FastCopyScreen:
  movem.l d1/d2/d6/a0-a1,-(sp)
  lsr.w   #3,d1
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextBlock
  adda.l  d2,a1
  dbf     d0,.NextLine
  movem.l (sp)+,d1/d2/d6/a0-a1
  move.l  #-1,d0
  rts

;--------------------------------------
; Clear the frame buffer
;
; @in a0.l frame buffer address
; @in d0.w number of lines to clear
; @in d1.w number of bytes per line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_FastClearScreen

_SAGE_FastClearScreen:
  movem.l d1/d2/d6/a0,-(sp)
  moveq.l #0,d2
  lsr.w   #3,d1
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  d2,(a0)+
  move.l  d2,(a0)+
  dbf     d6,.NextBlock
  dbf     d0,.NextLine
  movem.l (sp)+,d1/d2/d6/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Fill a 8bits bitmap with a color
;
; @in a0.l frame buffer address
; @in d0.w number of lines to clear
; @in d1.w number of pixels per line
; @in d2.l offset to bitmap next line
; @in d3.l fill value
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitFill8Bits

_SAGE_BlitFill8Bits:
  movem.l d1/d2/d3/d4/d6/a0,-(sp)
  move.b  d3,d4
  lsl.l   #8,d3
  move.b  d4,d3
  move.w  d3,d4
  swap    d3
  move.w  d4,d3                         ; extend the color value to 4 bytes
  lsr.w   #3,d1                         ; 8 bytes by loop (8 pixels)
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  d3,(a0)+
  move.l  d3,(a0)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  dbf     d0,.NextLine
  movem.l (sp)+,d1/d2/d3/d4/d6/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 8bits bitmap to another 8bits bitmap
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.w number of lines to copy
; @in d1.w number of pixels per line
; @in d2.l offset to next source line
; @in d3.l offset to next destination line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitCopy8Bits

_SAGE_BlitCopy8Bits:
  movem.l d1-d6/a0-a1,-(sp)
  tst.w   d0
  beq.s   .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq.s   .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000111,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 8
  subq.w  #1,d5                         ; Extra pixels to copy
  andi.w  #%1111111111111000,d1         ; Skip extra pixels
  lsr.w   #3,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.w  d5,d6
.NextExtraPixel:
  move.b  (a0)+,(a1)+
  dbf     d6,.NextExtraPixel
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 8
  subq.w  #1,d6
.NextExtraBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #3,d1                         ; 8 bytes by loop
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d6/a0-a1
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 8bits bitmap to another 8bits bitmap
; with zoom
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.l source width in pixels
; @in d1.l source height in pixels
; @in d2.l offset to next source line
; @in d3.l destination width in pixels
; @in d4.l destination height in pixels
; @in d5.l offset to next destination line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitZoomCopy8Bits

_SAGE_BlitZoomCopy8Bits:
  movem.l d1-d7/a0-a2,-(sp)
  tst.l   d4
  beq.s   .EndOfCopy                    ; Nothing to copy
  tst.l   d3
  beq.s   .EndOfCopy                    ; Nothing to copy
  fmove.l d0,fp0
  fmove.l d3,fp2
  fdiv    fp2,fp0                       ; x_step
  fmove.l d1,fp1
  fmove.l d4,fp2
  fdiv    fp2,fp1                       ; y_step
  fmove.s #0.0,fp3                      ; y_pixel
  subq.l  #1,d3
  subq.l  #1,d4
.NextLine:
  fmove.s #0.0,fp2                      ; x_pixel
  fmove.l fp3,d1                        ; y_pixel
  mulu.w  d2,d1                         ; src_offset * y_pixel
  movea.l a0,a2
  add.l   d1,a2                         ; src_buffer + (src_offset * y_pixel)
  move.w  d3,d7
.NextPixel:
  fmove.l fp2,d0                        ; x_pixel
  move.b  0(a2,d0),(a1)+                ; pixel
  fadd    fp0,fp2                       ; x_pixel + x_step
  dbf     d7,.NextPixel
  adda.l  d5,a1                         ; dst_buffer + dst_offset
  fadd    fp1,fp3                       ; y_pixel + y_step
  dbf     d4,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a2
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 8bits bitmap to another 8bits bitmap
; with transparency
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.w number of lines to copy
; @in d1.w number of pixels per line
; @in d2.l offset to next source line
; @in d3.l offset to next destination line
; @in d4.l transparent color index
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitTransparentCopy8Bits

_SAGE_BlitTransparentCopy8Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000111,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 8
  subq.w  #1,d5                         ; Extra pixels to copy
  andi.w  #%1111111111111000,d1         ; Skip extra pixels
  lsr.w   #3,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.w  d5,d6
.NextExtraPixel:
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtraPixel
  move.b  d7,(a1)
.SkipExtraPixel:
  addq.l  #1,a1
  dbf     d6,.NextExtraPixel
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 8
  subq.w  #1,d6
.NextExtraBlock:
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra1
  move.b  d7,(a1)
.SkipExtra1:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra2
  move.b  d7,(a1)
.SkipExtra2:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra3
  move.b  d7,(a1)
.SkipExtra3:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra4
  move.b  d7,(a1)
.SkipExtra4:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra5
  move.b  d7,(a1)
.SkipExtra5:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra6
  move.b  d7,(a1)
.SkipExtra6:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra7
  move.b  d7,(a1)
.SkipExtra7:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .SkipExtra8
  move.b  d7,(a1)
.SkipExtra8:
  addq.l  #1,a1
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #3,d1                         ; 8 bytes by loop
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip1
  move.b  d7,(a1)
.Skip1:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip2
  move.b  d7,(a1)
.Skip2:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip3
  move.b  d7,(a1)
.Skip3:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip4
  move.b  d7,(a1)
.Skip4:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip5
  move.b  d7,(a1)
.Skip5:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip6
  move.b  d7,(a1)
.Skip6:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip7
  move.b  d7,(a1)
.Skip7:
  addq.l  #1,a1
  move.b  (a0)+,d7
  cmp.b   d4,d7
  beq.s   .Skip8
  move.b  d7,(a1)
.Skip8:
  addq.l  #1,a1
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a1
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 8bits bitmap to another 8bits bitmap
; with zoom and transparency
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.l source width in pixels
; @in d1.l source height in pixels
; @in d2.l offset to next source line
; @in d3.l destination width in pixels
; @in d4.l destination height in pixels
; @in d5.l offset to next destination line
; @in d6.l transparent color index
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitTranspZoomCopy8Bits

_SAGE_BlitTranspZoomCopy8Bits:
  movem.l d1-d7/a0-a2,-(sp)
  tst.l   d4
  beq.s   .EndOfCopy                    ; Nothing to copy
  tst.l   d3
  beq.s   .EndOfCopy                    ; Nothing to copy
  fmove.l d0,fp0
  fmove.l d3,fp2
  fdiv    fp2,fp0                       ; x_step
  fmove.l d1,fp1
  fmove.l d4,fp2
  fdiv    fp2,fp1                       ; y_step
  fmove.s #0.0,fp3                      ; y_pixel
  subq.l  #1,d3
  subq.l  #1,d4
.NextLine:
  fmove.s #0.0,fp2                      ; x_pixel
  fmove.l fp3,d1                        ; y_pixel
  mulu.w  d2,d1                         ; src_offset * y_pixel
  movea.l a0,a2
  add.l   d1,a2                         ; src_buffer + (src_offset * y_pixel)
  move.w  d3,d7
.NextPixel:
  fmove.l fp2,d0                        ; x_pixel
  move.b  0(a2,d0),d1                   ; pixel
  cmp.b   d1,d6                         ; transparent ?
  beq.s   .SkipPixel
  move.b  d1,(a1)                       ; copy pixel
.SkipPixel:
  addq.l  #1,a1
  fadd    fp0,fp2                       ; x_pixel + x_step
  dbf     d7,.NextPixel
  adda.l  d5,a1                         ; dst_buffer + dst_offset
  fadd    fp1,fp3                       ; y_pixel + y_step
  dbf     d4,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a2
  move.l  #-1,d0
  rts

;--------------------------------------
; Fill a 16bits bitmap with a color
;
; @in a0.l frame buffer address
; @in d0.w number of lines to clear
; @in d1.w number of pixels per line
; @in d2.l offset to bitmap next line
; @in d3.l fill value
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitFill16Bits

_SAGE_BlitFill16Bits:
  movem.l d1/d2/d3/d4/d6/a0,-(sp)
  move.w  d3,d4
  swap    d3
  move.w  d4,d3                         ; extend the color value to 4 bytes
  lsr.w   #2,d1                         ; 8 bytes by loop (4 pixels)
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  d3,(a0)+
  move.l  d3,(a0)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  dbf     d0,.NextLine
  movem.l (sp)+,d1/d2/d3/d4/d6/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 16bits bitmap to another 16bits bitmap
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.w number of lines to copy
; @in d1.w number of pixels per line
; @in d2.l offset to next source line
; @in d3.l offset to next destination line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitCopy16Bits

_SAGE_BlitCopy16Bits:
  movem.l d1-d6/a0-a1,-(sp)
  tst.w   d0
  beq.s   .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq.s   .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000011,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 4
  subq.w  #1,d5                         ; Extra pixels to copy
  andi.w  #%1111111111111100,d1         ; Skip extra pixels
  lsr.w   #2,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.w  d5,d6
.NextExtraPixel:
  move.w  (a0)+,(a1)+
  dbf     d6,.NextExtraPixel
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 4
  subq.w  #1,d6
.NextExtraBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #2,d1                         ; 8 bytes by loop
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d6/a0-a1
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 16bits bitmap to another 16bits bitmap
; with zoom
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.l source width in pixels
; @in d1.l source height in pixels
; @in d2.l offset to next source line
; @in d3.l destination width in pixels
; @in d4.l destination height in pixels
; @in d5.l offset to next destination line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitZoomCopy16Bits

_SAGE_BlitZoomCopy16Bits:
  movem.l d1-d7/a0-a2,-(sp)
  tst.l   d4
  beq.s   .EndOfCopy                    ; Nothing to copy
  tst.l   d3
  beq.s   .EndOfCopy                    ; Nothing to copy
  fmove.l d0,fp0
  fmove.l d3,fp2
  fdiv    fp2,fp0                       ; x_step
  fmove.l d1,fp1
  fmove.l d4,fp2
  fdiv    fp2,fp1                       ; y_step
  fmove.s #0.0,fp3                      ; y_pixel
  subq.l  #1,d3
  subq.l  #1,d4
.NextLine:
  fmove.s #0.0,fp2                      ; x_pixel
  fmove.l fp3,d1                        ; y_pixel
  mulu.w  d2,d1                         ; src_offset * y_pixel
  movea.l a0,a2
  add.l   d1,a2                         ; src_buffer + (src_offset * y_pixel)
  move.w  d3,d7
.NextPixel:
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),(a1)+              ; pixel
  fadd    fp0,fp2                       ; x_pixel + x_step
  dbf     d7,.NextPixel
  adda.l  d5,a1                         ; dst_buffer + dst_offset
  fadd    fp1,fp3                       ; y_pixel + y_step
  dbf     d4,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a2
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 16bits bitmap to another 16bits bitmap
;  with transparency
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.w number of lines to copy
; @in d1.w number of pixels per line
; @in d2.l offset to next source line
; @in d3.l offset to next destination line
; @in d4.l transparent color
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitTransparentCopy16Bits

_SAGE_BlitTransparentCopy16Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000011,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 4
  subq.w  #1,d5                         ; Extra pixels to copy
  andi.w  #%1111111111111100,d1         ; Skip extra pixels
  lsr.w   #2,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.w  d5,d6
.NextExtraPixel:
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .SkipExtraPixel
  move.w  d7,(a1)
.SkipExtraPixel:
  addq.l  #2,a1
  dbf     d6,.NextExtraPixel
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 4
  subq.w  #1,d6
.NextExtraBlock:
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .SkipExtra1
  move.w  d7,(a1)
.SkipExtra1:
  addq.l  #2,a1
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .SkipExtra2
  move.w  d7,(a1)
.SkipExtra2:
  addq.l  #2,a1
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .SkipExtra3
  move.w  d7,(a1)
.SkipExtra3:
  addq.l  #2,a1
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .SkipExtra4
  move.w  d7,(a1)
.SkipExtra4:
  addq.l  #2,a1
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #2,d1                         ; 8 bytes by loop
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .Skip1
  move.w  d7,(a1)
.Skip1:
  addq.l  #2,a1
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .Skip2
  move.w  d7,(a1)
.Skip2:
  addq.l  #2,a1
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .Skip3
  move.w  d7,(a1)
.Skip3:
  addq.l  #2,a1
  move.w  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .Skip4
  move.w  d7,(a1)
.Skip4:
  addq.l  #2,a1
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a1
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 16bits bitmap to another 16bits bitmap
; with zoom and transparency
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.l source width in pixels
; @in d1.l source height in pixels
; @in d2.l offset to next source line
; @in d3.l destination width in pixels
; @in d4.l destination height in pixels
; @in d5.l offset to next destination line
; @in d6.l transparent color index
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitTranspZoomCopy16Bits

_SAGE_BlitTranspZoomCopy16Bits:
  movem.l d1-d7/a0-a2,-(sp)
  tst.l   d4
  beq.s   .EndOfCopy                    ; Nothing to copy
  tst.l   d3
  beq.s   .EndOfCopy                    ; Nothing to copy
  fmove.l d0,fp0
  fmove.l d3,fp2
  fdiv    fp2,fp0                       ; x_step
  fmove.l d1,fp1
  fmove.l d4,fp2
  fdiv    fp2,fp1                       ; y_step
  fmove.s #0.0,fp3                      ; y_pixel
  subq.l  #1,d3
  subq.l  #1,d4
.NextLine:
  fmove.s #0.0,fp2                      ; x_pixel
  fmove.l fp3,d1                        ; y_pixel
  mulu.w  d2,d1                         ; src_offset * y_pixel
  movea.l a0,a2
  add.l   d1,a2                         ; src_buffer + (src_offset * y_pixel)
  move.w  d3,d7
.NextPixel:
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),d1                 ; pixel
  cmp.w   d1,d6                         ; transparent ?
  beq.s   .SkipPixel
  move.w  d1,(a1)                       ; copy pixel
.SkipPixel:
  addq.l  #2,a1
  fadd    fp0,fp2                       ; x_pixel + x_step
  dbf     d7,.NextPixel
  adda.l  d5,a1                         ; dst_buffer + dst_offset
  fadd    fp1,fp3                       ; y_pixel + y_step
  dbf     d4,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a2
  move.l  #-1,d0
  rts

;--------------------------------------
; Fill a 24bits bitmap with a color
;
; @in a0.l frame buffer address
; @in d0.w number of lines to clear
; @in d1.w number of pixels per line
; @in d2.l offset to bitmap next line
; @in d3.l fill value
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitFill24Bits

_SAGE_BlitFill24Bits:
  movem.l d1/d2/d3/d4/d6/a0,-(sp)
  move.l  d3,d4
  lsl.l   #8,d3
  swap    d4
  or.w    d4,d3
  swap    d4                            ; extend the color value to 6 bytes
  lsr.w   #1,d1                         ; 6 bytes by loop (2 pixels)
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  d3,(a0)+
  move.w  d4,(a0)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  dbf     d0,.NextLine
  movem.l (sp)+,d1/d2/d3/d4/d6/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Fill a 32bits bitmap with a color
;
; @in a0.l frame buffer address
; @in d0.w number of lines to clear
; @in d1.w number of pixels per line
; @in d2.l offset to bitmap next line
; @in d3.l fill value
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitFill32Bits

_SAGE_BlitFill32Bits:
  movem.l d1/d2/d3/d6/a0,-(sp)
  lsr.w   #1,d1                         ; 8 bytes by loop (2 pixels)
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  d3,(a0)+
  move.l  d3,(a0)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  dbf     d0,.NextLine
  movem.l (sp)+,d1/d2/d3/d6/a0
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 32bits bitmap to another 32bits bitmap
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.w number of lines to copy
; @in d1.w number of pixels per line
; @in d2.l offset to next source line
; @in d3.l offset to next destination line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitCopy32Bits

_SAGE_BlitCopy32Bits:
  movem.l d1-d6/a0-a1,-(sp)
  tst.w   d0
  beq.s   .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq.s   .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000001,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 2
  andi.w  #%1111111111111110,d1         ; Skip extra pixels
  lsr.w   #1,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.l  (a0)+,(a1)+
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 2
  subq.w  #1,d6
.NextExtraBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #1,d1                         ; 8 bytes by loop
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  (a0)+,(a1)+
  move.l  (a0)+,(a1)+
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d6/a0-a1
  move.l  #-1,d0
  rts

;--------------------------------------
; Copy a 32bits bitmap to another 32bits bitmap
;  with transparency
;
; @in a0.l source buffer address
; @in a1.l destination buffer address
; @in d0.w number of lines to copy
; @in d1.w number of pixels per line
; @in d2.l offset to next source line
; @in d3.l offset to next destination line
; @in d4.l transparent color
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_BlitTransparentCopy32Bits

_SAGE_BlitTransparentCopy32Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000001,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 2
  andi.w  #%1111111111111110,d1         ; Skip extra pixels
  lsr.w   #1,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.l  (a0)+,d7
  cmp.l   d4,d7
  beq.s   .SkipExtraPixel
  move.l  d7,(a1)
.SkipExtraPixel:
  addq.l  #4,a1
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 2
  subq.w  #1,d6
.NextExtraBlock:
  move.l  (a0)+,d7
  cmp.l   d4,d7
  beq.s   .SkipExtra1
  move.l  d7,(a1)
.SkipExtra1:
  addq.l  #4,a1
  move.l  (a0)+,d7
  cmp.l   d4,d7
  beq.s   .SkipExtra2
  move.l  d7,(a1)
.SkipExtra2:
  addq.l  #4,a1
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #1,d1
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  move.l  (a0)+,d7
  cmp.l   d4,d7
  beq.s   .Skip1
  move.w  d7,(a1)
.Skip1:
  addq.l  #4,a1
  move.l  (a0)+,d7
  cmp.l   d4,d7
  beq.s   .Skip2
  move.l  d7,(a1)
.Skip2:
  addq.l  #4,a1
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a1
  move.l  #-1,d0
  rts

  END
