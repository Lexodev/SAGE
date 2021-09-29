/**
 * runner.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of an out run clone
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define ROAD_SIZE             4000L
#define IAMPI                 3.141592

#define VMAX                  320

// Backgrounds
#define BG_SKY_LEFT           0L
#define BG_SKY_TOP            240L
#define BG_SKY_WIDTH          640L
#define BG_SKY_HEIGHT         240L
#define BG_SKY_LAYER          1
#define BG_SKY_POSY           0

#define BG_MOUNT_LEFT         0L
#define BG_MOUNT_TOP          0L
#define BG_MOUNT_WIDTH        640L
#define BG_MOUNT_HEIGHT       240L
#define BG_MOUNT_LAYER        2
#define BG_MOUNT_POSY         40

#define BG_TREE_LEFT          0L
#define BG_TREE_TOP           480L
#define BG_TREE_WIDTH         640L
#define BG_TREE_HEIGHT        240L
#define BG_TREE_LAYER         3
#define BG_TREE_POSY          80

// Sprites
#define SPR_BANK              0
#define NB_SPRITES            16
#define TRANSP_COLOR          0xF81F

#define SPR_CARPOSX           SCREEN_WIDTH/2
#define SPR_CARPOSY           SCREEN_HEIGHT-8

#define SPR_CARM              0
#define SPR_CARL              1
#define SPR_CARR              2
#define SPR_CARBLUE           3
#define SPR_CARPINK           4
#define SPR_CARBROWN          5
#define SPR_SIGNM             6
#define SPR_BUSH              7
#define SPR_CACTUS            8
#define SPR_TREE              9
#define SPR_SIGNC             10
#define SPR_ROCK              11
#define SPR_SEGA              12
#define SPR_TOWER             13
#define SPR_BUISSON           14
#define SPR_COCONUT           15

#define MAIN_MUSIC            0

// Engine constant params
LONG road_width = 800;        // Width of the road
LONG segment_len = 200;       // Road segment length
LONG deep_view = 100;         // Deep of player view (how many segment are displayed)
FLOAT camera_depth = 0.84;    // Camera depth (distance between projection plane and user eye)
LONG playerH = 700;           // Height of player view
LONG playerAccel = 2;         // Player acceleration value (in reality it's not constant)
LONG playerDecel = 1;         // Player deceleration value (no brakes, no accel)
LONG playerBrake = 4;         // Player brake value (the car brakes more than it accelerate)
LONG playerSlow = 3;          // Player slow value (when car is out of road)

// Player data
FLOAT playerX = 0.0;
LONG playerPos = 0, playerSpeed = 0;

// Background data
FLOAT backX = 0.0;

// Sprites data
ULONG spr_data[NB_SPRITES*4] = {
  0, 0, 160, 82,
  0, 82, 160, 82,
  0, 164, 160, 82,
  0, 246, 160, 118,
  0, 364, 176, 110,
  0, 474, 160, 114,
  180, 0, 232, 220,
  180, 220, 240, 156,
  180, 376, 236, 118,
  180, 496, 196, 140,
  440, 0, 216, 220,
  440, 226, 176, 248,
  440, 476, 268, 170,
  680, 0, 204, 314,
  680, 316, 232, 152,
  920, 0, 216, 540
};

// Cars data
#define NB_CARS               25
#define NO_CAR                -1

// Road data
#define ROAD_GRASS1           0x00CC00
#define ROAD_GRASS2           0x008800
#define ROAD_RUMBLE1          0xEE0000
#define ROAD_RUMBLE2          0xFFFFFF
#define ROAD_RUBBER1          0xCCCCCC
#define ROAD_RUBBER2          0x888888
#define ROAD_LINE1            0xFFFFFF
#define ROAD_LINE2            0xFFFF00

LONG segment_color[8];

struct RoadLine
{
  FLOAT x,y,z;      // 3D center of line
  FLOAT X,Y,W;      // Screen coord
  FLOAT curve, scale, zoom, clip;
  UWORD sprLeft, sprRight;      // Sprites elements (tree, rock, sign, etc...)
  FLOAT sprLPos, sprRPos;       // Distance from center (-3.0 => 3.0, -1.2 => 1.2 is on the road)
  WORD cars[NB_CARS+1];
  BOOL hasCar;
};
struct RoadLine road[ROAD_SIZE];

struct Car
{
  UWORD sprite;
  LONG speed, pos;
  FLOAT offset;
};
struct Car cars[NB_CARS];

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
SAGE_Screen * screen;
ULONG screen_bpr;
UWORD * screen_buffer;
LONG left_grass_crd[SCREEN_HEIGHT];
LONG left_rumble_crd[SCREEN_HEIGHT];
LONG left_road_crd[SCREEN_HEIGHT];
LONG right_grass_crd[SCREEN_HEIGHT];
LONG right_rumble_crd[SCREEN_HEIGHT];
LONG right_road_crd[SCREEN_HEIGHT];

// Game data
BOOL finish = FALSE;
UBYTE string_buffer[256];

// Demo starts here

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    screen = SAGE_GetScreen();
    segment_color[0] = SAGE_RemapColor(ROAD_GRASS1);
    segment_color[1] = SAGE_RemapColor(ROAD_GRASS2);
    segment_color[2] = SAGE_RemapColor(ROAD_RUMBLE1);
    segment_color[3] = SAGE_RemapColor(ROAD_RUMBLE2);
    segment_color[4] = SAGE_RemapColor(ROAD_RUBBER1);
    segment_color[5] = SAGE_RemapColor(ROAD_RUBBER2);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackgroundLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load background picture");
  if ((picture = SAGE_LoadPicture("data/runner_bg.png")) != NULL) {
    SAGE_AppliLog("Create sky layer");
    if (SAGE_CreateLayer(BG_SKY_LAYER, BG_SKY_WIDTH, BG_SKY_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, BG_SKY_LEFT, BG_SKY_TOP, BG_SKY_WIDTH, BG_SKY_HEIGHT, BG_SKY_LAYER, 0, 0);
    }
    SAGE_AppliLog("Create mountain layer");
    if (SAGE_CreateLayer(BG_MOUNT_LAYER, BG_MOUNT_WIDTH, BG_MOUNT_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, BG_MOUNT_LEFT, BG_MOUNT_TOP, BG_MOUNT_WIDTH, BG_MOUNT_HEIGHT, BG_MOUNT_LAYER, 0, 0);
      SAGE_SetLayerTransparency(BG_MOUNT_LAYER, TRANSP_COLOR);
    }
    SAGE_AppliLog("Create tree layer");
    if (SAGE_CreateLayer(BG_TREE_LAYER, BG_TREE_WIDTH, BG_TREE_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, BG_TREE_LEFT, BG_TREE_TOP, BG_TREE_WIDTH, BG_TREE_HEIGHT, BG_TREE_LAYER, 0, 0);
      SAGE_SetLayerTransparency(BG_TREE_LAYER, TRANSP_COLOR);
    }
    SAGE_ReleasePicture(picture);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

ULONG Rand(ULONG max)
{
  return (rand()%max);
}

FLOAT Random(VOID)
{
  return (FLOAT)(rand()/(RAND_MAX+1.0));
}

BOOL InitSprites(VOID)
{
  SAGE_Picture * picture;
  UWORD sprite;

  SAGE_AppliLog("Load sprite picture");
  if ((picture = SAGE_LoadPicture("data/runner_spr.png")) != NULL) {
    SAGE_AppliLog("Create sprites bank");
    if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
      SAGE_SetSpriteBankTransparency(SPR_BANK, TRANSP_COLOR);
      SAGE_AppliLog("Add sprites to bank");
      for (sprite = 0;sprite < NB_SPRITES;sprite++) {
        SAGE_AddSpriteToBank(
          SPR_BANK,
          sprite,
          spr_data[sprite*4],
          spr_data[sprite*4+1],
          spr_data[sprite*4+2],
          spr_data[sprite*4+3],
          SSPR_HS_BOTMID
        );
      }
    }
    SAGE_ReleasePicture(picture);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

VOID InitRoad(VOID)
{
  LONG i;

  srand(time(NULL));
  for (i = 0; i < ROAD_SIZE;i++) {
    // Default segment
    road[i].x = 0;
    road[i].y = 0;
    road[i].z = i * segment_len;
    road[i].curve = 0;
    road[i].sprLeft = road[i].sprRight = 0;
    road[i].sprLPos = -2.2;
    road[i].sprRPos = 2.2;
    road[i].cars[0] = NO_CAR;
    road[i].hasCar = FALSE;
    // Add some sprites
    if (i>10 && i<=1000) {
      if (i%20==0) {
        road[i].sprLeft = SPR_BUSH;
        if (i<800) {
          road[i].sprRight = SPR_BUSH;
        } else {
          road[i].sprRight = SPR_COCONUT;
        }
      }
    }
    if (i>1000 && i<=2000) {
      if (i%20==0) {
        road[i].sprLeft = SPR_COCONUT;
        if (i<1900) {
          road[i].sprRight = SPR_COCONUT;
        } else {
          road[i].sprRight = SPR_TOWER;
        }
      }
    }
    if (i>2000 && i<=3000) {
      if (i%20==0) {
        road[i].sprLeft = SPR_CACTUS;
        road[i].sprRight = SPR_BUSH;
      }
    }
    if (i>3000 && i<=4000) {
      if (i%20==0) {
        if (i<3200) {
          road[i].sprLeft = SPR_ROCK;
        } else {
          road[i].sprLeft = SPR_TREE;
        }
        road[i].sprRight = SPR_BUISSON;
      }
    }
    if (i==1000) road[i].sprLeft = SPR_SIGNM;
    if (i==1300) road[i].sprRight = SPR_SIGNM;
    if (i==1400) road[i].sprRight = SPR_SEGA;
    if (i==2500) road[i].sprRight = SPR_SIGNC;
    if (i==3000) road[i].sprLeft = SPR_SIGNM;
    if (i==3040) road[i].sprLeft = SPR_SEGA;
    if (i==3300) road[i].sprLeft = SPR_SIGNM;
    if (i==3500) road[i].sprRight = SPR_SIGNC;
    // Add curves
    if (i>300 && i<700) road[i].curve = 0.5;
    if (i>1100 && i<2000) road[i].curve = -0.7;
    if (i>2500 && i<2800) road[i].curve = 0.9;
    if (i>2800 && i<3200) road[i].curve -= 0.9;
    if (i>3200 && i<3400) road[i].curve = 1.5;
    if (i>3150) road[i].y = sin((i-3150)/30.0)*3000;
    // Add hills
    if (i>750 && i<1600) road[i].y = sin((i-750)/30.0)*1500;
    if (i>3600 && i<3800) road[i].curve = -1.9;
  }
}

VOID InitCars(VOID)
{
  LONG i;
  
  for (i = 0;i < NB_CARS;i++) {
    cars[i].sprite = SPR_CARBLUE + Rand(3);
    cars[i].speed = (VMAX / 4) + Rand(VMAX / 2);
    cars[i].pos = Rand(ROAD_SIZE * segment_len);
    cars[i].offset = -0.8 + (Random() * 1.6);
  }
}

BOOL InitMusic(VOID)
{
  SAGE_Music * music = NULL;

  SAGE_AppliLog("Loading music");
  if ((music = SAGE_LoadMusic("data/runner.mod")) != NULL) {
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
  if (!InitBackgroundLayer()) {
    return FALSE;
  }
  if (!InitSprites()) {
    return FALSE;
  }
  InitRoad();
  InitCars();
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
  SAGE_ReleaseLayer(BG_SKY_LAYER);
  SAGE_ReleaseLayer(BG_MOUNT_LAYER);
  SAGE_ReleaseLayer(BG_TREE_LAYER);
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

VOID UpdateCars(VOID)
{
  LONG i, s, c;

  for (i = 0;i < ROAD_SIZE;i++) {
    road[i].cars[0] = NO_CAR;
    road[i].hasCar = FALSE;
  }
  for (i = 0;i < NB_CARS;i++) {
    cars[i].pos += cars[i].speed;
    while (cars[i].pos >= (ROAD_SIZE * segment_len)) cars[i].pos -= (ROAD_SIZE * segment_len);
    s = cars[i].pos / segment_len;
    road[s].hasCar = TRUE;
    c = 0;
    while (road[s].cars[c] != NO_CAR) {
      c++;
    }
    road[s].cars[c] = i;
    road[s].cars[c+1] = NO_CAR;
  }
}

VOID _Update(VOID)
{
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) finish = TRUE;
  // Update player car
  if (keyboard_state[KEY_LEFT]) {
    playerX -= 0.02;
  } else if (keyboard_state[KEY_RIGHT]) {
    playerX += 0.02;
  }
  if (playerX < -1.6) playerX = -1.6;
  if (playerX > 1.6) playerX = 1.6;
  if (keyboard_state[KEY_UP]) {
    playerSpeed += playerAccel;
  } else if (keyboard_state[KEY_DOWN]) {
    playerSpeed -= playerBrake;
  } else {
    playerSpeed -= playerDecel;
  }
  // playerX +/- 1.20 = ralentir
  if (playerX <= -1.2 || playerX >= 1.2) {
    if (playerSpeed > 30) {
      playerSpeed -= playerSlow;
    }
  }
  if (playerSpeed < 0) playerSpeed = 0;
  if (playerSpeed > VMAX) playerSpeed = VMAX;
  // Update other cars
  UpdateCars();
}

VOID Project(struct RoadLine * line, LONG camX, LONG camY, LONG camZ)
{
  line->scale = camera_depth / (line->z - camZ);
  line->X = (1 + line->scale * (line->x - camX)) * (SCREEN_WIDTH / 2);
  line->Y = (1 - line->scale * (line->y - camY)) * (SCREEN_HEIGHT / 2);
  line->W = line->scale * road_width  * (SCREEN_WIDTH / 2);
  line->zoom = line->scale * 1000;
  if (line->zoom >= 2.0) line->zoom = 2.0;
}

VOID DrawBackground(FLOAT curve)
{
  if (playerSpeed > 0) {
    backX += (curve*(1.0+(playerSpeed/(VMAX/4))));
  }
  if (backX < 0.0) backX += SCREEN_WIDTH;
  if (backX >= SCREEN_WIDTH) backX -= SCREEN_WIDTH;
  SAGE_BlitLayerToScreen(BG_SKY_LAYER, 0, BG_SKY_POSY);
  SAGE_SetLayerView(BG_MOUNT_LAYER, (ULONG)backX, 0, BG_MOUNT_WIDTH, BG_MOUNT_HEIGHT);
  SAGE_BlitLayerToScreen(BG_MOUNT_LAYER, 0, BG_MOUNT_POSY);
  SAGE_SetLayerView(BG_TREE_LAYER, (ULONG)(backX*2), 0, BG_TREE_WIDTH, BG_TREE_HEIGHT);
  SAGE_BlitLayerToScreen(BG_TREE_LAYER, 0, BG_TREE_POSY);
}

VOID DrawSegment(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD * buffer;
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
  gcolor = (segment>>2)%2 ? segment_color[0]:segment_color[1];
  bcolor = (segment>>3)%2 ? segment_color[2]:segment_color[3];
  rcolor = (segment>>2)%2 ? segment_color[4]:segment_color[5];
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor);
  SAGE_DrawFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor);
  SAGE_DrawFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor);
  SAGE_DrawFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor);
  SAGE_DrawFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor);
}

VOID DrawRoad(LONG startPos)
{
  SAGE_Bitmap * bitmap;
  LONG roadW, camH, maxy, n;
  struct RoadLine * l, * p;
  FLOAT x = 0, dx = 0;

  camH = (LONG) road[startPos].y + playerH;
  maxy = SCREEN_HEIGHT;
  x = 0;
  dx = 0;
  bitmap = SAGE_GetBackBitmap();
  screen_buffer = (UWORD *)SAGE_GetBitmapBuffer(bitmap);
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
}

VOID DrawSprites(LONG startPos)
{
  struct RoadLine * l;
  LONG n, c;
  WORD * segCars;

  for (n = (startPos + deep_view);n > startPos;n--) {
    l = &(road[n%ROAD_SIZE]);
    screen->clipping.bottom = (LONG)l->clip;
    if (l->sprLeft != 0) {
      SAGE_SetSpriteZoom(SPR_BANK, l->sprLeft, l->zoom, l->zoom);
      SAGE_BlitSpriteToScreen(SPR_BANK, l->sprLeft, (LONG)(l->X + (l->W*l->sprLPos)), (LONG)l->Y);
    }
    if (l->sprRight != 0) {
      SAGE_SetSpriteZoom(SPR_BANK, l->sprRight, l->zoom, l->zoom);
      SAGE_BlitSpriteToScreen(SPR_BANK, l->sprRight, (LONG)(l->X + (l->W*l->sprRPos)), (LONG)l->Y);
    }
    if (l->hasCar) {
      c = 0;
      segCars = l->cars;
      while (segCars[c] != NO_CAR) {
        SAGE_SetSpriteZoom(SPR_BANK, cars[segCars[c]].sprite, l->zoom, l->zoom);
        SAGE_BlitSpriteToScreen(SPR_BANK, cars[segCars[c]].sprite, (LONG)(l->X + (l->W*cars[segCars[c]].offset)), (LONG)l->Y);
        c++;
      }
    }
  }
  screen->clipping.bottom = SCREEN_HEIGHT;
}

VOID _Render(VOID)
{
  struct RoadLine * l;
  LONG startPos, sprCar;

  // Calcul road param
  playerPos += playerSpeed;
  while (playerPos >= (ROAD_SIZE * segment_len)) playerPos -= (ROAD_SIZE * segment_len);
  startPos = playerPos / segment_len;
  // Get first segment
  l = &(road[startPos%ROAD_SIZE]);
  // Set the car sprite
  if (l->curve > 0) {
    sprCar = SPR_CARR;
  } else if (l->curve < 0) {
    sprCar = SPR_CARL;
  } else {
    sprCar = SPR_CARM;
  }
  // Move car on curve (force centrifuge)
  playerX -= (l->curve*((playerSpeed/VMAX)/40.0));
  // Draw background
  DrawBackground(l->curve);
  // Draw road
  DrawRoad(startPos);
  // Draw sprites
  DrawSprites(startPos);
  // Draw the car sprite
  SAGE_BlitSpriteToScreen(SPR_BANK, sprCar, SPR_CARPOSX, SPR_CARPOSY);
  // Draw the fps counter
  sprintf(string_buffer, "%d fps", SAGE_GetFps());
  SAGE_PrintText(string_buffer, 10, 10);
  // Draw car position
  sprintf(string_buffer, "Pos %d", playerPos);
  SAGE_PrintText(string_buffer, 250, 10);  
  // Draw the car speed
  sprintf(string_buffer, "Speed %d", playerSpeed);
  SAGE_PrintText(string_buffer, 500, 10);  
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
