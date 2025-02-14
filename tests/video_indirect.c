/**
 * video_indirect.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test indirect bitmap
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
  SAGE_Event *event = NULL;
  BOOL finish;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (INDIRECT BITMAP) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES|SSCR_INDIRECT)) {
      SAGE_SetTextColor(0, 1);
      finish = FALSE;
      while (!finish) {

        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_MOUSEBT) {
            SAGE_AppliLog("Exit loop");
            finish = TRUE;
          }
        }

        SAGE_PrintDirectText("Press left mouse button to quit", 10, 10);

      }
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
