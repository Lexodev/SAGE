;--------------------------------------
; sage_vblint.asm
;
; SAGE (Small Amiga Game Engine) project
; VBL interrupt management
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 25.1 February 2025 (updated: 26/02/2025)
;--------------------------------------

_AddIntServer       EQU -$A8
_RemIntServer       EQU -$AE
INTB_VERTB          EQU 5
CUSTOM              EQU $DFF000

  SECTION vblint,code

;--------------------------------------
; Install the VBL interrupt handler
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_InstallVblInterrupt
  
_SAGE_InstallVblInterrupt:
  move.l  $4.w,a6
  moveq.l #INTB_VERTB,d0
  lea     _SAGE_VBLServer,a1
  jsr     _AddIntServer(a6)
  rts

;--------------------------------------
; Remove the VBL interrupt handler
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_RemoveVblInterrupt

_SAGE_RemoveVblInterrupt:
  move.l  $4.w,a6
  moveq.l #INTB_VERTB,d0
  lea     _SAGE_VBLServer,a1
  cmpi.l  #0,(a1)
  beq.s   .NoVblInstalled
  jsr     _RemIntServer(a6)
.NoVblInstalled:
  rts

;--------------------------------------
; Get the elapsed VLB
;
; @out d0.l number of VBL since last reset
;--------------------------------------
  xdef _SAGE_GetVblCount

_SAGE_GetVblCount:
  move.l  _SAGE_VblCount,d0
  rts

;--------------------------------------
; Reset the VBL counter
;--------------------------------------
  xdef _SAGE_ResetVblCount

_SAGE_ResetVblCount:
  move.l  #0,_SAGE_VblCount
  rts

;--------------------------------------
; Wait for vertical blank
;--------------------------------------
  xdef _SAGE_WaitVbl

_SAGE_WaitVbl:
  move.w  #0,_SAGE_VblFlag
.JustWait:
  tst.w   _SAGE_VblFlag
  beq.s   .JustWait
  rts

;--------------------------------------
; The VBL interrupt handler
;--------------------------------------
_SAGE_VblInterrupt:
  movem.l d0-d7/a0-a6,-(sp)
  addq.l  #1,_SAGE_VblCount
  move.w  #-1,_SAGE_VblFlag
  movem.l (sp)+,d0-d7/a0-a6
  move.l  #CUSTOM,a0              ; Mandatory setting if prio >= 10
  moveq.l #0,d0
  rts

  SECTION vblintdata,data

_SAGE_VblCount:
  dc.l    0                       ; Count the VBL

_SAGE_VblFlag:
  dc.w    0                       ; Flag for VBL Wait

_SAGE_VBLServer:
  dc.l    0,0                     ; ln_Succ, ln_Pred
  dc.b    2,42                    ; ln_Type, ln_Pri
  dc.l    _SAGE_VblItName         ; ln_Name
  dc.l    0,_SAGE_VblInterrupt    ; is_Data, is_Code

_SAGE_VblItName:
  dc.b    'SAGE VBL IT',0         ; Name of IT server

  END
