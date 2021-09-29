/**
 * sage_timer.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Timers management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_TIMER_H_
#define _SAGE_TIMER_H_

#include <exec/types.h>
#include <exec/io.h>
#include <devices/timer.h>

#define STIM_OVERFLOW         (61 << 12)
#define STIM_SECONDS_SHIFT    20
#define STIM_SECONDS_MASK     0xFFF
#define STIM_MICRO_MASK       0xFFFFF
#define STIM_TICKS            1000000L

/** SAGE timer structure */
typedef struct {
  /** Eladpsed time */
  struct timeval time_old, time_current;
  /** Device message port */
  struct MsgPort * timer_port;
  /** IO request */
  struct timerequest * io_request;
  /** System time */
  ULONG seconds, micro_seconds;
} SAGE_Timer;

/** Allocate a timer */
SAGE_Timer * SAGE_AllocTimer(VOID);

/** Release a timer */
VOID SAGE_ReleaseTimer(SAGE_Timer *);

/** Get the system time */
BOOL SAGE_GetSysTime(SAGE_Timer *);

/** Get the elapsed time between two calls */
ULONG SAGE_ElapsedTime(SAGE_Timer *);

/** Wait for a certain amount of time */
BOOL SAGE_Delay(SAGE_Timer *, ULONG);

#endif
