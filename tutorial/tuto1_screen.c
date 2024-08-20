/**
 * tuto1_screen.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Opening the screen and write some text
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
  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 1 : SCREEN & TEXT / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
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
      SAGE_PrintText("HELLO WORLD !", 20 , 20);
      SAGE_PrintText("WAITING 4 SECONDS NOW", 20, 80);
      // To see the text we have to switch screen buffers
      SAGE_RefreshScreen();
      // Just make a little pause
      SAGE_Pause(50*4);
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    }
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 1");
}
