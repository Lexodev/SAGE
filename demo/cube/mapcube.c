/**
 * mapcube.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of a mapped 3D cube
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2021
 */

#include <stdio.h>
#include <math.h>

#include "/src/sage.h"

// Wazp3D support only 16 bits screen
#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L

#define CUBE_POINTS           8
#define CUBE_FACES            6

#define TEX_VAMPIRE           1

#define BACK_LAYER            1

struct cube_point {
  FLOAT x, y, z;
};

struct cube_face {
  ULONG p1, p2, p3, p4, color;
  ULONG u1, v1, u2, v2, u3, v3, u4, v4;
};

struct cube_object {
  WORD anglex,angley,anglez;
  FLOAT posx, posy, posz;
  struct cube_point points[CUBE_POINTS];
  struct cube_face faces[CUBE_FACES];
};

// Demo Data
struct cube_object Cube = {
  0,0,0,
  0.0,0.0,50.0,
  {
    { -10.0,10.0,-10.0 },
    { 10.0,10.0,-10.0 },
    { 10.0,-10.0,-10.0 },
    { -10.0,-10.0,-10.0 },
    { -10.0,10.0,10.0 },
    { 10.0,10.0,10.0 },
    { 10.0,-10.0,10.0 },
    { -10.0,-10.0,10.0 }
  },
  {
    { 0,1,2,3, 1, 0,0,127,0,127,127,0,127 },
    { 1,5,6,2, 2, 0,0,127,0,127,127,0,127 },
    { 5,4,7,6, 3, 0,0,127,0,127,127,0,127 },
    { 4,0,3,7, 4, 0,0,127,0,127,127,0,127 },
    { 4,5,1,0, 5, 0,0,127,0,127,127,0,127 },
    { 3,2,6,7, 6, 0,0,127,0,127,127,0,127 }
  }
};
struct cube_point transf[8];
BOOL finish = FALSE, debug_on = FALSE;

// Controls
#define KEY_NBR               15
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_SPACE             4
#define KEY_A                 5
#define KEY_Z                 6
#define KEY_Q                 7
#define KEY_W                 8
#define KEY_QUIT              9
#define KEY_MOVEUP            10
#define KEY_MOVEDOWN          11
#define KEY_MOVELEFT          12
#define KEY_MOVERIGHT         13
#define KEY_D                 14

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_A, FALSE },
  { SKEY_FR_Z, FALSE },
  { SKEY_FR_Q, FALSE },
  { SKEY_FR_W, FALSE },
  { SKEY_FR_ESC, FALSE },
  { SKEY_FR_KPD8, FALSE },
  { SKEY_FR_KPD2, FALSE },
  { SKEY_FR_KPD4, FALSE },
  { SKEY_FR_KPD6, FALSE },
  { SKEY_FR_D, FALSE }
};

// Engine
#define RAD(x)                ((x)*PI/180.0)
FLOAT Sinus[360];
FLOAT Cosinus[360];

LONG screen_depth = 16;

// Render data
UBYTE string_buffer[256];

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, screen_depth, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    SAGE_HideMouse();
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitEngine(VOID)
{
  FLOAT angle;
  ULONG i;

  angle = 0.0;
  for (i = 0;i < 360;i++) {
    Sinus[i] = sin(RAD(angle));
    Cosinus[i] = cos(RAD(angle));
    angle += 1.0;
  }
  return TRUE;
}

BOOL InitCube(VOID)
{
  return TRUE;
}

BOOL InitTexture(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load texture");
  if (screen_depth == 8) {
    picture = SAGE_LoadPicture("data/vamptex.bmp");
  } else {
    picture = SAGE_LoadPicture("data/vampire.png");
  }
  if (picture != NULL) {
    if (screen_depth == 8) {
      SAGE_LoadPictureColorMap(picture);
      SAGE_RefreshColors(0, 256);
      SAGE_SetTextColor(200, 0);
    }
    SAGE_AppliLog("Create texture from picture");
    if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE128, picture)) {
      SAGE_AppliLog("Add texture to card memory");
      if (SAGE_AddTexture(TEX_VAMPIRE)) {
        SAGE_ReleasePicture(picture);
        return TRUE;
      }
    }
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer(VOID)
{
  SAGE_Picture * picture;

  if (screen_depth == 8) {
    return TRUE;
  }
  SAGE_AppliLog("Load back picture");
  if ((picture = SAGE_LoadPicture("data/desert.png")) != NULL) {
    SAGE_AppliLog("Create back layer");
    if (SAGE_CreateLayerFromPicture(BACK_LAYER, picture)) {
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init demo");
  if (!OpenScreen()) {
    return FALSE;
  }
  if (!InitEngine()) {
    return FALSE;
  }
  if (!InitCube()) {
    return FALSE;
  }
  if (!InitTexture()) {
    return FALSE;
  }
  if (!InitBackLayer()) {
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
  SAGE_AppliLog("Release layer");
  SAGE_ReleaseLayer(BACK_LAYER);
  SAGE_AppliLog("Release texture");
  SAGE_ReleaseTexture(TEX_VAMPIRE);
  SAGE_AppliLog("Restore demo");
  SAGE_ShowMouse();
  SAGE_AppliLog("Closing screen");
  SAGE_CloseScreen();
}

VOID ScanKeyboard(VOID)
{
  if (SAGE_ScanKeyboard(keys, KEY_NBR)) {
    keyboard_state[KEY_UP] = keys[KEY_UP].key_pressed;
    keyboard_state[KEY_DOWN] = keys[KEY_DOWN].key_pressed;
    keyboard_state[KEY_LEFT] = keys[KEY_LEFT].key_pressed;
    keyboard_state[KEY_RIGHT] = keys[KEY_RIGHT].key_pressed;
    keyboard_state[KEY_SPACE] = keys[KEY_SPACE].key_pressed;
    keyboard_state[KEY_A] = keys[KEY_A].key_pressed;
    keyboard_state[KEY_Z] = keys[KEY_Z].key_pressed;
    keyboard_state[KEY_Q] = keys[KEY_Q].key_pressed;
    keyboard_state[KEY_W] = keys[KEY_W].key_pressed;
    keyboard_state[KEY_QUIT] = keys[KEY_QUIT].key_pressed;
    keyboard_state[KEY_MOVEUP] = keys[KEY_MOVEUP].key_pressed;
    keyboard_state[KEY_MOVEDOWN] = keys[KEY_MOVEDOWN].key_pressed;
    keyboard_state[KEY_MOVELEFT] = keys[KEY_MOVELEFT].key_pressed;
    keyboard_state[KEY_MOVERIGHT] = keys[KEY_MOVERIGHT].key_pressed;
    keyboard_state[KEY_D] = keys[KEY_D].key_pressed;
  }
}

VOID _Update(VOID)
{
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) finish = TRUE;
  if (keyboard_state[KEY_SPACE]) {
    Cube.anglex = 0.0;
    Cube.angley = 0.0;
    Cube.anglez = 0.0;
    Cube.posx = 0;
    Cube.posy = 0;
    Cube.posz = 50;
  }
  if (keyboard_state[KEY_LEFT]) {
    Cube.angley += 1;
    if (Cube.angley >= 360) Cube.angley -= 360;
  } else if (keyboard_state[KEY_RIGHT]) {
    Cube.angley -= 1;
    if (Cube.angley < 0) Cube.angley += 360;
  }
  if (keyboard_state[KEY_UP]) {
    Cube.anglex += 1;
    if (Cube.anglex >= 360) Cube.anglex -= 360;
  } else if (keyboard_state[KEY_DOWN]) {
    Cube.anglex -= 1;
    if (Cube.anglex < 0) Cube.anglex += 360;
  }
  if (keyboard_state[KEY_A]) {
    Cube.anglez += 1;
    if (Cube.anglez >= 360) Cube.anglez -= 360;
  } else if (keyboard_state[KEY_Z]) {
    Cube.anglez -= 1;
    if (Cube.anglez < 0) Cube.anglez += 360;
  }
  if (keyboard_state[KEY_Q]) {
    Cube.posz += 1;
    if (Cube.posz > 200.0) Cube.posz = 200.0;
  } else if (keyboard_state[KEY_W]) {
    Cube.posz -= 1;
    if (Cube.posz < 30.0) Cube.posz = 30.0;
  }
  if (keyboard_state[KEY_MOVEUP]) {
    Cube.posy += 1.0;
  } else if (keyboard_state[KEY_MOVEDOWN]) {
    Cube.posy -= 1.0;
  }
  if (keyboard_state[KEY_MOVELEFT]) {
    Cube.posx -= 1.0;
  } else if (keyboard_state[KEY_MOVERIGHT]) {
    Cube.posx += 1.0;
  }
  if (keyboard_state[KEY_D]) {
    debug_on = TRUE;
  }
}

/**
 * Draw the cube
 */
 
VOID TransformPoints(VOID)
{
  FLOAT x,y,z,xa,ya,za,xb,yb,zb;
  LONG idx;

  for (idx = 0;idx < CUBE_POINTS;idx++) {
    xa = Cube.points[idx].x;
    ya = Cube.points[idx].y;
    za = Cube.points[idx].z;
    // Rotate X
    yb = ya * Cosinus[Cube.anglex] - za * Sinus[Cube.anglex];
    zb = ya * Sinus[Cube.anglex] + za * Cosinus[Cube.anglex];
    // Rotate Y
    xb = xa * Cosinus[Cube.angley] + zb * Sinus[Cube.angley];
    z = -xa * Sinus[Cube.angley] + zb * Cosinus[Cube.angley];
    // Rotate Z
    x = xb * Cosinus[Cube.anglez] - yb * Sinus[Cube.anglez];
    y = xb * Sinus[Cube.anglez] + yb * Cosinus[Cube.anglez];
    // Translate
    x += Cube.posx;
    y += Cube.posy;
    z += Cube.posz;
    // Perspective
    transf[idx].x = (x * 256.0) / z;
    transf[idx].y = (-y * 256.0) / z;
    transf[idx].z = z;
  }
}

VOID DrawCube(VOID)
{
  UWORD i;
  SAGE_3DTriangle triangle;

  TransformPoints();
  for (i = 0;i < CUBE_FACES;i++) {
    triangle.x1 = transf[Cube.faces[i].p1].x + (SCREEN_WIDTH/2);
    triangle.y1 = transf[Cube.faces[i].p1].y + (SCREEN_HEIGHT/2);
    triangle.z1 = transf[Cube.faces[i].p1].z;
    triangle.u1 = Cube.faces[i].u1;
    triangle.v1 = Cube.faces[i].v1;
    triangle.x2 = transf[Cube.faces[i].p2].x + (SCREEN_WIDTH/2);
    triangle.y2 = transf[Cube.faces[i].p2].y + (SCREEN_HEIGHT/2);
    triangle.z2 = transf[Cube.faces[i].p2].z;
    triangle.u2 = Cube.faces[i].u2;
    triangle.v2 = Cube.faces[i].v2;
    triangle.x3 = transf[Cube.faces[i].p3].x + (SCREEN_WIDTH/2);
    triangle.y3 = transf[Cube.faces[i].p3].y + (SCREEN_HEIGHT/2);
    triangle.z3 = transf[Cube.faces[i].p3].z;
    triangle.u3 = Cube.faces[i].u3;
    triangle.v3 = Cube.faces[i].v3;
    triangle.texture = TEX_VAMPIRE;
    if (((triangle.y1-triangle.y2)*(triangle.x2-triangle.x3)) < ((triangle.y2-triangle.y3)*(triangle.x1-triangle.x2))) {
      SAGE_Push3DTriangle(&triangle);
      triangle.x1 = transf[Cube.faces[i].p1].x + (SCREEN_WIDTH/2);
      triangle.y1 = transf[Cube.faces[i].p1].y + (SCREEN_HEIGHT/2);
      triangle.z1 = transf[Cube.faces[i].p1].z;
      triangle.u1 = Cube.faces[i].u1;
      triangle.v1 = Cube.faces[i].v1;
      triangle.x2 = transf[Cube.faces[i].p4].x + (SCREEN_WIDTH/2);
      triangle.y2 = transf[Cube.faces[i].p4].y + (SCREEN_HEIGHT/2);
      triangle.z2 = transf[Cube.faces[i].p4].z;
      triangle.u2 = Cube.faces[i].u4;
      triangle.v2 = Cube.faces[i].v4;
      triangle.x3 = transf[Cube.faces[i].p3].x + (SCREEN_WIDTH/2);
      triangle.y3 = transf[Cube.faces[i].p3].y + (SCREEN_HEIGHT/2);
      triangle.z3 = transf[Cube.faces[i].p3].z;
      triangle.u3 = Cube.faces[i].u3;
      triangle.v3 = Cube.faces[i].v3;
      SAGE_Push3DTriangle(&triangle);
    }
  }
  SAGE_Render3DTriangles();
}

VOID _Render(VOID)
{
  if (screen_depth == 8) {
    SAGE_ClearScreen();
  } else {
    // Draw the background
    SAGE_BlitLayerToScreen(BACK_LAYER, 0, 0);
  }
  DrawCube();
  // Draw the angles
  sprintf(string_buffer, "AX=%d  AY=%d  AZ=%d  ZOOM=%f  PX=%f  PY=%f", Cube.anglex, Cube.angley, Cube.anglez, Cube.posz, Cube.posx, Cube.posy);
  SAGE_PrintText(string_buffer, 10, 10);
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 560, 10);
}

void main(int argc, char ** argv)
{
  //SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library mapped 3D cube demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_3D|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    SAGE_Set3DRenderSystem(S3DD_S3DRENDER);
    if (argc > 2) {
      if (strcmp(argv[2], "W3D") == 0) {
        SAGE_Set3DRenderSystem(S3DD_W3DRENDER);
      } else if (strcmp(argv[2], "M3D") == 0) {
        SAGE_Set3DRenderSystem(S3DD_M3DRENDER);
      }
    }
    if (argc > 1) {
      if (strcmp(argv[1], "8") == 0) {
        screen_depth = 8;
      }
    }
    // Init the demo data
    if (_Init()) {
      SAGE_AppliLog("Entering main loop");
      while (!finish) {
        if (SAGE_IsFrontMostScreen()) {

          if (debug_on) {
            SAGE_SetTraceDebug(TRUE);
            debug_on = FALSE;
          } else {
            SAGE_SetTraceDebug(FALSE);
          }

          // Update the demo data
          _Update();
          // Render the demo
          _Render();
          // Refresh the screen
          if (!SAGE_RefreshScreen()) {
            SAGE_AppliLog("Error on refresh screen !");
            SAGE_DisplayError();
            finish = TRUE;
          }

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
