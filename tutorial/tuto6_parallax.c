/**
 * tuto6_parallax.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Parallax horizontal scrolling with transparent layer
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 August 2024
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         256L
#define SCREEN_DEPTH          16L

#define BG_WIDTH              640L
#define BG_HEIGHT             256L
#define BG_LAYER              0

#define FG_WIDTH              640L
#define FG_HEIGHT             256L
#define FG_LAYER              1
#define TRANSP_COLOR          0xFF00FF

void main(void)
{
  SAGE_Event *event = NULL;
  SAGE_Picture *picture = NULL;
  ULONG xbg_offset, xfg_offset;
  BOOL finish, ok = TRUE;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 6 : PARALLAX SCROLLING / %s", SAGE_GetVersion());
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
      if ((picture = SAGE_LoadPicture("data/background.png")) != NULL) {
        // Let's create a background layer
        if (SAGE_CreateLayer(BG_LAYER, BG_WIDTH, BG_HEIGHT)) {
          // And copy the picture on this layer
          SAGE_BlitPictureToLayer(picture, 0, 0, BG_WIDTH, BG_HEIGHT, BG_LAYER, 0, 0);
        } else {
          ok = FALSE;
        }
        // We don't need this picture anymore, release it before loading the foreground
        SAGE_ReleasePicture(picture);
      } else {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Loading the foreground picture");
      // Load the foreground picture
      if ((picture = SAGE_LoadPicture("data/foreground.png")) != NULL) {
        // Let's create a foreground layer
        if (SAGE_CreateLayer(FG_LAYER, FG_WIDTH, FG_HEIGHT)) {
          // And copy the picture on this layer
          SAGE_BlitPictureToLayer(picture, 0, 0, FG_WIDTH, FG_HEIGHT, FG_LAYER, 0, 0);
          // Now let's set the transparency color
          SAGE_SetLayerTransparency(FG_LAYER, TRANSP_COLOR);
        } else {
          ok = FALSE;
        }
        // We don't need this picture anymore
        SAGE_ReleasePicture(picture);
      } else {
        SAGE_DisplayError();
      }
      // Continue if the layers are OK
      if (ok) {
        // Let's init our layer coordinates
        xbg_offset = 0;
        xfg_offset = 0;
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
          SAGE_SetLayerView(BG_LAYER, xbg_offset, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
          // Set the view of the foreground layer
          SAGE_SetLayerView(FG_LAYER, xfg_offset, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
          // First blit the background layer to the screen
          SAGE_BlitLayerToScreen(BG_LAYER, 0, 0);
          // Then the foreground layer
          SAGE_BlitLayerToScreen(FG_LAYER, 0, 0);
          // To see the result we have to switch screen buffers
          SAGE_RefreshScreen();
          // Now we can update the layers offset to do the scrolling effect
          xbg_offset += 1;  // One pixel for the background
          if (xbg_offset >= BG_WIDTH) {
            xbg_offset = 0;
          }
          xfg_offset += 2; // Two pixel for the foreground
          if (xfg_offset >= FG_WIDTH) {
            xfg_offset = 0;
          }
        }
      } else {
        SAGE_AppliLog("Layers are not OK");
        SAGE_DisplayError();
      }
      // Release the layers
      SAGE_ReleaseLayer(FG_LAYER);
      SAGE_ReleaseLayer(BG_LAYER);
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    }
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 6");
}
