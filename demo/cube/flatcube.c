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

typedef struct {
  WORD anglex,angley,anglez;
  FLOAT posx, posy, posz;
  FLOAT dist, centerx, centery;
} Camera;

typedef struct {
  FLOAT x, y, z;
} Vertex;

typedef struct {
  BOOL culled;
  ULONG p1, p2, p3, p4, color;
} Face;

typedef struct {
  WORD anglex,angley,anglez;
  FLOAT posx, posy, posz;
  Vertex points[CUBE_POINTS];
  Face faces[CUBE_FACES];
  Vertex trans_points[CUBE_POINTS];
} CubeObject;

Camera ViewCam = { 0.0,0.0,0.0, 0,0,0, 200.0,SCREEN_WIDTH/2,SCREEN_HEIGHT/2 };

CubeObject FrontCube = {
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
    { FALSE, 0,1,2,3, 1 },
    { FALSE, 1,5,6,2, 2 },
    { FALSE, 5,4,7,6, 3 },
    { FALSE, 4,0,3,7, 4 },
    { FALSE, 4,5,1,0, 5 },
    { FALSE, 3,2,6,7, 6 }
  }
};

CubeObject LeftCube = {
  0,0,0,
  -50.0,0.0,0.0,
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
    { FALSE, 0,1,2,3, 1 },
    { FALSE, 1,5,6,2, 2 },
    { FALSE, 5,4,7,6, 3 },
    { FALSE, 4,0,3,7, 4 },
    { FALSE, 4,5,1,0, 5 },
    { FALSE, 3,2,6,7, 6 }
  }
};

CubeObject BottomCube = {
  0,0,0,
  0.0,-50.0,0.0,
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
    { FALSE, 0,1,2,3, 1 },
    { FALSE, 1,5,6,2, 2 },
    { FALSE, 5,4,7,6, 3 },
    { FALSE, 4,0,3,7, 4 },
    { FALSE, 4,5,1,0, 5 },
    { FALSE, 3,2,6,7, 6 }
  }
};

// Engine
#define RAD(x)                ((x)*PI/180.0)
FLOAT Sin[360];
FLOAT Cos[360];

Vertex ProjectedPoints[CUBE_POINTS];

SAGE_Matrix ViewMatrix;
SAGE_Matrix CubeMatrix;

// Render data
WORD cax = 0, cay = 0, crx = 0, cry = 0, crz = 0;
FLOAT cpx = 0.0, cpy = 0.0;
UBYTE string_buffer[256];
BOOL finish = FALSE, debug = FALSE;

VOID DumpMatrix(SAGE_Matrix * matrix)
{
  printf("Dump matrix 3x3\n");
  printf(" => %f\t%f\t%f\n", matrix->m11, matrix->m12, matrix->m13);
  printf(" => %f\t%f\t%f\n", matrix->m21, matrix->m22, matrix->m23);
  printf(" => %f\t%f\t%f\n", matrix->m31, matrix->m32, matrix->m33);
}

VOID DumpMatrix4(SAGE_Matrix4 * matrix)
{
  printf("Dump matrix 4x4\n");
  printf(" => %f\t%f\t%f\t%f\n", matrix->m11, matrix->m12, matrix->m13, matrix->m14);
  printf(" => %f\t%f\t%f\t%f\n", matrix->m21, matrix->m22, matrix->m23, matrix->m24);
  printf(" => %f\t%f\t%f\t%f\n", matrix->m31, matrix->m32, matrix->m33, matrix->m34);
  printf(" => %f\t%f\t%f\t%f\n", matrix->m41, matrix->m42, matrix->m43, matrix->m44);
}

VOID DumpCube(CubeObject * cube)
{
  UWORD index;
  
  printf("** Cube\n");
  printf(" - ax %d  ay %d  az %d\n", cube->anglex, cube->angley, cube->anglez);
  printf(" - px %f  py %f  pz %f\n", cube->posx, cube->posy, cube->posz);
  printf("* Points\n");
  for (index = 0;index < CUBE_POINTS;index++) {
    printf(" - #%d : %f, %f, %f\n", index, cube->points[index].x, cube->points[index].y, cube->points[index].z);
  }
  printf("* Faces\n");
  for (index = 0;index < CUBE_FACES;index++) {
    printf(" - #%d : %d->%d->%d->%d  0x%04X\n", index, cube->faces[index].p1, cube->faces[index].p2, cube->faces[index].p3, cube->faces[index].p4, cube->faces[index].color);
  }
  printf("* Transformed points\n");
  for (index = 0;index < CUBE_POINTS;index++) {
    printf(" - #%d : %f, %f, %f\n", index, cube->trans_points[index].x, cube->trans_points[index].y, cube->trans_points[index].z);
  }
}

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES|SSCR_TRACKMOUSE|SSCR_DELTAMOUSE)) {
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
    Sin[i] = sin(RAD(angle));
    Cos[i] = cos(RAD(angle));
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

VOID SetViewCamAngle(WORD ax, WORD ay, WORD az)
{
  ViewCam.anglex = ax;
  while (ViewCam.anglex < 0) ViewCam.anglex += 360;
  while (ViewCam.anglex >= 360) ViewCam.anglex -= 360;
  ViewCam.angley = ay;
  while (ViewCam.angley < 0) ViewCam.angley += 360;
  while (ViewCam.angley >= 360) ViewCam.angley -= 360;
  ViewCam.anglez = az;
  while (ViewCam.anglez < 0) ViewCam.anglez += 360;
  while (ViewCam.anglez >= 360) ViewCam.anglez -= 360;
}

VOID RotateCube(CubeObject * cube, WORD dx, WORD dy, WORD dz)
{
  cube->anglex += dx;
  while (cube->anglex < 0) cube->anglex += 360;
  while (cube->anglex >= 360) cube->anglex -= 360;
  cube->angley += dy;
  while (cube->angley < 0) cube->angley += 360;
  while (cube->angley >= 360) cube->angley -= 360;
  cube->anglez += dz;
  while (cube->anglez < 0) cube->anglez += 360;
  while (cube->anglez >= 360) cube->anglez -= 360;
}

VOID _Update(VOID)
{
  SAGE_Event * event = NULL;

  debug = FALSE;
  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
      if (event->code == SKEY_FR_D) {
        debug = TRUE;
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
  SetViewCamAngle(cax, cay, 0);
  RotateCube(&FrontCube, 0, 0, 1);
}

/**

Camera rotation matrix
We have cos(-x) = cos(x) and sin(-x) = -sin(x)

RX :    1     0     0
        0     Cos   -Sin
        0     Sin   Cos

RY :    Cos   0     Sin
        0     1     0
        -Sin   0    Cos

RZ :    Cos   -Sin  0
        Sin   Cos   0
        0     0     1

*/
VOID SetupViewMatrix(Camera * camera)
{
  SAGE_Matrix rx, ry, rz, tmp;

  if (debug) printf("=> SetupViewMatrix()\n");

  SAGE_IdentityMatrix(&rx);
  rx.m22 = Cos[camera->anglex];
  rx.m23 = -Sin[camera->anglex];
  rx.m32 = Sin[camera->anglex];
  rx.m33 = Cos[camera->anglex];
  
  SAGE_IdentityMatrix(&ry);
  ry.m11 = Cos[camera->angley];
  ry.m13 = Sin[camera->angley];
  ry.m31 = -Sin[camera->angley];
  ry.m33 = Cos[camera->angley];

  SAGE_IdentityMatrix(&rz);
  rz.m11 = Cos[camera->anglez];
  rz.m12 = -Sin[camera->anglez];
  rz.m21 = Sin[camera->anglez];
  rz.m22 = Cos[camera->anglez];

  SAGE_MultiplyMatrix(&tmp, &rz, &ry);
  SAGE_MultiplyMatrix(&ViewMatrix, &tmp, &rx);

  if (debug) DumpMatrix(&ViewMatrix);
}

BOOL CheckVisibility(CubeObject * cube, Camera * camera)
{
  if (debug) printf("=> CheckVisibility()\n");
  
  if (debug) printf("Cube is visible\n");
  return TRUE;
}

/**

Object rotation matrix

RX :    1     0     0
        0     Cos   Sin
        0     -Sin  Cos

RY :    Cos   0     -Sin
        0     1     0
        Sin   0     Cos

RZ :    Cos   Sin   0
        -Sin  Cos   0
        0     0     1

*/
VOID SetupCubeMatrix(CubeObject * cube)
{
  SAGE_Matrix rx, ry, rz, tmp;
  
  if (debug) printf("=> SetupCubeMatrix()\n");

  SAGE_IdentityMatrix(&rx);
  rx.m22 = Cos[cube->anglex];
  rx.m23 = Sin[cube->anglex];
  rx.m32 = -Sin[cube->anglex];
  rx.m33 = Cos[cube->anglex];

  SAGE_IdentityMatrix(&ry);
  ry.m11 = Cos[cube->angley];
  ry.m13 = -Sin[cube->angley];
  ry.m31 = Sin[cube->angley];
  ry.m33 = Cos[cube->angley];

  SAGE_IdentityMatrix(&rz);
  rz.m11 = Cos[cube->anglez];
  rz.m12 = Sin[cube->anglez];
  rz.m21 = -Sin[cube->anglez];
  rz.m22 = Cos[cube->anglez];

  SAGE_MultiplyMatrix(&tmp, &rx, &ry);
  SAGE_MultiplyMatrix(&CubeMatrix, &tmp, &rz);

  if (debug) DumpMatrix(&CubeMatrix);
}

VOID LocalToWorld(CubeObject * cube)
{
  UWORD index;
  FLOAT x, y, z;
  
  if (debug) printf("=> LocalToWorld()\n");
  for (index = 0;index < CUBE_POINTS;index++) {
    x = cube->points[index].x;
    y = cube->points[index].y;
    z = cube->points[index].z;
    cube->trans_points[index].x = x*CubeMatrix.m11 + y*CubeMatrix.m21 + z*CubeMatrix.m31 + cube->posx;
    cube->trans_points[index].y = x*CubeMatrix.m12 + y*CubeMatrix.m22 + z*CubeMatrix.m32 + cube->posy;
    cube->trans_points[index].z = x*CubeMatrix.m13 + y*CubeMatrix.m23 + z*CubeMatrix.m33 + cube->posz;
    if (debug) printf(" - point %d : x %f=>%f  y %f=>%f  z %f=>%f\n", index, x, cube->trans_points[index].x, y, cube->trans_points[index].y, z, cube->trans_points[index].z);
  }
}

VOID BackFaceCulling(CubeObject * cube, Camera * camera)
{
  UWORD index;
  
  if (debug) printf("=> BackFaceCulling()\n");
  for (index = 0;index < CUBE_FACES;index++) {
    cube->faces[index].culled = FALSE;
  }
}

VOID WorldToCamera(CubeObject * cube, Camera * camera)
{
  UWORD index;
  FLOAT x, y, z;
  
  if (debug) printf("=> WorldToCamera()\n");
  for (index = 0;index < CUBE_POINTS;index++) {
    x = cube->trans_points[index].x;
    y = cube->trans_points[index].y;
    z = cube->trans_points[index].z;
    cube->trans_points[index].x = x*ViewMatrix.m11 + y*ViewMatrix.m21 + z*ViewMatrix.m31;
    cube->trans_points[index].y = x*ViewMatrix.m12 + y*ViewMatrix.m22 + z*ViewMatrix.m32;
    cube->trans_points[index].z = x*ViewMatrix.m13 + y*ViewMatrix.m23 + z*ViewMatrix.m33;
    if (debug) printf(" - point %d : x %f=>%f  y %f=>%f  z %f=>%f\n", index, x, cube->trans_points[index].x, y, cube->trans_points[index].y, z, cube->trans_points[index].z);
  }
}

VOID Projection(CubeObject * cube, Camera * camera)
{
  UWORD index;

  if (debug) printf("=> Projection\n");
  for (index = 0;index < CUBE_POINTS;index++) {
    if (cube->trans_points[index].z > 0.0) {
      ProjectedPoints[index].x = (cube->trans_points[index].x * camera->dist / cube->trans_points[index].z) + camera->centerx;
      ProjectedPoints[index].y = (-cube->trans_points[index].y * camera->dist / cube->trans_points[index].z) + camera->centery;
      ProjectedPoints[index].z = cube->trans_points[index].z;
    } else {
      ProjectedPoints[index].x = 0.0;
      ProjectedPoints[index].y = 0.0;
      ProjectedPoints[index].z = 0.0;
    }
    if (debug) printf(" - vertex %d is %f, %f, %f\n", index, ProjectedPoints[index].x, ProjectedPoints[index].y, ProjectedPoints[index].z);
  }
}

VOID DrawWire(CubeObject * cube)
{
  LONG i;

  for (i = 0;i < CUBE_FACES;i++) {
    if (!cube->faces[i].culled) {
      SAGE_DrawClippedLine(
        (LONG)(ProjectedPoints[cube->faces[i].p1].x),
        (LONG)(ProjectedPoints[cube->faces[i].p1].y),
        (LONG)(ProjectedPoints[cube->faces[i].p2].x),
        (LONG)(ProjectedPoints[cube->faces[i].p2].y),
        cube->faces[i].color
      );
      SAGE_DrawClippedLine(
        (LONG)(ProjectedPoints[cube->faces[i].p2].x),
        (LONG)(ProjectedPoints[cube->faces[i].p2].y),
        (LONG)(ProjectedPoints[cube->faces[i].p3].x),
        (LONG)(ProjectedPoints[cube->faces[i].p3].y),
        cube->faces[i].color
      );
      SAGE_DrawClippedLine(
        (LONG)(ProjectedPoints[cube->faces[i].p3].x),
        (LONG)(ProjectedPoints[cube->faces[i].p3].y),
        (LONG)(ProjectedPoints[cube->faces[i].p4].x),
        (LONG)(ProjectedPoints[cube->faces[i].p4].y),
        cube->faces[i].color
      );
      SAGE_DrawClippedLine(
        (LONG)(ProjectedPoints[cube->faces[i].p4].x),
        (LONG)(ProjectedPoints[cube->faces[i].p4].y),
        (LONG)(ProjectedPoints[cube->faces[i].p1].x),
        (LONG)(ProjectedPoints[cube->faces[i].p1].y),
        cube->faces[i].color
      );
    }
  }
}

VOID DrawFlat(CubeObject * cube)
{
  LONG i;

  for (i = 0;i < CUBE_FACES;i++) {
    if (!cube->faces[i].culled) {
      SAGE_DrawClippedTriangle(
        (LONG)(ProjectedPoints[cube->faces[i].p1].x),
        (LONG)(ProjectedPoints[cube->faces[i].p1].y),
        (LONG)(ProjectedPoints[cube->faces[i].p2].x),
        (LONG)(ProjectedPoints[cube->faces[i].p2].y),
        (LONG)(ProjectedPoints[cube->faces[i].p3].x),
        (LONG)(ProjectedPoints[cube->faces[i].p3].y),
        cube->faces[i].color
      );
      SAGE_DrawClippedTriangle(
        (LONG)(ProjectedPoints[cube->faces[i].p1].x),
        (LONG)(ProjectedPoints[cube->faces[i].p1].y),
        (LONG)(ProjectedPoints[cube->faces[i].p4].x),
        (LONG)(ProjectedPoints[cube->faces[i].p4].y),
        (LONG)(ProjectedPoints[cube->faces[i].p3].x),
        (LONG)(ProjectedPoints[cube->faces[i].p3].y),
        cube->faces[i].color
      );
    }
  }
}

VOID DrawCube(CubeObject * cube, Camera * camera)
{
  if (CheckVisibility(cube, camera)) {
    SetupCubeMatrix(cube);
    LocalToWorld(cube);
    BackFaceCulling(cube, camera);
    WorldToCamera(cube, camera);
    Projection(cube, camera);
    if (debug) DumpCube(cube);
    DrawWire(cube);
  } else {
    if (debug) printf("Cube is not visible\n");
  }
}

VOID DrawWorld()
{
  SetupViewMatrix(&ViewCam);
  DrawCube(&FrontCube, &ViewCam);
  DrawCube(&LeftCube, &ViewCam);
  DrawCube(&BottomCube, &ViewCam);
}

VOID _Render(VOID)
{
  SAGE_ClearScreen();
  DrawWorld();
  // Draw the angles
  sprintf(string_buffer, "CAX=%d  CAY=%d", cax, cay);
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
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
