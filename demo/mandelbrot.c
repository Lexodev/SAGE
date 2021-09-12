/**
 * mandelbrot.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Demo of mandelbrot picture
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L
#define MAX_ITERATION         32L

#define SPACE_LEFT            -2.1
#define SPACE_TOP             -1.2
#define SPACE_RIGHT           0.6
#define SPACE_BOTTOM          1.2

VOID RenderMandelbrot(VOID)
{
  UBYTE * buffer;
  UWORD x, y;
  UBYTE iteration;
  FLOAT step_x, step_y, c_r, c_i, z_r, z_i, tmp;
  BOOL compute;

  step_x = (SPACE_RIGHT - SPACE_LEFT) / (FLOAT)SCREEN_WIDTH;
  step_y = (SPACE_BOTTOM - SPACE_TOP) / (FLOAT)SCREEN_HEIGHT;

  SAGE_PrintDirectText("Computing fractal, please wait", 10, 240);
  buffer = (UBYTE *) SAGE_GetBackBitmap()->bitmap_buffer;
  c_i = SPACE_TOP;
  for (y = 0;y < SCREEN_HEIGHT;y++) {
    c_r = SPACE_LEFT;
    for (x = 0;x < SCREEN_WIDTH;x++) {
      z_r = 0.0;
      z_i = 0.0;
      iteration = 0;

      compute = TRUE;
      while (compute) {
        tmp = z_r;
        z_r = z_r*z_r - z_i*z_i + c_r;
        z_i = 2*tmp*z_i + c_i;
        iteration++;
        if ((z_r*z_r + z_i*z_i) >= 4) {
          compute = FALSE;
        } else if (iteration >= MAX_ITERATION) {
          compute = FALSE;
        } 
      }
      c_r += step_x;

      *buffer++ = iteration;
    }
    c_i += step_y;

    if ((y%10) == 0) {
      SAGE_PrintDirectText(".", (35*6) + (y*6/10) + 10, 240);
    }
  }
}

void main(void)
{
  SAGE_Event * event = NULL;
  UBYTE index;
  BOOL finish;

  //SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library mandelbrot demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_AppliLog("Set colormap");
      for (index = 0;index < MAX_ITERATION;index++) {
        SAGE_SetColor(index, index*(SSCR_MAXCOLORS/MAX_ITERATION));    // Full blue
      }
      SAGE_SetColor(MAX_ITERATION, 0);                      // Full black
      SAGE_SetColor(MAX_ITERATION+1, 0xFFFFFF);             // Full white
      SAGE_RefreshColors(0, 256);
      SAGE_SetTextColor(MAX_ITERATION+1, 0);
      finish = FALSE;
      SAGE_AppliLog("Drawing fractal");
      RenderMandelbrot();
      if (!SAGE_RefreshScreen()) {
        SAGE_DisplayError();
        finish = TRUE;
      }
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
            if (event->code == SKEY_FR_ESC) {
              SAGE_AppliLog("Exit loop");
              finish = TRUE;
            }
          }
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
