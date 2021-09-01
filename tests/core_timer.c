/**
 * core_timer.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test timer functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

void main(void)
{
  ULONG elapsed_time, count, calcul;
  SAGE_Timer * timer1 = NULL, * timer2 = NULL;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library CORE test (TIMER) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NONE)) {
    if ((timer1 = SAGE_AllocTimer()) != NULL && (timer2 = SAGE_AllocTimer()) != NULL) {
      printf("Getting current time\n");
      if (SAGE_GetSysTime(timer1)) {
        printf("Time is : %d seconds, %d micro seconds on timer 1\n", timer1->seconds, timer1->micro_seconds);
      }
      if (SAGE_GetSysTime(timer2)) {
        printf("Time is : %d seconds, %d micro seconds on timer 2\n", timer2->seconds, timer2->micro_seconds);
      }
      printf("Initialize elapsed time\n");
      elapsed_time = SAGE_ElapsedTime(timer1);
      elapsed_time = SAGE_ElapsedTime(timer2);
      for (count = 0;count < 5000;count++) {
        calcul = 55 * count / 3;
      }
      elapsed_time = SAGE_ElapsedTime(timer1);
      printf("Elapsed time is %d (%d.%d) on timer 1\n", elapsed_time, (elapsed_time >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK, elapsed_time & STIM_MICRO_MASK);
      for (count = 0;count < 50000;count++) {
        calcul = count * 42 / 3;
      }
      elapsed_time = SAGE_ElapsedTime(timer2);
      printf("Elapsed time is %d (%d.%d) on timer 2\n", elapsed_time, (elapsed_time >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK, elapsed_time & STIM_MICRO_MASK);
      printf("Pause 1.8 seconds\n");
      SAGE_Delay(timer1, (1L << STIM_SECONDS_SHIFT) + 800000L);
      elapsed_time = SAGE_ElapsedTime(timer1);
      printf("Elapsed time is %d (%d.%d) on timer 1\n", elapsed_time, (elapsed_time >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK, elapsed_time & STIM_MICRO_MASK);
    } else {
      SAGE_DisplayError();
    }
    SAGE_ReleaseTimer(timer2);
    SAGE_ReleaseTimer(timer1);
  }
  SAGE_Exit();
  printf("End of test\n");
}
