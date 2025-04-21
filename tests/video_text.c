/**
 * video_screen.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test text print
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH      640L
#define SCREEN_HEIGHT     480L
#define SCREEN_DEPTH      16L

void fillbm(SAGE_Bitmap *bitmap)
{
  LONG width, height;
  UWORD * bitmap_data, pixel;

  bitmap_data = (UWORD *)bitmap->bitmap_buffer;
  pixel = 0;
  for (height = 0;height < SCREEN_HEIGHT;height++) {
    for (width = 0;width < SCREEN_WIDTH;width++) {
      *bitmap_data++ = pixel++;
    }
  }
}

void checkprint(void)
{
  SAGE_Event *event = NULL;
  LONG buffer = 1;
  BOOL finish = FALSE;

  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    SAGE_AppliLog("Loading fonts");
    if (!SAGE_SetFont("emerald.font", 17)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Clearing bitmaps");
    fillbm(SAGE_GetFrontBitmap());
    fillbm(SAGE_GetBackBitmap());
    fillbm(SAGE_GetWaitBitmap());
    SAGE_AppliLog("Run main loop");
    SAGE_HideMouse();
    SAGE_PrintDirectText("I AM THE SCREEN BUFFER", 20, 20);
    while (!finish) {
      while ((event = SAGE_GetEvent()) != NULL) {
        SAGE_AppliLog(
          "Event polled type %d, code %d, mouse %d,%d",
          event->type,
          event->code,
          event->mousex,
          event->mousey
        );
        if (event->type == SEVT_MOUSEBT) {
          SAGE_AppliLog("Exit loop");
          finish = TRUE;
        } else if (event->type == SEVT_RAWKEY) {
          switch (event->code) {
            case SKEY_FR_ESC:
              SAGE_AppliLog("Exit loop");
              finish = TRUE;
              break;
            case SKEY_FR_SPACE:
              if (!SAGE_RefreshScreen()) {
                SAGE_DisplayError();
              }
              SAGE_PrintDirectText("I AM AN OTHER SCREEN BUFFER", 20, 20 + (buffer*20));
              buffer++;
              SAGE_Pause(150);
              finish = TRUE;
              break;
          }
        }
      }
    }
    SAGE_ShowMouse();
    SAGE_AppliLog("Closing screen");
    SAGE_CloseScreen();
  }
}

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (PRINT) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    checkprint();
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
