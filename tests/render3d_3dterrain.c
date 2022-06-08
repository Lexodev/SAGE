/**
 * render3d_3dterrain.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D terrain management
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

// Metrics buffer
UBYTE string_buffer[256];

void main(void)
{
  SAGE_Event * event = NULL;
  WORD cax = 0, cay = 0;
  BOOL finish = FALSE;
  SAGE_EngineMetrics * metrics;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DTERRAIN) / %s", SAGE_GetVersion());
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
//      SAGE_Set3DRenderSystem(S3DD_W3DRENDER);
      SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      SAGE_AppliLog("Load terrain");
      if (SAGE_LoadHeightmapTerrain("/data/testterrain.gif", NULL, NULL)) {
        SAGE_SetActiveCamera(MAIN_CAMERA);
        SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
        SAGE_SetCameraPosition(MAIN_CAMERA, (FLOAT)128.0, (FLOAT)100.0, (FLOAT)128.0);
//        SAGE_SetCameraPosition(MAIN_CAMERA, (FLOAT)128.0, (FLOAT)100.0, (FLOAT)0.0);
        SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);

        SAGE_AppliLog("Available memory %d KB", SAGE_AvailMem());
        SAGE_AppliLog("Entering main loop");
        while (!finish) {
          SAGE_EngineDebug(FALSE);
          while ((event = SAGE_GetEvent()) != NULL) {
            if (event->type == SEVT_RAWKEY) {
              if (event->code == SKEY_FR_ESC) {
                SAGE_AppliLog("Exit loop");
                finish = TRUE;
              }
              if (event->code == SKEY_FR_D) {
                SAGE_AppliLog("Activate DEBUG mode");
                SAGE_EngineDebug(TRUE);
              }
              if (event->code == SKEY_FR_SPACE) {
                cay = 0; cax = 0;
              }
              if (event->code == SKEY_FR_W) {
                SAGE_Set3DRenderMode(S3DR_RENDER_WIRE);
              }
              if (event->code == SKEY_FR_F) {
                SAGE_Set3DRenderMode(S3DR_RENDER_FLAT);
              }
              if (event->code == SKEY_FR_T) {
                SAGE_Set3DRenderMode(S3DR_RENDER_TEXT);
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
