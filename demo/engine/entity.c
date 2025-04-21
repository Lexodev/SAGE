/**
 * entity.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D engine, entity
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 March 2025 (updated: 20/03/2025)
 */

#include <stdlib.h>
#include <math.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define BG_LAYER              1

#define MAIN_CAMERA           1
#define CAMERA_MOVE           4.0

#define ENTITY_CUBE           1
#define ENTITY_PYRAMIDE       4
#define ENTITY_DIAMAND        42

#define TEX_PIERRE            1
#define TEX_MARBRE            2
#define TEX_BRIQUE            3
#define TEX_BOIS              4
#define TEX_PAVE              5
#define TEX_ARDOISE           6
#define TEX_TORCHI            7
#define TEX_CREPI             8

/** Demo variables */
WORD cax = 0, cay = 0, rendering;
FLOAT cubez = 50.0, dcubez = 1.0, pyray = 0.0, dpyray = 0.5;
FLOAT cpx = 0, cpz = 0, min = -500, max = 500;
BOOL finish = FALSE;

#define NB_MATERIALS          8
SAGE_Material Materials[NB_MATERIALS] = {
  { "data/materials.png", "pierre", 0, 0, STEX_SIZE128, TEX_PIERRE },
  { "data/materials.png", "marbre", 128, 0, STEX_SIZE128, TEX_MARBRE },
  { "data/materials.png", "brique", 256, 0, STEX_SIZE128, TEX_BRIQUE },
  { "data/materials.png", "bois", 384, 0, STEX_SIZE128, TEX_BOIS },
  { "data/materials.png", "pave", 0, 128, STEX_SIZE128, TEX_PAVE },
  { "data/materials.png", "ardoise", 128, 128, STEX_SIZE128, TEX_ARDOISE },
  { "data/materials.png", "torchi", 256, 128, STEX_SIZE128, TEX_TORCHI },
  { "data/materials.png", "crepi", 384, 128, STEX_SIZE128, TEX_CREPI }
};

SAGE_Vertex CubeVertices[8] = {
  { -10.0,10.0,-10.0 },
  { 10.0,10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { -10.0,-10.0,-10.0 },
  { -10.0,10.0,10.0 },
  { 10.0,10.0,10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 }
};

SAGE_Face CubeFaces[6] = {
  { TRUE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 0,1,2,3, 0x550000, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_MARBRE, 1,5,6,2, 0x000066, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_BRIQUE, 5,4,7,6, 0xdd0000, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_BOIS, 4,0,3,7, 0x882211, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_PAVE, 4,5,1,0, 0x666666, 0,0,127,0,127,127,0,127 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_ARDOISE, 3,2,6,7, 0x552288, 0,0,127,0,127,127,0,127 }
};

SAGE_Vector CubeNormals[6];

SAGE_Entity Cube = {
  0, 0, 0,
  -30.0, 0.0, 50.0, 0.0,
  FALSE, FALSE, FALSE,
  8, 6, 0,
  CubeVertices,
  CubeFaces,
  CubeNormals
};

SAGE_Vertex PyraVertices[5] = {
  { 0.0,10.0,0.0 },
  { -10.0,-10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 }
};

SAGE_Face PyraFaces[5] = {
  { FALSE, FALSE, S3DE_NOCLIP, TEX_CREPI, 0,2,1,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_CREPI, 0,3,2,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_CREPI, 0,4,3,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_CREPI, 0,1,4,0, 0x0, 64,0,127,127,0,127,0,0 },
  { TRUE, FALSE, S3DE_NOCLIP, TEX_TORCHI, 1,2,3,4, 0x0, 0,0,127,0,127,127,0,127 }
};

SAGE_Vector PyraNormals[5];

SAGE_Entity Pyramide = {
  30*4, 0, 12,
  30.0, 0.0, 100.0, 0.0,
  FALSE, FALSE, FALSE,
  5, 5, 0,
  PyraVertices,
  PyraFaces,
  PyraNormals
};

SAGE_Vertex DiamandVertices[6] = {
  { 0.0,10.0,0.0 },
  { -10.0,-10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 },
  { 0.0,-20.0,0.0 }
};

SAGE_Face DiamandFaces[8] = {
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 0,2,1,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 0,3,2,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 0,4,3,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 0,1,4,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 5,1,2,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 5,2,3,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 5,3,4,0, 0x0, 64,0,127,127,0,127,0,0 },
  { FALSE, FALSE, S3DE_NOCLIP, TEX_PIERRE, 5,4,1,0, 0x0, 64,0,127,127,0,127,0,0 }
};

SAGE_Vector DiamandNormals[8];

SAGE_Entity Diamand = {
  20*4, 30*4, 24,
  0.0, 0.0, 80.0, 0.0,
  FALSE, FALSE, FALSE,
  6, 8, 0,
  DiamandVertices,
  DiamandFaces,
  DiamandNormals
};

ULONG screen_colors[8] = {
  0x000000,
  0xffffff,
  0xff0000,
  0x00ff00,
  0x0000ff,
  0xffff00,
  0xff00ff,
  0x00ffff
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
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES|SSCR_TRACKMOUSE|SSCR_DELTAMOUSE)) {
    SAGE_HideMouse();
    SAGE_SetColorMap(screen_colors, 0, 8);
    SAGE_RefreshColors(0, 8);
    SAGE_SetTextColor(1, 0);
    if (SAGE_Set3DRenderSystem(rendering)) {
      SAGE_AppliLog("%d rendering active", rendering);
    }
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitWorld(VOID)
{
  SAGE_Picture * picture;

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
  SAGE_AppliLog("Add cube entity");
  SAGE_InitEntity(&Cube);
  if (!SAGE_AddEntity(ENTITY_CUBE, &Cube)) {
    return FALSE;
  }
  SAGE_AppliLog("Add pyramide entity");
  SAGE_InitEntity(&Pyramide);
  if (!SAGE_AddEntity(ENTITY_PYRAMIDE, &Pyramide)) {
    return FALSE;
  }
  SAGE_AppliLog("Add diamond entity");
  SAGE_InitEntity(&Diamand);
  if (!SAGE_AddEntity(ENTITY_DIAMAND, &Diamand)) {
    return FALSE;
  }
  SAGE_AppliLog("Load background picture");
  picture = SAGE_LoadPicture("data/desert.png");
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

VOID StrafeLeft(VOID)
{
  SAGE_Camera *camera;
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
  SAGE_Camera *camera;
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
  SAGE_Camera *camera;
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
  SAGE_Camera *camera;
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
  SAGE_Event *event = NULL;

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
  pyray += dpyray;
  if (pyray < -200.0) dpyray = 0.5;
  if (pyray > 200.0) dpyray = -0.5;
  SAGE_MoveEntity(ENTITY_PYRAMIDE, (FLOAT)0.0, dpyray, (FLOAT)0.0);
  SAGE_RotateEntity(ENTITY_PYRAMIDE, 2, 0, 1);
  SAGE_RotateEntity(ENTITY_DIAMAND, 4, 4, 2);
}

VOID _Render(VOID)
{
  SAGE_EngineMetrics *metrics;

  SAGE_BlitLayerToScreen(BG_LAYER, 0, 0);
  SAGE_RenderWorld();
  // Draw the angles
  SAGE_PrintFText(10, 10, "CAM AX=%d  AY=%d  PX=%f  PZ=%f", cax, cay, cpx, cpz);
  // Draw the fps counter
  SAGE_PrintFText(560, 10, "%d fps", SAGE_GetFps());
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
  SAGE_AppliLog("SAGE library 3D engine entity demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_3D|SMOD_INTERRUPTION)) {
    
    if (argc > 1) {
      if (strcmp(argv[1], "S3D") == 0) {
        SAGE_AppliLog("Select S3D rendering");
        rendering = S3DD_S3DRENDER;
      } else if (strcmp(argv[1], "W3D") == 0) {
        SAGE_AppliLog("Select W3D rendering");
        rendering = S3DD_W3DRENDER;
      } else if (strcmp(argv[1], "M3D") == 0) {
        SAGE_AppliLog("Select M3D rendering");
        rendering = S3DD_M3DRENDER;
      }
    } else {
      SAGE_AppliLog("Select S3D rendering");
      rendering = S3DD_S3DRENDER;
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
