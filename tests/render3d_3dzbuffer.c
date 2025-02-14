/**
 * render3d_3dzbuffer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D Z buffer
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define TEX_VAMPIRE           1
#define TEX_WIDTH             256

void draw_triangle(void)
{
  SAGE_3DElement triangle;
  
  triangle.type = S3DR_ELEM_TRIANGLE;
  triangle.x1 = 310.0;
  triangle.y1 = 15.0;
  triangle.z1 = 10.0;
  triangle.u1 = 0.0;
  triangle.v1 = 0.0;
  triangle.x2 = 175.0;
  triangle.y2 = 112.0;
  triangle.z2 = 33.0;
  triangle.u2 = TEX_WIDTH-1;
  triangle.v2 = 0.0;
  triangle.x3 = 420.0;
  triangle.y3 = 275.0;
  triangle.z3 = 44.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH-1;
  triangle.color = 0xf800f800;
  triangle.texture = TEX_VAMPIRE;
  SAGE_Push3DElement(&triangle);

  triangle.x1 = 275.0;
  triangle.y1 = 35.0;
  triangle.z1 = 60.0;
  triangle.u1 = TEX_WIDTH-1;
  triangle.v1 = 0.0;
  triangle.x2 = 500.0;
  triangle.y2 = 110.0;
  triangle.z2 = 12.0;
  triangle.u2 = TEX_WIDTH-1;
  triangle.v2 = TEX_WIDTH-1;
  triangle.x3 = 0.0;
  triangle.y3 = 375.0;
  triangle.z3 = 23.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH-1;
  triangle.color = 0xf800f800;
  triangle.texture = TEX_VAMPIRE;
  SAGE_Push3DElement(&triangle);

  SAGE_Render3DElements();
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
      if ((picture = SAGE_LoadPicture("/data/testtex.gif")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        SAGE_AppliLog("Create texture from picture");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          SAGE_AppliLog("Add texture to card memory");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {

            if (SAGE_EnableZBuffer(TRUE)) {
              SAGE_AppliLog("Z buffer activated");
            }
            
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
        SAGE_ReleasePicture(picture);
      }
      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
