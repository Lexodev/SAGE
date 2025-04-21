/**
 * sage_keyboard.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Keyboard input management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#ifndef _SAGE_KEYBOARD_H_
#define _SAGE_KEYBOARD_H_

#include <sage/sage_frkey.h>
#include <sage/sage_enkey.h>
#include <sage/sage_dekey.h>

#define SINP_NB_KEY           128

/** Key sacn structure */
typedef struct {
  /** Key code */
  UWORD key_code;
  /** Key pressed flag */
  BOOL key_pressed;
} SAGE_KeyScan;

/** Scan keyboard and report keys status */
BOOL SAGE_ScanKeyboard(SAGE_KeyScan *, UBYTE);

/** Add a keyboard key handler */
BOOL SAGE_AddKeyboardHandler(UWORD, VOID (*handler)(BOOL));

/** Remove a keyboard key handler */
BOOL SAGE_RemoveKeyboardHandler(UWORD);

/** Clear all keyboard handlers */
BOOL SAGE_ClearKeyboardHandlers(VOID);

/** Prepare the keyboard handlers */
BOOL SAGE_InstallKeyboardHandlers(VOID);

#endif
