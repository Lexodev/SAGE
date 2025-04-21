/**
 * engine3d_3dentity.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D entity management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640
#define SCREEN_HEIGHT         480

#define TEX_VAMPIRE           1
#define MAIN_CAMERA           1
#define CUBE1_ENTITY          1
#define CUBE2_ENTITY          2
#define CUBE3_ENTITY          3

#define CUBE_VERTICES         8
#define CUBE_FACES            6

// Set the stack size
extern long int __stack = 16384;

// Camera view
WORD cax = 0, cay = 0;

// Cube vertices (x, y, z)
SAGE_Vertex CubeVertices[CUBE_VERTICES] = {
  { -10.0,10.0,-10.0 },
  { 10.0,10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { -10.0,-10.0,-10.0 },
  { -10.0,10.0,10.0 },
  { 10.0,10.0,10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 }
};

// Cube faces (quad, culled, clipped, texture num, edges (1->4), color, texture (u1/v1 -> u4/v4)
SAGE_Face CubeFaces[CUBE_FACES] = {
  { TRUE, FALSE, S3DE_NOCLIP, TEX_VAMPIRE, 0,1,2,3, 0xff0000, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_VAMPIRE, 1,5,6,2, 0x00ff00, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_VAMPIRE, 5,4,7,6, 0x0000ff, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_VAMPIRE, 4,0,3,7, 0xff00ff, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, STEX_USECOLOR, 4,5,1,0, 0xffff00, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, STEX_USECOLOR, 3,2,6,7, 0x00ffff, 0,0,127,0,127,127,0,127 }
};

// Cube faces normal
SAGE_Vector CubeNormals[CUBE_FACES];

// Cube
SAGE_Entity Cube = {
  0, 0, 0,                        // Angle
  0.0, 0.0, 80.0, 0.0,            // Position (x, y, z), radius
  FALSE, FALSE, FALSE,            // Disable, culled, clipped
  CUBE_VERTICES, CUBE_FACES, S3DE_LOD_FULL,   // Vertices, faces, LOD
  CubeVertices,                   // Vertices
  CubeFaces,                      // Faces
  CubeNormals                     // Normals
};

// Metrics buffer
UBYTE string_buffer[256];

void main(int argc, char **argv)
{
  SAGE_Event *event = NULL;
  BOOL finish = FALSE, pause = FALSE;
  SAGE_Entity *cube1, *cube2, *cube3;
  BOOL show_c1 = TRUE, show_c2 = TRUE, show_c3 = TRUE;
  SAGE_EngineMetrics *metrics;
  LONG depth = 16, render = S3DD_S3DRENDER;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DENTITY) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
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
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, depth, SSCR_STRICTRES|SSCR_TRACKMOUSE|SSCR_DELTAMOUSE)) {
      SAGE_HideMouse();
      SAGE_SetColor(0, 0x0);
      SAGE_SetColor(1, 0xffffff);
      SAGE_RefreshColors(0, 2);
      SAGE_SetTextColor(1, 0);

      SAGE_Set3DRenderSystem(render);

      if (SAGE_Init3DEngine()) {
        SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (SAGE_CreateTextureFromFile(TEX_VAMPIRE, "data/vamptex.bmp") && SAGE_AddTexture(TEX_VAMPIRE)) {
          SAGE_InitEntity(&Cube);
          cube1 = SAGE_CloneEntity(&Cube);
          cube2 = SAGE_CloneEntity(&Cube);
          cube3 = SAGE_CloneEntity(&Cube);
          if (SAGE_AddEntity(CUBE1_ENTITY, cube1) && SAGE_AddEntity(CUBE2_ENTITY, cube2) && SAGE_AddEntity(CUBE3_ENTITY, cube3)) {
            SAGE_SetActiveCamera(MAIN_CAMERA);
            SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
            SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
            SAGE_MoveEntity(CUBE2_ENTITY, (FLOAT)-50.0, (FLOAT)0.0, (FLOAT)0.0);
            SAGE_MoveEntity(CUBE3_ENTITY, (FLOAT)50.0, (FLOAT)0.0, (FLOAT)0.0);

            SAGE_AppliLog("Available memory %d KB", SAGE_AvailMem());
            SAGE_AppliLog("Entering main loop");
            while (!finish) {
              SAGE_EngineDebug(FALSE);
              SAGE_SetTraceDebug(FALSE);
              while ((event = SAGE_GetEvent()) != NULL) {
                if (event->type == SEVT_RAWKEY) {
                  if (event->code == SKEY_FR_ESC) {
                    SAGE_AppliLog("Exit loop");
                    finish = TRUE;
                  }
                  if (event->code == SKEY_FR_D) {
                    SAGE_AppliLog("Activate DEBUG mode");
                    SAGE_EngineDebug(TRUE);
                    SAGE_SetTraceDebug(TRUE);
                  }
                  if (event->code == SKEY_FR_P) {
                    SAGE_AppliLog("Switch pause");
                    if (pause) pause = FALSE; else pause = TRUE;
                  }
                  if (event->code == SKEY_FR_SPACE) {
                    cay = 0; cax = 0;
                  }
                  if (event->code == SKEY_FR_F1) {
                    show_c1 = !show_c1;
                    if (show_c1) {
                      SAGE_ShowEntity(CUBE1_ENTITY);
                    } else {
                      SAGE_HideEntity(CUBE1_ENTITY);
                    }
                  }
                  if (event->code == SKEY_FR_F2) {
                    show_c2 = !show_c2;
                    if (show_c2) {
                      SAGE_ShowEntity(CUBE2_ENTITY);
                    } else {
                      SAGE_HideEntity(CUBE2_ENTITY);
                    }
                  }
                  if (event->code == SKEY_FR_F3) {
                    show_c3 = !show_c3;
                    if (show_c3) {
                      SAGE_ShowEntity(CUBE3_ENTITY);
                    } else {
                      SAGE_HideEntity(CUBE3_ENTITY);
                    }
                  }
                  if (event->code == SKEY_FR_R) {
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
              SAGE_ClearScreen();
              SAGE_SetCameraAngle(MAIN_CAMERA, cax, cay, 0);
              if (!pause) {
                SAGE_RotateEntity(CUBE1_ENTITY, S3DE_ONEDEGREE, -S3DE_ONEDEGREE, S3DE_ONEDEGREE);
                SAGE_RotateEntity(CUBE2_ENTITY, -S3DE_ONEDEGREE, S3DE_ONEDEGREE, -S3DE_ONEDEGREE);
                SAGE_RotateEntity(CUBE3_ENTITY, S3DE_ONEDEGREE, S3DE_ONEDEGREE, S3DE_ONEDEGREE);
              }
              SAGE_RenderWorld();
              SAGE_PrintFText(10, 15, "CAM AX=%d  AY=%d", cax, cay);
              metrics = SAGE_GetEngineMetrics();
              SAGE_PrintFText(10, 470, "P=%d/%d  Z=%d/%d  E=%d/%d  V=%d/%d/%d  F=%d/%d  E=%d",
                metrics->rendered_planes, metrics->total_planes,
                metrics->rendered_zones, metrics->total_zones,
                metrics->rendered_entities, metrics->total_entities,
                metrics->rendered_vertices, metrics->calculated_vertices, metrics->total_vertices,
                metrics->rendered_faces, metrics->total_faces,
                metrics->rendered_elements
              );
              if (!SAGE_RefreshScreen()) {
                SAGE_ErrorLog("Error RefreshScreen !");
                SAGE_DisplayError();
                finish = TRUE;
              }
            }

            SAGE_RemoveEntity(CUBE3_ENTITY);
            SAGE_RemoveEntity(CUBE2_ENTITY);
            SAGE_RemoveEntity(CUBE1_ENTITY);
            SAGE_AppliLog("All done !");
          }
        }
        SAGE_ReleaseTexture(TEX_VAMPIRE);
      }
      SAGE_Release3DEngine();

      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
