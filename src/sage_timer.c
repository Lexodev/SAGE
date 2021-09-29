/**
 * sage_timer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Timers management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_timer.h"

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/timer_protos.h>

/** @var Timer library */
struct Library * TimerBase = NULL;

/**
 * Allocate a timer
 * 
 * @return SAGE timer structure
 */
SAGE_Timer * SAGE_AllocTimer()
{
  SAGE_Timer * timer;

  timer = (SAGE_Timer *) SAGE_AllocMem(sizeof(SAGE_Timer));
  if (timer) {
    timer->timer_port = CreatePort(NULL, 0L);
    if (timer->timer_port != NULL ) {
      timer->io_request = (struct timerequest *) CreateExtIO(timer->timer_port, sizeof(struct timerequest));
      if (timer->io_request != NULL ) {
        if (!OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *) timer->io_request, 0L)) {
          if (TimerBase == NULL) {
            TimerBase = (struct Library *) timer->io_request->tr_node.io_Device;
          }
          return timer;
        } else {
          SAGE_SetError(SERR_OPENDEVICE);
        }
      } else {
        SAGE_SetError(SERR_NO_IOREQUEST);
      }
    } else {
      SAGE_SetError(SERR_NO_PORT);
    }
    SAGE_ReleaseTimer(timer);
  }
  return FALSE;
}

/**
 * Release a timer
 * 
 * @param timer SAGE timer pointer
 */
VOID SAGE_ReleaseTimer(SAGE_Timer * timer)
{
  if (timer != NULL) {
    if (timer->io_request != NULL) {
      CloseDevice((struct IORequest *) timer->io_request);
      DeleteExtIO((struct IORequest *) timer->io_request);
    }
    if (timer->timer_port != NULL) {
      DeletePort(timer->timer_port);
    }
    SAGE_FreeMem(timer);
  }
}

/**
 * Get the system time
 * 
 * @param timer SAGE timer pointer
 * 
 * @return Operation success
 */
BOOL SAGE_GetSysTime(SAGE_Timer * timer)
{
  struct timeval tv;

  if (timer == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  GetSysTime(&tv);
  timer->seconds = tv.tv_secs;
  timer->micro_seconds = tv.tv_micro;
  return TRUE;
}

/**
 * Return the elapsed time between two calls
 * 
 * @param timer SAGE timer pointer
 * 
 * @return Elapsed time (12 bits seconds, 20 bits micro)
 */
ULONG SAGE_ElapsedTime(SAGE_Timer * timer)
{
  struct timeval tv;

  if (timer == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return -1;
  }
  GetSysTime(&(timer->time_current));
  tv.tv_secs = timer->time_current.tv_secs;
  tv.tv_micro = timer->time_current.tv_micro;
  SubTime(&tv, &(timer->time_old));
  timer->time_old.tv_secs = timer->time_current.tv_secs;
  timer->time_old.tv_micro = timer->time_current.tv_micro;
  if (tv.tv_secs > 60) {
    return STIM_OVERFLOW;
  }
  return (tv.tv_secs << STIM_SECONDS_SHIFT | tv.tv_micro);
}

/**
 * Wait a certain amount of time
 * 
 * @param timer    SAGE timer pointer
 * @param duration Pause duration (12 bits seconds, 20 bits micro)
 * 
 * @return Operation success
 */
BOOL SAGE_Delay(SAGE_Timer * timer, ULONG duration)
{
  struct timeval tv;

  if (timer == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  tv.tv_secs = (duration >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK;
  tv.tv_micro = duration & STIM_MICRO_MASK;
  timer->io_request->tr_node.io_Command = TR_ADDREQUEST;
  timer->io_request->tr_time = tv;
  DoIO((struct IORequest *) timer->io_request);
  return TRUE;
}
