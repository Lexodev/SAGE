/**
 * video_triangle.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test triangle drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640
#define SCREEN_HEIGHT         480

ULONG palette[16] = {
   0x000000,0xffffff,0xff0000,0x00ff00,
   0x0000ff,0xff00ff,0xffff00,0x00ffff,
   0x888888,0x666666,0x444444,0x222222,
   0x800000,0x008000,0x000080,0xaaaaaa
};

void draw_clipregion(void)
{
  SAGE_Screen *screen;

  screen = SAGE_GetScreen();
  if (screen) {
    SAGE_DrawLine(screen->clipping.left, screen->clipping.top, screen->clipping.right, screen->clipping.top, 0xFFFFFF);
    SAGE_DrawLine(screen->clipping.right, screen->clipping.top, screen->clipping.right, screen->clipping.bottom, 0xFFFFFF);
    SAGE_DrawLine(screen->clipping.left, screen->clipping.bottom, screen->clipping.right, screen->clipping.bottom, 0xFFFFFF);
    SAGE_DrawLine(screen->clipping.left, screen->clipping.top, screen->clipping.left, screen->clipping.bottom, 0xFFFFFF);
  }
}

void test_triangle8(void)
{
  // Random triangle
  SAGE_DrawTriangle(45, 27, 207, 103, 35, 64, 2);
  //SAGE_DrawClippedTriangle(359, 163, 269, 129, 359, 245, 3);
  // Flat top triangle
  SAGE_DrawTriangle(10, 10, 50, 10, 60, 80, 2);
  //SAGE_DrawClippedTriangle(10, 110, 50, 110, 60, 180, 2);
  // Flat bottom triangle
  SAGE_DrawTriangle(230, 10, 150, 95, 290, 95, 3);
  //SAGE_DrawClippedTriangle(230, 110, 150, 195, 290, 195, 3);
  // Top clipped triangle
  SAGE_DrawClippedTriangle(200, -25, 250, 190, 26, 220, 4);
  // Bottom clipped triangle
  SAGE_DrawClippedTriangle(250, 84, 350, 550, 120, 300, 5);
  // Left clipped triangle
  SAGE_DrawClippedTriangle(-10, 68, 190, 28, 70, 158, 6);
  // Right clipped triangle
  SAGE_DrawClippedTriangle(120, 130, 690, 320, 70, 250, 7);
}

void test_triangle16(void)
{
  FLOAT x1, y1, x2, y2, x3, y3;
  
  x1=8.082603;  y1=-4.786925;
  x2=269.843834;  y2=263.634514;
  x3=73.654457;  y3=106.325291;
  SAGE_DrawClippedTriangle(
    (LONG)x1,(LONG)y1,
    (LONG)x2,(LONG)y2,
    (LONG)x3,(LONG)y3,
    0xFF00FF
  );
  SAGE_DrawClippedTriangle(200, -25, 250, 190, 26, 220, 0xFF0000);
  SAGE_DrawClippedTriangle(250, 84, 350, 550, 120, 300, 0x00FF00);
  SAGE_DrawClippedTriangle(-10, 68, 190, 28, 70, 158, 0x00FFFF);
  SAGE_DrawClippedTriangle(120, 130, 690, 320, 70, 250, 0xFFFF00);
  SAGE_DrawClippedTriangle(160, -150, 250, 20, 44, 120, 0xFF0000);
  SAGE_DrawClippedTriangle(250, 284, 350, 550, 120, 459, 0x00FF00);
}

void main(int argc, char **argv)
{
  SAGE_Event *event = NULL;
  LONG depth = 16;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (TRIANGLE) / %s", SAGE_GetVersion());
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
      SAGE_HideMouse();
      SAGE_SetColorMap(palette, 0, 16);
      SAGE_RefreshColors(0, 16);
      SAGE_SetScreenClip(20, 20, SCREEN_WIDTH-40, SCREEN_HEIGHT-40);
      while (!finish) {
        SAGE_SetTraceDebug(FALSE);
        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                SAGE_AppliLog("Exit loop");
                finish = TRUE;
                break;
              case SKEY_FR_D:
                SAGE_SetTraceDebug(TRUE);
                break;
            }
          }
        }
        SAGE_ClearScreen();
        draw_clipregion();
        if (depth == 8) {
          test_triangle8();
        } else if (depth == 16) {
          test_triangle16();
        }
        if (!SAGE_RefreshScreen()) {
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
      SAGE_AppliLog("Closing screen");
      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
