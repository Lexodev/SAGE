/**
 * render3d_3dtriangle.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test SAGE texture mapper
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 26/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L

#define TEX_VAMPIRE           1
#define TEX_WIDTH             256

FLOAT posx, posy;
WORD type;
SAGE_3DElement flat_top, flat_bottom, generic1, generic2, special1, special2;

void init_triangle(void)
{
  posx = 0.0;
  posy = 0.0;
  type = 0;
  // Flat top
  flat_top.type = S3DR_ELEM_TRIANGLE;
  flat_top.x1 = 110.0;
  flat_top.y1 = 50.0;
  flat_top.z1 = 50.0;
  flat_top.u1 = 0.0;
  flat_top.v1 = 0.0;
  flat_top.x2 = 360.0;
  flat_top.y2 = 50.0;
  flat_top.z2 = 20.0;
  flat_top.u2 = TEX_WIDTH-1;
  flat_top.v2 = 0.0;
  flat_top.x3 = 320.0;
  flat_top.y3 = 280.0;
  flat_top.z3 = 100.0;
  flat_top.u3 = 0.0;
  flat_top.v3 = TEX_WIDTH-1;
  flat_top.color = 0xff0008;
  flat_top.texture = TEX_VAMPIRE;
  // Flat bottom
  flat_bottom.type = S3DR_ELEM_TRIANGLE;
  flat_bottom.x1 = 320.0;
  flat_bottom.y1 = 42.0;
  flat_bottom.z1 = 10.0;
  flat_bottom.u1 = 0.0;
  flat_bottom.v1 = 0.0;
  flat_bottom.x2 = 130.0;
  flat_bottom.y2 = 325.0;
  flat_bottom.z2 = 10.0;
  flat_bottom.u2 = 0.0;
  flat_bottom.v2 = TEX_WIDTH-1;
  flat_bottom.x3 = 420.0;
  flat_bottom.y3 = 325.0;
  flat_bottom.z3 = 10.0;
  flat_bottom.u3 = TEX_WIDTH-1;
  flat_bottom.v3 = TEX_WIDTH-1;
  flat_bottom.color = 0x00ff06;
  flat_bottom.texture = TEX_VAMPIRE;
  // Generic long left
  generic1.type = S3DR_ELEM_TRIANGLE;
  generic1.x1 = 320.0;
  generic1.y1 = 75.0;
  generic1.z1 = 10.0;
  generic1.u1 = 0.0;
  generic1.v1 = 0.0;
  generic1.x2 = 120.0;
  generic1.y2 = 470.0;
  generic1.z2 = 10.0;
  generic1.u2 = 0.0;
  generic1.v2 = TEX_WIDTH-1;
  generic1.x3 = 500.0;
  generic1.y3 = 360.0;
  generic1.z3 = 30.0;
  generic1.u3 = TEX_WIDTH-1;
  generic1.v3 = 0.0;
  generic1.color = 0x0000ff;
  generic1.texture = TEX_VAMPIRE;
  // Generic long right
  generic2.type = S3DR_ELEM_TRIANGLE;
  generic2.x1 = 320.0;
  generic2.y1 = 60.0;
  generic2.z1 = 10.0;
  generic2.u1 = 0.0;
  generic2.v1 = 0.0;
  generic2.x2 = 150.0;
  generic2.y2 = 268.0;
  generic2.z2 = 10.0;
  generic2.u2 = 0.0;
  generic2.v2 = TEX_WIDTH-1;
  generic2.x3 = 520.0;
  generic2.y3 = 407.0;
  generic2.z3 = 10.0;
  generic2.u3 = TEX_WIDTH-1;
  generic2.v3 = 0.0;
  generic2.color = 0xff00ff;
  generic2.texture = TEX_VAMPIRE;
  // Special 1
  special1.type = S3DR_ELEM_TRIANGLE;
  special1.x1 = 254.0;
  special1.y1 = 176.0;
  special1.z1 = 40.0;
  special1.u1 = 0.0;
  special1.v1 = 0.0;
  special1.x2 = 254.0;
  special1.y2 = 303.0;
  special1.z2 = 40.0;
  special1.u2 = 0.0;
  special1.v2 = TEX_WIDTH-1;
  special1.x3 = 382.0;
  special1.y3 = 304.0;
  special1.z3 = 39.0;
  special1.u3 = TEX_WIDTH-1;
  special1.v3 = TEX_WIDTH-1;
  special1.color = 0xffffff;
  special1.texture = STEX_USECOLOR;
  // Special 2
  special2.type = S3DR_ELEM_TRIANGLE;
  special2.x1 = 254.0;
  special2.y1 = 176.0;
  special2.z1 = 40.0;
  special2.u1 = 0.0;
  special2.v1 = 0.0;
  special2.x2 = 382.0;
  special2.y2 = 175.0;
  special2.z2 = 39.0;
  special2.u2 = TEX_WIDTH-1;
  special2.v2 = 0.0;
  special2.x3 = 382.0;
  special2.y3 = 304.0;
  special2.z3 = 39.0;
  special2.u3 = TEX_WIDTH-1;
  special2.v3 = TEX_WIDTH-1;
  special2.color = 0xff00ff;
  special2.texture = STEX_USECOLOR;
}

void draw_triangle(void)
{
  SAGE_3DElement *source, triangle;
  
  SAGE_ClearScreen();
  if (type == 0) {
    source = &flat_top;
  } else if (type == 1) {
    source = &flat_bottom;
  } else if (type == 2) {
    source = &generic1;
  } else if (type == 3) {
    source = &generic2;
  } else if (type == 4) {
    source = &special1;
  } else if (type == 5) {
    source = &special2;
  }
  triangle.type = source->type;
  triangle.x1 = source->x1 + posx;
  triangle.y1 = source->y1 + posy;
  triangle.z1 = source->z1;
  triangle.u1 = source->u1;
  triangle.v1 = source->v1;
  triangle.x2 = source->x2 + posx;
  triangle.y2 = source->y2 + posy;
  triangle.z2 = source->z2;
  triangle.u2 = source->u2;
  triangle.v2 = source->v2;
  triangle.x3 = source->x3 + posx;
  triangle.y3 = source->y3 + posy;
  triangle.z3 = source->z3;
  triangle.u3 = source->u3;
  triangle.v3 = source->v3;
  triangle.color = source->color;
  triangle.texture = source->texture;
  SAGE_Push3DElement(&triangle);
  SAGE_Render3DElements();
}

/**
 * Draw a mapped triangle
 * Usage : render3d_3dtriangle RENDER DEPTH
 *  with RENDER = S3D, M3D or W3D
 *  and DEPTH = 8, 16 or 32
 */
void main(int argc, char **argv)
{
  SAGE_Event *event = NULL;
  SAGE_Picture *picture = NULL;
  LONG depth = 16, render = S3DD_S3DRENDER;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DTRIANGLE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("usage : render3d_3dtriangle RENDER DEPTH");
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
    SAGE_AppliLog("User parameters RENDER=%d, DEPTH=%d", render, depth);
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, depth, SSCR_STRICTRES)) {
      SAGE_HideMouse();
      SAGE_Set3DRenderSystem(render);
      SAGE_AppliLog("Load and create texture");
      if (depth == 8) {
        picture = SAGE_LoadPicture("data/testtex.gif");
        if (picture != NULL) {
          SAGE_LoadPictureColorMap(picture);
          SAGE_RefreshColors(0, 256);
          if (!SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_FULLSIZE, picture)) {
            SAGE_ErrorLog("Can't create 8 bits texture !");
            finish = TRUE;
          }
          SAGE_ReleasePicture(picture);
        }
      } else {
        picture = SAGE_LoadPicture("data/testtex.png");
        if (picture != NULL) {
          if (!SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_FULLSIZE, picture)) {
            SAGE_ErrorLog("Can't create 16/24 bits texture !");
            finish = TRUE;
          }
          SAGE_ReleasePicture(picture);
        }
      }

      SAGE_AppliLog("Add texture to card memory");
      if (SAGE_AddTexture(TEX_VAMPIRE)) {

        init_triangle();
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
                case SKEY_FR_F1:
                  type = 0;
                  posx = posy = 0.0;
                  break;
                case SKEY_FR_F2:
                  type = 1;
                  posx = posy = 0.0;
                  break;
                case SKEY_FR_F3:
                  type = 2;
                  posx = posy = 0.0;
                  break;
                case SKEY_FR_F4:
                  type = 3;
                  posx = posy = 0.0;
                  break;
                case SKEY_FR_F8:
                  type = 4;
                  posx = posy = 0.0;
                  break;
                case SKEY_FR_F9:
                  type = 5;
                  posx = posy = 0.0;
                  break;
                case SKEY_FR_UP:
                  posy -= 10.0;
                  break;
                case SKEY_FR_DOWN:
                  posy += 10.0;
                  break;
                case SKEY_FR_LEFT:
                  posx -= 10.0;
                  break;
                case SKEY_FR_RIGHT:
                  posx += 10.0;
                  break;
                case SKEY_FR_F5:
                  SAGE_Set3DRenderMode(S3DR_RENDER_WIRE);
                  break;
                case SKEY_FR_F6:
                  SAGE_Set3DRenderMode(S3DR_RENDER_FLAT);
                  break;
                case SKEY_FR_F7:
                  SAGE_Set3DRenderMode(S3DR_RENDER_TEXT);
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
      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
