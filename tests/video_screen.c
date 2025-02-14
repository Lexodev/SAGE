/**
 * video_screen.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test screen system
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

void openscreens(void)
{
  SAGE_Bitmap *bitmap = NULL;
  LONG screen_data[4*3] = {
    320,240,8,
    640,480,16,
    640,480,24,
    640,480,32
  }, i, s, width, height, swidth, sheight, sdepth;
  UBYTE *bitmap8_data, pixel8, *bitmap24_data,pixel24;
  UWORD *bitmap16_data, pixel16;
  ULONG *bitmap32_data, pixel32;

  for (i = 0;i < 4;i++) {
    swidth = screen_data[i*3];
    sheight = screen_data[i*3+1];
    sdepth = screen_data[i*3+2];
    SAGE_AppliLog("Opening screen %d", i);
    if (SAGE_OpenScreen(swidth, sheight, sdepth, SSCR_STRICTRES)) {
      if (!SAGE_SetFont("UFOLarge.font", 15)) {
        SAGE_DisplayError();
      }
      bitmap = SAGE_GetBackBitmap();
      if (sdepth == 8) {
        pixel8 = 0;
        bitmap8_data = (UBYTE *)bitmap->bitmap_buffer;
        for (height = 0;height < sheight;height++) {
          for (width = 0;width < swidth;width++) {
            *bitmap8_data++ = pixel8;
          }
          pixel8++;
        }
      } else if (sdepth == 16) {
        bitmap16_data = (UWORD *)bitmap->bitmap_buffer;
        for (height = 0;height < sheight;height++) {
          pixel16 = 0;
          for (width = 0;width < swidth;width++) {
            *bitmap16_data++ = pixel16++;
          }
        }
      } else if (sdepth == 24) {
        bitmap24_data = (UBYTE *)bitmap->bitmap_buffer;
        for (height = 0;height < sheight;height++) {
          pixel24 = 0;
          for (width = 0;width < swidth;width++) {
            bitmap24_data[0] = 0;
            bitmap24_data[1] = pixel24++;
            bitmap24_data[2] = 0;
            bitmap24_data += 3;
          }
        }
      } else if (sdepth == 32) {
        bitmap32_data = (ULONG *)bitmap->bitmap_buffer;
        for (height = 0;height < sheight;height++) {
          pixel32 = 0;
          for (width = 0;width < swidth;width++) {
            pixel32 = (pixel32 + 0x10000) & 0xFF0000;
            *bitmap32_data++ = pixel32;
          }
        }
      }
      SAGE_HideMouse();
      SAGE_ResetVblCount();
      for (s = 0;s < 3;s++) {
        SAGE_PrintText("I AM THE FIRST BUFFER", 20 + (s*5), 20 + (s*15));
        SAGE_Pause(50);
        SAGE_RefreshScreen();
        SAGE_PrintText("I AM THE SECOND BUFFER", 20 + (s*5), 20 + (s*15));
        SAGE_Pause(50);
        SAGE_RefreshScreen();
        SAGE_PrintText("I AM THE THIRD BUFFER", 20 + (s*5), 20 + (s*15));
        SAGE_Pause(50);
        SAGE_RefreshScreen();
      }
      SAGE_Pause(50);
      SAGE_AppliLog("This animation runs during %d VBL", SAGE_GetVblCount());
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen %d", i);
      SAGE_CloseScreen();
    }
  }
}

void checkvbl(void)
{
  ULONG vbl_count;
  
  SAGE_AppliLog("Opening check screen");
  if (SAGE_OpenScreen(640, 480, 8, SSCR_STRICTRES)) {
    if (!SAGE_SetFont("UFOLarge.font", 15)) {
      SAGE_DisplayError();
    }
    SAGE_HideMouse();
    SAGE_PrintText("WAIT A LITTLE BIT", 20, 20);
    SAGE_RefreshScreen();
    SAGE_ResetVblCount();
    SAGE_Pause(50 * 5);
    vbl_count = SAGE_GetVblCount();
    SAGE_AppliLog("%d VBL have occured during 5 seconds", vbl_count);
    if (vbl_count > 290) {
      SAGE_AppliLog("VBL is about 60Hz");
    } else {
      SAGE_AppliLog("VBL is about 50Hz");
    }
    SAGE_CloseScreen();
  }
}

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (SCREEN) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    openscreens();
    checkvbl();
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
