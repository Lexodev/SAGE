/**
 * video_layer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test layer
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640
#define SCREEN_HEIGHT         480
#define SCREEN_DEPTH          16

#define LAYER_FRONT           0
#define LAYER_BACK            1
#define LAYER_WIDTH           800
#define LAYER_HEIGHT          600
#define TRANSPARENT_COLOR     0xFF00FF

void main(void)
{
  SAGE_Event *event = NULL;
  SAGE_Bitmap *bitmap = NULL;
  UWORD pixel, *buffer;
  ULONG width, height, xpos = 0, ypos = 0;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (LAYER) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_AppliLog("Create layers");
      if (SAGE_CreateLayer(LAYER_FRONT, LAYER_WIDTH, LAYER_HEIGHT) && SAGE_CreateLayer(LAYER_BACK, LAYER_WIDTH, LAYER_HEIGHT)) {
        SAGE_AppliLog("Fill back layer bitmap with silly pattern");
        pixel = 0;
        bitmap = SAGE_GetLayerBitmap(LAYER_BACK);
        buffer = (UWORD *)bitmap->bitmap_buffer;
        for (height = 0;height < LAYER_HEIGHT;height++) {
          for (width = 0;width < LAYER_WIDTH;width++) {
            *buffer++ = pixel++;
          }
        }
        SAGE_AppliLog("Fill front layer bitmap with semi transparent pattern");
        bitmap = SAGE_GetLayerBitmap(LAYER_FRONT);
        buffer = (UWORD *)bitmap->bitmap_buffer;
        for (height = 0;height < LAYER_HEIGHT;height++) {
          pixel = 0xffff;
          for (width = 0;width < LAYER_WIDTH;width++) {
            if (height > 150 && height < 250) {
              *buffer++ = 0xf81f;
            } else {
              *buffer++ = pixel--;
            }
          }
        }
        SAGE_SetLayerTransparency(LAYER_FRONT, TRANSPARENT_COLOR);
      } else {
        finish = TRUE;
      }
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          SAGE_AppliLog(
            "Event polled type %d, code %d, mouse %d,%d",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_MOUSEBT) {
            SAGE_AppliLog("Exit loop");
            finish = TRUE;
          }
        }
        if (!SAGE_SetLayerView(LAYER_BACK, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)) {
          SAGE_AppliLog("Error SetLayerView !");
          SAGE_DisplayError();
          finish = TRUE;
        }
        if (!SAGE_BlitLayerToScreen(LAYER_BACK, 0, 0)) {
          SAGE_AppliLog("Error BlitLayerToScreen !");
          SAGE_DisplayError();
          finish = TRUE;
        }
        xpos = (xpos + 2) % (800 - SCREEN_WIDTH);
        ypos = (ypos + 1) % (600 - SCREEN_HEIGHT);
        if (!SAGE_SetLayerView(LAYER_FRONT, xpos, ypos, SCREEN_WIDTH, SCREEN_HEIGHT)) {
          SAGE_AppliLog("Error SetLayerView !");
          SAGE_DisplayError();
          finish = TRUE;
        }
        if (!SAGE_BlitLayerToScreen(LAYER_FRONT, 0, 0)) {
          SAGE_AppliLog("Error BlitLayerToScreen !");
          SAGE_DisplayError();
          finish = TRUE;
        }
        if (!SAGE_RefreshScreen()) {
          SAGE_AppliLog("Error RefreshScreen !");
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
      SAGE_ReleaseLayer(LAYER_BACK);
      SAGE_ReleaseLayer(LAYER_FRONT);
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
