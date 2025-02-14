/**
 * render3d_maggie.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test Maggie texture map
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define TEX_VAMPIRE           1
#define TEX_WIDTH             256

void draw_triangle(void)
{
  SAGE_3DElement triangle;
  
  triangle.type = S3DR_ELEM_TRIANGLE;
  triangle.x1 = 20.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = 0.0;
  triangle.v1 = 0.0;
  triangle.x2 = 275.0;
  triangle.y2 = 15.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH-1;
  triangle.v2 = 0.0;
  triangle.x3 = 20.0;
  triangle.y3 = 275.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH-1;
  triangle.color = 0xff0000;
  triangle.texture = TEX_VAMPIRE;
//  triangle.texture = STEX_USECOLOR;
  SAGE_Push3DElement(&triangle);

  triangle.x1 = 275.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = TEX_WIDTH-1;
  triangle.v1 = 0.0;
  triangle.x2 = 500.0;
  triangle.y2 = 360.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH-1;
  triangle.v2 = TEX_WIDTH-1;
  triangle.x3 = 20.0;
  triangle.y3 = 275.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH-1;
  triangle.color = 0xff00;
  triangle.texture = TEX_VAMPIRE;
//  triangle.texture = STEX_USECOLOR;
  SAGE_Push3DElement(&triangle);

  triangle.x1 = 20.0;
  triangle.y1 = 275.0;
  triangle.z1 = 10.0;
  triangle.u1 = 0.0;
  triangle.v1 = 0.0;
  triangle.x2 = 20.0;
  triangle.y2 = 478.0;
  triangle.z2 = 10.0;
  triangle.u2 = 0.0;
  triangle.v2 = TEX_WIDTH-1;
  triangle.x3 = 500.0;
  triangle.y3 = 360.0;
  triangle.z3 = 10.0;
  triangle.u3 = TEX_WIDTH-1;
  triangle.v3 = 0.0;
  triangle.color = 0xff;
  triangle.texture = TEX_VAMPIRE;
//  triangle.texture = STEX_USECOLOR;
  SAGE_Push3DElement(&triangle);

  triangle.x1 = 500.0;
  triangle.y1 = 360.0;
  triangle.z1 = 10.0;
  triangle.u1 = TEX_WIDTH-1;
  triangle.v1 = 0.0;
  triangle.x2 = 500.0;
  triangle.y2 = 478.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH-1;
  triangle.v2 = TEX_WIDTH-1;
  triangle.x3 = 20.0;
  triangle.y3 = 478.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH-1;
  triangle.color = 0xffffff;
  triangle.texture = TEX_VAMPIRE;
//  triangle.texture = STEX_USECOLOR;
  SAGE_Push3DElement(&triangle);

  SAGE_Render3DElements();
}

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picture = NULL;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DMAGGIE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_Set3DRenderSystem(S3DD_M3DRENDER);
      SAGE_AppliLog("Create texture from file");
      if (SAGE_CreateTextureFromFile(TEX_VAMPIRE, "/data/testtex.gif")) {
        SAGE_AppliLog("Add texture to card memory");
        if (SAGE_AddTexture(TEX_VAMPIRE)) {

            while (!finish) {
              SAGE_EngineDebug(FALSE);
              while ((event = SAGE_GetEvent()) != NULL) {
                if (event->type == SEVT_RAWKEY) {
                  switch (event->code) {
                    case SKEY_FR_ESC:
                      SAGE_AppliLog("Exit loop");
                      finish = TRUE;
                      break;
                    case SKEY_FR_D:
                      SAGE_EngineDebug(TRUE);
                      break;
                  }
                }
              }
              draw_triangle();
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
      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
