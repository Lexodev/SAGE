/**
 * sage_interrupt.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Interruptions management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <exec/exec.h>
#include <libraries/lowlevel.h>

#include <sage/sage_debug.h>
#include <sage/sage_memory.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_context.h>
#include <sage/sage_interrupt.h>

#include <proto/exec.h>
#include <proto/lowlevel.h>

#define LOWLEVELVERSION       0L

/** @var Timer interruption handle */
APTR TimerIntHandle = NULL;

/** @var Lowlevel library */
extern struct Library *LowLevelBase;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Initialize the interruption module
 * and install the interruption server
 *
 * @return Operation success
 */
BOOL SAGE_InitInterruptionModule()
{
  UWORD index;

  SD(SAGE_DebugLog("Init Interruption module");)
  if (LowLevelBase == NULL) {
    if ((LowLevelBase = OpenLibrary("lowlevel.library", LOWLEVELVERSION)) == NULL) {
      SAGE_SetError(SERR_LOWLEVEL_LIB);
      return FALSE;
    }
  }
  // Init the interruption list
  for (index = 0;index < SINT_MAX_INTERRUPT;index++) {
    SageContext.Interruptions[index].used = FALSE;
  }
  // Install the interruption server
  if ((TimerIntHandle = AddTimerInt(SAGE_ITServer, &SageContext.Interruptions)) != NULL) {
    // 50000 microseconds = 0.05 seconds (x20 for 1 second)
    StartTimerInt(TimerIntHandle, 50000, TRUE);
  }
  return TRUE;
}

/**
 * Release the interruption module
 *
 * @return Operation success
 */
BOOL SAGE_ReleaseInterruptionModule()
{
  SD(SAGE_DebugLog("Release Interruption module");)
  SAGE_StopAllInterruptions();
  if (TimerIntHandle != NULL) {
    StopTimerInt(TimerIntHandle);
    RemTimerInt(TimerIntHandle);
    TimerIntHandle = NULL;
  }
  if (LowLevelBase != NULL) {
    CloseLibrary(LowLevelBase);
    LowLevelBase = NULL;
  }
  return TRUE;
}

/**
 * Add an interruption handler to the interruption list
 *
 * @param index   Interruption index
 * @param handler Interruption handler pointer
 * @param data    Interruption data pointer
 *
 * @return Operation success
 */
BOOL SAGE_AddInterruptionHandler(UWORD index, VOID ASM (*handler)(REG(a5, APTR)), APTR data)
{
  if (!SageContext.Interruptions[index].used) {
    SageContext.Interruptions[index].active = FALSE;
    SageContext.Interruptions[index].ticks = 1;
    SageContext.Interruptions[index].count = 0;
    SageContext.Interruptions[index].data = data;
    SageContext.Interruptions[index].handler = handler;
    SageContext.Interruptions[index].used = TRUE;
    SD(SAGE_DebugLog("Interruption handler added");)
    return TRUE;
  }
  SAGE_SetError(SERR_IT_USED);
  return FALSE;
}

/**
 * Remove an interruption handler from the interruption list
 *
 * @param index Interruption index
 *
 * @return Operation success
 */
BOOL SAGE_RemoveInterruptionHandler(UWORD index)
{
  if (index < SINT_MAX_INTERRUPT) {
    SageContext.Interruptions[index].active = FALSE;
    SageContext.Interruptions[index].used = FALSE;
    SageContext.Interruptions[index].ticks = 1;
    SageContext.Interruptions[index].count = 0;
    SageContext.Interruptions[index].data = NULL;
    SageContext.Interruptions[index].handler = NULL;
    SD(SAGE_DebugLog("Interruption handler removed");)
    return TRUE;
  }
  SAGE_SetError(SERR_IT_INDEX);
  return FALSE;
}

/**
 * Start an interruption
 *
 * @param index Interruption index
 * @param ticks Interruption interval (1 tick = 0,05s)
 *
 * @return Operation success
 */
BOOL SAGE_StartInterruption(UWORD index, UWORD ticks)
{
  if (index < SINT_MAX_INTERRUPT && ticks > 0) {
    SageContext.Interruptions[index].ticks = ticks;
    SageContext.Interruptions[index].count = 0;
    SageContext.Interruptions[index].active = TRUE;
    SD(SAGE_DebugLog("Interruption %d started", index);)
    return TRUE;
  }
  SAGE_SetError(SERR_IT_INDEX);
  return FALSE;
}

/**
 * Stop an interruption
 *
 * @param index Interruption index
 *
 * @return Operation success
 */
BOOL SAGE_StopInterruption(UWORD index)
{
  if (index < SINT_MAX_INTERRUPT) {
    SageContext.Interruptions[index].active = FALSE;
    SageContext.Interruptions[index].ticks = 1;
    SageContext.Interruptions[index].count = 0;
    SD(SAGE_DebugLog("Interruption %d stopped", index);)
    return TRUE;
  }
  SAGE_SetError(SERR_IT_INDEX);
  return FALSE;
}

/**
 * Stop all interruptions
 *
 * @return Operation success
 */
BOOL SAGE_StopAllInterruptions()
{
  UWORD index;
  
  for (index = 0;index < SINT_MAX_INTERRUPT;index++) {
    SAGE_StopInterruption(index);
  }
  return TRUE;
}
