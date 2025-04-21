/**
 * video_screen.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test screen system
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

#define NB_TESTS      10

ULONG palette[16] = {
  0x000000,0xffffff,0xff0000,0x00ff00,
  0x0000ff,0xff00ff,0xffff00,0x00ffff,
  0x888888,0x666666,0x444444,0x222222,
  0x800000,0x008000,0x000080,0xaaaaaa
};

void testscreens(void)
{
  SAGE_Bitmap *bitmap = NULL;
  LONG screen_data[NB_TESTS*3] = {
    320,240,8,
    320,240,24,
    640,480,8,
    640,480,16,
    640,480,32,
    800,600,8,
    800,600,16,
    800,600,24,
    1024,768,24,
    1024,768,32
  }, screen_width, screen_height, screen_depth;
  ULONG idx, color1, color2, color3;

  for (idx = 0;idx < NB_TESTS;idx++) {
    screen_width = screen_data[idx*3];
    screen_height = screen_data[idx*3+1];
    screen_depth = screen_data[idx*3+2];
    SAGE_AppliLog("Opening screen %d", idx);
    if (SAGE_OpenScreen(screen_width, screen_height, screen_depth, SSCR_STRICTRES)) {
      SAGE_SetColorMap(palette, 0, 16);
      SAGE_RefreshColors(0, 16);
      if (!SAGE_SetFont("ruby.font", 12)) {
        SAGE_DisplayError();
      }
      if (screen_depth == 8) {
        color1 = 2;
        color2 = 3;
        color3 = 4;
      } else {
        color1 = 0xff0000;
        color2 = 0x00ff00;
        color3 = 0x0000ff;
      }
      SAGE_HideMouse();
      SAGE_ResetVblCount();
      SAGE_SetScreenClip(10, 10, screen_width - 20, screen_height - 20);
 
      SAGE_FillScreen(color1);
      SAGE_PrintFText(10, 10, "SCREEN IS %dx%dx%d", screen_width, screen_height, screen_depth);
      SAGE_PrintText("I AM THE FIRST BUFFER", 25, 40);
      SAGE_RefreshScreen();
      SAGE_Pause(150);
      SAGE_FillView(color2);
      SAGE_PrintFText(10, 10, "SCREEN IS %dx%dx%d", screen_width, screen_height, screen_depth);
      SAGE_PrintText("I AM THE SECOND BUFFER", 35, 55);
      SAGE_RefreshScreen();
      SAGE_Pause(150);
      SAGE_FillArea(32, 17, screen_width - 56, screen_height - 29, color3);
      SAGE_PrintFText(10, 10, "SCREEN IS %dx%dx%d", screen_width, screen_height, screen_depth);
      SAGE_PrintText("I AM THE THIRD BUFFER", 45, 70);
      SAGE_RefreshScreen();
      SAGE_Pause(150);

      SAGE_AppliLog("This animation runs during %d VBL", SAGE_GetVblCount());
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen %d", idx);
      SAGE_CloseScreen();
    } else {
      SAGE_ErrorLog("Unable to open screen %dx%dx%d", screen_width, screen_height, screen_depth);
      SAGE_DisplayError();
    }
  }
}

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (SCREEN) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    testscreens();
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
