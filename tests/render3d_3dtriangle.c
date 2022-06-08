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
#define SCREEN_DEPTH          8L

#define TEX_VAMPIRE           1
#define TEX_WIDTH             255

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  SAGE_3DTriangle triangle1, triangle2;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library 3D test (3DTRIANGLE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    printf("Initialization successfull\n");
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      printf("Load texture picture\n");
      if ((picture = SAGE_LoadPicture("/data/texture.gif")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        printf("Create texture from picture\n");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          printf("Add texture to card memory\n");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {

            triangle1.x1 = 20.0;
            triangle1.y1 = 15.0;
            triangle1.z1 = 10.0;
            triangle1.u1 = 0.0;
            triangle1.v1 = 0.0;
            
            triangle1.x2 = 300.0;
            triangle1.y2 = 15.0;
            triangle1.z2 = 10.0;
            triangle1.u2 = TEX_WIDTH;
            triangle1.v2 = 0.0;

            triangle1.x3 = 20.0;
            triangle1.y3 = 200.0;
            triangle1.z3 = 10.0;
            triangle1.u3 = 0.0;
            triangle1.v3 = TEX_WIDTH;

            triangle1.texture = TEX_VAMPIRE;

            triangle2.x1 = 300.0;
            triangle2.y1 = 15.0;
            triangle2.z1 = 10.0;
            triangle2.u1 = TEX_WIDTH;
            triangle2.v1 = 0.0;
            
            triangle2.x2 = 300.0;
            triangle2.y2 = 200.0;
            triangle2.z2 = 10.0;
            triangle2.u2 = TEX_WIDTH;
            triangle2.v2 = TEX_WIDTH;

            triangle2.x3 = 20.0;
            triangle2.y3 = 200.0;
            triangle2.z3 = 10.0;
            triangle2.u3 = 0.0;
            triangle2.v3 = TEX_WIDTH;

            triangle2.texture = TEX_VAMPIRE;

            while (!finish) {
              SAGE_SetTraceDebug(FALSE);
              while ((event = SAGE_GetEvent()) != NULL) {
                if (event->type == SEVT_RAWKEY) {
                  switch (event->code) {
                    case SKEY_FR_ESC:
                      printf("Exit loop\n");
                      finish = TRUE;
                      break;
                    case SKEY_FR_D:
                      SAGE_SetTraceDebug(TRUE);
                      break;
                  }
                }
              }

              SAGE_Push3DTriangle(&triangle1);
//              SAGE_Push3DTriangle(&triangle2);
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
