/**
 * mandelbrot.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of mandelbrot fractal
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 September 2021
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

#define MANDEL_LAYER          1

BOOL finish = FALSE, zoom = FALSE, move = FALSE;
FLOAT user_left = SPACE_LEFT, user_top = SPACE_TOP, user_right = SPACE_RIGHT, user_bottom = SPACE_BOTTOM;
FLOAT step_x, step_y;
WORD mouse_x, mouse_y;
LONG start_x, start_y, end_x, end_y;

BOOL _Init(VOID)
{
  UBYTE index;

  SAGE_AppliLog("Open main screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    SAGE_HideMouse();
    // Hiding mouse disable the mouse move tracking, we should enable it for our purpose
    SAGE_TrackMouse(TRUE);
    SAGE_AppliLog("Set colormap");
    for (index = 0;index < MAX_ITERATION;index++) {
      SAGE_SetColor(index, index*(SSCR_MAXCOLORS/MAX_ITERATION));    // Full blue
    }
    SAGE_SetColor(MAX_ITERATION, 0);                      // Full black
    SAGE_SetColor(MAX_ITERATION+1, 0xFFFFFF);             // Full white
    SAGE_RefreshColors(0, 256);
    SAGE_SetTextColor(MAX_ITERATION+1, 0);
    SAGE_AppliLog("Create fractal layer");
    if (SAGE_CreateLayer(MANDEL_LAYER, SCREEN_WIDTH, SCREEN_HEIGHT)) {
      return TRUE;
    }
  }
  return FALSE;
}

VOID _Restore(VOID)
{
  SAGE_AppliLog("Release fractal layer");
  SAGE_ReleaseLayer(MANDEL_LAYER);
  SAGE_ShowMouse();
  SAGE_AppliLog("Close screen");
  SAGE_CloseScreen();
}

VOID RenderHelp(VOID)
{
  SAGE_PrintDirectText("Mandelbrot exploration V1.0", 10, 20);
  SAGE_PrintDirectText("- Right click activate/deactivate zoom mode", 10, 40);
  SAGE_PrintDirectText("- Left click+move to define your zoom area when zoom mode is active", 10, 52);
  SAGE_PrintDirectText("- Enter to start the zoom calculation", 10, 64);
  SAGE_PrintDirectText("- Space to reset the zoom factor", 10, 76);
  SAGE_PrintDirectText("- Escape to exit the program", 10, 88);
  SAGE_Pause(50 * 5);
}

VOID RenderMandelbrot(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom)
{
  UBYTE * buffer;
  UWORD x, y;
  UBYTE iteration;
  FLOAT c_r, c_i, z_r, z_i, tmp;
  BOOL compute;

  SAGE_PrintDirectText("Computing fractal, please wait...", 10, 240);
  buffer = (UBYTE *) SAGE_GetBitmapBuffer(SAGE_GetLayerBitmap(MANDEL_LAYER));
  step_x = (right - left) / (FLOAT)SCREEN_WIDTH;
  step_y = (bottom - top) / (FLOAT)SCREEN_HEIGHT;
  SAGE_AppliLog("Step value by pixel %f, %f", step_x, step_y);
  c_i = top;
  for (y = 0;y < SCREEN_HEIGHT;y++) {
    c_r = left;
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
      *buffer++ = iteration;
      c_r += step_x;
    }
    c_i += step_y;
    if ((y%10) == 0) {
      SAGE_PrintDirectText(".", (34*6) + (y*6/10) + 10, 240);
    }
  }
}

VOID _Animate(VOID)
{
  SAGE_Event * event = NULL;
  LONG tmp;

  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_MOUSEBT) {
      if (event->code == SMBT_RMBUP) {
        zoom = !zoom;
      }
      if (event->code == SMBT_LMBUP) {
        SAGE_AppliLog("Move OFF");
        move = FALSE;
      } else if (event->code == SMBT_LMBDOWN) {
        SAGE_AppliLog("Move ON");
        move = TRUE;
        start_x = end_x = event->mousex;
        start_y = end_y = event->mousey;
      }
    } else if (event->type == SEVT_MOUSEMV) {
      mouse_x = event->mousex;
      mouse_y = event->mousey;
    } else if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit program");
        finish = TRUE;
      } else if (event->code == SKEY_FR_ENTER) {
        zoom = move = FALSE;
        if (end_x < start_x) {
          tmp = start_x;
          start_x = end_x;
          end_x = tmp;
        }
        if (end_y < start_y) {
          tmp = start_y;
          start_y = end_y;
          end_y = tmp;
        }
        user_right = user_left + (end_x * step_x);
        user_bottom = user_top + (end_y * step_y);
        user_left += (start_x * step_x);
        user_top += (start_y * step_y);
        SAGE_AppliLog("Zooming on %f, %f to %f, %f", user_left, user_top, user_right, user_bottom);
        RenderMandelbrot(user_left, user_top, user_right, user_bottom);
      } else if (event->code == SKEY_FR_SPACE) {
        zoom = move = FALSE;
        user_left = SPACE_LEFT;
        user_top = SPACE_TOP;
        user_right = SPACE_RIGHT;
        user_bottom = SPACE_BOTTOM;
        RenderMandelbrot(user_left, user_top, user_right, user_bottom);
      }
    }
  }
  if (!zoom) {
    start_x = end_x = start_y = end_y = 0;
  }
  if (move) {
    end_x = mouse_x;
    end_y = mouse_y;
  }
}

VOID _Render(VOID)
{
  if (SAGE_BlitLayerToScreen(MANDEL_LAYER, 0, 0)) {
    if (zoom) {
      SAGE_DrawLine(mouse_x, 0, mouse_x, SCREEN_HEIGHT-1, MAX_ITERATION+1);
      SAGE_DrawLine(0, mouse_y, SCREEN_WIDTH-1, mouse_y, MAX_ITERATION+1);
      if (move) {
        SAGE_DrawLine(start_x, start_y, mouse_x, start_y, MAX_ITERATION+1);
        SAGE_DrawLine(start_x, start_y, start_x, mouse_y, MAX_ITERATION+1);
      } else {
        SAGE_DrawLine(start_x, start_y, end_x, start_y, MAX_ITERATION+1);
        SAGE_DrawLine(start_x, start_y, start_x, end_y, MAX_ITERATION+1);
        SAGE_DrawLine(end_x, end_y, start_x, end_y, MAX_ITERATION+1);
        SAGE_DrawLine(end_x, end_y, end_x, start_y, MAX_ITERATION+1);
      }
    }
  }
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library mandelbrot demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO)) {
    if (_Init()) {
      RenderHelp();
      RenderMandelbrot(user_left, user_top, user_right, user_bottom);
      while (!finish) {
        _Animate();
        _Render();
        if (!SAGE_RefreshScreen()) {
          finish = TRUE;
        }
      }
      _Restore();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
