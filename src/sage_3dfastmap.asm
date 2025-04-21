;--------------------------------------
; sage_3dfastmap.asm
;
; SAGE (Simple Amiga Game Engine) project
; Fast mapping functions
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 25.1 February 2025 (updated: 10/03/2025)
;--------------------------------------

; SAGE_TextureMapping struct
STM_FRAME_BUFFER    EQU 0*4
STM_FB_BPR          EQU 1*4
STM_Z_BUFFER        EQU 2*4
STM_ZB_BPR          EQU 3*4
STM_LCLIP           EQU 4*4
STM_RCLIP           EQU 5*4
STM_START_Y         EQU 6*4
STM_NB_LINE         EQU 7*4
STM_DXDYL           EQU 8*4
STM_DXDYR           EQU 9*4
STM_DZDYL           EQU 10*4
STM_DZDYR           EQU 11*4
STM_DUDYL           EQU 12*4
STM_DUDYR           EQU 13*4
STM_DVDYL           EQU 14*4
STM_DVDYR           EQU 15*4
STM_TEX_BUFFER      EQU 16*4
STM_TB_BPR          EQU 17*4
STM_COLOR           EQU 18*4
STM_DU              EQU 19*4
STM_DV              EQU 20*4
STM_DZ              EQU 21*4
STM_XL              EQU 22*4
STM_XR              EQU 23*4
STM_ZL              EQU 24*4
STM_ZR              EQU 25*4
STM_UL              EQU 26*4
STM_UR              EQU 27*4
STM_VL              EQU 28*4
STM_VR              EQU 29*4

NO_TRANSP_COLOR     EQU $BADCBADC

FIXP16_ROUND_UP     EQU $8000

  SECTION fastmap,code

;--------------------------------------
; Clear the z buffer
;
; @in a0.l z buffer address
; @in d0.w number of lines to clear
; @in d1.w number of bytes per line
;
; @out d0.l Operation success
;--------------------------------------
  xdef _SAGE_FastClearZBuffer

_SAGE_FastClearZBuffer:
  movem.l d1/d2/d6/a0,-(sp)
  move.l  #$FFFFFFFF,d2
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

;------------------------------------------------------------------------------
;           8 bits functions
;------------------------------------------------------------------------------

;--------------------------------------
; Map a 8bits flat color
;
; @in a0.l texmap structure address
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastMap8BitsColor

_SAGE_FastMap8BitsColor:
  movem.l d1-d7/a0-a6,-(sp)
  move.l  STM_NB_LINE(a0),d0            ; no lines ?
  ble     .EndDraw

; Calcul start line address
  move.l  STM_START_Y(a0),d3
  move.l  STM_FRAME_BUFFER(a0),a1
  move.l  STM_FB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a1                         ; FB start line addr
  move.l  STM_Z_BUFFER(a0),a2
  move.l  STM_ZB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a2                         ; ZB start line addr
  move.l  STM_COLOR(a0),d3              ; Color

  subq.l  #1,d0
.MapNextLine:

; Calcul edge coords
  move.l  STM_XL(a0),d1
  addi.l  #FIXP16_ROUND_UP,d1
  swap    d1
  ext.l   d1                            ; xs
  move.l  STM_XR(a0),d2
  addi.l  #FIXP16_ROUND_UP,d2
  swap    d2
  ext.l   d2                            ; xe

; Check for left/right clipping
.PointsOnScreen:
  cmp.l   STM_RCLIP(a0),d1
  bge    .Interpolate
  cmp.l   STM_LCLIP(a0),d2
  blt    .Interpolate

  move.l  STM_ZR(a0),d5
  sub.l   STM_ZL(a0),d5

  move.l  d2,d7
  sub.l   d1,d7                         ; dx
  ble.s   .DxNegative
  divs.l  d7,d5                         ; dz
  bra.s   .StoreDelta
.DxNegative:
  moveq.l #0,d7
.StoreDelta:
  move.l  d5,STM_DZ(a0)

  move.l  STM_ZL(a0),d5
  addi.l  #FIXP16_ROUND_UP,d5           ; zi

; Horizontal clipping
  cmp.l   STM_LCLIP(a0),d1
  bge.s   .NoLeftClip
  move.l  STM_LCLIP(a0),d7
  sub.l   d1,d7
  move.l  STM_DZ(a0),d6
  muls.l  d7,d6
  add.l   d6,d5
  move.l  STM_LCLIP(a0),d1
  move.l  d2,d7
  sub.l   d1,d7
.NoLeftClip:
  cmp.l   STM_RCLIP(a0),d2
  blt.s   .NoRightClip
  move.l  STM_RCLIP(a0),d7
  subq.l  #1,d7
  sub.l   d1,d7
.NoRightClip:

; Calcul start address
  movea.l a1,a4
  adda.l  d1,a4                         ; start address

; Draw the line
  tst.l   a2
  beq.s   .NoZBuffer
.ZBuffer:
  movea.l a2,a5
  lsl.l   #1,d1                         ; 16bits zbuffer
  adda.l  d1,a5
.NextTexelZ:
  move.l  d5,d1
  swap    d1
  andi.l  #$ffff,d1
  moveq.l #0,d2
  move.w  (a5),d2
  cmp.l   d1,d2
  ble.s   .DoNotDraw
  move.w  d1,(a5)
  move.b  d3,(a4)
.DoNotDraw:
  adda.l  #1,a4
  adda.l  #2,a5
; Interpolate z
  add.l   STM_DZ(a0),d5
.NextTexelZEnd:
  dbf     d7,.NextTexelZ
  bra.s   .Interpolate

.NoZBuffer:
  addq.l  #1,d7                         ; at least we should draw 1 pixel
  move.l  d7,d6                         ; save the value
  andi.l  #$7,d6                        ; look for a multiple of 8
  beq.s   .DrawFastTexel                ; draw only block of eight texels
  subq.l  #1,d6                         ; extra texels to draw
.DrawExtraTexel:
  move.b  d3,(a4)+
  dbf     d6,.DrawExtraTexel
.DrawFastTexel:
  andi.l  #$fffffff8,d7                 ; clear low bits
  beq.s   .Interpolate                  ; nothing more to draw
  lsr.l   #3,d7                         ; draw 8 texels each time
  subq.l  #1,d7                         ; texels to draw
; Draw the line
.NextTexel:
  move.l  d3,(a4)+
  move.l  d3,(a4)+
  dbf     d7,.NextTexel

; Interpolate next points
.Interpolate:
  move.l  STM_DXDYL(a0),d1
  add.l   d1,STM_XL(a0)
  move.l  STM_DZDYL(a0),d2
  add.l   d2,STM_ZL(a0)
  move.l  STM_DXDYR(a0),d1
  add.l   d1,STM_XR(a0)
  move.l  STM_DZDYR(a0),d2
  add.l   d2,STM_ZR(a0)

; Next line address
  adda.l  STM_FB_BPR(a0),a1
  adda.l  STM_ZB_BPR(a0),a2

.MapNextLineEnd:
  dbf     d0,.MapNextLine

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

;--------------------------------------
; Map a 8bits texture
;
; @in a0.l texmap structure address
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastMap8BitsTexture

_SAGE_FastMap8BitsTexture:
  movem.l d1-d7/a0-a6,-(sp)
  move.l  STM_NB_LINE(a0),d0            ; no lines ?
  ble     .EndDraw

; Calcul start line address
  move.l  STM_START_Y(a0),d3
  move.l  STM_FRAME_BUFFER(a0),a1
  move.l  STM_FB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a1                         ; FB start line addr
  move.l  STM_Z_BUFFER(a0),a2
  move.l  STM_ZB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a2                         ; ZB start line addr
  move.l  STM_TEX_BUFFER(a0),a3

  subq.l  #1,d0
.MapNextLine:

; Calcul edge coords
  move.l  STM_XL(a0),d1
  addi.l  #FIXP16_ROUND_UP,d1
  swap    d1
  ext.l   d1                            ; xs
  move.l  STM_XR(a0),d2
  addi.l  #FIXP16_ROUND_UP,d2
  swap    d2
  ext.l   d2                            ; xe

; Check for left/right clipping
.PointsOnScreen:
  cmp.l   STM_RCLIP(a0),d1
  bge    .Interpolate
  cmp.l   STM_LCLIP(a0),d2
  blt    .Interpolate

; Calcul texture interpolation
  move.l  STM_UR(a0),d3
  sub.l   STM_UL(a0),d3
  move.l  STM_VR(a0),d4
  sub.l   STM_VL(a0),d4
  move.l  STM_ZR(a0),d5
  sub.l   STM_ZL(a0),d5
  move.l  d2,d7
  sub.l   d1,d7                         ; dx
  ble.s   .DxNegative
  divs.l  d7,d3                         ; du
  divs.l  d7,d4                         ; dv
  divs.l  d7,d5                         ; dz
  bra.s   .StoreDelta
.DxNegative:
  moveq.l #0,d7
.StoreDelta:
  move.l  d3,STM_DU(a0)
  move.l  d4,STM_DV(a0)
  move.l  d5,STM_DZ(a0)

; Calcul texture coords
  move.l  STM_UL(a0),d3
  addi.l  #FIXP16_ROUND_UP,d3           ; ui
  move.l  STM_VL(a0),d4
  addi.l  #FIXP16_ROUND_UP,d4           ; vi
; Calcul Z value
  move.l  STM_ZL(a0),d5
  addi.l  #FIXP16_ROUND_UP,d5           ; zi

; Horizontal clipping
  cmp.l   STM_LCLIP(a0),d1
  bge.s   .NoLeftClip
  move.l  STM_LCLIP(a0),d7
  sub.l   d1,d7
  move.l  STM_DU(a0),d6
  muls.l  d7,d6
  add.l   d6,d3
  move.l  STM_DV(a0),d6
  muls.l  d7,d6
  add.l   d6,d4
  move.l  STM_DZ(a0),d6
  muls.l  d7,d6
  add.l   d6,d5
  move.l  STM_LCLIP(a0),d1
  move.l  d2,d7
  sub.l   d1,d7
.NoLeftClip:
  cmp.l   STM_RCLIP(a0),d2
  blt.s   .NoRightClip
  move.l  STM_RCLIP(a0),d7
  subq.l  #1,d7
  sub.l   d1,d7
.NoRightClip:

; Calcul start address
  movea.l a1,a4
  adda.l  d1,a4                         ; start address

; Draw the line
  tst.l   a2
  beq.s   .NoZBuffer
.ZBuffer:
  movea.l a2,a5
  lsl.l   #1,d1                         ; 16bits
  adda.l  d1,a5
.NextTexelZ:
  move.l  d5,d1
  swap    d1
  andi.l  #$ffff,d1
  moveq.l #0,d2
  move.w  (a5),d2
  cmp.l   d1,d2
  ble.s   .DoNotDraw
  move.w  d1,(a5)
  move.l  d3,d1
  swap    d1
  ext.l   d1                            ; (ui >> FIXP16_SHIFT)
  move.l  d4,d2
  swap    d2
  ext.l   d2
  mulu.l  STM_TB_BPR(a0),d2             ; (vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr
  add.l   d1,d2                         ; texel_x + texel_y
  move.b  0(a3,d2.l),(a4)
.DoNotDraw:
  adda.l  #1,a4
  adda.l  #2,a5
; Interpolate u, v & z
  add.l   STM_DU(a0),d3
  add.l   STM_DV(a0),d4
  add.l   STM_DZ(a0),d5
.NextTexelZEnd:
  dbf     d7,.NextTexelZ
  bra.s   .Interpolate

.NoZBuffer:
.NextTexel:
; Write the texel
  move.l  d3,d1
  swap    d1
  ext.l   d1                            ; (ui >> FIXP16_SHIFT)
  move.l  d4,d2
  swap    d2
  ext.l   d2
  mulu.l  STM_TB_BPR(a0),d2             ; (vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr
  add.l   d1,d2                         ; texel_x + texel_y
  move.b  0(a3,d2.l),(a4)+
; Interpolate u & v
  add.l   STM_DU(a0),d3
  add.l   STM_DV(a0),d4
.NextTexelEnd:
  dbf     d7,.NextTexel

; Interpolate next points
.Interpolate:
  move.l  STM_DXDYL(a0),d1
  add.l   d1,STM_XL(a0)
  move.l  STM_DZDYL(a0),d2
  add.l   d2,STM_ZL(a0)
  move.l  STM_DUDYL(a0),d3
  add.l   d3,STM_UL(a0)
  move.l  STM_DVDYL(a0),d4
  add.l   d4,STM_VL(a0)
  move.l  STM_DXDYR(a0),d1
  add.l   d1,STM_XR(a0)
  move.l  STM_DZDYR(a0),d2
  add.l   d2,STM_ZR(a0)
  move.l  STM_DUDYR(a0),d3
  add.l   d3,STM_UR(a0)
  move.l  STM_DVDYR(a0),d4
  add.l   d4,STM_VR(a0)

; Next line address
  adda.l  STM_FB_BPR(a0),a1
  adda.l  STM_ZB_BPR(a0),a2
.MapNextLineEnd:
  dbf     d0,.MapNextLine

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

;--------------------------------------
; Map a 8bits transparent texture
;
; @in a0.l texmap structure address
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastMap8BitsTransparent

_SAGE_FastMap8BitsTransparent:
  movem.l d1-d7/a0-a6,-(sp)

; Draw the line
.NextTexel:
; Write the texel
  move.l  d6,d4
  swap    d4
  mulu.w  d1,d4                         ; (vi >> FIXP16_SHIFT) * texture_width
  move.l  d5,d3
  swap    d3
  ext.l   d3
  add.l   d3,d4                         ; + ui >> FIXP16_SHIFT
  move.b  0(a0,d4.l),d3
;  cmp.b   CRD_TCOLOR(a3),d3
  beq.s   .Transparent
  move.b  d3,(a4)
.Transparent:
  adda.l  #1,a4
; Interpolate u & v
;  add.l   DELTA_DU(a2),d5
;  add.l   DELTA_DV(a2),d6
  dbf     d7,.NextTexel

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

;------------------------------------------------------------------------------
;           16 bits functions
;------------------------------------------------------------------------------

;--------------------------------------
; Map a 16bits flat color
;
; @in a0.l texmap structure address
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastMap16BitsColor

_SAGE_FastMap16BitsColor:
  movem.l d1-d7/a0-a6,-(sp)
  move.l  STM_NB_LINE(a0),d0            ; no lines ?
  ble     .EndDraw

; Calcul start line address
  move.l  STM_START_Y(a0),d3
  move.l  STM_FRAME_BUFFER(a0),a1
  move.l  STM_FB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a1                         ; FB start line addr
  move.l  STM_Z_BUFFER(a0),a2
  move.l  STM_ZB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a2                         ; ZB start line addr
  move.l  STM_COLOR(a0),d3              ; Color

  subq.l  #1,d0
.MapNextLine:

; Calcul edge coords
  move.l  STM_XL(a0),d1
  addi.l  #FIXP16_ROUND_UP,d1
  swap    d1
  ext.l   d1                            ; xs
  move.l  STM_XR(a0),d2
  addi.l  #FIXP16_ROUND_UP,d2
  swap    d2
  ext.l   d2                            ; xe

; Check for left/right clipping
.PointsOnScreen:
  cmp.l   STM_RCLIP(a0),d1
  bge    .Interpolate
  cmp.l   STM_LCLIP(a0),d2
  blt    .Interpolate

  move.l  STM_ZR(a0),d5
  sub.l   STM_ZL(a0),d5

  move.l  d2,d7
  sub.l   d1,d7                         ; dx
  ble.s   .DxNegative
  divs.l  d7,d5                         ; dz
  bra.s   .StoreDelta
.DxNegative:
  moveq.l #0,d7
.StoreDelta:
  move.l  d5,STM_DZ(a0)

  move.l  STM_ZL(a0),d5
  addi.l  #FIXP16_ROUND_UP,d5           ; zi

; Horizontal clipping
  cmp.l   STM_LCLIP(a0),d1
  bge.s   .NoLeftClip
  move.l  STM_LCLIP(a0),d7
  sub.l   d1,d7
  move.l  STM_DZ(a0),d6
  muls.l  d7,d6
  add.l   d6,d5
  move.l  STM_LCLIP(a0),d1
  move.l  d2,d7
  sub.l   d1,d7
.NoLeftClip:
  cmp.l   STM_RCLIP(a0),d2
  blt.s   .NoRightClip
  move.l  STM_RCLIP(a0),d7
  subq.l  #1,d7
  sub.l   d1,d7
.NoRightClip:

; Calcul start address
  movea.l a1,a4
  lsl.l   #1,d1                         ; 16bits
  adda.l  d1,a4                         ; start address

; Draw the line
  tst.l   a2
  beq.s   .NoZBuffer
.ZBuffer:
  movea.l a2,a5
  adda.l  d1,a5
.NextTexelZ:
  move.l  d5,d1
  swap    d1
  andi.l  #$ffff,d1
  moveq.l #0,d2
  move.w  (a5),d2
  cmp.l   d1,d2
  ble.s   .DoNotDraw
  move.w  d1,(a5)
  move.w  d3,(a4)
.DoNotDraw:
  adda.l  #2,a4
  adda.l  #2,a5
; Interpolate z
  add.l   STM_DZ(a0),d5
.NextTexelZEnd:
  dbf     d7,.NextTexelZ
  bra.s   .Interpolate

.NoZBuffer:
  addq.l  #1,d7                         ; at least we should draw 1 pixel
  move.l  d7,d6                         ; save the value
  andi.l  #$3,d6                        ; look for a multiple of 4
  beq.s   .DrawFastTexel                ; draw only block of eight texels
  subq.l  #1,d6                         ; extra texels to draw
.DrawExtraTexel:
  move.w  d3,(a4)+
  dbf     d6,.DrawExtraTexel
.DrawFastTexel:
  andi.l  #$fffffffc,d7                 ; clear low bits
  beq.s   .Interpolate                  ; nothing more to draw
  lsr.l   #2,d7                         ; draw 4 texels each time
  subq.l  #1,d7                         ; texels to draw
.NextTexel:
  move.l  d3,(a4)+
  move.l  d3,(a4)+
  dbf     d7,.NextTexel

; Interpolate next points
.Interpolate:
  move.l  STM_DXDYL(a0),d1
  add.l   d1,STM_XL(a0)
  move.l  STM_DZDYL(a0),d2
  add.l   d2,STM_ZL(a0)
  move.l  STM_DXDYR(a0),d1
  add.l   d1,STM_XR(a0)
  move.l  STM_DZDYR(a0),d2
  add.l   d2,STM_ZR(a0)

; Next line address
  adda.l  STM_FB_BPR(a0),a1
  adda.l  STM_ZB_BPR(a0),a2

.MapNextLineEnd:
  dbf     d0,.MapNextLine

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

;--------------------------------------
; Map a 16bits texture (new version)
;
; @in a0.l texmap structure address
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastMap16BitsTexture

; Register usage
; - a0 = data structure
; - d0 = number of lines to draw
; - a1 = frame buffer line address
; - a2 = z buffer line address
; - a3 = texture address
; - a4 = screen address
; - d1 = xs
; - d2 = xe
; - d3 = du then ui
; - d4 = dv then vi
; - d5 = dz then zi
; - d7 = dx
_SAGE_FastMap16BitsTexture:
  movem.l d1-d7/a0-a6,-(sp)

;  nblines = s3dm_texmap.nb_line;
  move.l  STM_NB_LINE(a0),d0            ; no lines ?
  ble     .EndDraw

; Calcul start line address
  move.l  STM_START_Y(a0),d3
;  fb_line = s3dm_texmap.frame_buffer + (s3dm_texmap.start_y * s3dm_texmap.fb_bpr);
  move.l  STM_FRAME_BUFFER(a0),a1
  move.l  STM_FB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a1                         ; FB start line addr
;  zb_line = s3dm_texmap.z_buffer + (s3dm_texmap.start_y * s3dm_texmap.zb_bpr);
  move.l  STM_Z_BUFFER(a0),a2
  move.l  STM_ZB_BPR(a0),d1
  mulu.w  d3,d1
  adda.l  d1,a2                         ; ZB start line addr
; s3dm_texmap.tex_buffer
  move.l  STM_TEX_BUFFER(a0),a3

;  while (nblines--)
  subq.l  #1,d0
.MapNextLine:

; Calcul edge coords
;  xs = (s3dm_texmap.xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
  move.l  STM_XL(a0),d1
  addi.l  #FIXP16_ROUND_UP,d1
  swap    d1
  ext.l   d1                            ; xs
;  xe = (s3dm_texmap.xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
  move.l  STM_XR(a0),d2
  addi.l  #FIXP16_ROUND_UP,d2
  swap    d2
  ext.l   d2                            ; xe

; Check for left/right clipping
;  if (xs < s3dm_texmap.rclip && xe >= s3dm_texmap.lclip)
.PointsOnScreen:
  cmp.l   STM_RCLIP(a0),d1
  bge    .Interpolate
  cmp.l   STM_LCLIP(a0),d2
  blt    .Interpolate

; Calcul texture interpolation
;  du = s3dm_texmap.ur - s3dm_texmap.ul;
  move.l  STM_UR(a0),d3
  sub.l   STM_UL(a0),d3
;  dv = s3dm_texmap.vr - s3dm_texmap.vl;
  move.l  STM_VR(a0),d4
  sub.l   STM_VL(a0),d4
;  dz = s3dm_texmap.zr - s3dm_texmap.zl;
  move.l  STM_ZR(a0),d5
  sub.l   STM_ZL(a0),d5
;  dx = xe - xs;
  move.l  d2,d7
  sub.l   d1,d7                         ; dx
;  if (dx > 0)
  ble.s   .DxNegative
;  du /= dx;
  divs.l  d7,d3                         ; du
;  dv /= dx;
  divs.l  d7,d4                         ; dv
;  dz /= dx;
  divs.l  d7,d5                         ; dz
  bra.s   .StoreDelta
.DxNegative:
  moveq.l #0,d7
.StoreDelta:
  move.l  d3,STM_DU(a0)
  move.l  d4,STM_DV(a0)
  move.l  d5,STM_DZ(a0)

; Calcul texture coords
;  ui = s3dm_texmap.ul + FIXP16_ROUND_UP;
  move.l  STM_UL(a0),d3
  addi.l  #FIXP16_ROUND_UP,d3           ; ui
;  vi = s3dm_texmap.vl + FIXP16_ROUND_UP;
  move.l  STM_VL(a0),d4
  addi.l  #FIXP16_ROUND_UP,d4           ; vi
; Calcul Z value
;  zi = s3dm_texmap.zl + FIXP16_ROUND_UP;
  move.l  STM_ZL(a0),d5
  addi.l  #FIXP16_ROUND_UP,d5           ; zi

; Horizontal clipping
;  if (xs < s3dm_texmap.lclip)
  cmp.l   STM_LCLIP(a0),d1
  bge.s   .NoLeftClip
;  dx = s3dm_texmap.lclip - xs;
  move.l  STM_LCLIP(a0),d7
  sub.l   d1,d7
;  ui += dx * du;
  move.l  STM_DU(a0),d6
  muls.l  d7,d6
  add.l   d6,d3
;  vi += dx * dv;
  move.l  STM_DV(a0),d6
  muls.l  d7,d6
  add.l   d6,d4
;  zi += dx * dz;
  move.l  STM_DZ(a0),d6
  muls.l  d7,d6
  add.l   d6,d5
;  xs = s3dm_texmap.lclip;
  move.l  STM_LCLIP(a0),d1
;  dx = xe - xs;
  move.l  d2,d7
  sub.l   d1,d7
.NoLeftClip:
;  if (xe >= s3dm_texmap.rclip)
  cmp.l   STM_RCLIP(a0),d2
  blt.s   .NoRightClip
;  dx = (s3dm_texmap.rclip - 1) - xs;
  move.l  STM_RCLIP(a0),d7
  subq.l  #1,d7
  sub.l   d1,d7
.NoRightClip:

; Calcul start address
;  screen = fb_line + (xs * 2);    // Because screen is 16bits
  movea.l a1,a4
  lsl.l   #1,d1                         ; 16bits
  adda.l  d1,a4                         ; start address

; d1, d2 & d6 are free for use

; Draw the line
;  if (s3dm_texmap.z_buffer != NULL)
  tst.l   a2
  beq.s   .NoZBuffer
.ZBuffer:
;  zbuffer = (UWORD *)(zb_line + (xs * 2)); // Because zbuffer is always 16bits
  movea.l a2,a5
  adda.l  d1,a5
;  while (dx--)
.NextTexelZ:
;  if (*zbuffer > (UWORD)(zi >> FIXP16_SHIFT))
  move.l  d5,d1
  swap    d1
  andi.l  #$ffff,d1
  moveq.l #0,d2
  move.w  (a5),d2
  cmp.l   d1,d2
  ble.s   .DoNotDraw
;  *zbuffer = (UWORD)(zi >> FIXP16_SHIFT);
  move.w  d1,(a5)
;  texture = (UBYTE *)s3dm_texmap.tex_buffer + (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr);
  move.l  d3,d1
  swap    d1
  ext.l   d1
  add.l   d1,d1                         ; (ui >> FIXP16_SHIFT) * 2
  move.l  d4,d2
  swap    d2
  ext.l   d2
  mulu.l  STM_TB_BPR(a0),d2             ; (vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr
  add.l   d1,d2                         ; texel_x + texel_y
  move.w  0(a3,d2.l),(a4)
.DoNotDraw:
;  screen++;
  adda.l  #2,a4
;  zbuffer++;
  adda.l  #2,a5
; Interpolate u, v & z
;  ui += du;
  add.l   STM_DU(a0),d3
;  vi += dv;
  add.l   STM_DV(a0),d4
;  zi += dz
  add.l   STM_DZ(a0),d5
.NextTexelZEnd:
  dbf     d7,.NextTexelZ
  bra.s   .Interpolate

.NoZBuffer:
;  while (dx--)
.NextTexel:
; Write the texel
;  texture = (UBYTE *)s3dm_texmap.tex_buffer + ((ui >> FIXP16_SHIFT) * 2) + ((vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr);
  move.l  d3,d1
  swap    d1
  ext.l   d1
  add.l   d1,d1                         ; (ui >> FIXP16_SHIFT) * 2
  move.l  d4,d2
  swap    d2
  ext.l   d2
  mulu.l  STM_TB_BPR(a0),d2             ; (vi >> FIXP16_SHIFT) * s3dm_texmap.tb_bpr
  add.l   d1,d2                         ; texel_x + texel_y
  move.w  0(a3,d2.l),(a4)+
; Interpolate u & v
;  ui += du;
  add.l   STM_DU(a0),d3
;  vi += dv;
  add.l   STM_DV(a0),d4
.NextTexelEnd:
  dbf     d7,.NextTexel

; Interpolate next points
.Interpolate:
;  s3dm_texmap.xl += s3dm_texmap.dxdyl;
  move.l  STM_DXDYL(a0),d1
  add.l   d1,STM_XL(a0)
;  s3dm_texmap.zl += s3dm_texmap.dzdyl;
  move.l  STM_DZDYL(a0),d2
  add.l   d2,STM_ZL(a0)
;  s3dm_texmap.ul += s3dm_texmap.dudyl;
  move.l  STM_DUDYL(a0),d3
  add.l   d3,STM_UL(a0)
;  s3dm_texmap.vl += s3dm_texmap.dvdyl;
  move.l  STM_DVDYL(a0),d4
  add.l   d4,STM_VL(a0)
;  s3dm_texmap.xr += s3dm_texmap.dxdyr;
  move.l  STM_DXDYR(a0),d1
  add.l   d1,STM_XR(a0)
;  s3dm_texmap.zr += s3dm_texmap.dzdyr;
  move.l  STM_DZDYR(a0),d2
  add.l   d2,STM_ZR(a0)
;  s3dm_texmap.ur += s3dm_texmap.dudyr;
  move.l  STM_DUDYR(a0),d3
  add.l   d3,STM_UR(a0)
;  s3dm_texmap.vr += s3dm_texmap.dvdyr;
  move.l  STM_DVDYR(a0),d4
  add.l   d4,STM_VR(a0)

; Next line address
;  fb_line += s3dm_texmap.fb_bpr;
  adda.l  STM_FB_BPR(a0),a1
;  zb_line += s3dm_texmap.zb_bpr;
  adda.l  STM_ZB_BPR(a0),a2
.MapNextLineEnd:
  dbf     d0,.MapNextLine

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

;--------------------------------------
; Map a 16bits transparent texture
;
; @in a0.l texmap structure address
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_FastMap16BitsTransparent

_SAGE_FastMap16BitsTransparent:
  movem.l d1-d7/a0-a6,-(sp)

; Draw the line
.NextTexel:
; Write the texel
  move.l  d6,d4
  swap    d4
  ext.l   d4
  mulu.l  d1,d4                         ; (vi >> FIXP16_SHIFT) * texture_width
  move.l  d5,d3
  swap    d3
  ext.l   d3
  add.l   d3,d3                         ; 16bits
  add.l   d3,d4                         ; + ui >> FIXP16_SHIFT
  move.w  0(a0,d4.l),d3
;  cmp.w   CRD_TCOLOR(a3),d3
  beq.s   .Transparent
  move.w  d3,(a4)
.Transparent:
  adda.l  #2,a4
; Interpolate u & v
;  add.l   DELTA_DU(a2),d5
;  add.l   DELTA_DV(a2),d6
  dbf     d7,.NextTexel

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

  END
