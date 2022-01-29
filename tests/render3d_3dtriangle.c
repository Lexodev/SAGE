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
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  SAGE_3DTriangle triangle;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library 3D test (3DTRIANGLE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    printf("Initialization successfull\n");
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_Set3DRenderMode(S3DR_S3DMODE);
      printf("Load texture picture\n");
      if ((picture = SAGE_LoadPicture("/data/vampire.bmp")) != NULL) {
        printf("Create texture from picture\n");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          printf("Add texture to card memory\n");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {

            triangle.x1 = 85.0;
            triangle.y1 = 12.0;
            triangle.z1 = 10.0;
            triangle.u1 = 0.0;
            triangle.v1 = 0.0;
            
            triangle.x2 = 230.0;
            triangle.y2 = 179.0;
            triangle.z2 = 10.0;
            triangle.u2 = 255.0;
            triangle.v2 = 0.0;

            triangle.x3 = 15.0;
            triangle.y3 = 120.0;
            triangle.z3 = 10.0;
            triangle.u3 = 0.0;
            triangle.v3 = 255.0;

            triangle.texture = TEX_VAMPIRE;

            while (!finish) {
              while ((event = SAGE_GetEvent()) != NULL) {
                if (event->type == SEVT_RAWKEY) {
                  switch (event->code) {
                    case SKEY_FR_ESC:
                      printf("Exit loop\n");
                      finish = TRUE;
                      break;
                  }
                }
              }

              SAGE_Push3DTriangle(&triangle);
              SAGE_Render3DTriangles();

              if (!SAGE_RefreshScreen()) {
                printf("Error RefreshScreen !\n");
                SAGE_DisplayError();
                finish = TRUE;
              }
            }

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
      SAGE_ShowMouse();
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
