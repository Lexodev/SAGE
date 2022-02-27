/**
 * sonic.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test Sonic graph remap
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 March 2022
 */

#include <proto/dos.h>        // For Delay function

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define TITLE_LAYER           1

#define TRANSP_COLOR          0xF81F

BOOL finish = FALSE;

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTitleLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load title picture");
  if ((picture = SAGE_LoadPicture("data/Title.gif")) != NULL) {
    SAGE_AppliLog("Create title layer");
    if (SAGE_CreateLayerFromPicture(TITLE_LAYER, picture)) {
      SAGE_SetLayerTransparency(TITLE_LAYER, TRANSP_COLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGraphx(VOID)
{
  SAGE_AppliLog("Init graphx");
  if (!InitTitleLayer()) {
    return FALSE;
  }
  return TRUE;
}

VOID ReleaseGraphx(VOID)
{
  SAGE_AppliLog("Release graphx");
  SAGE_ReleaseLayer(TITLE_LAYER);
}

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init demo");
  if (!OpenScreen()) {
    return FALSE;
  }
  SAGE_HideMouse();
  if (!InitGraphx()) {
    return FALSE;
  }
  return TRUE;
}

VOID _Restore(VOID)
{
  SAGE_AppliLog("Restore demo");
  ReleaseGraphx();
  SAGE_ShowMouse();
  SAGE_AppliLog("Closing screen");
  SAGE_CloseScreen();
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
    }
  }
}

VOID _Render(VOID)
{
  // Draw the some pieces of title layer
  SAGE_SetLayerView(TITLE_LAYER, 100, 100, 200, 200);
  SAGE_BlitLayerToScreen(TITLE_LAYER, 0, 200);
  SAGE_SetLayerView(TITLE_LAYER, 10, 50, 300, 250);
  SAGE_BlitLayerToScreen(TITLE_LAYER, 30, 20);
  // Refresh the screen
  SAGE_RefreshScreen();
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Sonic test demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    if (_Init()) {

      SAGE_AppliLog("Entering main loop");
      while (!finish) {
        // Update the demo data
        _Update();
        // Render the demo
        _Render();
      }
      // Restore the demo
      _Restore();
    }

  } else {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("Closing SAGE");
  if (!SAGE_Exit()) {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("End of demo");
}
