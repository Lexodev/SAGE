/**
 * tuto7_sprite.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Sprite and scrolling
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define SKY_WIDTH             256L
#define SKY_HEIGHT            80L

#define BACK_LAYER            0

#define SCROLL_WIDTH          1024L
#define SCROLL_HEIGHT         192L
#define SCROLL_TRANSP         1

#define FRONT_LAYER           1

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  ULONG xbg_offset, xfg_offset;
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
      if ((picture = SAGE_LoadPicture("/data/troll_sky.gif")) != NULL) {
        // Let's create a background layer
        if (SAGE_CreateLayer(BACK_LAYER, SKY_WIDTH+SCREEN_WIDTH, SKY_HEIGHT)) {
          // And copy the picture on this layer, we should blit it 3 times to totally fill the layer width
          SAGE_BlitPictureToLayer(picture, 0, 0, SKY_WIDTH, SKY_HEIGHT, BACK_LAYER, 0, 0);
          SAGE_BlitPictureToLayer(picture, 0, 0, SKY_WIDTH, SKY_HEIGHT, BACK_LAYER, SKY_WIDTH, 0);
          SAGE_BlitPictureToLayer(picture, 0, 0, (SCREEN_WIDTH-SKY_WIDTH), SKY_HEIGHT, BACK_LAYER, SKY_WIDTH*2, 0);
          // Load the picture palette to the screen
          SAGE_LoadPictureColorMap(picture);
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
      if ((picture = SAGE_LoadPicture("/data/troll_bg.gif")) != NULL) {
        // Let's create a foreground layer
        if (SAGE_CreateLayer(FRONT_LAYER, (SCROLL_WIDTH+SCREEN_WIDTH), SCROLL_HEIGHT)) {
          // And copy the picture on this layer
          SAGE_BlitPictureToLayer(picture, 0, 0, SCROLL_WIDTH, SCROLL_HEIGHT, FRONT_LAYER, 0, 0);
          SAGE_BlitPictureToLayer(picture, 0, 0, SCREEN_WIDTH, SCROLL_HEIGHT, FRONT_LAYER, SCROLL_WIDTH, 0);
          // Now let's set the transparency color
          SAGE_SetLayerTransparency(FRONT_LAYER, SCROLL_TRANSP);
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
        // Refresh the screen colors
        SAGE_RefreshColors(0, 256);
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
          SAGE_SetLayerView(BACK_LAYER, xbg_offset, 0, SCREEN_WIDTH, SKY_HEIGHT);
          // Set the view of the foreground layer
          SAGE_SetLayerView(FRONT_LAYER, xfg_offset, 0, SCREEN_WIDTH, SCROLL_HEIGHT);
          // First blit the background layer to the screen
          SAGE_BlitLayerToScreen(BACK_LAYER, 0, 0);
          // Then the foreground layer
          SAGE_BlitLayerToScreen(FRONT_LAYER, 0, 0);
          // To see the result we have to switch screen buffers
          SAGE_RefreshScreen();
          // Now we can update the layers offset to do the scrolling effect
          xbg_offset += 1;  // One pixel for the background
          if (xbg_offset >= SKY_WIDTH) {
            xbg_offset = 0;
          }
          xfg_offset += 2; // Two pixel for the foregrounf
          if (xfg_offset >= SCROLL_WIDTH) {
            xfg_offset = 0;
          }
        }
      } else {
        SAGE_AppliLog("Layers are not OK");
        SAGE_DisplayError();
      }
      // Release the layers
      SAGE_ReleaseLayer(FRONT_LAYER);
      SAGE_ReleaseLayer(BACK_LAYER);
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    } else {
      // Display the last error
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 6");
}
