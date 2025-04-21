/**
 * video_draw.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test drawing primitives
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640
#define SCREEN_HEIGHT         480

LONG coords[SCREEN_HEIGHT];

void dump_coords(LONG *crd, ULONG nb)
{
  LONG index;
  
  SAGE_AppliLog("** Dump edge coordinates with %d points", nb);
  index = 0;
  while (index < nb) {
    SAGE_AppliLog("- point %d = %d", index, crd[index]);
    index++;
  }
  SAGE_AppliLog("***************************", nb);
}

void test_edgecalc(void)
{
  SAGE_Clipping clip = {0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};
  LONG points;

  SAGE_AppliLog("*** EDGE WITHOUT CLIP ***");
  SAGE_AppliLog("** Vertical line coords 100,10 -> 100,84 **");
  points = SAGE_EdgeCalc(coords, 100, 10, 100, 84);     // OK
  dump_coords(coords, points);
  SAGE_AppliLog("** Horizontal line + coords 10,50 -> 104,50 **");
  points = SAGE_EdgeCalc(coords, 10, 50, 104, 50);      // OK
  dump_coords(coords, points);
  SAGE_AppliLog("** Diagonal line + coords 10,10 -> 96,96 **");
  points = SAGE_EdgeCalc(coords, 10, 10, 96, 96);       // OK
  dump_coords(coords, points);
  SAGE_AppliLog("** Soft slope line + coords 80,10 -> 250,98 **");
  points = SAGE_EdgeCalc(coords, 80, 10, 250, 98);      // OK
  dump_coords(coords, points);
  SAGE_AppliLog("** Hard slope line + coords 10,10 -> 60,93 **");
  points = SAGE_EdgeCalc(coords, 10, 10, 60, 93);       // OK
  dump_coords(coords, points);

  SAGE_AppliLog("*** EDGE WITH CLIP ***");
  SAGE_AppliLog("** Soft slope line + left clip coords -10,6 -> 120,80 **");
  points = SAGE_ClippedEdgeCalc(coords, -10, 6, 120, 80, clip.top, clip.bottom);
  dump_coords(coords, points);
  SAGE_AppliLog("** Soft slope line + right clip coords 50,10 -> 350,110 **");
  points = SAGE_ClippedEdgeCalc(coords, 50, 10, 350, 110, clip.top, clip.bottom);
  dump_coords(coords, points);
  SAGE_AppliLog("** Hard slope + left clipped coords -4,6 -> 50,180 **");
  points = SAGE_ClippedEdgeCalc(coords, -4, 6, 50, 180, clip.top, clip.bottom);
  dump_coords(coords, points);
  SAGE_AppliLog("** Hard slope + right clipped coords 30,6 -> 360,180 **");
  points = SAGE_ClippedEdgeCalc(coords, 30, 6, 360, 180, clip.top, clip.bottom);
  dump_coords(coords, points);
}

void test_pixels(void)
{
  LONG x = 0, y = 0, color = 0;

  SAGE_DrawPixel(x%SCREEN_WIDTH, y%SCREEN_HEIGHT, SAGE_RemapColor(color));
  x++;
  if (x%SCREEN_WIDTH == 0) y++;
  if (y > 50) y = 0;
  color += 16;
}

void test_flatquad(void)
{
  // Draw clipping zone
  SAGE_DrawLine(0, 0, SCREEN_WIDTH-1, 0, 0xff0000);
  SAGE_DrawLine(SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, 0xff0000);
  SAGE_DrawLine(0, SCREEN_HEIGHT-1, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, 0xff0000);
  SAGE_DrawLine(0, 0, 0, SCREEN_HEIGHT-1, 0xff0000);

  SAGE_DrawFlatQuad(10, 260, 22, 35, 241, 66, 0xFFFF01);
  SAGE_DrawClippedFlatQuad(-10, 260, 102, 35, 341, 206, 0x00FFFF);
  SAGE_DrawClippedFlatQuad(600, 700, 350, 700, 850, 550, 0x008800);
}

void main(int argc, char **argv)
{
  SAGE_Event *event = NULL;
  LONG depth = 16;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (DRAW) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    if (argc >= 2) {
      if (strcmp(argv[1], "8") == 0) {
        depth = 8;
      } else if (strcmp(argv[1], "24") == 0) {
        depth = 24;
      } else if (strcmp(argv[1], "32") == 0) {
        depth = 32;
      }
    }
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, depth, SSCR_STRICTRES)) {
      test_edgecalc();
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                SAGE_AppliLog("Exit loop");
                finish = TRUE;
                break;
            }
          }
        }
        test_pixels();
        test_flatquad();
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
