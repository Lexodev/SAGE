/**
 * video_triangle.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test triangle drawing
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

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
  //SAGE_DrawTriangle(359,163,269,129,359,245,1);
  SAGE_DrawClippedTriangle(359,163,269,129,359,245,1);
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

void main(void)
{
  SAGE_Event * event = NULL;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (TRIANGLE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_SetColorMap(palette, 0, 16);
      SAGE_RefreshColors(0, 16);
      SAGE_SetScreenClip(10, 10, 620, 460);
        test_triangle8();
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          printf(
            "Event polled type %d, code %d, mouse %d,%d\n",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                printf("Exit loop\n");
                finish = TRUE;
                break;
            }
          }
        }
        draw_clipregion();
        if (!SAGE_RefreshScreen()) {
          printf("Error RefreshScreen !\n");
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
      printf("Closing screen\n");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
