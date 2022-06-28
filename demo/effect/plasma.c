/**
 * plasma.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Plasma effect
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

// Source https://lodev.org/cgtutor/plasma.html

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define PI_VAL                3.14159265358979323846f

// Y-coordinate first because we use horizontal scanlines
unsigned int plasma[SCREEN_HEIGHT][SCREEN_WIDTH];

UBYTE string_buffer[256];

void main(void)
{
  SAGE_Event * event = NULL;
  BOOL finish;
  int x, y, r, g, b, paletteShift = 0;
  unsigned char * buffer;
  
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library plasma demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INTERRUPTION)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();

      if (!SAGE_EnableFrameCount(TRUE)) {
        SAGE_ErrorLog("Can't activate frame rate counter !");
      }

      // generate the palette
      for (x = 0;x < 256;x++) {
        r = (int)(128.0 + 128 * sin(PI_VAL * x / 32.0));
        g = (int)(128.0 + 128 * sin(PI_VAL * x / 64.0));
        b = (int)(128.0 + 128 * sin(PI_VAL * x / 128.0));
        SAGE_SetColor(x, (r<<16)+(g<<8)+b);
      } 
      SAGE_RefreshColors(0, 256);

      // generate the plasma once
      for (y = 0;y < SCREEN_HEIGHT;y++) {
        for(x = 0;x < SCREEN_WIDTH;x++) {
          // the plasma buffer is a sum of sines
          int color = (int)
          (
              128.0 + (128.0 * sin(x / 32.0))
            + 128.0 + (128.0 * sin(y / 16.0))
            + 128.0 + (128.0 * sin((x + y) / 32.0))
            + 128.0 + (128.0 * sin(sqrt((double)(x * x + y * y)) / 16.0))
          ) / 4;
          plasma[y][x] = color;
        }
      }

      SAGE_AppliLog("Running main loop");
      finish = FALSE;
      while (!finish) {
        
        buffer = (unsigned char *) SAGE_GetBackBitmap()->bitmap_buffer;
        
        paletteShift++;

        // draw every pixel again, with the shifted palette color
        for(y = 0;y < SCREEN_HEIGHT;y++) {
          for(x = 0; x < SCREEN_WIDTH; x++) {
            *buffer++ = (plasma[y][x] + paletteShift) % 256;
          }
        }

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
        sprintf(string_buffer, "%d fps", SAGE_GetFps());
        SAGE_PrintText(string_buffer, 10, 10);
        if (!SAGE_RefreshScreen()) {
          SAGE_DisplayError();
        }
      }
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
