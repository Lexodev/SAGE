;--------------------------------------
; sage_itserver.asm
;
; SAGE (Small Amiga Game Engine) project
; Interruption server
; 
; @author Fabrice Labrador <fabrice.labrador@gmail.com>
; @version 1.0 April 2020
;--------------------------------------

SINT_INTERRUPTS     EQU 8

; SAGE_Interruption struct
SINT_USED           EQU 0
SINT_ACTIVE         EQU 2
SINT_TICKS          EQU 4
SINT_COUNT          EQU 6
SINT_DATA           EQU 8
SINT_FUNCTION       EQU 12
SINT_SIZE           EQU 16

  SECTION itserver,code

;--------------------------------------
; Interruption server
;
; @in a1.l interruption handlers array
;
; @out d0.l operation success
;--------------------------------------
  xdef _SAGE_ITServer
  
_SAGE_ITServer:
  movem.l d2-d7/a2-a4,-(sp)
  move.w  #SINT_INTERRUPTS-1,d7         ; interruption loop
.NextInterruption:
  tst.w   SINT_USED(a1)                 ; used flag
  beq.s   .NotUsed
  tst.w   SINT_ACTIVE(a1)               ; active flag
  beq.s   .NotActive
  move.w  SINT_COUNT(a1),d1             ; ticks count
  bne.s   .NotRun
  move.w  SINT_TICKS(a1),d1             ; reset ticks
  move.l  SINT_DATA(a1),a5              ; function data
  move.l  SINT_FUNCTION(a1),a6          ; interruption function
  jsr     (a6)
.NotRun:
  subq.w  #1,d1
  move.w  d1,SINT_COUNT(a1)             ; save ticks count
.NotActive:
.NotUsed:
  adda.l  #SINT_SIZE,a1                 ; next interruption params
  dbf     d7,.NextInterruption
.InterruptError:
  movem.l (sp)+,d2-d7/a2-a4
  moveq.l #0,d0
  rts

  END
