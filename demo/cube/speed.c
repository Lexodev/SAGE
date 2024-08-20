/**
 * mapcube.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Textmap speed test
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L

#define TEX_VAMPIRE           1

BOOL finish = FALSE;
LONG screen_depth = 16, nb_triangles = 100;
UWORD render = S3DD_S3DRENDER;
SAGE_3DElement *triangles;

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, screen_depth, SSCR_STRICTRES)) {
    SAGE_HideMouse();
    if (SAGE_Set3DRenderSystem(render)) {
      SAGE_AppliLog("Rendering system %d enable", render);
    } else {
      SAGE_ErrorLog("Failed to activate %d rendering system", render);
    }
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTexture(VOID)
{
  SAGE_Picture *picture;

  SAGE_AppliLog("Load texture");
  if (screen_depth == 8) {
    picture = SAGE_LoadPicture("data/vamptex.bmp");
  } else {
    picture = SAGE_LoadPicture("data/vampire.png");
  }
  if (picture != NULL) {
    if (screen_depth == 8) {
      SAGE_LoadPictureColorMap(picture);
      SAGE_RefreshColors(0, 256);
      SAGE_SetTextColor(200, 0);
    }
    SAGE_AppliLog("Create texture from picture");
    if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE128, picture)) {
      SAGE_AppliLog("Add texture to card memory");
      if (SAGE_AddTexture(TEX_VAMPIRE)) {
        SAGE_ReleasePicture(picture);
        return TRUE;
      }
    }
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTriangles(VOID)
{
  UWORD idx;
  
  SAGE_AppliLog("Init triangles");
  triangles = (SAGE_3DElement *)SAGE_AllocMem(sizeof(SAGE_3DElement)*nb_triangles);
  if (triangles != NULL) {
    srand(0x68C0DE);
    for (idx = 0;idx < nb_triangles;idx++) {
      triangles[idx].type = S3DR_ELEM_TRIANGLE;
      triangles[idx].x1 = (FLOAT)(rand() % SCREEN_WIDTH);
      triangles[idx].y1 = (FLOAT)(rand() % SCREEN_HEIGHT);
      triangles[idx].z1 = 50.0;
      triangles[idx].u1 = 0.0F;
      triangles[idx].v1 = 0.0F;
      triangles[idx].x2 = (FLOAT)(rand() % SCREEN_WIDTH);
      triangles[idx].y2 = (FLOAT)(rand() % SCREEN_HEIGHT);
      triangles[idx].z2 = 50.0;
      triangles[idx].u2 = 0.0F;
      triangles[idx].v2 = 127.0F;
      triangles[idx].x3 = (FLOAT)(rand() % SCREEN_WIDTH);
      triangles[idx].y3 = (FLOAT)(rand() % SCREEN_HEIGHT);
      triangles[idx].z3 = 50.0;
      triangles[idx].u3 = 127.0F;
      triangles[idx].v3 = 64.0F;
      triangles[idx].texture = TEX_VAMPIRE;
      triangles[idx].color = 0xFFFFFFFF;
      SAGE_DebugLog(" => add triangle %d : %f,%f -> %f,%f -> %f,%f", idx,
        triangles[idx].x1,triangles[idx].y1,
        triangles[idx].x2,triangles[idx].y2,
        triangles[idx].x3,triangles[idx].y3
      );
    }
    return TRUE;
  }
  return FALSE;
}

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init demo");
  if (!OpenScreen()) {
    return FALSE;
  }
  if (!InitTexture()) {
    return FALSE;
  }
  if (!InitTriangles()) {
    return FALSE;
  }
  if (!SAGE_EnableFrameCount(TRUE)) {
    SAGE_ErrorLog("Can't activate frame rate counter !");
  }
  SAGE_MaximumFPS(60);
  SAGE_VerticalSynchro(FALSE);
  return TRUE;
}

VOID _Restore(VOID)
{
  SAGE_AppliLog("Release triangles");
  SAGE_FreeMem(triangles);
  SAGE_AppliLog("Release texture");
  SAGE_ReleaseTexture(TEX_VAMPIRE);
  SAGE_AppliLog("Restore demo");
  SAGE_ShowMouse();
  SAGE_AppliLog("Closing screen");
  SAGE_CloseScreen();
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
    }
  }
}

VOID DrawTriangles(VOID)
{
  UWORD idx;

  for (idx = 0;idx < nb_triangles;idx++) {
    SAGE_Push3DElement(&(triangles[idx]));
  }
  SAGE_Render3DElements();
}

VOID _Render(VOID)
{
  SAGE_ClearScreen();
  DrawTriangles();
  // Draw the parameters
  SAGE_PrintFText(10, 10, "NBTRI=%d", nb_triangles);
  // Draw the fps counter
  SAGE_PrintFText(560, 10, "%d fps", SAGE_GetFps());
}

/**
 * Run : speed depth nb_tri render
 *   - depth = screen depth (8/16/24/32)
 *   - nb_tri = number of triangles to draw
 *   - render = rendering system (S3D/W3D/M3D)
 */
void main(int argc, char **argv)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library texmap speed test V1.0 **");
  SAGE_AppliLog("** usage : speed depth nb_tri render");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_3D|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloCore()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    if (argc > 3) {
      if (strcmp(argv[3], "W3D") == 0) {
        render = S3DD_W3DRENDER;
      } else if (strcmp(argv[3], "M3D") == 0) {
        render = S3DD_M3DRENDER;
      }
    }
    if (argc > 2) {
      nb_triangles = atoi(argv[2]);
    }
    if (argc > 1) {
      screen_depth = atoi(argv[1]);
    }
    if ((render == S3DD_W3DRENDER || render == S3DD_M3DRENDER) && screen_depth == 8) {
      SAGE_ErrorLog("You can't activate Warp3D or Maggie3D for a 8bit screen mode");
      render = S3DD_S3DRENDER;
    }
    SAGE_AppliLog("Selected options : render is %d, depth is %d, nb triangles is %d", render, screen_depth, nb_triangles);
    // Init the demo data
    if (_Init()) {
      SAGE_AppliLog("Entering main loop");
      while (!finish) {
        if (SAGE_IsFrontMostScreen()) {

          // Update the demo data
          _Update();
          // Render the demo
          _Render();
          // Refresh the screen
          if (!SAGE_RefreshScreen()) {
            SAGE_AppliLog("Error on refresh screen !");
            SAGE_DisplayError();
            finish = TRUE;
          }

        }
      }
      // Restore the demo
      _Restore();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
