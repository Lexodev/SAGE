/**
 * interrupt_interrupt.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test interruption functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include <proto/dos.h>

#include "/src/sage.h"

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
  
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library INTERRUPT test (HANDLER) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_INTERRUPTION)) {
    mydata.value = 0;
    count = 0;
    printf("Install IT handler\n");
    if (SAGE_AddInterruptionHandler(HANDLER_INDEX, MyHandler, (APTR) &mydata)) {
      printf("Count is %d and data value is %d\n", count, mydata.value);
      printf("Start our IT every one second\n");
      if (SAGE_StartInterruption(HANDLER_INDEX, SINT_ONE_SECOND)) {
        printf("Pause main prog for 4 seconds\n");
        Delay(50*4);
        printf("Now count is %d and data value is %d\n", count, mydata.value);
        printf("Stop our IT for 3 seconds\n");
        if (SAGE_StopInterruption(HANDLER_INDEX)) {
          Delay(50*3);
          printf("Again count is %d and data value is %d\n", count, mydata.value);
          printf("Restart our IT every 1/2 second\n");
          if (SAGE_StartInterruption(HANDLER_INDEX, (SINT_ONE_SECOND / 2))) {
            printf("Pause main prog for 4 seconds\n");
            Delay(50*4);
            printf("Finally count is %d and data value is %d\n", count, mydata.value);
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
  printf("End of test\n");
}
