/**
 * engine.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of 3D engine
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

#define ENTITY_CUBE           1
#define ENTITY_SKYBOX         2
#define ENTITY_PYRAMIDE       4
#define ENTITY_DIAMAND        42
#define ENTITY_HOUSE          10

#define TEX_PIERRE            1
#define TEX_MARBRE            2
#define TEX_BRIQUE            3
#define TEX_BOIS              4
#define TEX_PAVE              5
#define TEX_ARDOISE           6
#define TEX_TORCHI            7
#define TEX_CREPI             8

#define TEX_SKYFRONT          32
#define TEX_SKYRIGHT          33
#define TEX_SKYBACK           34
#define TEX_SKYLEFT           35
#define TEX_SKYTOP            36
#define TEX_SKYBOTTOM         37

/** Demo variables */
WORD cax = 0, cay = 0;
FLOAT cubez = 50.0, dcubez = 1.0, pyray = 0.0, dpyray = 0.5;
BOOL finish = FALSE, debug = FALSE;
UBYTE string_buffer[256];

SAGE_Material Materials[14] = {
  { "data/materials.png", "pierre", 0, 0, STEX_SIZE128, TEX_PIERRE },
  { "data/materials.png", "marbre", 128, 0, STEX_SIZE128, TEX_MARBRE },
  { "data/materials.png", "brique", 256, 0, STEX_SIZE128, TEX_BRIQUE },
  { "data/materials.png", "bois", 384, 0, STEX_SIZE128, TEX_BOIS },
  { "data/materials.png", "pave", 0, 128, STEX_SIZE128, TEX_PAVE },
  { "data/materials.png", "ardoise", 128, 128, STEX_SIZE128, TEX_ARDOISE },
  { "data/materials.png", "torchi", 256, 128, STEX_SIZE128, TEX_TORCHI },
  { "data/materials.png", "crepi", 384, 128, STEX_SIZE128, TEX_CREPI },
  { "data/skybox.png", "front", 0, 0, STEX_SIZE128, TEX_SKYFRONT },
  { "data/skybox.png", "right", 128, 0, STEX_SIZE128, TEX_SKYRIGHT },
  { "data/skybox.png", "back", 256, 0, STEX_SIZE128, TEX_SKYBACK },
  { "data/skybox.png", "left", 384, 0, STEX_SIZE128, TEX_SKYLEFT },
  { "data/skybox.png", "top", 0, 128, STEX_SIZE128, TEX_SKYTOP },
  { "data/skybox.png", "bottom", 128, 128, STEX_SIZE128, TEX_SKYBOTTOM }
};

SAGE_EntityVertex CubeVertices[8] = {
  { -10.0,10.0,-10.0 },
  { 10.0,10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { -10.0,-10.0,-10.0 },
  { -10.0,10.0,10.0 },
  { 10.0,10.0,10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 }
};

SAGE_EntityVertex CubeTVertices[8];

SAGE_EntityFace CubeFaces[6] = {
  { TRUE, TRUE, S3DE_NOCLIP, 0,1,2,3, 238,TEX_PIERRE, 0,0,127,0,127,127,0,127 },
  { TRUE, TRUE, S3DE_NOCLIP, 1,5,6,2, 239,TEX_MARBRE, 0,0,127,0,127,127,0,127 },
  { TRUE, TRUE, S3DE_NOCLIP, 5,4,7,6, 240,TEX_BRIQUE, 0,0,127,0,127,127,0,127 },
  { TRUE, TRUE, S3DE_NOCLIP, 4,0,3,7, 241,TEX_BOIS, 0,0,127,0,127,127,0,127 },
  { TRUE, TRUE, S3DE_NOCLIP, 4,5,1,0, 242,TEX_PAVE, 0,0,127,0,127,127,0,127 },
  { TRUE, TRUE, S3DE_NOCLIP, 3,2,6,7, 243,TEX_ARDOISE, 0,0,127,0,127,127,0,127 }
};

SAGE_Entity Cube = {
  0, 0, 0,
  -30.0, 0.0, 50.0, 0.0,
  FALSE, FALSE,
  8, 6, S3DE_RENDER_TEXT,
  CubeVertices,
  CubeTVertices,
  CubeFaces
};

SAGE_EntityVertex PyraVertices[5] = {
  { 0.0,10.0,0.0 },
  { -10.0,-10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 }
};

SAGE_EntityVertex PyraTVertices[5];

SAGE_EntityFace PyraFaces[5] = {
  { FALSE, TRUE, S3DE_NOCLIP, 0,2,1,0, 206,TEX_CREPI, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 0,3,2,0, 207,TEX_CREPI, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 0,4,3,0, 208,TEX_CREPI, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 0,1,4,0, 210,TEX_CREPI, 64,0,127,127,0,127,0,0 },
  { TRUE, TRUE, S3DE_NOCLIP, 1,2,3,4, 211,TEX_TORCHI, 0,0,127,0,127,127,0,127 }
};

SAGE_Entity Pyramide = {
  30*4, 0, 12,
  30.0, 0.0, 100.0, 0.0,
  FALSE, FALSE,
  5, 5, S3DE_RENDER_TEXT,
  PyraVertices,
  PyraTVertices,
  PyraFaces
};

SAGE_EntityVertex DiamandVertices[6] = {
  { 0.0,10.0,0.0 },
  { -10.0,-10.0,-10.0 },
  { 10.0,-10.0,-10.0 },
  { 10.0,-10.0,10.0 },
  { -10.0,-10.0,10.0 },
  { 0.0,-20.0,0.0 }
};

SAGE_EntityVertex DiamandTVertices[6];

SAGE_EntityFace DiamandFaces[8] = {
  { FALSE, TRUE, S3DE_NOCLIP, 0,2,1,0, 20,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 0,3,2,0, 21,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 0,4,3,0, 22,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 0,1,4,0, 23,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 5,1,2,0, 24,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 5,2,3,0, 25,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 5,3,4,0, 26,TEX_PIERRE, 64,0,127,127,0,127,0,0 },
  { FALSE, TRUE, S3DE_NOCLIP, 5,4,1,0, 27,TEX_PIERRE, 64,0,127,127,0,127,0,0 }
};

SAGE_Entity Diamand = {
  20*4, 30*4, 24,
  0.0, 0.0, 80.0, 0.0,
  FALSE, FALSE,
  6, 8, S3DE_RENDER_TEXT,
  DiamandVertices,
  DiamandTVertices,
  DiamandFaces
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

  SAGE_AppliLog("Init world");
  SAGE_Init3DEngine();
  SAGE_Set3DRenderMode(S3DR_S3DMODE);
  SAGE_AppliLog("Add materials");
  if (!SAGE_AddMaterials(Materials, 14)) {
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
  SAGE_AppliLog("Set skybox");
  SAGE_SetSkyboxTextures(TEX_SKYFRONT, TEX_SKYBACK, TEX_SKYLEFT, TEX_SKYRIGHT, TEX_SKYTOP, TEX_SKYBOTTOM);
  SAGE_EnableSkybox(TRUE);
  SAGE_AppliLog("Add cube entity");
  if (!SAGE_AddEntity(ENTITY_CUBE, &Cube)) {
    return FALSE;
  }
  SAGE_AppliLog("Add pyramide entity");
  if (!SAGE_AddEntity(ENTITY_PYRAMIDE, &Pyramide)) {
    return FALSE;
  }
  SAGE_AppliLog("Add diamond entity");
  if (!SAGE_AddEntity(ENTITY_DIAMAND, &Diamand)) {
    return FALSE;
  }
  /*entity = SAGE_LoadEntity("data/maison.lwo");
  if (entity == FALSE || !SAGE_AddEntity(ENTITY_HOUSE, entity)) {
    return FALSE;
  }
  SAGE_SetEntityPosition(ENTITY_HOUSE, (FLOAT)10.0, (FLOAT)30.0, (FLOAT)90.0);
  SAGE_SetEntityRenderMode(ENTITY_HOUSE, S3DE_RENDER_FLAT);
  SAGE_ReleaseEntity(entity);*/
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

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
      if (event->code == SKEY_FR_D) {
        SAGE_AppliLog("Debug");
        debug = TRUE;
      }
      if (event->code == SKEY_FR_SPACE) {
        SAGE_SetEntityAngle(ENTITY_CUBE, 0, 0, 0);
      }
    } else if (event->type == SEVT_MOUSEMV) {
      cay += event->mousex;
      cax += event->mousey;
    }
  }
  SAGE_SetCameraAngle(MAIN_CAMERA, -cax, -cay, 0);
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
  //SAGE_RotateEntity(ENTITY_HOUSE, 6, 6, 6);
}

VOID _Render(VOID)
{
  SAGE_ClearScreen();
  SAGE_RenderWorld();
  debug = FALSE;
  // Draw the angles
  sprintf(string_buffer, "CAMERA AX=%d  CAMERA AY=%d  ZCUBE=%f", cax, cay, cubez);
  SAGE_PrintText(string_buffer, 10, 10);
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 560, 10);
}

void main(void)
{
  //SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library engine demo V1.0");
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
