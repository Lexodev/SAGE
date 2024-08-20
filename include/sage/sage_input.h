/**
 * sage_input.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Input devices management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#ifndef _SAGE_INPUT_H_
#define _SAGE_INPUT_H_

#include <sage/sage_keyboard.h>
#include <sage/sage_joyport.h>

/** SAGE input structure */
typedef struct {
  /** Keyboard handlers */
  VOID (*keyboard_handlers[SINP_NB_KEY])(BOOL);
  /** Keyboard scan array */
  SAGE_KeyScan key_scan[SINP_NB_KEY];
  /** Active handlers */
  UWORD nb_handlers;
  /** Joyport handlers */
  VOID (*joyport_handler[SINP_NB_JOYPORT])(SAGE_PortScan *);
  /** Active joyport */
  UWORD active_joyport;
} SAGE_InputDevice;

/** Init the input module */
BOOL SAGE_InitInputModule(VOID);

/** Release the input module */
BOOL SAGE_ReleaseInputModule(VOID);

/** Allocate the input device */
BOOL SAGE_AllocInputDevice(VOID);

/** Free the input device */
BOOL SAGE_FreeInputDevice(VOID);

/** Call the input handlers */
BOOL SAGE_HandleInputEvents(VOID);

#endif
