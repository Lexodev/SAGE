/**
 * core_thread.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test thread functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

#include <stdio.h>

#include "/src/sage.h"

struct Crd {
  int x, y;
};

LONG my_thread(APTR data)
{
  struct Crd * crd;
  
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
  printf("Doh ! you killed me !\n");
  return 0;
}

void main(void)
{
  SAGE_Thread * thread = NULL;
  struct Crd crd;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library CORE test (THREAD) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NONE)) {
    if ((thread = SAGE_CreateThread(my_thread, &crd)) != NULL) {
      printf("Thread is running, wait till it finish\n");
      SAGE_WaitThread(thread);
      printf("Result %d is %d, %d\n", thread->result, crd.x, crd.y);
    } else {
      SAGE_DisplayError();
    }
    printf("Let's try with another thread\n");
    if ((thread = SAGE_CreateThread(other_thread, &crd)) != NULL) {
      printf("Thread is running, wait a bit then kill it\n");
      SAGE_Pause(150);
      printf("Kill the thread\n");
      SAGE_KillThread(thread);
    } else {
      SAGE_DisplayError();
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
