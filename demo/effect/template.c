/**
 * template.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Template file for doslike demos
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

// Port of doslike samples, doslike is coded by Mattias Gustavsson
// Check https://github.com/mattiasgustavsson/dos-like

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

void main(void)
{
  SAGE_Event * event = NULL;
  unsigned char * buffer;
  BOOL finish;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library template file demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_AppliLog("Running main loop");
      finish = FALSE;
      while (!finish) {
        buffer = (unsigned char *) SAGE_GetBackBitmap()->bitmap_buffer;

        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_MOUSEBT) {
            SAGE_AppliLog("Exit loop");
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY) {
            if (event->code == SKEY_FR_ESC) {
              SAGE_AppliLog("Exit loop");
              finish = TRUE;
            }
          }
        }
        if (!SAGE_RefreshScreen()) {
          SAGE_DisplayError();
        }
      }
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("Closing SAGE");
  if (!SAGE_Exit()) {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("End of demo");
}
