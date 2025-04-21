;--------------------------------------
; sage_ammxblit.asm
;
; SAGE (Simple Amiga Game Engine) project
; Bitmap copy functions using AMMX
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 25.1 February 2025 (updated: 26/02/2025)
;--------------------------------------

  SECTION ammxblit,code

;------------------------------------------------------------------------------
;         8 BITS FUNCTIONS
;------------------------------------------------------------------------------

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

;------------------------------------------------------------------------------
;         16 BITS FUNCTIONS
;------------------------------------------------------------------------------

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
  andi.w  #%0000000000000011,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 4
  subq.w  #1,d5                         ; Extra pixels to copy
  andi.w  #%1111111111111100,d1         ; Skip extra pixels
  lsr.w   #2,d1                         ; 4 pixels by loop
  subq.w  #1,d0
.NextExtraLine:
  move.w  d5,d6
.NextExtraPixel:
  move.w  (a0)+,d7
  cmpi.w  #$F81F,d7
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

;--------------------------------------
; Copy a 16bits bitmap to another 16bits bitmap
; with zoom and transparency, using new AMMX instruction
;
; @in a0.l source buffer address
; @in d0.l source width in pixels
; @in d1.l source height in pixels
; @in d2.l offset to next source line
; @in a1.l destination buffer address
; @in d3.l destination width in pixels
; @in d4.l destination height in pixels
; @in d5.l offset to next destination line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_AMMXBlitCookieCutZoom16Bits

_SAGE_AMMXBlitCookieCutZoom16Bits:
  movem.l d1-d7/a0-a4,-(sp)
  tst.l   d4
  beq     .EndOfCopy                    ; Nothing to copy
  move.l  d3,d7
  beq     .EndOfCopy                    ; Nothing to copy
  lea     _SAGE_CopyBuffer,a3           ; Small copy buffer
  fmove.l d0,fp0
  fmove.l d3,fp2
  fdiv    fp2,fp0                       ; x_step
  fmove.l d1,fp1
  fmove.l d4,fp2
  fdiv    fp2,fp1                       ; y_step
  fmove.s #0.0,fp3                      ; y_pixel
  andi.w  #%0000000000000011,d7
  beq     .StandardCopy                 ; The width is a multiple of 4
  andi.w  #%1111111111111100,d3         ; Skip extra pixels
  lsr.w   #2,d3                         ; 4 pixels by loop
  subq.w  #1,d7                         ; Extra pixels to copy
  subq.w  #1,d4                         ; Number of lines to copy
.NextLine:
  fmove.s #0.0,fp2                      ; x_pixel
  movea.l a0,a2                         ; Source start address
  fmove.l fp3,d1                        ; y_pixel
  mulu.w  d2,d1                         ; src_offset * y_pixel
  add.l   d1,a2                         ; src_buffer + (src_offset * y_pixel)
  move.w  d7,d6
.NextExtraPixel:
  fmove.l fp2,d1                        ; x_pixel
  move.w  0(a2,d1*2),d1                 ; Source pixel
  cmpi.w  #$F81F,d1                     ; Transparent ?
  beq.s   .SkipExtraPixel               ; Yes
  move.w  d1,(a1)                       ; Write it
.SkipExtraPixel:
  adda.l  #2,a1
  fadd    fp0,fp2                       ; x_pixel + x_step
  dbf     d6,.NextExtraPixel
  move.w  d3,d6                         ; Pixels to copy
  beq.s   .OnlyExtraPixel               ; Width is less than 4
  subq.w  #1,d6
.NextPixels:
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),(a3)               ; pixel 1
  fadd    fp0,fp2                       ; x_pixel + x_step
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),2(a3)              ; pixel 2
  fadd    fp0,fp2                       ; x_pixel + x_step
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),4(a3)              ; pixel 3
  fadd    fp0,fp2                       ; x_pixel + x_step
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),6(a3)              ; pixel 4
  fadd    fp0,fp2                       ; x_pixel + x_step
  load    (a3),e0                       ; Load 8 bytes from buffer
  storem3 e0,d2,(a1)+                   ; Store only words where the color is not $F81F
  dbf     d6,.NextPixels
.OnlyExtraPixel:
  adda.l  d5,a1                         ; dst_buffer + dst_offset
  fadd    fp1,fp3                       ; y_pixel + y_step
  dbf     d4,.NextLine
  bra.s   .EndOfCopy
.StandardCopy:
  lsr.w   #2,d3                         ; 4 pixels by loop
  subq.l  #1,d3                         ; Number of pixels group to copy
  subq.l  #1,d4                         ; Number of lines to copy
.NextStdLine:
  fmove.s #0.0,fp2                      ; x_pixel
  movea.l a0,a2                         ; Source start address
  fmove.l fp3,d1                        ; y_pixel
  mulu.w  d2,d1                         ; src_offset * y_pixel
  add.l   d1,a2                         ; src_buffer + (src_offset * y_pixel)
  move.w  d3,d7
.NextStdPixel:
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),(a3)               ; pixel 1
  fadd    fp0,fp2                       ; x_pixel + x_step
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),2(a3)              ; pixel 2
  fadd    fp0,fp2                       ; x_pixel + x_step
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),4(a3)              ; pixel 3
  fadd    fp0,fp2                       ; x_pixel + x_step
  fmove.l fp2,d0                        ; x_pixel
  move.w  0(a2,d0*2),6(a3)              ; pixel 4
  fadd    fp0,fp2                       ; x_pixel + x_step
  load    (a3),e0                       ; Load 8 bytes from buffer
  storem3 e0,d2,(a1)+                   ; Store only words where the color is not $F81F
  dbf     d7,.NextStdPixel
  adda.l  d5,a1                         ; dst_buffer + dst_offset
  fadd    fp1,fp3                       ; y_pixel + y_step
  dbf     d4,.NextStdLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a4
  move.l  #-1,d0
  rts

;------------------------------------------------------------------------------
;         32 BITS FUNCTIONS
;------------------------------------------------------------------------------

;--------------------------------------
; Copy a 32bits bitmap to another 32bits bitmap
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
  xdef _SAGE_AMMXBlitTranspCopy32Bits

_SAGE_AMMXBlitTranspCopy32Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  vperm   #$4567CDEF,d4,d4,e4           ; Extend d4 value to 64 bits
  andi.w  #%0000000000000001,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 2
  andi.w  #%1111111111111110,d1         ; Skip extra pixels
  add.w   d1,d1                         ; 8 bytes by loop
  subq.w  #1,d0
.NextExtraLine:
  move.l  (a0)+,d7
  cmp.w   d4,d7
  beq.s   .SkipExtraPixel
  move.l  d7,(a1)
.SkipExtraPixel:
  addq.l  #4,a1
  move.w  d1,d6
  beq.s   .OnlyExtraPixel               ; Width is less than 2
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
  add.w   d1,d1                         ; 8 bytes by loop
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
; Copy a 32bits bitmap to another 32bits bitmap
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
  xdef _SAGE_AMMXBlitCookieCut32Bits

_SAGE_AMMXBlitCookieCut32Bits:
  movem.l d1-d7/a0-a1,-(sp)
  tst.w   d0
  beq     .EndOfCopy                    ; Nothing to copy
  move.w  d1,d5
  beq     .EndOfCopy                    ; Nothing to copy
  andi.w  #%0000000000000001,d5
  beq.s   .StandardCopy                 ; The width is a multiple of 2
  andi.w  #%1111111111111110,d1         ; Skip extra pixels
  add.w   d1,d1                         ; 8 bytes by loop
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
  load    (a0)+,e0                      ; Load 8 bytes from source
  storem3 e0,d0,(a1)+                   ; Store only words where the color is not transparent
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
  storem3 e0,d0,(a1)+                   ; Store only words where the color is not transparent
  dbf     d6,.NextBlock
  adda.l  d2,a0
  adda.l  d3,a1
  dbf     d0,.NextLine
.EndOfCopy:
  movem.l (sp)+,d1-d7/a0-a1
  move.l  #-1,d0
  rts

  SECTION ammxblitdata,data

_SAGE_CopyBuffer:
  dc.l    0,0                           ; Small buffer for transparent copy

  END
