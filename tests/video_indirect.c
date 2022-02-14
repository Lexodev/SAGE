/**
 * video_indirect.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test indirect bitmap
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

void main(void)
{
  SAGE_Event * event = NULL;
  BOOL finish;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (INDIRECT BITMAP) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES|SSCR_INDIRECT)) {
      SAGE_SetTextColor(0, 1);
      finish = FALSE;
      while (!finish) {

        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_MOUSEBT) {
            printf("Exit loop\n");
            finish = TRUE;
          }
        }

        SAGE_PrintDirectText("Press left mouse button to quit", 10, 10);

      }
      printf("Closing screen\n");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
