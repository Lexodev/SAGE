/**
 * flatcube.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of a flat 3D cube
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#include <stdio.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define CUBE_POINTS           8
#define CUBE_FACES            6

ULONG cube_colors[8] = {
  0x00000000,0x00FF0000,0x0000FF00,0x000000FF,
  0x00FFFF00,0x00FF00FF,0x0000FFFF,0x00FFFFFF
};

struct cube_point {
  FLOAT x, y, z;
};

struct cube_face {
  ULONG p1, p2, p3, p4, color;
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
    { 0,1,2,3, 1 },
    { 1,5,6,2, 2 },
    { 5,4,7,6, 3 },
    { 4,0,3,7, 4 },
    { 4,5,1,0, 5 },
    { 3,2,6,7, 6 }
  }
};

struct cube_point transf[CUBE_POINTS];
BOOL finish = FALSE;

// Controls
#define KEY_NBR               10
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
  { SKEY_FR_ESC, FALSE }
};

// Engine
#define RAD(x)                ((x)*PI/180.0)
FLOAT Sinus[360];
FLOAT Cosinus[360];

// Render data
UBYTE string_buffer[256];

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    SAGE_SetColorMap(cube_colors, 0, 8);
    SAGE_RefreshColors(0, 256);
    SAGE_SetTextColor(6, 0);
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
    Cube.posz = 50.0;
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
    if (Cube.posz < 20.0) Cube.posz = 20.0;
  }
}

VOID DrawCube(VOID)
{
  FLOAT x,y,z,xa,ya,za,xb,yb,zb;
  LONG x1,y1,x2,y2,x3,y3,x4,y4;
  UWORD i;
  
  for (i=0;i < CUBE_POINTS;i++) {
    xa = Cube.points[i].x;
    ya = Cube.points[i].y;
    za = Cube.points[i].z;
    // Rotate X
    yb = ya * Cosinus[Cube.anglex] - za * Sinus[Cube.anglex];
    zb = ya * Sinus[Cube.anglex] + za * Cosinus[Cube.anglex];
    // Rotate Y
    xb = xa * Cosinus[Cube.angley] + zb * Sinus[Cube.angley];
    z = -xa * Sinus[Cube.angley] + zb * Cosinus[Cube.angley];
    // Rotate Z
    x = xb * Cosinus[Cube.anglez] - yb * Sinus[Cube.anglez];
    y = xb * Sinus[Cube.anglez] + yb * Cosinus[Cube.anglez];
    SAGE_DebugLog(" => rx=%f, ry=%f, rz=%f", x, y, z);
    // Perspective
    z += Cube.posz;
    transf[i].x = (x * 256.0) / z;
    transf[i].y = (-y * 256.0) / z;
    transf[i].z = z;
  }
  for (i = 0;i < CUBE_FACES;i++) {
    x1 = (LONG) transf[Cube.faces[i].p1].x + (SCREEN_WIDTH/2);
    y1 = (LONG) transf[Cube.faces[i].p1].y + (SCREEN_HEIGHT/2);
    x2 = (LONG) transf[Cube.faces[i].p2].x + (SCREEN_WIDTH/2);
    y2 = (LONG) transf[Cube.faces[i].p2].y + (SCREEN_HEIGHT/2);
    x3 = (LONG) transf[Cube.faces[i].p3].x + (SCREEN_WIDTH/2);
    y3 = (LONG) transf[Cube.faces[i].p3].y + (SCREEN_HEIGHT/2);
    x4 = (LONG) transf[Cube.faces[i].p4].x + (SCREEN_WIDTH/2);
    y4 = (LONG) transf[Cube.faces[i].p4].y + (SCREEN_HEIGHT/2);
    if (((y1-y2)*(x2-x3)) < ((y2-y3)*(x1-x2))) {
      SAGE_DrawClippedTriangle(x1 ,y1, x2, y2, x3, y3, Cube.faces[i].color);
      SAGE_DrawClippedTriangle(x1 ,y1, x4, y4, x3, y3, Cube.faces[i].color);
    }
  }
}

VOID _Render(VOID)
{
  SAGE_ClearScreen();
  DrawCube();
  // Draw the angles
  sprintf(string_buffer, "AX=%d  AY=%d  AZ=%d  ZOOM=%f", Cube.anglex, Cube.angley, Cube.anglez, Cube.posz);
  SAGE_PrintText(string_buffer, 10, 10);
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 560, 10);
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library flat 3D cube demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    // Init the demo data
    if (_Init()) {
      SAGE_AppliLog("Entering main loop");
      while (!finish) {
        if (SAGE_IsFrontMostScreen()) {

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
  } else {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("Closing SAGE");
  if (!SAGE_Exit()) {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("End of demo");
}
