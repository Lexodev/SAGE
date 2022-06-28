/**
 * render3d_3dtexture.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D texture management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2021
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          16L

#define TEX_VAMPIRE           1

void main(void)
{
  SAGE_Picture * picture = NULL;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library 3D test (3DTEXTURE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    printf("Initialization successfull\n");
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      printf("Load texture picture\n");
      if ((picture = SAGE_LoadPicture("/data/vampire.bmp")) != NULL) {
        printf("Create texture from picture\n");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          printf("Add texture to card memory\n");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {
            printf("All done !\n");
          } else {
            SAGE_DisplayError();
          }
          SAGE_ReleaseTexture(TEX_VAMPIRE);
        } else {
          SAGE_DisplayError();
        }
        SAGE_ReleasePicture(picture);
      } else {
        SAGE_DisplayError();
      }
      SAGE_Pause(50);
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
