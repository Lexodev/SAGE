/**
 * sage_event.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Event container management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_EVENT_H_
#define _SAGE_EVENT_H_

#include <exec/types.h>
#include <intuition/intuitionbase.h>

#include "sage_frkey.h"
#include "sage_enkey.h"
#include "sage_dekey.h"

#define SIGMASK(w) (1L<<((w)->UserPort->mp_SigBit))
#define GETIMSG(w) ((struct IntuiMessage *)GetMsg((w)->UserPort))

#define SEVT_UNKNOWN          0
#define SEVT_RAWKEY           1
#define SEVT_KEYDOWN          1
#define SEVT_KEYUP            2
#define SEVT_MOUSEBT          3
#define SEVT_MOUSEMV          4
#define SEVT_JOYSTICK         5
#define SEVT_PADDLE           6

#define SMBT_LMBUP            SELECTUP
#define SMBT_LMBDOWN          SELECTDOWN
#define SMBT_RMBUP            MENUUP
#define SMBT_RMBDOWN          MENUDOWN

/** SAGE event */
typedef struct {
  /** Event type */
  UWORD type;
  /** Event code */
  UWORD code;
  /** Mouse coords */
  WORD mousex, mousey;
} SAGE_Event;

/** Allocate an event structure */
SAGE_Event * SAGE_AllocEvent(VOID);

/** Release an event structure */
VOID SAGE_ReleaseEvent(SAGE_Event *);

#endif
