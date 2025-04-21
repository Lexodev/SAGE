/**
 * sage_interrupt.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Interruptions management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#ifndef _SAGE_INTERRUPT_H_
#define _SAGE_INTERRUPT_H_

#include <exec/types.h>

#include <sage/sage_compiler.h>

#define SINT_MAX_INTERRUPT    8

#define SINT_ONE_SECOND       20

/** SAGE interruption structure */
typedef struct {
  /** Interruption used and active */
  BOOL used, active;
  /** Interruption frequency */
  UWORD ticks, count;
  /** User data */
  APTR data;
  /** Interruption handler */
  VOID ASM (*handler)(REG(a5, APTR));
} SAGE_Interruption;

/** Init the interruption module */
BOOL SAGE_InitInterruptionModule(VOID);

/** Release the interruption module */
BOOL SAGE_ReleaseInterruptionModule(VOID);

/** Add an intetruption handler to the interruptions list */
BOOL SAGE_AddInterruptionHandler(UWORD, VOID ASM (*f)(REG(a5, APTR)), APTR);

/** Remove an interruption handler */
BOOL SAGE_RemoveInterruptionHandler(UWORD);

/** Start an interruption */
BOOL SAGE_StartInterruption(UWORD, UWORD);

/** Stop an interruption */
BOOL SAGE_StopInterruption(UWORD);

/** Stop all interruptions */
BOOL SAGE_StopAllInterruptions(VOID);

/** External function for interruption management */
extern VOID ASM SAGE_ITServer(VOID);

#endif
