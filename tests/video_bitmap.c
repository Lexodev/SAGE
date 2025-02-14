/**
 * video_bitmap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test bitmap
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

void main(void)
{
  SAGE_Bitmap *bitmap = NULL;
  SAGE_Event *event = NULL;
  UWORD pixel, *buffer;
  ULONG width, height;
  BOOL finish;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (BITMAP) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_AppliLog("Allocate bitmap");
      if ((bitmap = SAGE_AllocBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, 0, SAGE_GetPixelFormat(), NULL)) != NULL) {
        SAGE_AppliLog("Fill and blit bitmap");
        pixel = 0;
        buffer = (UWORD *)bitmap->bitmap_buffer;
        for (height = 0;height < SCREEN_HEIGHT;height++) {
          for (width = 0;width < SCREEN_WIDTH;width++) {
            *buffer++ = pixel++;
          }
        }
        SAGE_BlitBitmap(bitmap, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SAGE_GetBackBitmap(), 0, 0);
        SAGE_AppliLog("Refresh screen");
        if (!SAGE_RefreshScreen()) {
          SAGE_DisplayError();
        }
        SAGE_ReleaseBitmap(bitmap);
      }
      finish = FALSE;
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
            SAGE_ClearScreen();
            if (!SAGE_RefreshScreen()) {
              SAGE_DisplayError();
            }
          }
        }
      }
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
