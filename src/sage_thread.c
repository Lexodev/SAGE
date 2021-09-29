/**
 * sage_thread.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Thread management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

/**
 * Actually you can't use console output in a thread
 * i should investigate to know why
 */
 
#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_thread.h"

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

#include <stdlib.h>
#include <stdio.h>

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Add a thread to the thread pool
 *
 * @param thread Thread pointer
 *
 * @return Operation success
 */
BOOL SAGE_AddThread(SAGE_Thread * thread)
{
  UWORD index;
  
  for (index = 0;index < STHD_MAX_THREAD;index++) {
    if (SageContext.Threads[index] == NULL) {
      SageContext.Threads[index] = thread;
      thread->ident = index;
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Run the thread
 *
 * @param args Thread data
 *
 * @return Operation success
 */
__saveds __asm BOOL SAGE_RunThread(register __a0 STRPTR args)
{
  SAGE_Thread * thread;
  LONG (*user_func)(APTR);
  APTR user_data;
  
  // OK, let me explain this "trick", CreateNewProc only support
  // shell like arguments, so if we have to pass a structure we should
  // pass the structure address as a standard shell argument, a string,
  // then we just convert back the argument string to a memory address,
  // and that's it
  thread = (SAGE_Thread *)atol(args);
  thread->task = FindTask(NULL);
  if (thread->task == NULL) {
    return FALSE;
  }
  user_func = thread->user_func;
  user_data = thread->user_data;
  thread->running = TRUE;
  // Tell parent that thread has start
  Signal(thread->parent, SIGBREAKF_CTRL_E);
  // Run the user code
  thread->result = user_func(user_data);
  thread->running = FALSE;
  // Tell parent that thread has stop
  Signal(thread->parent, SIGBREAKF_CTRL_F);
  return TRUE;
}

/**
 * Create a thread and add it to the pool
 *
 * @param user_func User function
 * @param user_data User data
 *
 * @return Thread pointer or NULL
 */
SAGE_Thread * SAGE_CreateThread(LONG (*user_func)(APTR), APTR user_data)
{
  SAGE_Thread * thread;
  BYTE thread_address[20];
  
  thread = (SAGE_Thread *) SAGE_AllocMem(sizeof(SAGE_Thread));
  if (thread != NULL) {
    // Add the thread to the thread pool
    if (!SAGE_AddThread(thread)) {
      SAGE_SetError(SERR_NO_THREAD);
      SAGE_FreeMem(thread);
      return NULL;
    }
    thread->running = FALSE;
    thread->parent = FindTask(NULL);
    if (thread->parent == NULL) {
      SAGE_SetError(SERR_FINDTASK);
      SAGE_FreeMem(thread);
      return NULL;
    }
    thread->user_func = user_func;
    thread->user_data = user_data;
    // Do the trick for passing thread structure address to the new process
    sprintf(thread_address, "%ld", thread);
    thread->process = CreateNewProcTags(
      NP_Output, Output(),
      NP_Input, Input(),
      NP_Name, "SAGE Thread",
      NP_CloseOutput, FALSE,
      NP_CloseInput, FALSE, 
      NP_StackSize, 64000,
      NP_Entry, SAGE_RunThread,
      NP_Arguments, (ULONG)thread_address,
      TAG_DONE
    );
    if (thread->process == NULL) {
      SAGE_RemoveThread(thread);
      SAGE_SetError(SERR_NEWPROCESS);
      return NULL;
    }
    /** Wait for thread to really start */
    Wait(SIGBREAKF_CTRL_E);
  }
  return thread;
}

/**
 * Wait for a thread to finish
 *
 * @param thread Thread pointer
 */
VOID SAGE_WaitThread(SAGE_Thread * thread)
{
  if (thread != NULL) {
    while (thread->running) {
      SetSignal(0L, SIGBREAKF_CTRL_F|SIGBREAKF_CTRL_C);
      Wait(SIGBREAKF_CTRL_F|SIGBREAKF_CTRL_C);
    }
  }
}

/**
 * Kill a thread
 */
VOID SAGE_KillThread(SAGE_Thread * thread)
{
  if (thread != NULL && thread->task != NULL) {
    Signal(thread->task, SIGBREAKF_CTRL_C);
  }
}

/**
 * Tell if thread has receive a break signal
 *
 * @return Thread has receive break signal
 */
BOOL SAGE_BreakThread(VOID)
{
  if (SetSignal(0L, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Remove a thread from the pool
 *
 * @param thread Thread pointer
 */
BOOL SAGE_RemoveThread(SAGE_Thread * thread)
{
  if (thread == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (thread->running) {
    SAGE_KillThread(thread);
  }
  Delay(5);
  if (thread->running) {
    SAGE_ErrorLog("Thread can't be stopped or removed");
    return FALSE;
  }
  SageContext.Threads[thread->ident] = NULL;
  SAGE_FreeMem(thread);
  return TRUE;
}

/**
 * Dump a thread
 *
 * @param thread Thread pointer
 */
VOID SAGE_DumpThread(SAGE_Thread * thread)
{
  if (thread != NULL) {
    SAGE_DebugLog("Dumping SAGE Thread at 0x%X", thread);
    SAGE_DebugLog(" Identifier       %d", thread->ident);
    SAGE_DebugLog(" Process          0x%X", thread->process);
    SAGE_DebugLog(" Task             0x%X", thread->task);
    SAGE_DebugLog(" Parent           0x%X", thread->parent);
    SAGE_DebugLog(" Running          %d", thread->running);
    SAGE_DebugLog(" User function    0x%X", thread->user_func);
    SAGE_DebugLog(" User data        0x%X", thread->user_data);
  }
}
