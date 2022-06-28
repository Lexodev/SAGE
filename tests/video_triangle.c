/**
 * video_triangle.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test triangle drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

ULONG palette[16] = {
   0x000000,0xffffff,0xff0000,0x00ff00,
   0x0000ff,0xff00ff,0xffff00,0x00ffff,
   0x888888,0x666666,0x444444,0x222222,
   0x800000,0x008000,0x000080,0xaaaaaa
};

void draw_clipregion(void)
{
  SAGE_Screen * screen;

  screen = SAGE_GetScreen();
  if (screen) {
    SAGE_DrawLine(screen->clipping.left, screen->clipping.top, screen->clipping.right, screen->clipping.top, 1);
    SAGE_DrawLine(screen->clipping.right, screen->clipping.top, screen->clipping.right, screen->clipping.bottom, 1);
    SAGE_DrawLine(screen->clipping.left, screen->clipping.bottom, screen->clipping.right, screen->clipping.bottom, 1);
    SAGE_DrawLine(screen->clipping.left, screen->clipping.top, screen->clipping.left, screen->clipping.bottom, 1);
  }
}

void test_triangle8(void)
{
  // Random triangle
  SAGE_DrawTriangle(71,12,77,43,75,34,1);
  //SAGE_DrawClippedTriangle(359,163,269,129,359,245,1);
  // Flat top triangle
  //SAGE_DrawTriangle(10, 10, 50, 10, 60, 80, 2);
  //SAGE_DrawClippedTriangle(10, 110, 50, 110, 60, 180, 2);
  // Flat bottom triangle
  //SAGE_DrawTriangle(230, 10, 150, 95, 290, 95, 3);
  //SAGE_DrawClippedTriangle(230, 110, 150, 195, 290, 195, 3);
  // Top clipped triangle
  //SAGE_DrawClippedTriangle(200, -25, 250, 190, 20, 220, 2);
  // Bottom clipped triangle
  //SAGE_DrawClippedTriangle(150, 84, 250, 550, 20, 300, 3);
  // Left clipped triangle
  //SAGE_DrawClippedTriangle(-10, 60, 190, 20, 70, 150, 4);
  // Right clipped triangle
  //SAGE_DrawClippedTriangle(120, 30, 690, 220, 70, 150, 5);
}

void test_triangle16(void)
{
  FLOAT x1, y1, x2, y2, x3, y3;
  
  x1=68.082603;  y1=-4.786925;
  x2=69.843834;  y2=3.634514;
  x3=73.654457;  y3=26.325291;
  SAGE_DrawClippedTriangle(
    (LONG)x1,(LONG)y1,
    (LONG)x2,(LONG)y2,
    (LONG)x3,(LONG)y3,
    0xFFFFFFFF
  );
}

void main(void)
{
  SAGE_Event * event = NULL;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (TRIANGLE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_SetColorMap(palette, 0, 16);
      SAGE_RefreshColors(0, 16);
//      SAGE_SetScreenClip(10, 10, 620, 460);
        test_triangle16();
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
//        SAGE_ClearScreen();
//        test_triangle16();
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
