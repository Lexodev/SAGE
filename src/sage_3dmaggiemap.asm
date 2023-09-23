;--------------------------------------
; sage_maggiemap.asm
;
; SAGE (Simple Amiga Game Engine) project
; Maggie mapping functions
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 1.0 January 2022
;--------------------------------------

; Delta variables
DELTA_DXDYL         EQU 0*4
DELTA_DUDYL         EQU 1*4
DELTA_DVDYL         EQU 2*4
DELTA_DXDYR         EQU 3*4
DELTA_DUDYR         EQU 4*4
DELTA_DVDYR         EQU 5*4
DELTA_DU            EQU 6*4
DELTA_DV            EQU 7*4
DELTA_DZDYL         EQU 8*4
DELTA_DZDYR         EQU 9*4
DELTA_DZ            EQU 10*4

; Coordinate variables
CRD_XL              EQU 0*4
CRD_XR              EQU 1*4
CRD_UL              EQU 2*4
CRD_VL              EQU 3*4
CRD_UR              EQU 4*4
CRD_VR              EQU 5*4
CRD_LINE            EQU 6*4
CRD_LCLIP           EQU 7*4
CRD_RCLIP           EQU 8*4
CRD_TCOLOR          EQU 9*4
CRD_ZL              EQU 10*4
CRD_ZR              EQU 11*4

FIXP16_ROUND_UP     EQU $8000

; Registers base address
CUSTOM    = $DFF000
DMACONR   = $002              ; DMA control (and blitter status) read

; Maggie registers
TEXTPT    = $250              ; Texture adr
TEXDPT    = $254              ; Dest adr
TEXZPT    = $258              ; Z buffer adr
TEXSTRT   = $25E              ; Line len & start maggie
TEXMIP    = $260              ; Texture size
TEXMODE   = $262              ; Render mode
TEXDMODE  = $266              ; Dest modulo (texel size)
TEXUVAL   = $270              ; U value in fixed 16:16
TEXVVAL   = $274              ; V value in fixed 16:16
TEXDUVAL  = $278              ; dU value in fixed 16:16
TEXDVVAL  = $27C              ; dV value in fixed 16:16
TEXLIGHT  = $280              ; Ligth value in fixed 8:8
TEXDLIGHT = $282              ; Delta light value in fixed 8:8
TEXLCOLOR = $284              ; Light color in ARGB
TEXZVAL   = $288              ; Z value in fixed 16:16
TEXDZVAL  = $28C              ; dZ value in fixed 16:16

; Maggie mode
MAGGIE_BILINEAR     = 1       ; Bilinear filtering
MAGGIE_ZBUFFER      = 2       ; Activate Z-Buffer
MAGGIE_16BITS       = 4       ; Output in 16 bits (R5G6V5)

  SECTION maggiemap,code

;--------------------------------------
; Map a 16bits texture with Maggie
;
; @in d0.l number of lines to map
; @in a0.l texture address
; @in d1.w texture size
; @in a1.l bitmap address
; @in d2.l bitmap width
; @in a2.l deltas array
; @in a3.l coords array
; @in a4.l Z buffer
; @in d3.w render mode
; @in d4.w texel modulo
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_MaggieMap16BitsTexture

_SAGE_MaggieMap16BitsTexture:
  movem.l d1-d7/a0-a6,-(sp)
  tst.l   d0                            ; no lines ?
  ble     .EndDraw

  move.l  CRD_LINE(a3),d7
  lsl.l   #1,d7
  move.l  d7,CRD_LINE(a3)               ; 16bits

  lea     CUSTOM,a6

; Wait for Blitter to finish
  tst.b   DMACONR(a6)
.WaitBlitter:
  btst    #6,DMACONR(a6)
  bne.s   .WaitBlitter

  move.l  a0,TEXTPT(a6)                 ; The texture adr
  move.l  a4,TEXZPT(a6)                 ; The Z buffer adr
  move.w  d1,TEXMIP(a6)                 ; Texture size (9=512/8=256/7=128/6=64)
  move.w  d3,TEXMODE(a6)                ; Maggie mode (16bits output)
  move.w  d4,TEXDMODE(a6)               ; Texel size in byte (16bits = 2 bytes)
  move.w  #$ffff,TEXLIGHT(a6)           ; Light value in fixed 8:8
  move.w  #0,TEXDLIGHT(a6)              ; Delta ligth in fixed 8:8
  move.l  #$ffffff,TEXLCOLOR(a6)        ; Light color in ARGB

  subq.l  #1,d0
.MapNextLine:

; Calcul edge coords
;  xs = (s3dm_coords[CRD_XL] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
  move.l  CRD_XL(a3),d3
  addi.l  #FIXP16_ROUND_UP,d3
  swap    d3
  ext.l   d3                            ; xs
;  xe = (s3dm_coords[CRD_XR] + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
  move.l  CRD_XR(a3),d4
  addi.l  #FIXP16_ROUND_UP,d4
  swap    d4
  ext.l   d4                            ; xe

; Check for left/right clipping
;  if (xs < s3dm_coords[CRD_RCLIP] && xe >= s3dm_coords[CRD_LCLIP]) {
.PointsOnScreen:
  cmp.l   CRD_RCLIP(a3),d3
  bge    .Interpolate
  cmp.l   CRD_LCLIP(a3),d4
  blt    .Interpolate

; Calcul texture interpolation
;  du = s3dm_coords[CRD_UR] - s3dm_coords[CRD_UL];
  move.l  CRD_UR(a3),d5
  sub.l   CRD_UL(a3),d5
;  dv = s3dm_coords[CRD_VR] - s3dm_coords[CRD_VL];
  move.l  CRD_VR(a3),d6
  sub.l   CRD_VL(a3),d6
;  dz = s3dm_coords[CRD_ZR] - s3dm_coords[CRD_ZL];
  move.l  CRD_ZR(a3),d1
  sub.l   CRD_ZL(a3),d1
;  dx = xe - xs;
  move.l  d4,d7
  sub.l   d3,d7                         ; dx
;  if (dx > 0) {
  ble.s   .DxNegative
;    du /= dx;
  divs.l  d7,d5                         ; du
;    dv /= dx;
  divs.l  d7,d6                         ; dv
;    dz /= dx;
  divs.l  d7,d1                         ; dz
;  }
.DxNegative:
  move.l  d5,DELTA_DU(a2)
  move.l  d6,DELTA_DV(a2)
  move.l  d1,DELTA_DZ(a2)

; Calcul texture coords
;  ui = s3dm_coords[CRD_UL] + FIXP16_ROUND_UP;
  move.l  CRD_UL(a3),d5                 ; ui
  addi.l  #FIXP16_ROUND_UP,d5
;  vi = s3dm_coords[CRD_VL] + FIXP16_ROUND_UP;
  move.l  CRD_VL(a3),d6                 ; vi
  addi.l  #FIXP16_ROUND_UP,d6
;  zi = s3dm_coords[CRD_ZL] + FIXP16_ROUND_UP;
  move.l  CRD_ZL(a3),d1                 ; zi
  addi.l  #FIXP16_ROUND_UP,d1

; Horizontal clipping
;  if (xs < s3dm_coords[CRD_LCLIP]) {
  cmp.l   CRD_LCLIP(a3),d3
  bge.s   .NoLeftClip
;    dx = s3dm_coords[CRD_LCLIP] - xs;
  move.l  CRD_LCLIP(a3),d7
  sub.l   d3,d7
;    ui += dx * du;
  move.l  DELTA_DU(a2),d3
  muls.l  d7,d3
  add.l   d3,d5
;    vi += dx * dv;
  move.l  DELTA_DV(a2),d3
  muls.l  d7,d3
  add.l   d3,d6
;    zi += dx * dz;
  move.l  DELTA_DZ(a2),d3
  muls.l  d7,d3
  add.l   d3,d1
;    xs = s3dm_coords[CRD_LCLIP];
  move.l  CRD_LCLIP(a3),d3
;    dx = xe - xs;
  move.l  d4,d7
  sub.l   d3,d7
;  }
.NoLeftClip:
;  if (xe >= s3dm_coords[CRD_RCLIP]) {
  cmp.l   CRD_RCLIP(a3),d4
  blt.s   .NoRightClip
;    dx = (s3dm_coords[CRD_RCLIP] - 1) - xs;
  move.l  CRD_RCLIP(a3),d7
  subq.l  #1,d7
  sub.l   d3,d7
;  }
.NoRightClip:

; Calcul start address
;  screen_pixel = s3dm_coords[CRD_LINE] + xs;
  movea.l a1,a4
  adda.l  CRD_LINE(a3),a4
  lsl.l   #1,d3                         ; 16bits
  adda.l  d3,a4                         ; start address

; Wait for Maggie to finish the job
  tst.b   DMACONR(a6)
.WaitMaggie:
  btst    #6,DMACONR(a6)
  bne.s   .WaitMaggie

; Draw the line
;  dx++;    // Real number of points to draw
;  while (dx--) {
; Write the texel
;    texture_pixel = (ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_width);
;    screen_buffer[screen_pixel++] = texture[texture_pixel];
; Interpolate u, v & z
;    ui += du;
;    vi += dv;
;    zi += dz
;  }
  addq.l  #1,d7
  move.l  a4,TEXDPT(a6)                 ; Start adr of the destination
  move.l  CRD_UL(a3),TEXUVAL(a6)        ; U value in fixed 16:16
  move.l  CRD_VL(a3),TEXVVAL(a6)        ; V value in fixed 16:16
  move.l  DELTA_DU(a2),TEXDUVAL(a6)     ; Delta U in fixed 16:16
  move.l  DELTA_DV(a2),TEXDVVAL(a6)     ; Delta V in fixed 16:16
  move.l  CRD_ZL(a3),TEXZVAL(a6)        ; Z value in fixed 16:16
  move.l  DELTA_DZ(a2),TEXDZVAL(a6)     ; Delta Z in fixed 16:16
  move.w  d7,TEXSTRT(a6)                ; Line size & start Maggie

;  }

; Interpolate next points
.Interpolate:
;  s3dm_coords[CRD_XL] += s3dm_deltas[DELTA_DXDYL];
  move.l  DELTA_DXDYL(a2),d3
  add.l   d3,CRD_XL(a3)
;  s3dm_coords[CRD_ZL] += s3dm_deltas[DELTA_DZDYL];
  move.l  DELTA_DZDYL(a2),d1
  add.l   d1,CRD_ZL(a3)
;  s3dm_coords[CRD_UL] += s3dm_deltas[DELTA_DUDYL];
  move.l  DELTA_DUDYL(a2),d4
  add.l   d4,CRD_UL(a3)
;  s3dm_coords[CRD_VL] += s3dm_deltas[DELTA_DVDYL];
  move.l  DELTA_DVDYL(a2),d5
  add.l   d5,CRD_VL(a3)
;  s3dm_coords[CRD_XR] += s3dm_deltas[DELTA_DXDYR];
  move.l  DELTA_DXDYR(a2),d3
  add.l   d3,CRD_XR(a3)
;  s3dm_coords[CRD_ZR] += s3dm_deltas[DELTA_DZDYR];
  move.l  DELTA_DZDYR(a2),d1
  add.l   d1,CRD_ZR(a3)
;  s3dm_coords[CRD_UR] += s3dm_deltas[DELTA_DUDYR];
  move.l  DELTA_DUDYR(a2),d4
  add.l   d4,CRD_UR(a3)
;  s3dm_coords[CRD_VR] += s3dm_deltas[DELTA_DVDYR];
  move.l  DELTA_DVDYR(a2),d5
  add.l   d5,CRD_VR(a3)

; Next line address
;  s3dm_coords[CRD_LINE] += screen_width;
  add.l   d2,CRD_LINE(a3)

  dbf     d0,.MapNextLine

.EndDraw:
  movem.l (sp)+,d1-d7/a0-a6
  move.l  #-1,d0
  rts

  END
