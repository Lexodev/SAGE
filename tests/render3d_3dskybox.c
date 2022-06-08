/**
 * render3d_3dskybox.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D skybox management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2022
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define MAIN_CAMERA           1

#define TEX_SKYFRONT          32
#define TEX_SKYRIGHT          33
#define TEX_SKYBACK           34
#define TEX_SKYLEFT           35
#define TEX_SKYTOP            36
#define TEX_SKYBOTTOM         37

#define NB_MATERIALS          6
SAGE_Material Materials[NB_MATERIALS] = {
  { "/data/skybox.png", "front", 0, 0, STEX_SIZE128, TEX_SKYFRONT },
  { "/data/skybox.png", "right", 128, 0, STEX_SIZE128, TEX_SKYRIGHT },
  { "/data/skybox.png", "back", 256, 0, STEX_SIZE128, TEX_SKYBACK },
  { "/data/skybox.png", "left", 384, 0, STEX_SIZE128, TEX_SKYLEFT },
  { "/data/skybox.png", "top", 0, 128, STEX_SIZE128, TEX_SKYTOP },
  { "/data/skybox.png", "bottom", 128, 128, STEX_SIZE128, TEX_SKYBOTTOM }
};

// Metrics buffer
UBYTE string_buffer[256];

void main(void)
{
  SAGE_Event * event = NULL;
  WORD cax = 0, cay = 0;
  BOOL finish = FALSE;
  SAGE_EngineMetrics * metrics;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DSKYBOX) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES|SSCR_TRACKMOUSE|SSCR_DELTAMOUSE)) {
      SAGE_HideMouse();
      SAGE_SetColor(0, 0x0);
      SAGE_SetColor(1, 0xffffff);
      SAGE_RefreshColors(0, 2);
      SAGE_SetTextColor(1, 0);

      SAGE_Init3DEngine();
      SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      SAGE_AppliLog("Load materials");
      if (SAGE_AddMaterialList(Materials, NB_MATERIALS) && SAGE_LoadMaterials()) {
        SAGE_SetActiveCamera(MAIN_CAMERA);
        SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
        SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
        SAGE_SetSkyboxTextures(TEX_SKYFRONT, TEX_SKYBACK, TEX_SKYLEFT, TEX_SKYRIGHT, TEX_SKYTOP, TEX_SKYBOTTOM);
        SAGE_EnableSkybox(TRUE);

        SAGE_AppliLog("Available memory %d KB", SAGE_AvailMem());
        SAGE_AppliLog("Entering main loop");
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
                  SAGE_AppliLog("Activate DEBUG mode");
                  SAGE_EngineDebug(TRUE);
                  break;
              }
            } else if (event->type == SEVT_MOUSEMV) {
              cay += event->mousex;
              cax += event->mousey;
            }
          }
          SAGE_SetCameraAngle(MAIN_CAMERA, cax, cay, 0);
          SAGE_ClearScreen();
          SAGE_RenderWorld();
          sprintf(string_buffer, "CAM AX=%d  AY=%d", cax, cay);
          SAGE_PrintText(string_buffer, 10, 15);
          metrics = SAGE_GetEngineMetrics();
          sprintf(
            string_buffer,
            "P=%d/%d  Z=%d/%d  E=%d/%d  V=%d/%d/%d  F=%d/%d  T=%d",
            metrics->rendered_planes, metrics->total_planes,
            metrics->rendered_zones, metrics->total_zones,
            metrics->rendered_entities, metrics->total_entities,
            metrics->rendered_vertices, metrics->calculated_vertices, metrics->total_vertices,
            metrics->rendered_faces, metrics->total_faces,
            metrics->rendered_triangles
          );
          SAGE_PrintText(string_buffer, 10, 470);
          if (!SAGE_RefreshScreen()) {
            SAGE_ErrorLog("Error RefreshScreen !");
            SAGE_DisplayError();
            finish = TRUE;
          }
        }

        SAGE_AppliLog("All done !");
      }
      SAGE_Release3DEngine();
      SAGE_DisplayError();

      SAGE_ShowMouse();
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
