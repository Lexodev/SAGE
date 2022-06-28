/**
 * engine.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D engine, Gran Turismo car
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

#define BG_LAYER              1

#define MAIN_CAMERA           1
#define ENTITY_TOYOTA         1

/** Demo variables */
WORD cax = 0, cay = 0, vax = 0, vay = 0, vaz = 0;
BOOL finish = FALSE;
UBYTE string_buffer[256];

// Controls
#define KEY_NBR               8
#define KEY_ROTATEPX          0
#define KEY_ROTATENX          1
#define KEY_ROTATEPY          2
#define KEY_ROTATENY          3
#define KEY_ROTATEPZ          4
#define KEY_ROTATENZ          5

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_KPD8, FALSE },
  { SKEY_FR_KPD2, FALSE },
  { SKEY_FR_KPD4, FALSE },
  { SKEY_FR_KPD6, FALSE },
  { SKEY_FR_KPD7, FALSE },
  { SKEY_FR_KPD9, FALSE }
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
  SAGE_Entity * entity;
  SAGE_Picture * picture;

  SAGE_AppliLog("Init world");
  SAGE_Init3DEngine();
  SAGE_AppliLog("Add camera");
  if (!SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return FALSE;
  }
  SAGE_SetActiveCamera(MAIN_CAMERA);
  SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
  SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
  SAGE_AppliLog("Load entity");
  entity = SAGE_LoadEntity("data/toyota.obj");
  if (entity == NULL || !SAGE_AddEntity(ENTITY_TOYOTA, entity)) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_TOYOTA, (FLOAT)0.0, (FLOAT)0.0, (FLOAT)50.0);
  SAGE_AppliLog("Load background picture");
  picture = SAGE_LoadPicture("data/gt1.jpg");
  if (picture == NULL) {
    return FALSE;
  }
  if (!SAGE_CreateLayerFromPicture(BG_LAYER, picture)) {
    SAGE_ReleasePicture(picture);
    return FALSE;
  }
  SAGE_ReleasePicture(picture);
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

VOID ScanKeyboard(VOID)
{
  if (SAGE_ScanKeyboard(keys, KEY_NBR)) {
    keyboard_state[KEY_ROTATEPX] = keys[KEY_ROTATEPX].key_pressed;
    keyboard_state[KEY_ROTATENX] = keys[KEY_ROTATENX].key_pressed;
    keyboard_state[KEY_ROTATEPY] = keys[KEY_ROTATEPY].key_pressed;
    keyboard_state[KEY_ROTATENY] = keys[KEY_ROTATENY].key_pressed;
    keyboard_state[KEY_ROTATEPZ] = keys[KEY_ROTATEPZ].key_pressed;
    keyboard_state[KEY_ROTATENZ] = keys[KEY_ROTATENZ].key_pressed;
  }
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  SAGE_EngineDebug(FALSE);
  ScanKeyboard();
  if (keyboard_state[KEY_ROTATEPX]) {
    vax += S3DE_ONEDEGREE;
  } else if (keyboard_state[KEY_ROTATENX]) {
    vax -= S3DE_ONEDEGREE;
  }
  if (keyboard_state[KEY_ROTATEPY]) {
    vay += S3DE_ONEDEGREE;
  } else if (keyboard_state[KEY_ROTATENY]) {
    vay -= S3DE_ONEDEGREE;
  }
  if (keyboard_state[KEY_ROTATEPZ]) {
    vaz += S3DE_ONEDEGREE;
  } else if (keyboard_state[KEY_ROTATENZ]) {
    vaz -= S3DE_ONEDEGREE;
  }
  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
      if (event->code == SKEY_FR_D) {
        SAGE_AppliLog("Debug");
        SAGE_EngineDebug(TRUE);
      }
      if (event->code == SKEY_FR_SPACE) {
        cay = 0; cax = 0;
        vax = 0; vay = 0; vaz = 0;
      }
    } else if (event->type == SEVT_MOUSEMV) {
      cay += event->mousex;
      cax += event->mousey;
    }
  }
  SAGE_SetCameraAngle(MAIN_CAMERA, cax, cay, 0);
  SAGE_SetEntityAngle(ENTITY_TOYOTA, vax, vay, vaz);
}

VOID _Render(VOID)
{
  SAGE_EngineMetrics * metrics;

  SAGE_BlitLayerToScreen(BG_LAYER, 0, 0);
  SAGE_RenderWorld();
  // Draw the angles
  sprintf(string_buffer, "CAM AX=%d  AY=%d / CAR AX=%d  AY=%d AZ=%d", cax, cay, vax, vay, vaz);
  SAGE_PrintText(string_buffer, 10, 15);
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 560, 15);
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
  SAGE_PrintText(string_buffer, 10, 470);
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library engine GT car demo V1.0");
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
