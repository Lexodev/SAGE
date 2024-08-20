/**
 * tuto7_sprite.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Sprite and scrolling
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 August 2024
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define BG_WIDTH              1280L
#define BG_HEIGHT             720L
#define BACK_LAYER            0

#define NB_SPRITES            1
#define SPR_TRANSP            0xff00ff
#define SPR_LEFT              0L
#define SPR_TOP               0L
#define SPR_WIDTH             360L
#define SPR_HEIGHT            256L
#define SPR_BANK              0
#define SPR_NUM               0

void main(void)
{
  SAGE_Event *event = NULL;
  SAGE_Picture *picture = NULL;
  LONG bg_yoffset;
  BOOL finish, ok = TRUE;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 7 : SPRITE / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO)) {
    // Open the application screen, screen always use double buffer
    // We use SSCR_STRICTRES option to tell that we don't accept any other resolution
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      // Let's hide the mouse pointer
      SAGE_HideMouse();
      SAGE_AppliLog("Loading the background picture");
      // Load the background picture
      if ((picture = SAGE_LoadPicture("data/galaxy.png")) != NULL) {
        // Let's create a background layer
        if (!SAGE_CreateLayerFromPicture(BACK_LAYER, picture)) {
          ok = FALSE;
          SAGE_DisplayError();
        }
        // We don't need this picture anymore, release it before loading the sprite
        SAGE_ReleasePicture(picture);
      } else {
        ok = FALSE;
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Loading the sprite picture");
      // Load the sprite picture
      if ((picture = SAGE_LoadPicture("data/spaceship.png")) != NULL) {
        // Let's create a sprite bank
        if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
          // Set the transparency color
          SAGE_SetSpriteBankTransparency(SPR_BANK, SPR_TRANSP);
          // Add our sprite and set the hotspot as the middle of the sprite
          if (!SAGE_AddSpriteToBank(SPR_BANK, SPR_NUM, SPR_LEFT, SPR_TOP, SPR_WIDTH, SPR_HEIGHT, SSPR_HS_MIDDLE)) {
            ok = FALSE;
            SAGE_DisplayError();
          }
        } else {
          ok = FALSE;
          SAGE_DisplayError();
        }
        // We don't need this picture anymore
        SAGE_ReleasePicture(picture);
      } else {
        ok = FALSE;
        SAGE_DisplayError();
      }
      // Continue if everything is OK
      if (ok) {
        // Let's init our layer coordinates
        bg_yoffset = BG_HEIGHT - 1;
        finish = FALSE;
        while (!finish) {
          // Read all events raised by the screen
          while ((event = SAGE_GetEvent()) != NULL) {
            // Display some info about the event
            SAGE_AppliLog("Event polled type %d, code %d, mouse %d,%d", event->type, event->code, event->mousex, event->mousey);
            if (event->type == SEVT_MOUSEBT) {
              // If we click on mouse button, we stop the loop
              finish = TRUE;
            } else if (event->type == SEVT_RAWKEY && event->code == SKEY_FR_ESC) {
              // If we press the ESC key, we stop the loop
              finish = TRUE;
            }
          }
          // Set the view of the background layer, the view define what part of the layer will be blit to the screen
          SAGE_SetLayerView(BACK_LAYER, 0, bg_yoffset, SCREEN_WIDTH, SCREEN_HEIGHT);
          // First blit the background layer to the screen
          SAGE_BlitLayerToScreen(BACK_LAYER, 0, 0);
          // Then blit the sprite
          SAGE_BlitSpriteToScreen(SPR_BANK, SPR_NUM, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
          // To see the result we have to switch screen buffers
          SAGE_RefreshScreen();
          // Now we can update the layers offset to do the scrolling effect
          bg_yoffset -= 1;  // One pixel for the background
          if (bg_yoffset < 0) {
            bg_yoffset = BG_HEIGHT - 1;
          }
        }
      } else {
        SAGE_AppliLog("Init error");
        SAGE_DisplayError();
      }
      // Release the graphics
      SAGE_ReleaseLayer(BACK_LAYER);
      SAGE_ReleaseSpriteBank(SPR_BANK);
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    }
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 7");
}
