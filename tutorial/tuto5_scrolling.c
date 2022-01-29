/**
 * tuto5_scrolling.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Simple vertical scroll of a picture
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include "/src/sage.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
#define SCREEN_DEPTH    16

#define PICTURE_WIDTH   640
#define PICTURE_HEIGHT  480

#define SCROLL_LAYER    0

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  ULONG y_offset;
  BOOL finish;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 5 : LAYER SCROLLING / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO)) {
    // Open the application screen, screen always use double buffer
    // We use SSCR_STRICTRES option to tell that we don't accept any other resolution
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      // Let's hide the mouse pointer
      SAGE_HideMouse();
      // Load our picture by using the datatypes libraries
      if ((picture = SAGE_LoadPicture("/data/desert.png")) != NULL) {
        // Let's create a layer that is twice the height of the screen
        // A layer has automatically a bitmap that is compliant with the screen bitmap
        if (SAGE_CreateLayer(SCROLL_LAYER, SCREEN_WIDTH, SCREEN_HEIGHT * 2)) {
          // Let's now copy our picture into the layer
          SAGE_BlitPictureToLayer(picture, 0, 0, PICTURE_WIDTH, PICTURE_HEIGHT, SCROLL_LAYER, 0, 0);
          // A second time for the scrolling
          SAGE_BlitPictureToLayer(picture, 0, 0, PICTURE_WIDTH, PICTURE_HEIGHT, SCROLL_LAYER, 0, SCREEN_HEIGHT);
          // We don't need picture anymore, release it
          SAGE_ReleasePicture(picture);
          // Now let's scroll this picture vertically until the user click on mouse button or press ESC key
          y_offset = 0;
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
            // Set the view of the layer, the view define what part of the layer will be blit to the screen
            SAGE_SetLayerView(SCROLL_LAYER, 0, y_offset, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Blit the layer to the screen
            SAGE_BlitLayerToScreen(SCROLL_LAYER, 0, 0);
            // To see the result we have to switch screen buffers
            SAGE_RefreshScreen();
            // Scroll by 2 lines, go back to top if we reach the bottom of the picture
            y_offset += 2;
            if (y_offset >= SCREEN_HEIGHT) {
              y_offset = 0;
            }
          }
        } else {
          SAGE_DisplayError();
        }
      } else {
        SAGE_DisplayError();
      }
      // Release the layer
      SAGE_ReleaseLayer(SCROLL_LAYER);
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
  SAGE_AppliLog("End of tutorial 5");
}
