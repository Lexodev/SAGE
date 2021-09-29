/**
 * video_screen.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test double buffer system
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include <proto/dos.h>

#include "/src/sage.h"

#define SCREEN_WIDTH      640L
#define SCREEN_HEIGHT     480L
#define SCREEN_DEPTH      16L

void checkdblbuf(void)
{
  SAGE_Bitmap * bitmap = NULL;
  LONG  s, width, height;
  UWORD * bitmap_data, pixel;

  printf("Opening dblbuf screen\n");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    if (!SAGE_SetFont("UFOLarge.font", 15)) {
      SAGE_DisplayError();
    }
    bitmap = SAGE_GetFrontBitmap();
    bitmap_data = (UWORD *) bitmap->bitmap_buffer;
    pixel = 0;
    for (height = 0;height < SCREEN_HEIGHT;height++) {
      for (width = 0;width < SCREEN_WIDTH;width++) {
        *bitmap_data++ = pixel;
      }
      pixel += 8;
    }
    bitmap = SAGE_GetBackBitmap();
    bitmap_data = (UWORD *) bitmap->bitmap_buffer;
    pixel = 16000;
    for (height = 0;height < SCREEN_HEIGHT;height++) {
      for (width = 0;width < SCREEN_WIDTH;width++) {
        *bitmap_data++ = pixel;
      }
      pixel += 8;
    }
    SAGE_HideMouse();
    SAGE_ResetVblCount();
    for (s = 0;s < 3;s++) {
      SAGE_PrintText("I AM THE FIRST BUFFER", 20 + (s*5), 20 + (s*15));
      SAGE_RefreshScreen();
      Delay(100);
      SAGE_PrintText("I AM THE SECOND BUFFER", 320 + (s*5), 20 + (s*15));
      SAGE_RefreshScreen();
      Delay(100);
    }
    Delay(100);
    printf("This animation runs during %d VBL\n", SAGE_GetVblCount());
    SAGE_ShowMouse();
    printf("Closing screen\n");
    SAGE_CloseScreen();
  } else {
    SAGE_DisplayError();
  }
}

void checkvbl(void)
{
  ULONG vbl_count;
  
  printf("Opening check screen\n");
  if (SAGE_OpenScreen(640, 480, 8, SSCR_STRICTRES)) {
    if (!SAGE_SetFont("UFOLarge.font", 15)) {
      SAGE_DisplayError();
    }
    SAGE_HideMouse();
    SAGE_PrintText("WAIT A LITTLE BIT", 20, 20);
    SAGE_RefreshScreen();
    SAGE_ResetVblCount();
    Delay(50 * 5);
    vbl_count = SAGE_GetVblCount();
    printf("%d VBL have occured during 5 seconds\n", vbl_count);
    if (vbl_count > 290) {
      printf("VBL is about 60Hz\n");
    } else {
      printf("VBL is about 50Hz\n");
    }
    SAGE_CloseScreen();
  } else {
    SAGE_DisplayError();
  }
}

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (DBLBUF) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    checkdblbuf();
    //checkvbl();
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
