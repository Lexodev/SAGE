/**
 * maggiecube.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of a Maggie cube
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 September 2024
 */

#include <stdio.h>
#include <math.h>

#include <proto/Maggie.h>
#include <maggie_vec.h>
#include <maggie_vertex.h>
#include <maggie_flags.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

BOOL finish = FALSE;

/*****************************************************************************/

struct Library *MaggieBase = NULL;

static struct MaggieVertex CubeVertices[6 * 4] = 
{
  { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
  { {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
  { { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
  { { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },

  { {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
  { { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
  { { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
  { {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },

  { {-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
  { { 1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
  { { 1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
  { {-1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },

  { {-1.0f,  1.0f, -1.0f}, {0.0f,  1.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
  { {-1.0f,  1.0f,  1.0f}, {0.0f,  1.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
  { { 1.0f,  1.0f,  1.0f}, {0.0f,  1.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
  { { 1.0f,  1.0f, -1.0f}, {0.0f,  1.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },

  { {-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
  { {-1.0f, -1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
  { {-1.0f,  1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
  { {-1.0f,  1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },

  { { 1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, { { 1.0f, 1.0f } }, 0x00ffffff },
  { { 1.0f,  1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, { { 1.0f, 0.0f } }, 0x00ffffff },
  { { 1.0f,  1.0f,  1.0f}, { 1.0f, 0.0f, 0.0f}, { { 0.0f, 0.0f } }, 0x00ffffff },
  { { 1.0f, -1.0f,  1.0f}, { 1.0f, 0.0f, 0.0f}, { { 0.0f, 1.0f } }, 0x00ffffff },
};

UWORD CubeIndices[5 * 6 - 1] =
{
  0,  1,  2,  3, 0xffff,
  4,  5,  6,  7, 0xffff,
  8,  9, 10, 11, 0xffff,
  12, 13, 14, 15, 0xffff,
  16, 17, 18, 19, 0xffff,
  20, 21, 22, 23
};

UWORD txtr = 0xffff;
mat4 worldMatrix, viewMatrix, perspective;
float targetRatio = 9.0f / 16.0f;
int vBuffer;
int iBuffer;
float xangle = 0.0f;
float yangle = 0.0f;

/*****************************************************************************/

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

BOOL LoadTexture(const char *filename)
{
  UBYTE *data = NULL;
  FILE *fp;
  int size;

  fp = fopen(filename, "rb");
  if (!fp) return FALSE;

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 128, SEEK_SET);

  data = AllocMem(size - 128, MEMF_ANY);
  fread(data, 1, size - 128, fp);
  fclose(fp);

  txtr = magAllocateTexture(8);
  magUploadTexture(txtr, 8, data, 0);
  FreeMem(data, size - 128);

  return TRUE;
}

BOOL InitEngine(VOID)
{
  mat4_identity(&worldMatrix);
  mat4_translate(&viewMatrix, 0.0f, 0.0f, 9.0f);
  mat4_perspective(&perspective, 60.0f, targetRatio, 0.01f, 100.0f);
  vBuffer = magAllocateVertexBuffer(6 * 4);
  iBuffer = magAllocateIndexBuffer(6 * 5 - 1);
  magUploadVertexBuffer(vBuffer, CubeVertices, 0, 6 * 4);
  magUploadIndexBuffer(iBuffer, CubeIndices, 0, 6 * 5 - 1);
  return TRUE;
}

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init demo");
  MaggieBase = OpenLibrary((UBYTE *)"maggie.library", 0);
  if (!MaggieBase) {
    SAGE_ErrorLog("Can't open maggie.library");
    return FALSE;
  }
  if (!OpenScreen()) {
    return FALSE;
  }
  if (!LoadTexture("data/TestTex.dds")) {
    return FALSE;
  }
  if (!InitEngine()) {
    return FALSE;
  }

  return TRUE;
}

VOID _Restore(VOID)
{
  magFreeTexture(txtr);
  magFreeVertexBuffer(vBuffer);
  magFreeIndexBuffer(iBuffer);
  SAGE_ShowMouse();
  SAGE_AppliLog("Closing screen");
  SAGE_CloseScreen();
  if (MaggieBase != NULL) {
    CloseLibrary(MaggieBase);
  }
}

VOID _Update(VOID)
{
  SAGE_Event *event = NULL;
  mat4 xRot, yRot;

  while ((event = SAGE_GetEvent()) != NULL) {
    if (event->type == SEVT_RAWKEY) {
      if (event->code == SKEY_FR_ESC) {
        SAGE_AppliLog("Exit loop");
        finish = TRUE;
      }
    }
  }
  mat4_rotateX(&xRot, xangle);
  mat4_rotateY(&yRot, yangle);
  mat4_mul(&worldMatrix, &xRot, &yRot);
  xangle += 0.01f;
  yangle += 0.0123f;
}

VOID _Render(VOID)
{
  SAGE_Bitmap *bitmap;
  
  bitmap = SAGE_GetBackBitmap();

  magBeginScene();

  magSetDrawMode(MAG_DRAWMODE_BILINEAR);

  magSetScreenMemory(bitmap->bitmap_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);

  magClear(MAG_CLEAR_COLOUR);

  magSetWorldMatrix((float *)&worldMatrix);
  magSetViewMatrix((float *)&viewMatrix);
  magSetPerspectiveMatrix((float *)&perspective);

  magSetTexture(0, txtr);
  magSetVertexBuffer(vBuffer);
  magSetIndexBuffer(iBuffer);

  magDrawIndexedPolygons(0, 6 * 4, 0, 6 * 5 - 1);

  magEndScene();
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Maggie cube demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO)) {

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
