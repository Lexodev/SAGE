/**
 * camera.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D engine, camera view
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define MAIN_CAMERA           1

#define ENTITY_CUBEFRONT      1
#define ENTITY_CUBELEFT       2
#define ENTITY_CUBERIGHT      3
#define ENTITY_CUBEBACK       4
#define ENTITY_CUBETOP        5
#define ENTITY_CUBEBOTTOM     6

#define ENTITY_NOTEXT         -1

/** Demo variables */
WORD cax = 0, cay = 0, crx = 0, cry = 0, crz = 0;
FLOAT cpx = 0.0, cpy = 0.0;
BOOL finish = FALSE;
UBYTE string_buffer[256];

#define CUBE_VERTICES         8
#define CUBE_FACES            6

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

SAGE_Face CubeFaces[CUBE_FACES] = {
  { TRUE, FALSE, S3DE_NOCLIP, ENTITY_NOTEXT, 0,1,2,3, 0xff0000, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, ENTITY_NOTEXT, 1,5,6,2, 0x00ff00, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, ENTITY_NOTEXT, 5,4,7,6, 0x0000ff, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, ENTITY_NOTEXT, 4,0,3,7, 0xff00ff, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, ENTITY_NOTEXT, 4,5,1,0, 0xffff00, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, ENTITY_NOTEXT, 3,2,6,7, 0x00ffff, 0,0,127,0,127,127,0,127 }
};

SAGE_Vector CubeNormals[CUBE_FACES];

SAGE_Entity Cube = {
  0, 0, 0,                        // Angle
  0.0, 0.0, 0.0, 0.0,             // Position (x, y, z), radius
  FALSE, FALSE, FALSE,            // Disable, culled, clipped
  CUBE_VERTICES, CUBE_FACES, 0,   // Vertices, faces, LOD
  CubeVertices,                   // Vertices
  CubeFaces,                      // Faces
  CubeNormals                     // Normals
};

/*****************************************************************************/

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Open screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES|SSCR_TRACKMOUSE|SSCR_DELTAMOUSE)) {
    SAGE_HideMouse();
    SAGE_SetColor(0, 0x0);
    SAGE_SetColor(1, 0xffffff);
    SAGE_RefreshColors(0, 2);
    SAGE_SetTextColor(1, 0);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitWorld(VOID)
{
  SAGE_AppliLog("Init world");
  SAGE_Init3DEngine();
  SAGE_AppliLog("Add camera");
  if (!SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return FALSE;
  }
  SAGE_SetActiveCamera(MAIN_CAMERA);
  SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
  SAGE_SetCameraPosition(MAIN_CAMERA, (FLOAT)0.0, (FLOAT)0.0, (FLOAT)0.0);
  SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
  SAGE_InitEntity(&Cube);
  SAGE_AppliLog("Add front cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBEFRONT, SAGE_CloneEntity(&Cube))) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_CUBEFRONT, (FLOAT)0.0, (FLOAT)0.0, (FLOAT)50.0);
  SAGE_AppliLog("Add left cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBELEFT, SAGE_CloneEntity(&Cube))) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_CUBELEFT, (FLOAT)-50.0, (FLOAT)0.0, (FLOAT)0.0);
  SAGE_AppliLog("Add right cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBERIGHT, SAGE_CloneEntity(&Cube))) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_CUBERIGHT, (FLOAT)50.0, (FLOAT)0.0, (FLOAT)0.0);
  SAGE_AppliLog("Add back cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBEBACK, SAGE_CloneEntity(&Cube))) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_CUBEBACK, (FLOAT)0.0, (FLOAT)0.0, (FLOAT)-50.0);
  SAGE_AppliLog("Add top cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBETOP, SAGE_CloneEntity(&Cube))) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_CUBETOP, (FLOAT)0.0, (FLOAT)50.0, (FLOAT)0.0);
  SAGE_AppliLog("Add bottom cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBEBOTTOM, SAGE_CloneEntity(&Cube))) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_CUBEBOTTOM, (FLOAT)0.0, (FLOAT)-50.0, (FLOAT)0.0);
  return TRUE;
}

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init demo");
  if (!OpenScreen()) {
    return FALSE;
  }
  if (!InitWorld()) {
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
  SAGE_AppliLog("Restore demo");
  SAGE_ShowMouse();
  SAGE_AppliLog("Remove camera");
  SAGE_FlushCameras();
  SAGE_AppliLog("Remove entities");
  SAGE_FlushEntities();
  SAGE_Release3DEngine();
  SAGE_AppliLog("Close screen");
  SAGE_CloseScreen();
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  SAGE_EngineDebug(FALSE);
  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
      if (event->code == SKEY_FR_D) {
        SAGE_AppliLog("**** Debug ****");
        SAGE_EngineDebug(TRUE);
      }
      if (event->code == SKEY_FR_SPACE) {
        cax = 0; cay = 0;
        cpx = 0.0; cpy = 0.0;
      }
      if (event->code == SKEY_FR_UP) {
        cpy += 0.5;
        if (cpy > 50.0) cpy = 50.0;
      } else if (event->code == SKEY_FR_DOWN) {
        cpy -= 0.5;
        if (cpy < -50.0) cpy = -50.0;
      }
    } else if (event->type == SEVT_MOUSEMV) {
      cay += event->mousex;
      cax += event->mousey;
    }
  }
  SAGE_SetCameraAngle(MAIN_CAMERA, cax, cay, 0);
  SAGE_SetCameraPosition(MAIN_CAMERA, cpx, cpy, (FLOAT)0.0);
  crx += 2; cry += 1; crz += 3;
  SAGE_SetEntityAngle(ENTITY_CUBEFRONT, crx, cry, crz);
}

VOID _Render(VOID)
{
  SAGE_EngineMetrics * metrics;

  SAGE_ClearScreen();
  SAGE_RenderWorld();
  // Draw the angles
  sprintf(string_buffer, "CAX=%d  CAY=%d  CPX=%f  CPY=%f", cax, cay, cpx, cpy);
  SAGE_PrintText(string_buffer, 10, 10);
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 560, 10);
  // Draw the metrics
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
  SAGE_PrintText(string_buffer, 10, 460);
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library engine test demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D|SMOD_INTERRUPTION)) {
    // Init the demo data
    if (_Init()) {

      SAGE_AppliLog("Entering main loop");
      while (!finish) {

        // Update the demo data
        _Update();
        // Render the demo
        _Render();
        // Refresh the screen
        if (!SAGE_RefreshScreen()) {
          SAGE_AppliLog("Error on refresh screen !");
          finish = TRUE;
        }

      }
      // Restore the demo
      _Restore();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("Closing SAGE");
  if (!SAGE_Exit()) {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("End of demo");
}
