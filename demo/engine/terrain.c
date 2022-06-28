/**
 * terrain.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D engine, terrain
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2022
 */

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define MAIN_CAMERA           1

#define TERRAIN_SIZE          128
#define CELL_SIZE             4.0
#define CAMERA_MOVE           1.0

/** Demo variables */
WORD cax = 0, cay = 0;
FLOAT cpx = (CELL_SIZE * TERRAIN_SIZE / 2), cpz = (CELL_SIZE * TERRAIN_SIZE / 2), min = CELL_SIZE, max = (CELL_SIZE * TERRAIN_SIZE);
BOOL finish = FALSE;
UBYTE string_buffer[256];

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

BOOL InitTerrain(VOID)
{
  SAGE_AppliLog("Init world");
  SAGE_Init3DEngine();
  SAGE_AppliLog("Add camera");
  if (!SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return FALSE;
  }
  SAGE_SetActiveCamera(MAIN_CAMERA);
  SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
  SAGE_SetCameraPosition(MAIN_CAMERA, cpx, (FLOAT)130.0, cpz);
  SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
  if (!SAGE_LoadHeightmapTerrain("data/terrain2.gif", "data/color2.gif", NULL)) {
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
  if (!InitTerrain()) {
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
  SAGE_SetCameraPosition(MAIN_CAMERA, cpx, (FLOAT)100.0, cpz);
}

VOID _Render(VOID)
{
  SAGE_EngineMetrics * metrics;

  SAGE_ClearScreen();
  SAGE_RenderWorld();
  // Draw the angles
  sprintf(string_buffer, "CAM AX=%d  AY=%d  PX=%f  PZ=%f", cax, cay, cpx, cpz);
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
//  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library terrain demo V1.0");
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
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
