/**
 * video_event.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test event polling
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
  printf("* SAGE library VIDEO test (EVENT) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_DOUBLEBUF)) {
      finish = FALSE;
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          printf(
            "Event polled type %d, code %d, mouse %d,%d\n",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_MOUSEBT) {
            printf("Exit loop\n");
            finish = TRUE;
          }
        }
      }
      printf("Closing screen\n");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
