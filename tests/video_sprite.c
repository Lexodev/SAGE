/**
 * video_sprite.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test sprites
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define NB_SPRITES            6
#define SPR_TRANSP            1
#define SPR_DELAY             10
#define SPR_BANK              0
#define SPR_WIDTH             100
#define SPR_HEIGHT            112

ULONG sprites[NB_SPRITES*4] = {
  6,4,96,112,
  108,4,80,112,
  190,4,80,112,
  278,4,96,112,
  380,4,80,112,
  470,4,80,112
};

void main(void)
{
  SAGE_Picture * picture = NULL;
  SAGE_Event * event = NULL;
  ULONG sprxpos = 0, sprypos = 250, sprnpos = 0;
  UWORD sprite = 0, delay = 0;
  BOOL finish = FALSE, ok = TRUE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (SPRITE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
      SAGE_HideMouse();
      printf("Load sprite picture and create sprite bank\n");
      if ((picture = SAGE_LoadPicture("/data/troll_sprite.gif")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
          SAGE_SetSpriteBankTransparency(SPR_BANK, SPR_TRANSP);
          for (sprite = 0;sprite < NB_SPRITES;sprite++) {
            if (!SAGE_AddSpriteToBank(SPR_BANK, sprite, sprites[sprite*4], sprites[sprite*4+1], sprites[sprite*4+2], sprites[sprite*4+3], SSPR_HS_TOPLEFT)) {
              finish = TRUE;
              SAGE_DisplayError();
            }
          }
        } else {
          ok = FALSE;
        }
        SAGE_ReleasePicture(picture);
      }
      if (ok) {
        printf("Load a picture for the background\n");
        if ((picture = SAGE_LoadPicture("/data/desert256.png")) == NULL) {
          finish = TRUE;
          SAGE_DisplayError();
        }
        SAGE_BlitPictureToScreen(picture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
        SAGE_RefreshScreen();
        SAGE_BlitPictureToScreen(picture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
        SAGE_RefreshScreen();
        SAGE_BlitPictureToScreen(picture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
      }
      delay = 0;
      sprite = 0;
      while (!finish && ok) {
        if (SAGE_IsFrontMostScreen()) {
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
          if (ok) {
            if (!SAGE_BlitPictureToScreen(picture, sprnpos, sprypos, SPR_WIDTH, SPR_HEIGHT, sprnpos, sprypos)) {
              printf("Error blitting picture !\n");
              finish = TRUE;
              SAGE_DisplayError();
            }
            if (++delay >= SPR_DELAY) {
              delay = 0;
              if (++sprite >= NB_SPRITES) {
                sprite = 0;
              }
            }
            if (!SAGE_BlitSpriteToScreen(SPR_BANK, sprite, sprxpos, sprypos)) {
              printf("Error blitting sprite (%d) !\n", sprite);
              finish = TRUE;
              SAGE_DisplayError();
            }
            sprnpos = sprxpos;
            sprxpos++;
            if (sprxpos >= SCREEN_WIDTH) {
              sprxpos = 0;
            }
          }
          if (!SAGE_RefreshScreen()) {
            finish = TRUE;
            SAGE_DisplayError();
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
