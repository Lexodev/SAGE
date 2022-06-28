/**
 * skybox.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D engine, skybox
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
#define CAMERA_MOVE           4.0

#define ENTITY_CUBE           1

#define TEX_PIERRE            1
#define TEX_MARBRE            2
#define TEX_BRIQUE            3
#define TEX_BOIS              4

#define TEX_SKYFRONT          32
#define TEX_SKYRIGHT          33
#define TEX_SKYBACK           34
#define TEX_SKYLEFT           35
#define TEX_SKYTOP            36
#define TEX_SKYBOTTOM         37

/** Demo variables */
WORD cax = 0, cay = 0;
FLOAT cubez = 50.0, dcubez = 1.0;
FLOAT cpx = 0, cpz = 0, min = -500, max = 500;
BOOL finish = FALSE;
UBYTE string_buffer[256];

#define NB_MATERIALS          10
SAGE_Material Materials[NB_MATERIALS] = {
  { "data/materials.png", "pierre", 0, 0, STEX_SIZE128, TEX_PIERRE },
  { "data/materials.png", "marbre", 128, 0, STEX_SIZE128, TEX_MARBRE },
  { "data/materials.png", "brique", 256, 0, STEX_SIZE128, TEX_BRIQUE },
  { "data/materials.png", "bois", 384, 0, STEX_SIZE128, TEX_BOIS },
  { "data/skybox.png", "front", 0, 0, STEX_SIZE128, TEX_SKYFRONT },
  { "data/skybox.png", "right", 128, 0, STEX_SIZE128, TEX_SKYRIGHT },
  { "data/skybox.png", "back", 256, 0, STEX_SIZE128, TEX_SKYBACK },
  { "data/skybox.png", "left", 384, 0, STEX_SIZE128, TEX_SKYLEFT },
  { "data/skybox.png", "top", 0, 128, STEX_SIZE128, TEX_SKYTOP },
  { "data/skybox.png", "bottom", 128, 128, STEX_SIZE128, TEX_SKYBOTTOM }
};

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
  { TRUE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 0,1,2,3, 0x550000, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_MARBRE, 1,5,6,2, 0x000066, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_BRIQUE, 5,4,7,6, 0xdd0000, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_BOIS, 4,0,3,7, 0x882211, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_MARBRE, 4,5,1,0, 0x666666, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 3,2,6,7, 0x552288, 0,0,127,0,127,127,0,127 }
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

// Controls
#define KEY_NBR               8
#define KEY_FORWARD           0
#define KEY_BACKWARD          1
#define KEY_STRAFELEFT        2
#define KEY_STRAFERIGHT       3
#define KEY_SPACE             4
#define KEY_DEBUG             5
#define KEY_PAUSE             6
#define KEY_QUIT              7

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_D, FALSE },
  { SKEY_FR_P, FALSE },
  { SKEY_FR_ESC, FALSE }
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
  SAGE_AppliLog("Add materials");
  if (!SAGE_AddMaterialList(Materials, NB_MATERIALS)) {
    return FALSE;
  }
  SAGE_AppliLog("Load materials");
  if (!SAGE_LoadMaterials()) {
    return FALSE;
  }
  SAGE_AppliLog("Add camera");
  if (!SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return FALSE;
  }
  SAGE_SetActiveCamera(MAIN_CAMERA);
  SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
  SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
  SAGE_SetSkyboxTextures(TEX_SKYFRONT, TEX_SKYBACK, TEX_SKYLEFT, TEX_SKYRIGHT, TEX_SKYTOP, TEX_SKYBOTTOM);
  SAGE_EnableSkybox(TRUE);
  SAGE_AppliLog("Add cube entity");
  SAGE_InitEntity(&Cube);
  if (!SAGE_AddEntity(ENTITY_CUBE, &Cube)) {
    return FALSE;
  }
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
  SAGE_AppliLog("Remove materials");
  SAGE_FlushMaterials();
  SAGE_AppliLog("Remove camera");
  SAGE_FlushCameras();
  SAGE_AppliLog("Remove entities");
  SAGE_FlushEntities();
  SAGE_Release3DEngine();
  SAGE_AppliLog("Close screen");
  SAGE_CloseScreen();
}

VOID StrafeLeft(VOID)
{
  SAGE_Camera * camera;
  FLOAT x, z;

  camera = SAGE_GetCamera(MAIN_CAMERA);
  x = -CAMERA_MOVE * SAGE_FastCosine(camera->angley);
  z = -CAMERA_MOVE * -SAGE_FastSine(camera->angley);
  cpx += x;
  if (cpx < min) cpx = min;
  if (cpx > max) cpx = max;
  cpz += z;
  if (cpz < min) cpz = min;
  if (cpz > max) cpz = max;
}

VOID StrafeRight(VOID)
{
  SAGE_Camera * camera;
  FLOAT x, z;

  camera = SAGE_GetCamera(MAIN_CAMERA);
  x = CAMERA_MOVE * SAGE_FastCosine(camera->angley);
  z = CAMERA_MOVE * -SAGE_FastSine(camera->angley);
  cpx += x;
  if (cpx < min) cpx = min;
  if (cpx > max) cpx = max;
  cpz += z;
  if (cpz < min) cpz = min;
  if (cpz > max) cpz = max;
}

VOID Forward(VOID)
{
  SAGE_Camera * camera;
  FLOAT x, z;

  camera = SAGE_GetCamera(MAIN_CAMERA);
  x = CAMERA_MOVE * SAGE_FastSine(camera->angley);
  z = CAMERA_MOVE * SAGE_FastCosine(camera->angley);
  cpx += x;
  if (cpx < min) cpx = min;
  if (cpx > max) cpx = max;
  cpz += z;
  if (cpz < min) cpz = min;
  if (cpz > max) cpz = max;
}

VOID Backward(VOID)
{
  SAGE_Camera * camera;
  FLOAT x, z;

  camera = SAGE_GetCamera(MAIN_CAMERA);
  x = -CAMERA_MOVE * SAGE_FastSine(camera->angley);
  z = -CAMERA_MOVE * SAGE_FastCosine(camera->angley);
  cpx += x;
  if (cpx < min) cpx = min;
  if (cpx > max) cpx = max;
  cpz += z;
  if (cpz < min) cpz = min;
  if (cpz > max) cpz = max;
}

VOID ScanKeyboard(VOID)
{
  if (SAGE_ScanKeyboard(keys, KEY_NBR)) {
    keyboard_state[KEY_FORWARD] = keys[KEY_FORWARD].key_pressed;
    keyboard_state[KEY_BACKWARD] = keys[KEY_BACKWARD].key_pressed;
    keyboard_state[KEY_STRAFELEFT] = keys[KEY_STRAFELEFT].key_pressed;
    keyboard_state[KEY_STRAFERIGHT] = keys[KEY_STRAFERIGHT].key_pressed;
    keyboard_state[KEY_SPACE] = keys[KEY_SPACE].key_pressed;
    keyboard_state[KEY_DEBUG] = keys[KEY_DEBUG].key_pressed;
    keyboard_state[KEY_PAUSE] = keys[KEY_PAUSE].key_pressed;
    keyboard_state[KEY_QUIT] = keys[KEY_QUIT].key_pressed;
  }
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  SAGE_EngineDebug(FALSE);
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) {
    SAGE_AppliLog("Exit loop");
    finish = TRUE;
  }
  if (keyboard_state[KEY_DEBUG]) {
    SAGE_EngineDebug(TRUE);
  }
  // Update camera position
  if (keyboard_state[KEY_STRAFELEFT]) {
    StrafeLeft();
  } else if (keyboard_state[KEY_STRAFERIGHT]) {
    StrafeRight();
  }
  if (keyboard_state[KEY_FORWARD]) {
    Forward();
  } else if (keyboard_state[KEY_BACKWARD]) {
    Backward();
  }
  if (keyboard_state[KEY_SPACE]) {
    cay = 0;
    cax = 0;
    cpx = 0.0;
    cpz = 0.0;
  }
  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_MOUSEMV) {
      cay += event->mousex;
      cax += event->mousey;
    }
  }
  SAGE_SetCameraAngle(MAIN_CAMERA, cax, cay, 0);
  SAGE_SetCameraPosition(MAIN_CAMERA, cpx, (FLOAT)0.0, cpz);
  cubez += dcubez;
  if (cubez < 30.0) dcubez = 1.0;
  if (cubez > 300.0) dcubez = -1.0;
  SAGE_MoveEntity(ENTITY_CUBE, (FLOAT)0.0, (FLOAT)0.0, dcubez);
  SAGE_RotateEntity(ENTITY_CUBE, 1, 2, 0);
}

VOID _Render(VOID)
{
  SAGE_EngineMetrics * metrics;

  SAGE_ClearScreen();
  SAGE_RenderWorld();
  // Draw the angles
  sprintf(string_buffer, "CAM : AX=%d  AY=%d  PX=%f  PZ=%f", (cax/SMTH_PRECISION), (cay/SMTH_PRECISION), cpx, cpz);
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
  SAGE_AppliLog("SAGE library 3D engine skybox demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_3D|SMOD_INTERRUPTION)) {
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
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
