/**
 * video_zoom.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test zoom functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define SPR_TRANSP            0xFF00FF
#define SPR_BANK              0
#define SPR_INDEX             0
#define SPR_NUMBER            4

#define BG_LAYER              0

void main(void)
{
  SAGE_Picture *picture = NULL;
  SAGE_Event *event = NULL;
  LONG sprxpos = 320, sprypos = 240;
  FLOAT zoom = 1.0, step = 0.02;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (ZOOM) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INTERRUPTION)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      if (!SAGE_EnableFrameCount(TRUE)) {
        SAGE_ErrorLog("Can't activate frame rate counter !");
      }
      SAGE_MaximumFPS(60);
      SAGE_VerticalSynchro(FALSE);
      SAGE_AppliLog("Load sprite picture and create sprite bank");
      if ((picture = SAGE_LoadPicture("data/vampire.bmp")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        if (SAGE_CreateSpriteBank(SPR_BANK, SPR_NUMBER, picture)) {
          SAGE_SetSpriteBankTransparency(SPR_BANK, SPR_TRANSP);
          if (!SAGE_AddSpriteToBank(SPR_BANK, SPR_INDEX, 0, 0, 360, 360, SSPR_HS_MIDDLE)) {
            finish = TRUE;
          }
        }
        SAGE_ReleasePicture(picture);
      }
      SAGE_AppliLog("Load a picture for the background");
      if ((picture = SAGE_LoadPicture("data/desert.bmp")) == NULL) {
        finish = FALSE;
      }
      while (!finish) {
        SAGE_BlitPictureToScreen(picture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
        zoom += step;
        if (zoom > 1.4) step *= -1.0;
        if (zoom < 0.1) step *= -1.0;
        SAGE_SetSpriteZoom(SPR_BANK, 0, zoom, zoom);
        if (!SAGE_BlitSpriteToScreen(SPR_BANK, SPR_INDEX, sprxpos, sprypos)) {
          finish = TRUE;
          SAGE_DisplayError();
        }
        // Draw the fps counter
        SAGE_PrintFText(10, 10, "%d fps", SAGE_GetFps());
        SAGE_RefreshScreen();
        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                SAGE_AppliLog("Exit loop");
                finish = TRUE;
                break;
              case SKEY_FR_LEFT:
                sprxpos -= 10;
                if (sprxpos < -128) sprxpos = -128;
                break;
              case SKEY_FR_RIGHT:
                sprxpos += 10;
                if (sprxpos > SCREEN_WIDTH+128) sprxpos = SCREEN_WIDTH+128;
                break;
              case SKEY_FR_UP:
                sprypos -= 10;
                if (sprxpos < -128) sprxpos = -128;
                break;
              case SKEY_FR_DOWN:
                sprypos += 10;
                if (sprxpos > SCREEN_HEIGHT+128) sprxpos = SCREEN_HEIGHT+128;
                break;
              case SKEY_FR_SPACE:
                sprxpos = SCREEN_WIDTH / 2;
                sprypos = SCREEN_HEIGHT / 2;
                zoom = 1.0;
                break;
            }
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
