/**
 * dino.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D dinosaurus (PSX demo)
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.5 March 2025
 */

/**
 * Dino :
 *   - Turn with left/right key
 *   - Zoom with up/down key
 *   - Reset with space key
 *   - Texture on/off with T key
 *   - Quit with ESC
 */

#include <stdlib.h>
#include <math.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define BG_LAYER              1

#define MAIN_CAMERA           1
#define ENTITY_DINO           1

#define ZOOM_STEP             1.0

/** Demo variables */
WORD rotate = S3DE_ONEDEGREE * 180;
FLOAT zoom = 50.0;
LONG render = S3DD_S3DRENDER;
BOOL finish = FALSE, textured = TRUE;

// Controls
#define KEY_NBR               4
#define KEY_ROTATEPY          0
#define KEY_ROTATENY          1
#define KEY_ZOOMIN            2
#define KEY_ZOOMOUT           3

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE }
};

/*****************************************************************************/

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Open screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
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
  entity = SAGE_LoadEntity("data/dino.obj");
  if (entity == NULL || !SAGE_OptimizeEntity(entity) || !SAGE_AddEntity(ENTITY_DINO, entity)) {
    return FALSE;
  }
  SAGE_AppliLog("Load background picture");
  picture = SAGE_LoadPicture("data/background.png");
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
  if (SAGE_Set3DRenderSystem(render)) {
    SAGE_AppliLog("Rendering system %d enable", render);
  }
  if (SAGE_EnableZBuffer(TRUE)) {
    SAGE_AppliLog("Z buffer enable");
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
    keyboard_state[KEY_ROTATEPY] = keys[KEY_ROTATEPY].key_pressed;
    keyboard_state[KEY_ROTATENY] = keys[KEY_ROTATENY].key_pressed;
    keyboard_state[KEY_ZOOMIN] = keys[KEY_ZOOMIN].key_pressed;
    keyboard_state[KEY_ZOOMOUT] = keys[KEY_ZOOMOUT].key_pressed;
  }
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  SAGE_EngineDebug(FALSE);
  ScanKeyboard();
  if (keyboard_state[KEY_ROTATEPY]) {
    rotate += S3DE_ONEDEGREE;
  } else if (keyboard_state[KEY_ROTATENY]) {
    rotate -= S3DE_ONEDEGREE;
  }
  if (keyboard_state[KEY_ZOOMIN]) {
    zoom -= ZOOM_STEP;
    if (zoom < 30.0) { zoom = 30.0; }
  } else if (keyboard_state[KEY_ZOOMOUT]) {
    zoom += ZOOM_STEP;
    if (zoom > 200.0) { zoom = 200.0; }
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
      if (event->code == SKEY_FR_T) {
        if (textured) {
          textured = FALSE;
        } else {
          textured = TRUE;
        }
      }
      if (event->code == SKEY_FR_SPACE) {
        rotate = S3DE_ONEDEGREE*180;
        zoom = 50.0F;
      }
    }
  }
  SAGE_SetEntityAngle(ENTITY_DINO, 0, rotate, 0);
  SAGE_SetEntityPosition(ENTITY_DINO, (FLOAT)0.0, (FLOAT)-20.0, zoom);
}

VOID _Render(VOID)
{
  SAGE_EngineMetrics * metrics;

  SAGE_BlitLayerToScreen(BG_LAYER, 0, 0);
  if (textured) {
    SAGE_Set3DRenderMode(S3DR_RENDER_TEXT);
  } else {
    SAGE_Set3DRenderMode(S3DR_RENDER_WIRE);
  }
  SAGE_RenderWorld();
  // Draw the angles
  SAGE_PrintFText(10, 15, "DINO ROTATE=%d  ZOOM=%f", (rotate/SMTH_PRECISION), zoom);
  // Draw the fps counter
  SAGE_PrintFText(560, 15, "%d fps", SAGE_GetFps());
  // Draw the metrics
  metrics = SAGE_GetEngineMetrics();
  SAGE_PrintFText(10, 470,
    "P=%d/%d  Z=%d/%d  E=%d/%d  V=%d/%d/%d  F=%d/%d  EL=%d",
    metrics->rendered_planes, metrics->total_planes,
    metrics->rendered_zones, metrics->total_zones,
    metrics->rendered_entities, metrics->total_entities,
    metrics->rendered_vertices, metrics->calculated_vertices, metrics->total_vertices,
    metrics->rendered_faces, metrics->total_faces,
    metrics->rendered_elements
  );
}

void main(int argc, char **argv)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library engine Dino demo V1.5");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_3D|SMOD_INTERRUPTION)) {
    if (argc >= 2) {
      if (strcmp(argv[1], "M3D") == 0) {
        render = S3DD_M3DRENDER;
      } else if (strcmp(argv[1], "W3D") == 0) {
        render = S3DD_W3DRENDER;
      }
    }
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
