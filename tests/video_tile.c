/**
 * video_tile.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test tiles
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define NB_TILES              20
#define MAP_WIDTH             10
#define MAP_HEIGHT            10
#define TILE_WIDTH            32
#define TILE_HEIGHT           20
#define TILE_BANK             0

/** A fake tile map */
UWORD TileMap[MAP_WIDTH*MAP_HEIGHT] = {
  0,1,2,3,4,5,6,7,8,9,
  9,8,7,6,5,4,3,2,1,0,
  10,11,12,13,14,15,16,17,18,19,
  19,18,17,16,15,14,13,12,11,10,
  0,2,4,6,8,10,12,14,16,18,
  1,3,5,7,9,11,13,15,17,19,
  18,16,14,12,10,8,6,4,2,0,
  19,17,15,13,11,9,7,5,3,1,
  1,5,9,7,5,3,8,6,2,4,
  15,12,14,18,13,16,19,10,11,17
};

void main(void)
{
  SAGE_Picture * picture = NULL;
  SAGE_Event * event = NULL;
  ULONG x_pos = 0, y_pos = 0;
  UWORD tile = 0;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (TILE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      printf("Load tile picture and create tile bank\n");
      if ((picture = SAGE_LoadPicture("/data/Odysseus_Tiles.bmp")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        if (SAGE_CreateTileBank(TILE_BANK, TILE_WIDTH, TILE_HEIGHT, NB_TILES, picture)) {
          if (!SAGE_AddTilesToBank(TILE_BANK)) {
            finish = TRUE;
            SAGE_DisplayError();
          }
        } else {
          finish = TRUE;
          SAGE_DisplayError();
        }
        SAGE_ReleasePicture(picture);
      }
      while (!finish) {
        if (SAGE_IsFrontMostScreen()) {
          while ((event = SAGE_GetEvent()) != NULL) {
            printf(
              "Event polled type %d, code %d, mouse %d,%d\n",
              event->type,
              event->code,
              event->mousex,
              event->mousey
            );
            if (event->type == SEVT_RAWKEY) {
              switch (event->code) {
                case SKEY_FR_ESC:
                  printf("Exit loop\n");
                  finish = TRUE;
                  break;
              }
            }
          }
          x_pos = 0;
          y_pos = 0;
          for (tile = 0;tile < (MAP_WIDTH*MAP_HEIGHT);tile++) {
            if (!SAGE_BlitTileToScreen(TILE_BANK, TileMap[tile], x_pos, y_pos)) {
              finish = TRUE;
              SAGE_DisplayError();
            }
            x_pos += TILE_WIDTH;
            if ((tile+1) % 10 == 0) {
              y_pos += TILE_HEIGHT;
              x_pos = 0;
            }
          }
          if (!SAGE_RefreshScreen()) {
            finish = TRUE;
            SAGE_DisplayError();
          }
        }
      }
      printf("Release tiles\n");
      SAGE_ReleaseTileBank(TILE_BANK);
      SAGE_ShowMouse();
      printf("Closing screen\n");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
