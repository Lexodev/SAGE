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

  END
