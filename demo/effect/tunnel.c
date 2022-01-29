/**
 * tunnel.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Tunnel effect
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

// Source https://lodev.org/cgtutor/tunnel.html

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define TEX_WIDTH             256
#define TEX_HEIGHT            256

#define PI_VAL                3.14159265358979323846f

// Make the tables twice as big as the screen. The center of the buffers is now the position (w,h).
int distanceTable[2 * SCREEN_WIDTH][2 * SCREEN_HEIGHT];
int angleTable[2 * SCREEN_WIDTH][2 * SCREEN_HEIGHT];

UBYTE string_buffer[256];

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture;
  int x, y, w, h, angle, distance;
  float ratio = 32.0f, animation = 0.0f;
  unsigned char * buffer, * texture;
  int shiftX, shiftY, shiftLookX, shiftLookY, color;
  BOOL finish;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library tunnel demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INTERRUPTION)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();

      if (!SAGE_EnableFrameCount(TRUE)) {
        SAGE_ErrorLog("Can't activate frame rate counter !");
      }

      SAGE_AppliLog("Load texture");
      picture = SAGE_LoadPicture("data/tunnel.gif");
      SAGE_LoadPictureColorMap(picture);
      SAGE_RefreshColors(0, 256);

      w = SCREEN_WIDTH;
      h = SCREEN_HEIGHT;
      texture = picture->bitmap->bitmap_buffer;

      // generate non-linear transformation table, now for the bigger buffers (twice as big)
      for (y = 0;y < SCREEN_HEIGHT * 2;y++) {
        for (x = 0;x < SCREEN_WIDTH * 2;x++) {
          // these formulas are changed to work with the new center of the tables
          distance = (int)(ratio * TEX_HEIGHT / sqrt((float)((x - w) * (x - w) + (y - h) * (y - h)))) % TEX_HEIGHT;
          angle = (int)(0.5 * TEX_WIDTH * atan2((float)(y - h), (float)(x - w)) / PI_VAL);
          distanceTable[x][y] = distance;
          angleTable[x][y] = angle;
        }
      }

      SAGE_AppliLog("Running main loop");
      finish = FALSE;
      while (!finish) {

        buffer = (unsigned char *) SAGE_GetBackBitmap()->bitmap_buffer;

        animation += 1.0f / 60.0f;

        // calculate the shift values out of the animation value
        shiftX = (int)(TEX_WIDTH * 1.0 * animation);
        shiftY = (int)(TEX_HEIGHT * 0.25 * animation);
        // calculate the look values out of the animation value
        // by using sine functions, it'll alternate between looking left/right and up/down
        // make sure that x + shiftLookX never goes outside the dimensions of the table, same for y
        shiftLookX = w / 2 + (int)(w / 2 * sin(animation));
        shiftLookY = h / 2 + (int)(h / 2 * sin(animation * 2.0));

        for (y = 0;y < h;y++) {
          for (x = 0;x < w;x++) {
            // get the texel from the texture by using the tables, shifted with the animation variable
            // now, x and y are shifted as well with the "look" animation values
            color = texture[
                      ((unsigned int)(distanceTable[x + shiftLookX][y + shiftLookY] + shiftX)  % TEX_WIDTH)
                      + (((unsigned int)(angleTable[x + shiftLookX][y + shiftLookY] + shiftY) % TEX_HEIGHT) * TEX_WIDTH)
                    ];
            *buffer++ = (unsigned char)color;
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
