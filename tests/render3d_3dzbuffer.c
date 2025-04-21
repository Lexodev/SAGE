/**
 * render3d_3dzbuffer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D Z buffer
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 26/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640
#define SCREEN_HEIGHT         480

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
  triangle.color = 0xf8f804;
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
  triangle.color = 0x00f806;
  triangle.texture = TEX_VAMPIRE;
  SAGE_Push3DElement(&triangle);

  triangle.x1 = 320.0;
  triangle.y1 = 20.0;
  triangle.z1 = 10.0;
  triangle.u1 = TEX_WIDTH-1;
  triangle.v1 = 0.0;
  triangle.x2 = 500.0;
  triangle.y2 = 350.0;
  triangle.z2 = 10.0;
  triangle.u2 = TEX_WIDTH-1;
  triangle.v2 = TEX_WIDTH-1;
  triangle.x3 = 120.0;
  triangle.y3 = 350.0;
  triangle.z3 = 10.0;
  triangle.u3 = 0.0;
  triangle.v3 = TEX_WIDTH-1;
  triangle.color = 0xf800f808;
  triangle.texture = TEX_VAMPIRE;
//  SAGE_Push3DElement(&triangle);

  SAGE_Render3DElements();
}

void main(int argc, char **argv)
{
  SAGE_Event *event = NULL;
  SAGE_Picture *picture = NULL;
  LONG depth = 16, render = S3DD_S3DRENDER;
  BOOL finish = FALSE, zbuffer = TRUE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DTRIANGLE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("usage : render3d_3dzbuffer RENDER DEPTH");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    if (argc >= 2) {
      if (strcmp(argv[1], "M3D") == 0) {
        render = S3DD_M3DRENDER;
      } else if (strcmp(argv[1], "W3D") == 0) {
        render = S3DD_W3DRENDER;
      }
    }
    if (argc >= 3) {
      if (strcmp(argv[2], "8") == 0 && render == S3DD_S3DRENDER) {
        depth = 8;
      } else if (strcmp(argv[2], "32") == 0) {
        depth = 32;
      }
    }
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, depth, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_Set3DRenderSystem(render);
      SAGE_AppliLog("Load texture picture");
      if ((picture = SAGE_LoadPicture("data/testtex.gif")) != NULL) {
        SAGE_LoadPictureColorMap(picture);
        SAGE_RefreshColors(0, 256);
        SAGE_AppliLog("Create texture from picture");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          SAGE_AppliLog("Add texture to card memory");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {

            if (SAGE_EnableZBuffer(zbuffer)) {
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
                    case SKEY_FR_Z:
                      SAGE_AppliLog("Change Z buffer mode");
                      if (zbuffer) zbuffer = FALSE; else zbuffer = TRUE;
                      SAGE_EnableZBuffer(zbuffer);
                      break;
                    case SKEY_FR_D:
                      SAGE_SetTraceDebug(TRUE);
                      break;
                    case SKEY_FR_R:
                      SAGE_Set3DRenderMode(S3DR_RENDER_WIRE);
                      break;
                    case SKEY_FR_F:
                      SAGE_Set3DRenderMode(S3DR_RENDER_FLAT);
                      break;
                    case SKEY_FR_T:
                      SAGE_Set3DRenderMode(S3DR_RENDER_TEXT);
                      break;
                  }
                }
              }
              SAGE_ClearScreen();
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
