;--------------------------------------
; sage_ammxblit.asm
;
; SAGE (Simple Amiga Game Engine) project
; Bitmap copy functions using AMMX
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 1.0 December 2020
;--------------------------------------

  SECTION ammxblit,code

;--------------------------------------
; Copy a 8bits bitmap to another 8bits bitmap
;  with transparency, using AMMX
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
  xdef _SAGE_AMMXBlitTranspCopy8Bits

_SAGE_AMMXBlitTranspCopy8Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  vperm   #$4567CDEF,d4,d4,e4           ; Extend d4 value to 64 bits
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  pcmpeqb e4,e0,e1                      ; Check for transparent pixel
  storeilm  e0,e1,(a1)+                 ; Store only bytes where the check didn't match
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #3,d1
  beq.s   .EndOfCopy
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  load    (a0)+,e0                      ; Load 8 bytes from source
  pcmpeqb e4,e0,e1                      ; Check for transparent pixel
  storeilm  e0,e1,(a1)+                 ; Store only bytes where the check didn't match
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
;  with transparency, using new AMMX instruction
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
  xdef _SAGE_AMMXBlitCookieCut8Bits

_SAGE_AMMXBlitCookieCut8Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  vperm   #$4567CDEF,d4,d4,e4           ; Extend d4 value to 64 bits
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  storem3 e0,d1,(a1)+                   ; Store only bytes where the color is not $00
  dbf     d6,.NextExtraBlock
.OnlyExtraPixel:
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextExtraLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #3,d1
  beq.s   .EndOfCopy
  subq.w  #1,d1
  subq.w  #1,d0
.NextLine:
  move.w  d1,d6
.NextBlock:
  load    (a0)+,e0                      ; Load 8 bytes from source
  storem3 e0,d1,(a1)+                   ; Store only bytes where the color is not $00
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
;  with transparency, using AMMX
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
  xdef _SAGE_AMMXBlitTranspCopy16Bits

_SAGE_AMMXBlitTranspCopy16Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  vperm   #$4567CDEF,d4,d4,e4           ; Extend d4 value to 64 bits
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  pcmpeqw e4,e0,e1                      ; Check for transparent pixel
  storeilm  e0,e1,(a1)+                 ; Store only words where the check didn't match
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  pcmpeqw e4,e0,e1                      ; Check for transparent pixel
  storeilm  e0,e1,(a1)+                 ; Store only words where the check didn't match
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
;  with transparency, using new AMMX instruction
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
  xdef _SAGE_AMMXBlitCookieCut16Bits

_SAGE_AMMXBlitCookieCut16Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  vperm   #$4567CDEF,d4,d4,e4           ; Extend d4 value to 64 bits
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  storem3 e0,d2,(a1)+                   ; Store only words where the color is not $F81F
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  storem3 e0,d2,(a1)+                   ; Store only words where the color is not $F81F
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a1
  move.l  #-1,d0
  rts


; WORK IN PROGRESS !!!!!!!!!!!!!!!!!

;--------------------------------------
; Copy a 16bits bitmap to another 16bits bitmap
; with zoom and transparency, using new AMMX instruction
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
  xdef _SAGE_AMMXBlitCookieCutZoom16Bits

_SAGE_AMMXBlitCookieCutZoom16Bits:
  movem.l d1-d7/a0-a4,-(sp)
  tst.l   d4
  beq.s   .EndOfCopy                    ; Nothing to copy
  load    d3,e0
  beq.s   .EndOfCopy                    ; Nothing to copy
  vperm   #$4567CDEF,d6,d6,e4           ; Extend d6 value to 64 bits
  pand    #$3,e0,e0
  beq.s   .StandardCopy                 ; The width is a multiple of 4
  subq.w  #1,d5                         ; Extra pixels to copy

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
  movem.l (sp)+,d1-d7/a0-a4
  move.l  #-1,d0
  rts
.StandardCopy:
  movem.l (sp)+,d1-d7/a0-a4
  move.l  #-1,d0
  rts

  SECTION ammxblitdata,data

_SAGE_CopyBuffer:
  dc.l    0,0                           ; Small buffer for transparent copy

  END
