/**
 * rotozoom.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Rotozoom effect
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

// Port of rotozoom code by seancode https://seancode.com/demofx/

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define TEX_WIDTH             256
#define TEX_HEIGHT            200

#define PI_VAL                3.14159265358979323846f

UBYTE string_buffer[256];

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture;
  int x, y, u, v, angle = 0;
  float s, c;
  unsigned char * buffer, * texture;
  BOOL finish;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library rotozoom demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INTERRUPTION)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();

      if (!SAGE_EnableFrameCount(TRUE)) {
        SAGE_ErrorLog("Can't activate frame rate counter !");
      }

      SAGE_AppliLog("Load texture");
      picture = SAGE_LoadPicture("data/rotozoom.gif");
      SAGE_LoadPictureColorMap(picture);
      SAGE_RefreshColors(0, 256);

      texture = picture->bitmap->bitmap_buffer;

      SAGE_AppliLog("Running main loop");
      finish = FALSE;
      while (!finish) {

        buffer = (unsigned char *) SAGE_GetBackBitmap()->bitmap_buffer;

        s = (float)sin(angle * PI / 180.0f );
        c = (float)cos(angle * PI / 180.0f );
        angle = (angle + 1) % 360;
        for (y = 0;y < SCREEN_HEIGHT;++y) {
          for (x = 0;x < SCREEN_WIDTH;++x) {
            u = (int)((x * c - y * s) * (s + 1) + 64) % TEX_WIDTH;
            v = (int)((x * s + y * c) * (s + 1) + 64) % TEX_HEIGHT;
            if (u < 0) u += TEX_WIDTH;
            if (v < 0) v += TEX_HEIGHT;                
            *buffer++ = texture[(u + v * TEX_WIDTH)];
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
      SAGE_ReleasePicture(picture);
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
