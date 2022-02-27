/**
 * sage_thread.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Thread management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

#ifndef _SAGE_THREAD_H_
#define _SAGE_THREAD_H_

#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dostags.h>

#define STHD_MAX_THREAD       8

/** SAGE thread structure */
typedef struct {
  /** Thread ID */
  UWORD ident;
  /** Thread process */
  struct Process * process;
  /** Thread tasks */
  struct Task * task, * parent;
  /** Thread is running */
  BOOL running;
  /** Thread user function */
  LONG (*user_func)(APTR), result;
  /** Thread user data */
  APTR user_data;
} SAGE_Thread;

/** Create a thread */
SAGE_Thread * SAGE_CreateThread(LONG (*fn)(APTR), APTR);

/** Wait for a thread to finish */
VOID SAGE_WaitThread(SAGE_Thread *);

/** Kill a thread */
VOID SAGE_KillThread(SAGE_Thread *);

/** Remove a thread */
BOOL SAGE_RemoveThread(SAGE_Thread *);

/** Tell if thread has receive a break signal */
BOOL SAGE_BreakThread(VOID);

/** Dump a thread structure, for debug purpose only */
VOID SAGE_DumpThread(SAGE_Thread *);

#endif
