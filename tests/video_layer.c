/**
 * video_layer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test layer
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define LAYER_INDEX           0
#define LAYER_WIDTH           800L
#define LAYER_HEIGHT          600L

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Bitmap * bitmap = NULL;
  UWORD pixel, * buffer;
  ULONG width, height, xpos = 0, ypos = 0;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (LAYER) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      printf("Create layer\n");
      if (SAGE_CreateLayer(LAYER_INDEX, LAYER_WIDTH, LAYER_HEIGHT)) {
        printf("Fill layer bitmap with silly pattern\n");
        pixel = 0;
        bitmap = SAGE_GetLayerBitmap(LAYER_INDEX);
        buffer = (UWORD *) bitmap->bitmap_buffer;
        for (height = 0;height < LAYER_HEIGHT;height++) {
          for (width = 0;width < LAYER_WIDTH;width++) {
            *buffer++ = pixel++;
          }
        }
      } else {
        finish = TRUE;
      }
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
        xpos = (xpos + 2) % (800 - SCREEN_WIDTH);
        ypos = (ypos + 1) % (600 - SCREEN_HEIGHT);
        if (!SAGE_SetLayerView(LAYER_INDEX, xpos, ypos, SCREEN_WIDTH, SCREEN_HEIGHT)) {
          printf("Error SetLayerView !\n");
          SAGE_DisplayError();
          finish = TRUE;
        }
        if (!SAGE_BlitLayerToScreen(LAYER_INDEX, 0, 0)) {
          printf("Error BlitLayerToScreen !\n");
          SAGE_DisplayError();
          finish = TRUE;
        }
        if (!SAGE_RefreshScreen()) {
          printf("Error RefreshScreen !\n");
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
      SAGE_ReleaseLayer(LAYER_INDEX);
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
