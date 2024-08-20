/**
 * tuto2_event.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Opening the screen and wait for user input
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 August 2024
 */

#include <sage/sage.h>

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
#define SCREEN_DEPTH    8

void main(void)
{
  SAGE_Event *event = NULL;
  BOOL finish;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 2 : SCREEN & EVENT / %s", SAGE_GetVersion());
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
      // Clear the screen
      SAGE_ClearScreen();
      // And print some dummy text, the text is always printed in the back buffer
      SAGE_PrintText("Click the mouse or ESC to quit", 20, 20);
      // To see the text we have to switch screen buffers
      SAGE_RefreshScreen();
      // Let's wait for user to click on a mouse button
      finish = FALSE;
      while (!finish) {
        // Read all events raised by the screen
        while ((event = SAGE_GetEvent()) != NULL) {
          // Display some info about the event
          SAGE_AppliLog("Event polled type %d, code %d, mouse %d,%d", event->type, event->code, event->mousex, event->mousey);
          if (event->type == SEVT_MOUSEBT) {
            // If we click on mouse button, we stop the loop
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                // Press the ESC key stop te application
                finish = TRUE;
                break;
              default:
                // If we push a key, we just print a lazy message
                SAGE_AppliLog("Noooo, click the mouse not the keyboard !");
            }
          }
        }
      }
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    }
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 2");
}
