/**
 * video_screen.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test text print
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include <proto/dos.h>

#include "/src/sage.h"

#define SCREEN_WIDTH      640L
#define SCREEN_HEIGHT     480L
#define SCREEN_DEPTH      16L

void fillbm(SAGE_Bitmap * bitmap)
{
  LONG width, height;
  UWORD * bitmap_data, pixel;

  bitmap_data = (UWORD *) bitmap->bitmap_buffer;
  pixel = 0;
  for (height = 0;height < SCREEN_HEIGHT;height++) {
    for (width = 0;width < SCREEN_WIDTH;width++) {
      *bitmap_data++ = pixel;
    }
  }
}

void checkprint(void)
{
  SAGE_Event * event = NULL;
  LONG buffer = 1;
  BOOL finish;

  printf("Opening screen\n");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    printf("Loading fonts\n");
    if (!SAGE_SetFont("UFOLarge.font", 15)) {
      SAGE_DisplayError();
    }
    printf("Clearing bitmaps\n");
    fillbm(SAGE_GetFrontBitmap());
    fillbm(SAGE_GetBackBitmap());
    printf("Run main loop\n");
    SAGE_HideMouse();
    finish = FALSE;
    SAGE_PrintText("I AM THE SCREEN BUFFER", 20, 20);
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
        } else if (event->type == SEVT_RAWKEY) {
          switch (event->code) {
            case SKEY_FR_ESC:
              printf("Exit loop\n");
              finish = TRUE;
              break;
            case SKEY_FR_SPACE:
              if (!SAGE_RefreshScreen()) {
                SAGE_DisplayError();
              }
              SAGE_PrintText("I AM THE OTHER SCREEN BUFFER", 20, 20 + (buffer*20));
              buffer++;
              Delay(50);
              break;
          }
        }
      }
    }
    SAGE_ShowMouse();
    printf("Closing screen\n");
    SAGE_CloseScreen();
  }
}

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (PRINT) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    checkprint();
  }
  SAGE_Exit();
  printf("End of test\n");
}
