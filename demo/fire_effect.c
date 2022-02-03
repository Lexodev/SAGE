/**
 * fire_effect.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of fire effect
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

#include <stdlib.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L
#define FOYER                 1
#define SIZE_TAB              SCREEN_WIDTH*32
#define MASK_TAB              SIZE_TAB-1

UBYTE rand_color[SIZE_TAB];
ULONG id_rc = 0;
UBYTE rand_foyer[SIZE_TAB];
ULONG id_rf = 0;
UBYTE rand_x[SIZE_TAB];
ULONG id_rx = 0;

// Render data
UBYTE string_buffer[256];

VOID InitPrecalc(VOID)
{
  ULONG index;
  
  SAGE_AppliLog("Precalc fire colors");
  for (index = 0;index < SIZE_TAB;index++) {
    rand_color[index] = 32 + (rand() % 48);
    rand_foyer[index] = rand() % 16;
    rand_x[index] = 1 + (rand() % (SCREEN_WIDTH - 2));
  }
}

VOID FeedFoyer(UBYTE * bitmap)
{
  ULONG x,y,f;

  for (y = SCREEN_HEIGHT-FOYER;y < SCREEN_HEIGHT;y++) {
    for (x = 0;x < SCREEN_WIDTH;x++) {
      bitmap[x+y*SCREEN_WIDTH] = rand_color[(id_rc++ & MASK_TAB)];
    }
  }
  for (f = 0;f < rand_foyer[(id_rf++ & MASK_TAB)];f++) {
    x = rand_x[(id_rx++ & MASK_TAB)];
    for (y = SCREEN_HEIGHT-FOYER;y < SCREEN_HEIGHT;y++) {
      bitmap[(x-1)+y*SCREEN_WIDTH] = 255;
      bitmap[x+y*SCREEN_WIDTH] = 255;
      bitmap[(x+1)+y*SCREEN_WIDTH] = 255;
    }
  }
}

VOID SetFlames(UBYTE * physic, UBYTE * logic)
{
  ULONG x,y;

  for (y = 0;y < SCREEN_HEIGHT-FOYER;y++) {
    for (x = 1;x < SCREEN_WIDTH-1;x++) {
      logic[x+y*SCREEN_WIDTH] = (
          physic[(x-1)+y*SCREEN_WIDTH] +
          physic[(x+1)+y*SCREEN_WIDTH] +
          physic[(x-1)+(y+1)*SCREEN_WIDTH] +
          physic[(x)+(y+1)*SCREEN_WIDTH] +
          physic[(x+1)+(y+1)*SCREEN_WIDTH] +
          physic[(x-1)+(y+2)*SCREEN_WIDTH] +
          physic[(x)+(y+2)*SCREEN_WIDTH] +
          physic[(x+1)+(y+2)*SCREEN_WIDTH]
      ) >> 3;
    }
  }
}

VOID SetFlames2(UBYTE * physic, UBYTE * logic)
{
  ULONG x,y;

  for (y = 0;y < SCREEN_HEIGHT-FOYER;y++) {
    for (x = 1;x < SCREEN_WIDTH-1;x++) {
      logic[x+y*SCREEN_WIDTH] = (
          physic[(x-1)+(y+1)*SCREEN_WIDTH] +
          physic[(x)+(y+1)*SCREEN_WIDTH] +
          physic[(x+1)+(y+1)*SCREEN_WIDTH] +
          physic[(x)+(y+2)*SCREEN_WIDTH]
      ) * 32 / 129;
    }
  }
}

void main(void)
{
  SAGE_Event * event = NULL;
  UWORD index;
  ULONG color;
  BOOL finish;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library fire demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INTERRUPTION)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES|SSCR_INDIRECT)) {
      if (!SAGE_EnableFrameCount(TRUE)) {
        SAGE_ErrorLog("Can't activate frame rate counter !");
      }
      SAGE_AppliLog("Load colormap");
      SAGE_LoadFileColorMap("/data/flame.rgb", 0, 256, SSCR_RGBCOLOR);
      InitPrecalc();
      for (index = 0;index < SSCR_MAXCOLORS;index++) {
        color = SAGE_GetColor(index);
        // A little bit brighter
        SAGE_SetColor(index, (color + color + color));
      }
      SAGE_RefreshColors(0, 256);
      SAGE_HideMouse();
      SAGE_AppliLog("Running main loop");
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
            if (event->code == SKEY_FR_ESC) {
              SAGE_AppliLog("Exit loop");
              finish = TRUE;
            }
          }
        }
        FeedFoyer((UBYTE *) SAGE_GetBackBitmap()->bitmap_buffer);
        SetFlames2((UBYTE *) SAGE_GetFrontBitmap()->bitmap_buffer, (UBYTE *) SAGE_GetBackBitmap()->bitmap_buffer);

        // Draw the fps counter
        sprintf(string_buffer, "%d fps", SAGE_GetFps());
        SAGE_PrintText(string_buffer, 260, 10);

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
