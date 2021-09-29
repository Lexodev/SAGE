/**
 * sage_event.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Event container management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdlib.h>

#include "sage_debug.h"
#include "sage_memory.h"
#include "sage_event.h"

#include <proto/exec.h>
#include <proto/intuition.h>

/**
 * Allocate an event struture
 * 
 * @return SAGE event structure
 */
SAGE_Event * SAGE_AllocEvent()
{
  return (SAGE_Event *) SAGE_AllocMem(sizeof(SAGE_Event));
}

/**
 * Release an event
 *
 * @param event SAGE Event structure pointer
 */
VOID SAGE_ReleaseEvent(SAGE_Event * event)
{
  if (event != NULL) {
    SAGE_FreeMem(event);
  }
}
