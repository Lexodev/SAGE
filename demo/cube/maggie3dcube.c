/**
 * maggie3dcube.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of a Maggie3D cube
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2021
 */

#include <stdio.h>
#include <math.h>

#include <proto/Maggie3D.h>

#include <Maggie3D/Maggie3D.h>
#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define CUBE_POINTS           8
#define CUBE_FACES            6

#define TEX_WIDTH             128
#define TEX_VAMPIRE           1
#define BACK_LAYER            1

M3D_Context *mycontext = NULL;
M3D_Texture *mytexture = NULL;
LONG error;

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
    { 0,1,2,3, 0xffffff, 0,0,TEX_WIDTH-1,0,TEX_WIDTH-1,TEX_WIDTH-1,0,TEX_WIDTH-1 },
    { 1,5,6,2, 0xff00ff, 0,0,TEX_WIDTH-1,0,TEX_WIDTH-1,TEX_WIDTH-1,0,TEX_WIDTH-1 },
    { 5,4,7,6, 0xff0000, 0,0,TEX_WIDTH-1,0,TEX_WIDTH-1,TEX_WIDTH-1,0,TEX_WIDTH-1 },
    { 4,0,3,7, 0x00ffff, 0,0,TEX_WIDTH-1,0,TEX_WIDTH-1,TEX_WIDTH-1,0,TEX_WIDTH-1 },
    { 4,5,1,0, 0x00ff00, 0,0,TEX_WIDTH-1,0,TEX_WIDTH-1,TEX_WIDTH-1,0,TEX_WIDTH-1 },
    { 3,2,6,7, 0x0000ff, 0,0,TEX_WIDTH-1,0,TEX_WIDTH-1,TEX_WIDTH-1,0,TEX_WIDTH-1 }
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

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
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

  if ((Maggie3DBase = OpenLibrary(MAGGIE3DNAME, MAGGIE3DVERSION)) == NULL) {
    SAGE_ErrorLog("can't open Maggie3D.library V%d\n", MAGGIE3DVERSION);
    return FALSE;
  }
  mycontext = M3D_CreateContext(&error, SAGE_GetSystemFrontBitmap());
  if (mycontext == NULL) {
    SAGE_ErrorLog("Maggie3D context creation error !");
    return FALSE;
  }
  angle = 0.0;
  for (i = 0;i < 360;i++) {
    Sinus[i] = sin(RAD(angle));
    Cosinus[i] = cos(RAD(angle));
    angle += 1.0;
  }
  return TRUE;
}

BOOL InitTexture(VOID)
{
  SAGE_Picture *picture;
  ULONG pixformat;

  SAGE_AppliLog("Create texture from file");
  SAGE_AutoRemapPicture(FALSE);
  if ((picture = SAGE_LoadPicture("data/vampire.png")) != NULL) {
    switch (picture->bitmap->pixformat) {
      case PIXFMT_CLUT:
        pixformat = M3D_PIXFMT_CLUT;
        break;
      case PIXFMT_RGB16:
        pixformat = M3D_PIXFMT_RGB16;
        break;
      case PIXFMT_RGB24:
        pixformat = M3D_PIXFMT_RGB24;
        break;
      case PIXFMT_ARGB32:
        pixformat = M3D_PIXFMT_ARGB32;
        break;
      default:
        pixformat = M3D_PIXFMT_UNKNOWN;
    }
    SAGE_DebugLog("Texture %dx%d %d", picture->bitmap->width, picture->bitmap->height, pixformat);
    mytexture = M3D_AllocTexture(
        mycontext,
        &error,
        picture->bitmap->bitmap_buffer,
        pixformat,
        picture->bitmap->width,
        picture->bitmap->height,
        picture->color_map
    );
    SAGE_ReleasePicture(picture);
    if (mytexture != NULL) {
      return TRUE;
    }
    SAGE_ErrorLog("Maggie3D allocate texture error %d !", error);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer(VOID)
{
  SAGE_Picture *picture;

  SAGE_AppliLog("Load back picture");
  SAGE_AutoRemapPicture(TRUE);
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
  //SAGE_AppliLog("Release texture");
  //SAGE_ReleaseTexture(TEX_VAMPIRE);
  SAGE_AppliLog("Restore demo");
  M3D_FreeTexture(mycontext, mytexture);
  //M3D_FreeZBuffer(mycontext);
  M3D_DestroyContext(mycontext);
  if (Maggie3DBase != NULL) {
    CloseLibrary(Maggie3DBase);
  }
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
    Cube.posz = 30;
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
  M3D_Triangle mytriangle;

  TransformPoints();
  mytriangle.texture = mytexture;
  for (i = 0;i < CUBE_FACES;i++) {
    mytriangle.color = Cube.faces[i].color;
    mytriangle.v1.x = transf[Cube.faces[i].p1].x + (SCREEN_WIDTH/2);
    mytriangle.v1.y = transf[Cube.faces[i].p1].y + (SCREEN_HEIGHT/2);
    mytriangle.v1.z = transf[Cube.faces[i].p1].z;
    mytriangle.v1.u = Cube.faces[i].u1;
    mytriangle.v1.v = Cube.faces[i].v1;
    mytriangle.v1.light = 1.0;
    mytriangle.v2.x = transf[Cube.faces[i].p2].x + (SCREEN_WIDTH/2);
    mytriangle.v2.y = transf[Cube.faces[i].p2].y + (SCREEN_HEIGHT/2);
    mytriangle.v2.z = transf[Cube.faces[i].p2].z;
    mytriangle.v2.u = Cube.faces[i].u2;
    mytriangle.v2.v = Cube.faces[i].v2;
    mytriangle.v2.light = 0.75;
    mytriangle.v3.x = transf[Cube.faces[i].p3].x + (SCREEN_WIDTH/2);
    mytriangle.v3.y = transf[Cube.faces[i].p3].y + (SCREEN_HEIGHT/2);
    mytriangle.v3.z = transf[Cube.faces[i].p3].z;
    mytriangle.v3.u = Cube.faces[i].u3;
    mytriangle.v3.v = Cube.faces[i].v3;
    mytriangle.v3.light = 0.50;
    if (((mytriangle.v1.y-mytriangle.v2.y)*(mytriangle.v2.x-mytriangle.v3.x)) < ((mytriangle.v2.y-mytriangle.v3.y)*(mytriangle.v1.x-mytriangle.v2.x))) {
      M3D_DrawTriangle(mycontext, &mytriangle);
      mytriangle.v1.x = transf[Cube.faces[i].p1].x + (SCREEN_WIDTH/2);
      mytriangle.v1.y = transf[Cube.faces[i].p1].y + (SCREEN_HEIGHT/2);
      mytriangle.v1.z = transf[Cube.faces[i].p1].z;
      mytriangle.v1.u = Cube.faces[i].u1;
      mytriangle.v1.v = Cube.faces[i].v1;
      mytriangle.v1.light = 1.0;
      mytriangle.v2.x = transf[Cube.faces[i].p4].x + (SCREEN_WIDTH/2);
      mytriangle.v2.y = transf[Cube.faces[i].p4].y + (SCREEN_HEIGHT/2);
      mytriangle.v2.z = transf[Cube.faces[i].p4].z;
      mytriangle.v2.u = Cube.faces[i].u4;
      mytriangle.v2.v = Cube.faces[i].v4;
      mytriangle.v2.light = 0.25;
      mytriangle.v3.x = transf[Cube.faces[i].p3].x + (SCREEN_WIDTH/2);
      mytriangle.v3.y = transf[Cube.faces[i].p3].y + (SCREEN_HEIGHT/2);
      mytriangle.v3.z = transf[Cube.faces[i].p3].z;
      mytriangle.v3.u = Cube.faces[i].u3;
      mytriangle.v3.v = Cube.faces[i].v3;
      mytriangle.v3.light = 0.50;
      M3D_DrawTriangle(mycontext, &mytriangle);
    }
  }
}

VOID _Render(VOID)
{
  // Draw the background
  SAGE_BlitLayerToScreen(BACK_LAYER, 0, 0);
  M3D_SetDrawRegion(mycontext, SAGE_GetSystemBackBitmap(), NULL);
  //M3D_ClearZBuffer(mycontext);
  if (M3D_LockHardware(mycontext) == M3D_SUCCESS) {
    DrawCube();
    M3D_UnlockHardware(mycontext);
  } else {
    SAGE_ErrorLog("Hardware lock failed !");
  }
  // Draw the angles
  SAGE_PrintFText(10, 10, "AX=%d  AY=%d  AZ=%d  ZOOM=%f  PX=%f  PY=%f", Cube.anglex, Cube.angley, Cube.anglez, Cube.posz, Cube.posx, Cube.posy);
  // Draw the fps counter
  SAGE_PrintFText(560, 10, "%d fps", SAGE_GetFps());
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Maggie 3D cube demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloCore()) {
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
    }
    // Restore the demo
    _Restore();
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
