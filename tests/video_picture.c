/**
 * video_picture.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test picture loading & rendering
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH        640L
#define SCREEN_HEIGHT       480L

/**
 * Load a picture and display it on screen
 * Usage : video_picture TYPE DEPTH SCREEN
 *  with TYPE = BMP or PNG
 *  and DEPTH = 8 or 16
 *  and SCREEN = 8, 16, 24 or 32
 */
void main(int argc, char **argv)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  LONG depth = 32;
  BOOL bmp = FALSE, clut = FALSE, finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (PICTURE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    if (argc >= 2 && strcmp(argv[1], "BMP") == 0) {
      bmp = TRUE;
    }
    if (argc >= 3 && strcmp(argv[2], "8") == 0) {
      clut = TRUE;
    }
    if (argc >= 4) {
      if (strcmp(argv[3], "8") == 0) {
        depth = 8;
      } else if (strcmp(argv[3], "16") == 0) {
        depth = 16;
      } else if (strcmp(argv[3], "24") == 0) {
        depth = 24;
      }
    }
    SAGE_AppliLog("User parameters DEPTH=%d , BMP=%d , CLUT=%d", depth, bmp, clut);
    SAGE_AppliLog("Loading picture");
    if (bmp && clut) {
      picture = SAGE_LoadPicture("data/desert256.bmp");
    } else if (bmp && !clut) { 
      picture = SAGE_LoadPicture("data/desert.bmp");
    } else if (!bmp && clut) {
      picture = SAGE_LoadPicture("data/desert256.png");
    } else {
      picture = SAGE_LoadPicture("data/desert.png");
    }
    SAGE_DisplayError();
    SAGE_AppliLog("Opening screen");
    if (picture != NULL && SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, depth, SSCR_STRICTRES)) {
      SAGE_AppliLog("Remapping picture");
      SAGE_RemapPicture(picture);
      SAGE_AppliLog("Blitting picture on screen");
      if (!SAGE_BlitPictureToScreen(picture, 0, 0, picture->bitmap->width, picture->bitmap->height, 0, 0)) {
        SAGE_DisplayError();
      }
      if (picture->bitmap->depth == SBMP_DEPTH8) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
      }
      if (!SAGE_RefreshScreen()) {
        SAGE_DisplayError();
      }
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          SAGE_AppliLog("Event polled type %d, code %d, mouse %d,%d", event->type, event->code, event->mousex, event->mousey);
          if (event->type == SEVT_MOUSEBT) {
            SAGE_AppliLog("Exit loop");
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY) {
            if (picture->bitmap->depth == SBMP_DEPTH8) {
              if (SAGE_LoadFileColorMap("data/flame.rgb32", 0, 256, SSCR_RGB32COLOR)) {
                SAGE_RefreshColors(0, 256);
              } else {
                SAGE_DisplayError();
              }
            }
          }
        }
      }
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
