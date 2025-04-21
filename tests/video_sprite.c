/**
 * video_sprite.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test sprites
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640
#define SCREEN_HEIGHT         480
#define SCREEN_DEPTH          16

#define NB_SPRITES            6
#define SPR_TRANSP            0xFF00FF
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
  SAGE_Picture *picture = NULL;
  SAGE_Event *event = NULL;
  ULONG spr1xpos = 0, spr1ypos = 150, spr2xpos = 0, spr2ypos = 300, spr3xpos = 0, spr3ypos = 50;
  UWORD sprite = 0, delay = 0;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (SPRITE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_AppliLog("Load sprite picture and create sprite bank");
      if ((picture = SAGE_LoadPicture("data/troll_sprite.gif")) != NULL) {
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
          finish = TRUE;
          SAGE_DisplayError();
        }
        SAGE_ReleasePicture(picture);
      } else {
        finish = TRUE;
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Load a picture for the background");
      if ((picture = SAGE_LoadPicture("data/desert.png")) == NULL) {
        finish = TRUE;
        SAGE_DisplayError();
      }
      delay = 0;
      sprite = 0;
      while (!finish) {
        SAGE_SetTraceDebug(FALSE);
        if (SAGE_IsFrontMostScreen()) {
          while ((event = SAGE_GetEvent()) != NULL) {
            SAGE_AppliLog(
              "Event polled type %d, code %d, mouse %d,%d",
              event->type,
              event->code,
              event->mousex,
              event->mousey
            );
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
          if (!SAGE_BlitPictureToScreen(picture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0)) {
            SAGE_AppliLog("Error blitting picture !");
            finish = TRUE;
            SAGE_DisplayError();
          }
          if (++delay >= SPR_DELAY) {
            delay = 0;
            if (++sprite >= NB_SPRITES) {
              sprite = 0;
            }
          }
          SAGE_SetSpriteZoom(SPR_BANK, sprite, 1.0f, 1.0f);
          if (!SAGE_BlitSpriteToScreen(SPR_BANK, sprite, spr1xpos, spr1ypos)) {
            SAGE_AppliLog("Error blitting sprite (%d) !", sprite);
            finish = TRUE;
            SAGE_DisplayError();
          }
          spr1xpos++;
          if (spr1xpos >= SCREEN_WIDTH) {
            spr1xpos = 0;
          }
          SAGE_SetSpriteZoom(SPR_BANK, sprite, 1.5f, 1.5f);
          if (!SAGE_BlitSpriteToScreen(SPR_BANK, sprite, spr2xpos, spr2ypos)) {
            SAGE_AppliLog("Error blitting sprite (%d) !", sprite);
            finish = TRUE;
            SAGE_DisplayError();
          }
          spr2xpos++;
          if (spr2xpos >= SCREEN_WIDTH) {
            spr2xpos = 0;
          }
          SAGE_SetSpriteZoom(SPR_BANK, sprite, 0.5f, 0.5f);
          if (!SAGE_BlitSpriteToScreen(SPR_BANK, sprite, spr3xpos, spr3ypos)) {
            SAGE_AppliLog("Error blitting sprite (%d) !", sprite);
            finish = TRUE;
            SAGE_DisplayError();
          }
          spr3xpos++;
          if (spr3xpos >= SCREEN_WIDTH) {
            spr3xpos = 0;
          }
        }
        if (!SAGE_RefreshScreen()) {
          finish = TRUE;
          SAGE_DisplayError();
        }
      }
      SAGE_AppliLog("Release picture");
      SAGE_ReleasePicture(picture);
      SAGE_AppliLog("Release sprites");
      SAGE_ReleaseSpriteBank(SPR_BANK);
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
