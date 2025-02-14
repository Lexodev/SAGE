/**
 * video_layer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test layer
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define LAYER_INDEX           0
#define LAYER_WIDTH           800L
#define LAYER_HEIGHT          600L

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
      SAGE_AppliLog("Create layer");
      if (SAGE_CreateLayer(LAYER_INDEX, LAYER_WIDTH, LAYER_HEIGHT)) {
        SAGE_AppliLog("Fill layer bitmap with silly pattern");
        pixel = 0;
        bitmap = SAGE_GetLayerBitmap(LAYER_INDEX);
        buffer = (UWORD *)bitmap->bitmap_buffer;
        for (height = 0;height < LAYER_HEIGHT;height++) {
          for (width = 0;width < LAYER_WIDTH;width++) {
            *buffer++ = pixel++;
          }
        }
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
        xpos = (xpos + 2) % (800 - SCREEN_WIDTH);
        ypos = (ypos + 1) % (600 - SCREEN_HEIGHT);
        if (!SAGE_SetLayerView(LAYER_INDEX, xpos, ypos, SCREEN_WIDTH, SCREEN_HEIGHT)) {
          SAGE_AppliLog("Error SetLayerView !");
          SAGE_DisplayError();
          finish = TRUE;
        }
        if (!SAGE_BlitLayerToScreen(LAYER_INDEX, 0, 0)) {
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
      SAGE_ReleaseLayer(LAYER_INDEX);
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
