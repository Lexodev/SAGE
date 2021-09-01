/**
 * video_picture.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test picture loading & rendering
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>
#include <string.h>
#include <proto/graphics.h>

#include "/src/sage.h"

#define SCREEN_WIDTH        640L
#define SCREEN_HEIGHT       480L

/**
 * Load a picture and display it on screen
 * Usage : video_picture TYPE DEPTH SCREEN
 *  with TYPE = BMP or PNG
 *  and DEPTH = 8 or 16
 *  and SCREEN = 8 or 16
 */
void main(int argc, char **argv)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  LONG depth;
  BOOL bmp = FALSE, clut = FALSE, finish;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (PICTURE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_DumpVideoModes();
    printf("Loading unexistant picture\n");
    picture = SAGE_LoadPicture("erreur.pic");
    SAGE_DisplayError();
    if (argc >= 2 && strcmp(argv[1], "BMP") == 0) {
      bmp = TRUE;
    }
    if (argc >= 3 && strcmp(argv[2], "8") == 0) {
      clut = TRUE;
    }
    depth = 32;
    if (argc == 4) {
      if (strcmp(argv[3], "8") == 0) {
        depth = 8;
      } else if (strcmp(argv[3], "16") == 0) {
        depth = 16;
      } else if (strcmp(argv[3], "24") == 0) {
        depth = 24;
      }
    }
    if (bmp && clut) {
      picture = SAGE_LoadPicture("/data/desert256.bmp");
    } else if (bmp && !clut) { 
      picture = SAGE_LoadPicture("/data/desert.bmp");
    } else if (!bmp && clut) {
      picture = SAGE_LoadPicture("/data/desert256.png");
    } else {
      picture = SAGE_LoadPicture("/data/desert.png");
    }
    SAGE_DisplayError();
    printf("Opening screen\n");
    if (picture != NULL && SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, depth, SSCR_STRICTRES)) {
      printf("Remapping picture\n");
      SAGE_RemapPicture(picture);
      printf("Blitting picture on screen\n");
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
      finish = FALSE;
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          printf("Event polled type %d, code %d, mouse %d,%d\n", event->type, event->code, event->mousex, event->mousey);
          if (event->type == SEVT_MOUSEBT) {
            printf("Exit loop\n");
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY) {
            if (picture->bitmap->depth == SBMP_DEPTH8) {
              if (SAGE_LoadFileColorMap("/data/flame.rgb32", 0, 256, SSCR_RGB32COLOR)) {
                SAGE_RefreshColors(0, 256);
              } else {
                SAGE_DisplayError();
              }
            }
          }
        }
      }
      printf("Closing screen\n");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
    SAGE_ReleasePicture(picture);
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
