/**
 * video_zoom.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test zoom functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
//#define SCREEN_DEPTH          8L
#define SCREEN_DEPTH          16L

//#define SPR_TRANSP            1
#define SPR_TRANSP            0xF81F
#define SPR_BANK              0
#define SPR_INDEX             0
#define SPR_NUMBER            4

#define BG_LAYER              0

/**
 * Sprite 6,4 96x112
 */
void Test8bitsZoomAndClip(void)
{
  SAGE_SetSpriteZoom(SPR_BANK, SPR_INDEX, 1.5F, 1.5F);
  SAGE_BlitSpriteToScreen(SPR_BANK, SPR_INDEX, 320, 240);
  SAGE_RefreshScreen();
}

/**
 * Sprite 0,0 176x259
 */
void Test16bitsZoomAndClip(void)
{
  SAGE_SetSpriteZoom(SPR_BANK, SPR_INDEX, 1.5F, 1.5F);
  SAGE_BlitSpriteToScreen(SPR_BANK, SPR_INDEX, 320, 240);
  SAGE_RefreshScreen();
}

void main(void)
{
  SAGE_Picture * picture = NULL;
  SAGE_Event * event = NULL;
  LONG sprxpos = 320, sprypos = 240;
  FLOAT zoom = 1.0, step = 0.05;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (ZOOM) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
      SAGE_HideMouse();
// 8Bits assets
      /*printf("Load sprite picture and create sprite bank\n");
      if ((picture = SAGE_LoadPicture("/data/troll_sprite.gif")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        if (SAGE_CreateSpriteBank(SPR_BANK, SPR_NUMBER, picture)) {
          SAGE_SetSpriteBankTransparency(SPR_BANK, SPR_TRANSP);
          if (!SAGE_AddSpriteToBank(SPR_BANK, SPR_INDEX, 6, 4, 96, 112, SSPR_HS_MIDDLE)) {
            finish = TRUE;
          }
        }
        SAGE_ReleasePicture(picture);
      }
      printf("Load a picture for the background\n");
      if ((picture = SAGE_LoadPicture("/data/desert256.png")) == NULL) {
        finish = FALSE;
      }*/
//      Test8bitsZoomAndClip();

// 16Bits assets
      printf("Load sprite picture and create sprite bank\n");
      if ((picture = SAGE_LoadPicture("/data/fighter.bmp")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        if (SAGE_CreateSpriteBank(SPR_BANK, SPR_NUMBER, picture)) {
          SAGE_SetSpriteBankTransparency(SPR_BANK, SPR_TRANSP);
          if (!SAGE_AddSpriteToBank(SPR_BANK, SPR_INDEX, 0, 0, 176, 259, SSPR_HS_MIDDLE)) {
            finish = TRUE;
          }
        }
        SAGE_ReleasePicture(picture);
      }
      printf("Load a picture for the background\n");
      if ((picture = SAGE_LoadPicture("/data/background.bmp")) == NULL) {
        finish = FALSE;
      }
//      Test16bitsZoomAndClip();
      while (!finish) {
        SAGE_BlitPictureToScreen(picture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
        SAGE_SetSpriteZoom(SPR_BANK, 0, zoom, zoom);
        if (!SAGE_BlitSpriteToScreen(SPR_BANK, SPR_INDEX, sprxpos, sprypos)) {
          finish = TRUE;
          SAGE_DisplayError();
        }
        SAGE_RefreshScreen();
        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                printf("Exit loop\n");
                finish = TRUE;
                break;
              case SKEY_FR_LEFT:
                sprxpos -= 4;
                if (sprxpos < -128) sprxpos = -128;
                break;
              case SKEY_FR_RIGHT:
                sprxpos += 4;
                if (sprxpos > SCREEN_WIDTH+128) sprxpos = SCREEN_WIDTH+128;
                break;
              case SKEY_FR_UP:
                sprypos -= 4;
                if (sprxpos < -128) sprxpos = -128;
                break;
              case SKEY_FR_DOWN:
                sprypos += 4;
                if (sprxpos > SCREEN_HEIGHT+128) sprxpos = SCREEN_HEIGHT+128;
                break;
              case SKEY_FR_SPACE:
                sprxpos = 320;
                sprypos = 240;
                zoom = 1.0;
                break;
              case SKEY_FR_A:
                zoom += step;
                if (zoom > 4.0) zoom = 4.0;
                break;
              case SKEY_FR_Q:
                zoom -= step;
                if (zoom < 0) zoom = 0;
                break;
            }
          }
        }
      }
      printf("Release picture\n");
      SAGE_ReleasePicture(picture);
      printf("Release sprites\n");
      SAGE_ReleaseSpriteBank(SPR_BANK);
      SAGE_ShowMouse();
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
