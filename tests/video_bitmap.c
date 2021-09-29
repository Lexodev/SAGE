/**
 * video_bitmap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test bitmap
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
  SAGE_Bitmap * bitmap = NULL;
  SAGE_Event * event = NULL;
  UWORD pixel, * buffer;
  ULONG width, height;
  BOOL finish;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (BITMAP) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      printf("Allocate bitmap\n");
      if ((bitmap = SAGE_AllocBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SAGE_GetPixelFormat(), NULL)) != NULL) {
        printf("Fill and blit bitmap\n");
        pixel = 0;
        buffer = (UWORD *) bitmap->bitmap_buffer;
        for (height = 0;height < SCREEN_HEIGHT;height++) {
          for (width = 0;width < SCREEN_WIDTH;width++) {
            *buffer++ = pixel++;
          }
        }
        SAGE_BlitBitmap(bitmap, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SAGE_GetBackBitmap(), 0, 0);
        printf("Refresh screen\n");
        if (!SAGE_RefreshScreen()) {
          SAGE_DisplayError();
        }
        SAGE_ReleaseBitmap(bitmap);
      }
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
          } else if (event->type == SEVT_RAWKEY) {
            SAGE_ClearScreen();
            if (!SAGE_RefreshScreen()) {
              SAGE_DisplayError();
            }
          }
        }
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
