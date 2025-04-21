/**
 * core_thread.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test thread functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

struct Crd {
  int x, y;
};

LONG my_thread(APTR data)
{
  struct Crd *crd;
  
  crd = (struct Crd *) data;
  crd->x = 42;
  crd->y = 666;
  SAGE_Pause(150);
  return 1234;
}

LONG other_thread(APTR data)
{
  while (!SAGE_BreakThread()) {
    // Never ending story
    SAGE_Pause(50);
  }
  SAGE_AppliLog("Doh ! you killed me !");
  return 0;
}

void main(void)
{
  SAGE_Thread *thread = NULL;
  struct Crd crd;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (THREAD) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    if ((thread = SAGE_CreateThread(my_thread, &crd)) != NULL) {
      SAGE_AppliLog("Thread is running, wait till it finish");
      SAGE_WaitThread(thread);
      SAGE_AppliLog("Result %d is %d, %d", thread->result, crd.x, crd.y);
    } else {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Let's try with another thread");
    if ((thread = SAGE_CreateThread(other_thread, &crd)) != NULL) {
      SAGE_AppliLog("Thread is running, wait a bit then kill it");
      SAGE_Pause(150);
      SAGE_AppliLog("Kill the thread");
      SAGE_KillThread(thread);
    } else {
      SAGE_DisplayError();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
