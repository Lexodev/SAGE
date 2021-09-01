/**
 * runner.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Demo of an out run clone
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#include <stdio.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         360L
#define SCREEN_DEPTH          8L

#define ROAD_SIZE             1600L

// Game gfx
#define SKY_WIDTH             640L
#define SKY_HEIGHT            87L
#define SKY_LAYER             1
#define SKY_POSY              0

#define MOUNT_WIDTH           640
#define MOUNT_HEIGHT          132
#define MOUNT_LAYER           2
#define MOUNT_POSY            28

#define TREE_WIDTH            640
#define TREE_HEIGHT           171
#define TREE_LAYER            3
#define TREE_POSY             130

#define SPR_BANK              0
#define NB_SPRITES            1
#define TRANSP_COLOR          41

#define SPR_CAR               0

// Game Data
LONG width = SCREEN_WIDTH;
LONG height = SCREEN_HEIGHT;
LONG road_width = 800;        // Width of the road
LONG segment_len = 200;       // Road segment length
LONG deep_view = 150;         // Deep of player view
FLOAT camera_depth = 0.84;    // Camera depth
BOOL finish = FALSE;

// Road data
struct RoadLine
{
  FLOAT x,y,z;      // 3D center of line
  FLOAT X,Y,W;      // Screen coord
  FLOAT curve,clip,scale;
};
struct RoadLine road[ROAD_SIZE];

ULONG road_colors[6] = {
  0x00008000,0x0000A000,
  0x00800000,0x00F0F0F0,
  0x00808080,0x00A0A0A0
};

// Player data
FLOAT playerX = 0.0, playerPos = 0.0, backX = 0.0;
LONG playerSpeed = 0, playerH = 1000;

// Controls
#define KEY_NBR               8
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_SPACE             4
#define KEY_Q                 5
#define KEY_W                 6
#define KEY_QUIT              7

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_Q, FALSE },
  { SKEY_FR_W, FALSE },
  { SKEY_FR_ESC, FALSE }
};

// Render data
LONG left_grass_crd[SCREEN_HEIGHT];
LONG left_rumble_crd[SCREEN_HEIGHT];
LONG left_road_crd[SCREEN_HEIGHT];
LONG right_grass_crd[SCREEN_HEIGHT];
LONG right_rumble_crd[SCREEN_HEIGHT];
LONG right_road_crd[SCREEN_HEIGHT];
ULONG screen_bpr;
UBYTE * screen_buffer;
UBYTE string_buffer[256];

#define MAIN_MUSIC            0

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer1(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load bg picture");
  if ((picture = SAGE_LoadPicture("/data/runner_bg1.png")) != NULL) {
    SAGE_AppliLog("Create bg layer 1");
    if (SAGE_CreateLayerFromPicture(SKY_LAYER, picture)) {
      SAGE_LoadPictureColorMap(picture);
      SAGE_SetColorMap(road_colors, 250, 6);
      SAGE_RefreshColors(0, 256);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer2(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load bg picture");
  if ((picture = SAGE_LoadPicture("/data/runner_bg2.png")) != NULL) {
    SAGE_AppliLog("Create bg layer 2");
    if (SAGE_CreateLayerFromPicture(MOUNT_LAYER, picture)) {
      SAGE_SetLayerTransparency(MOUNT_LAYER, 41);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer3(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load bg picture");
  if ((picture = SAGE_LoadPicture("/data/runner_bg3.png")) != NULL) {
    SAGE_AppliLog("Create bg layer 3");
    if (SAGE_CreateLayerFromPicture(TREE_LAYER, picture)) {
      SAGE_SetLayerTransparency(TREE_LAYER, 41);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitSprites(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load sprite picture");
  if ((picture = SAGE_LoadPicture("/data/runner_car.png")) != NULL) {
    SAGE_AppliLog("Create sprites bank");
    if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
      SAGE_SetSpriteBankTransparency(SPR_BANK, TRANSP_COLOR);
      SAGE_AppliLog("Add sprite to bank");
      if (SAGE_AddSpriteToBank(SPR_BANK, SPR_CAR, 0, 0, 160, 81, SSPR_HS_TOPLEFT)) {
        SAGE_ReleasePicture(picture);
        return TRUE;
      }
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

VOID InitRoad(VOID)
{
  LONG i;

  for (i = 0; i < ROAD_SIZE;i++) {
    road[i].x = 0;
    road[i].y = 0;
    road[i].z = i * segment_len;
    road[i].curve = 0;
    if (i>300 && i<700) road[i].curve = 0.5;
    if (i>1100) road[i].curve=-0.7;
    if (i>750) road[i].y = sin(i/30.0)*1500;
  }
}

BOOL InitMusic(VOID)
{
  SAGE_Music * music = NULL;

  SAGE_AppliLog("Loading music");
  if ((music = SAGE_LoadMusic("/data/runner.mod")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (SAGE_AddMusic(MAIN_MUSIC, music)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init game");
  if (!OpenScreen()) {
    return FALSE;
  }
  SAGE_HideMouse();
  if (!InitBackLayer1()) {
    return FALSE;
  }
  if (!InitBackLayer2()) {
    return FALSE;
  }
  if (!InitBackLayer3()) {
    return FALSE;
  }
  if (!InitSprites()) {
    return FALSE;
  }
  InitRoad();
  if (!InitMusic()) {
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
  SAGE_AppliLog("Release music");
  SAGE_StopMusic();
  SAGE_ClearMusic();
  SAGE_AppliLog("Restore game");
  SAGE_ReleaseLayer(SKY_LAYER);
  SAGE_ReleaseLayer(MOUNT_LAYER);
  SAGE_ReleaseLayer(TREE_LAYER);
  SAGE_ShowMouse();
  SAGE_AppliLog("Closing screen");
  SAGE_CloseScreen();
}

VOID ScanKeyboard(VOID)
{
  if (SAGE_ScanKeyboard(keys, KEY_NBR)) {
    keyboard_state[KEY_UP] = keys[0].key_pressed;
    keyboard_state[KEY_DOWN] = keys[1].key_pressed;
    keyboard_state[KEY_LEFT] = keys[2].key_pressed;
    keyboard_state[KEY_RIGHT] = keys[3].key_pressed;
    keyboard_state[KEY_SPACE] = keys[4].key_pressed;
    keyboard_state[KEY_Q] = keys[5].key_pressed;
    keyboard_state[KEY_W] = keys[6].key_pressed;
    keyboard_state[KEY_QUIT] = keys[7].key_pressed;
  }
}

VOID _Update(VOID)
{
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) finish = TRUE;
  if (keyboard_state[KEY_SPACE]) playerSpeed *= 3;
  if (keyboard_state[KEY_UP]) {
    playerSpeed = 200;
  } else if (keyboard_state[KEY_DOWN]) {
    playerSpeed = -200;
  } else {
    playerSpeed = 0;
  }
  if (keyboard_state[KEY_LEFT]) {
    playerX -= 0.02;
    if (playerX < -0.8) playerX = -0.8;
  } else if (keyboard_state[KEY_RIGHT]) {
    playerX += 0.02;
    if (playerX > 0.8) playerX = 0.8;
  }
  if (keyboard_state[KEY_Q]) {
    playerH += 100;
    if (playerH > 1500) playerH = 1500;
  } else if (keyboard_state[KEY_W]) {
    playerH -= 100;
    if (playerH < 500) playerH = 500;
  }
}

VOID Project(struct RoadLine * line, LONG camX, LONG camY, LONG camZ)
{
  line->scale = camera_depth / (line->z - camZ);
  line->X = (1 + line->scale * (line->x - camX)) * (width / 2);
  line->Y = (1 - line->scale * (line->y - camY)) * (height / 2);
  line->W = line->scale * road_width  * (width / 2);
}

VOID DrawSegment(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UBYTE * buffer;
  LONG gcolor, bcolor, rcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  end = (LONG) y2;
  row = SAGE_FastClippedLeftEdgeCalc(left_grass_crd, 0, start, 0, end, &clip);
  SAGE_FastClippedLeftEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, &clip);
  SAGE_FastClippedLeftEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, &clip);
  SAGE_FastClippedRightEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, &clip);
  SAGE_FastClippedRightEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, &clip);
  SAGE_FastClippedRightEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, &clip);
  gcolor = (segment>>2)%2 ? 250:251;
  gcolor = (gcolor << 24) + (gcolor << 16) + (gcolor << 8) + gcolor;
  bcolor = (segment>>3)%2 ? 252:253;
  bcolor = (bcolor << 24) + (bcolor << 16) + (bcolor << 8) + bcolor;
  rcolor = (segment>>2)%2 ? 254:255;
  rcolor = (rcolor << 24) + (rcolor << 16) + (rcolor << 8) + rcolor;
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawFlatQuad8Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor);
  SAGE_DrawFlatQuad8Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor);
  SAGE_DrawFlatQuad8Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor);
  SAGE_DrawFlatQuad8Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor);
  SAGE_DrawFlatQuad8Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor);
}

VOID _Render(VOID)
{
  SAGE_Bitmap * bitmap;
  FLOAT x = 0, dx = 0;
  LONG startPos, roadW, camH, maxy, n;
  struct RoadLine * l, * p;

  // Calcul road param
  playerPos += playerSpeed;
  while (playerPos >= ROAD_SIZE * segment_len) playerPos -= ROAD_SIZE * segment_len;
  while (playerPos < 0) playerPos += ROAD_SIZE * segment_len;
  startPos = playerPos / segment_len;
  camH = (LONG) road[startPos].y + playerH;
  maxy = height;
  x = 0;
  dx = 0;
  // Draw background
  if (!SAGE_BlitLayerToScreen(SKY_LAYER, 0, SKY_POSY)) {
    SAGE_AppliLog("Error blitting background layer 1 !");
    SAGE_DisplayError();
    finish = TRUE;
  }
  l = &(road[startPos%ROAD_SIZE]);
  if (playerSpeed > 0) backX += (l->curve * 2.0);
  if (playerSpeed < 0) backX -= (l->curve * 2.0);
  if (backX < 0.0) backX += width;
  if (backX >= width) backX -= width;
  SAGE_SetLayerView(MOUNT_LAYER, (ULONG) backX, 0, MOUNT_WIDTH, MOUNT_HEIGHT);
  if (!SAGE_BlitLayerToScreen(MOUNT_LAYER, 0, MOUNT_POSY)) {
    SAGE_AppliLog("Error blitting background layer 2 !");
    SAGE_DisplayError();
    finish = TRUE;
  }
  SAGE_SetLayerView(TREE_LAYER, (ULONG) backX*2, 0, TREE_WIDTH, TREE_HEIGHT);
  if (!SAGE_BlitLayerToScreen(TREE_LAYER, 0, TREE_POSY)) {
    SAGE_AppliLog("Error blitting background layer 3 !");
    SAGE_DisplayError();
    finish = TRUE;
  }
  // Draw road
  bitmap = SAGE_GetBackBitmap();
  if (bitmap == NULL) {
    SAGE_SetError(SERR_NO_BITMAP);
    return;
  }
  screen_buffer = (UBYTE *)bitmap->bitmap_buffer;
  screen_bpr = bitmap->bpr;
  for (n = startPos;n < (startPos + deep_view);n++) {
    l = &(road[n%ROAD_SIZE]);
    roadW = (LONG) (playerX * road_width - x);
    Project(l, roadW, camH, startPos * segment_len - ( n>=ROAD_SIZE ? ROAD_SIZE*segment_len:0));
    x += dx;
    dx += l->curve;
    l->clip = maxy;
    if (l->Y >= maxy) continue;
    maxy = l->Y;
    p = &(road[(n-1)%ROAD_SIZE]);     // Previous line
    DrawSegment(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
  }
  // Draw the car sprite
  if (!SAGE_BlitSpriteToScreen(SPR_BANK, SPR_CAR, (SCREEN_WIDTH/2) - 80 , SCREEN_HEIGHT - 86)) {
    SAGE_AppliLog("Error blitting car sprite !");
    return;
  }
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 10, 10);
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Runner demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    // Init the game data
    if (_Init()) {
      if (!SAGE_PlayMusic(MAIN_MUSIC)) {
        SAGE_DisplayError();
        finish = TRUE;
      }
      SAGE_AppliLog("Entering main loop");
      while (!finish) {
        if (SAGE_IsFrontMostScreen()) {

          // Update the game data
          _Update();
          // Render the game
          _Render();
          // Refresh the screen
          if (!SAGE_RefreshScreen()) {
            SAGE_AppliLog("Error on refresh screen !");
            SAGE_DisplayError();
            finish = TRUE;
          }

        }
      }
      // Restore the game
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
