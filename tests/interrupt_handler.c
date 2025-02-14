/**
 * interrupt_interrupt.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test interruption functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define HANDLER_INDEX         1

ULONG count;

struct _my_data {
  ULONG value;
};

/**
 * this is our interrupt handler
 */
ASM INTERRUPT SAVEDS void MyHandler(
  REG(a5, APTR UserData)
)
{
  struct _my_data * data;
  
  count++;
  data = (struct _my_data *) UserData;
  data->value += 2;
  return;
}

void main(void)
{
  struct _my_data mydata;
  
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library INTERRUPT test (HANDLER) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_INTERRUPTION)) {
    mydata.value = 0;
    count = 0;
    SAGE_AppliLog("Install IT handler");
    if (SAGE_AddInterruptionHandler(HANDLER_INDEX, MyHandler, (APTR) &mydata)) {
      SAGE_AppliLog("Count is %d and data value is %d", count, mydata.value);
      SAGE_AppliLog("Start our IT every one second");
      if (SAGE_StartInterruption(HANDLER_INDEX, SINT_ONE_SECOND)) {
        SAGE_AppliLog("Pause main prog for 4 seconds");
        SAGE_Pause(50*4);
        SAGE_AppliLog("Now count is %d and data value is %d", count, mydata.value);
        SAGE_AppliLog("Stop our IT for 3 seconds");
        if (SAGE_StopInterruption(HANDLER_INDEX)) {
          SAGE_Pause(50*3);
          SAGE_AppliLog("Again count is %d and data value is %d", count, mydata.value);
          SAGE_AppliLog("Restart our IT every 1/2 second");
          if (SAGE_StartInterruption(HANDLER_INDEX, (SINT_ONE_SECOND / 2))) {
            SAGE_AppliLog("Pause main prog for 4 seconds");
            SAGE_Pause(50*4);
            SAGE_AppliLog("Finally count is %d and data value is %d", count, mydata.value);
          } else {
            SAGE_DisplayError();
          }
        }
      } else {
        SAGE_DisplayError();
      }
      SAGE_RemoveInterruptionHandler(HANDLER_INDEX);
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
