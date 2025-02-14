/**
 * video_line.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test line drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

void main(void)
{
  SAGE_Event *event = NULL;
  LONG color, x1, y1, x2, y2, ix1, iy1, ix2, iy2;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (LINE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      x1 = 0;
      ix1 = 1;
      y1 = 0;
      iy1 = 0;
      x2 = SCREEN_WIDTH - 1;
      ix2 = -1;
      y2 = SCREEN_HEIGHT - 1;
      iy2 = 0;
      color = 0;
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          SAGE_AppliLog(
            "Event polled type %d, code %d, mouse %d,%d",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                SAGE_AppliLog("Exit loop");
                finish = TRUE;
                break;
            }
          }
        }
        x1 += ix1;
        if (x1 >= SCREEN_WIDTH) {
          ix1 = 0;
          iy1 = 1;
        } else if (x1 < 0) {
          ix1 = 0;
          iy1 = -1;
        }
        y1 += iy1;
        if (y1 >= SCREEN_HEIGHT) {
          iy1 = 0;
          ix1 = -1;
        } else if (y1 < 0) {
          iy1 = 0;
          ix1 = 1;
        }
        x2 += ix2;
        if (x2 >= SCREEN_WIDTH) {
          ix2 = 0;
          iy2 = 1;
        } else if (x2 < 0) {
          ix2 = 0;
          iy2 = -1;
        }
        y2 += iy2;
        if (y2 >= SCREEN_HEIGHT) {
          iy2 = 0;
          ix2 = -1;
        } else if (y2 < 0) {
          iy2 = 0;
          ix2 = 1;
        }
        if (SCREEN_DEPTH == 8) {
          color++;
          color %= 252;
        } else {
          color = 0xffff;
        }
        SAGE_DrawLine(x1, y1, x2, y2, color + 2);
        if (!SAGE_RefreshScreen()) {
          SAGE_AppliLog("Error RefreshScreen !");
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
