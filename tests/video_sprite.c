/**
 * video_sprite.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test sprites
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

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
  SAGE_Picture *picture = NULL;
  SAGE_Event *event = NULL;
  ULONG sprxpos = 0, sprypos = 250, sprnpos = 0;
  UWORD sprite = 0, delay = 0;
  BOOL finish = FALSE, ok = TRUE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (SPRITE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_AppliLog("Load sprite picture and create sprite bank");
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
          SAGE_DisplayError();
        }
        SAGE_ReleasePicture(picture);
      } else {
        ok = FALSE;
        SAGE_DisplayError();
      }
      if (ok) {
        SAGE_AppliLog("Load a picture for the background");
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
              }
            }
          }
          if (ok) {
            if (!SAGE_BlitPictureToScreen(picture, sprnpos, sprypos, SPR_WIDTH, SPR_HEIGHT, sprnpos, sprypos)) {
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
            if (!SAGE_BlitSpriteToScreen(SPR_BANK, sprite, sprxpos, sprypos)) {
              SAGE_AppliLog("Error blitting sprite (%d) !", sprite);
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
