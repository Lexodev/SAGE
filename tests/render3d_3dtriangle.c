/**
 * render3d_3dtexture.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2021
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define TEX_VAMPIRE           1
#define TEX_WIDTH             255

void draw_textri(void)
{
  SAGE_3DTriangle triangle;
  
  triangle.x1 = 20.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = 0.0;
  triangle.v1 = 0.0;
  triangle.x2 = 300.0;
  triangle.y2 = 15.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH;
  triangle.v2 = 0.0;
  triangle.x3 = 20.0;
  triangle.y3 = 200.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH;
  triangle.texture = TEX_VAMPIRE;
  SAGE_Push3DTriangle(&triangle);

  triangle.x1 = 300.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = TEX_WIDTH;
  triangle.v1 = 0.0;
  triangle.x2 = 500.0;
  triangle.y2 = 400.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH;
  triangle.v2 = TEX_WIDTH;
  triangle.x3 = 20.0;
  triangle.y3 = 200.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH;
  triangle.texture = TEX_VAMPIRE;
  SAGE_Push3DTriangle(&triangle);

  SAGE_Render3DTriangles();
}

void draw_coltri(void)
{
  SAGE_3DTriangle triangle;
  
  triangle.x1 = 20.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = 0.0;
  triangle.v1 = 0.0;
  triangle.x2 = 300.0;
  triangle.y2 = 15.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH;
  triangle.v2 = 0.0;
  triangle.x3 = 20.0;
  triangle.y3 = 200.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH;
  triangle.color = 0xf000f000;
  triangle.texture = STEX_USECOLOR;
  SAGE_Push3DTriangle(&triangle);

  triangle.x1 = 300.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = TEX_WIDTH;
  triangle.v1 = 0.0;
  triangle.x2 = 500.0;
  triangle.y2 = 400.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH;
  triangle.v2 = TEX_WIDTH;
  triangle.x3 = 20.0;
  triangle.y3 = 200.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH;
  triangle.color = 0x000f000f;
  triangle.texture = STEX_USECOLOR;
  SAGE_Push3DTriangle(&triangle);

  SAGE_Render3DTriangles();
}

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DTRIANGLE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_AppliLog("Load texture picture");
      if ((picture = SAGE_LoadPicture("/data/texture.gif")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        SAGE_AppliLog("Create texture from picture");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          SAGE_AppliLog("Add texture to card memory");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {

            while (!finish) {
              SAGE_SetTraceDebug(FALSE);
              while ((event = SAGE_GetEvent()) != NULL) {
                if (event->type == SEVT_RAWKEY) {
                  switch (event->code) {
                    case SKEY_FR_ESC:
                      SAGE_AppliLog("Exit loop");
                      finish = TRUE;
                      break;
                    case SKEY_FR_D:
                      SAGE_SetTraceDebug(TRUE);
                      break;
                  }
                }
              }
              draw_textri();
//              draw_coltri();
              if (!SAGE_RefreshScreen()) {
                SAGE_ErrorLog("Error RefreshScreen !");
                SAGE_DisplayError();
                finish = TRUE;
              }
            }

            SAGE_AppliLog("All done !");
          }
          SAGE_ReleaseTexture(TEX_VAMPIRE);
        }
        SAGE_ReleasePicture(picture);
      }
      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
