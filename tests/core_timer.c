/**
 * core_timer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test timer functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

void main(void)
{
  ULONG elapsed_time, count, calcul;
  SAGE_Timer *timer1 = NULL, *timer2 = NULL;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (TIMER) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    if ((timer1 = SAGE_AllocTimer()) != NULL && (timer2 = SAGE_AllocTimer()) != NULL) {
      SAGE_AppliLog("Getting current time");
      if (SAGE_GetSysTime(timer1)) {
        SAGE_AppliLog("Time is : %d seconds, %d micro seconds on timer 1", timer1->seconds, timer1->micro_seconds);
      }
      if (SAGE_GetSysTime(timer2)) {
        SAGE_AppliLog("Time is : %d seconds, %d micro seconds on timer 2", timer2->seconds, timer2->micro_seconds);
      }
      SAGE_AppliLog("Initialize elapsed time");
      elapsed_time = SAGE_ElapsedTime(timer1);
      elapsed_time = SAGE_ElapsedTime(timer2);
      for (count = 0;count < 5000;count++) {
        calcul = 55 * count / 3;
      }
      elapsed_time = SAGE_ElapsedTime(timer1);
      SAGE_AppliLog("Elapsed time is %d (%d.%d) on timer 1", elapsed_time, (elapsed_time >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK, elapsed_time & STIM_MICRO_MASK);
      for (count = 0;count < 50000;count++) {
        calcul = count * 42 / 3;
      }
      elapsed_time = SAGE_ElapsedTime(timer2);
      SAGE_AppliLog("Elapsed time is %d (%d.%d) on timer 2", elapsed_time, (elapsed_time >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK, elapsed_time & STIM_MICRO_MASK);
      SAGE_AppliLog("Pause 1.8 seconds");
      SAGE_Delay(timer1, (1L << STIM_SECONDS_SHIFT) + 800000L);
      elapsed_time = SAGE_ElapsedTime(timer1);
      SAGE_AppliLog("Elapsed time is %d (%d.%d) on timer 1", elapsed_time, (elapsed_time >> STIM_SECONDS_SHIFT) & STIM_SECONDS_MASK, elapsed_time & STIM_MICRO_MASK);
    } else {
      SAGE_DisplayError();
    }
    SAGE_ReleaseTimer(timer2);
    SAGE_ReleaseTimer(timer1);
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
