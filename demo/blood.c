/**
 * blood.h
 *
 * SAGE (Small Amiga Game Engine) project
 * Simple wolfenstein like
 *
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#include <dos/dos.h>
#include <proto/dos.h>

#include "/src/sage.h"

#include "blood_control.h"
#include "blood_engine.h"
#include "blood.h"

// Game palette
ULONG palette[COLOR_NBR] = {
  0x00030302,0x00b59d85,0x002f1b0b,0x00342a13,
  0x002a2a29,0x0030302f,0x00030353,0x003f472b,
  0x00510303,0x004a381c,0x004e3a28,0x00710c0c,
  0x00474f33,0x005c4527,0x005b4735,0x0053573b,
  0x006e5034,0x0014270c,0x00ae0303,0x00725a46,
  0x006d6d52,0x00686868,0x007b7f63,0x00797979,
  0x00890f0f,0x008b2727,0x005b6347,0x00a13b3b,
  0x00955610,0x00895b35,0x0099633b,0x00ad7b1f,
  0x00a16b3f,0x00816b50,0x00927b64,0x00ae7245,
  0x00cc0303,0x00ef0f0f,0x00fd3f3f,0x00c97f4f,
  0x00fd5f5f,0x00fd7b7b,0x009d8266,0x00a78f79,
  0x00c1992f,0x00cf8355,0x00dd9163,0x00d5b943,
  0x00e5996b,0x00eda173,0x00e9d957,0x00fdfd6b,
  0x00878787,0x00515151,0x00bda58d,0x00b1b1b1,
  0x00fd9999,0x00fdb487,0x00fdb9b9,0x00fdcbaf,
  0x00d0d0d0,0x00fdd9d9,0x00fde7da,0x00eaeaea
};

// Game data
UBYTE * chunky_buffer = NULL;
UBYTE * texture_buffer = NULL;
UBYTE * texture_adr[TEXTURE_NBR];

extern UBYTE keyboard_state[];

UBYTE string_buffer[256];

/**
 * Code start here !
 */

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Open screen");
  if (SAGE_OpenScreen(320, 240, 8, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

VOID InitPalette(VOID)
{
  LONG index, shade, color, bright, red, green, blue;

  SAGE_AppliLog(" Init palette");
  index = SHADE_NBR;
  bright = 0;
  for (shade = 1;shade < (COLOR_NBR/SHADE_NBR);shade++) {
    for (color = 0;color < SHADE_NBR;color++) {
      red = ((palette[color] >> 16) & 0x000000ff) - bright;
      if (red < 0) {
        red = 0;
      }
      green = ((palette[color] >> 8) & 0x000000ff) - bright;
      if (green < 0) {
        green = 0;
      }
      blue = (palette[color] & 0x000000ff) - bright;
      if (blue < 0) {
        blue = 0;
      }
      palette[index++] = red << 16 | green << 8 | blue;
    }
    bright += SHADE_LEVEL;
  }
  SAGE_SetColorMap(palette, 0, COLOR_NBR);
  SAGE_RefreshColors(0, COLOR_NBR);
}

BOOL LoadTextures(VOID)
{
  BPTR handle;

  SAGE_AppliLog(" Load textures");
  texture_buffer = SAGE_AllocMem(TEXTURE_WIDTH * TEXTURE_HEIGHT);
  if (texture_buffer == NULL) {
    return FALSE;
  }
  handle = Open("/data/blood_texture.chk", MODE_OLDFILE);
  if (handle == 0) {
    SAGE_FreeMem(texture_buffer);
    return FALSE;
  }
  Read(handle, texture_buffer, TEXTURE_WIDTH * TEXTURE_HEIGHT);
  Close(handle);
  return TRUE;
}

BOOL InitTextures(VOID)
{
  UBYTE * temp, * src1, * src2, * dst;
  LONG row, x, y;

  SAGE_AppliLog(" Init textures");
  temp = SAGE_AllocMem(TEXTURE_WIDTH * TEXTURE_HEIGHT);
  if (temp == NULL) {
    return FALSE;
  }
  for (row = 0;row < TEXTURE_NBR;row++) {
    texture_adr[row] = texture_buffer + (row * BLOC_WIDTH * BLOC_WIDTH);
  }
  src1 = texture_buffer;
  dst = temp;
  for (row = 0;row < ROW_NBR;row++) {
    for (x = 0;x < TEXTURE_WIDTH;x++) {
      src2 = src1;
      for (y = 0;y < BLOC_WIDTH;y++) {
        *dst = *src2;
        dst++;
        src2 += TEXTURE_WIDTH;
      }
      src1++;
    }
    src1 += TEXTURE_WIDTH * (BLOC_WIDTH - 1);
  }
  for (x = 0;x < (TEXTURE_WIDTH * TEXTURE_HEIGHT);x++) {
    texture_buffer[x] = temp[x];
  }
  SAGE_FreeMem(temp);
  SAGE_AppliLog(" Done");
  return TRUE;
}

BOOL InitGraphx(VOID)
{
  SAGE_AppliLog("Init graphx");
  InitPalette();
  if (!LoadTextures()) {
    return FALSE;
  }
  if (!InitTextures()) {
    return FALSE;
  }
  if (!SAGE_EnableFrameCount(TRUE)) {
    SAGE_ErrorLog("Can't activate frame rate counter !");
  }
  SAGE_MaximumFPS(60);
  SAGE_VerticalSynchro(FALSE);
  return TRUE;
}

VOID ReleaseGraphx(VOID)
{
  if (texture_buffer != NULL) {
    SAGE_FreeMem(texture_buffer);
  }
}

VOID main(VOID)
{
  SAGE_Bitmap * bitmap;
  BOOL finish = FALSE;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Blood demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    if (OpenScreen()) {
      SAGE_HideMouse();
      if (InitGraphx() && InitInput() && InitEngine()) {
        SAGE_AppliLog("Entering main loop");
        while (!finish) {
          if (SAGE_IsFrontMostScreen()) {

            ScanKeyboard();
            if (keyboard_state[KEY_QUIT]) {
              SAGE_AppliLog("Quit !");
              finish = TRUE;
            } else {
              bitmap = SAGE_GetBackBitmap();
              chunky_buffer = (UBYTE *) bitmap->bitmap_buffer;
              Engine();
              // Draw the fps counter
              sprintf(string_buffer, "%d fps", SAGE_GetFps());
              SAGE_PrintText(string_buffer, 10, 10);
            }
            // Refresh the screen
            if (!SAGE_RefreshScreen()) {
              SAGE_AppliLog("Error on refresh screen !");
              SAGE_DisplayError();
              finish = TRUE;
            }
            
          }
        }
      } else {
        SAGE_AppliLog("Initialization failed");
        SAGE_DisplayError();
      }
      ReleaseGraphx();
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
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
