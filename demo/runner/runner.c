/**
 * runner.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of an out run clone
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 2.0 March 2025
 */

// Sound effect https://www.sfxbuzz.com/summary/20-cars-trucks/294-race-car-idle-and-rev-away-sound-effects

/**
 * Runner :
 *   - Accelerate with up key
 *   - Brake with down key
 *   - Turn with left/right key
 *   - Quit with ESC
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define ROAD_SIZE             4600L
#define IAMPI                 3.141592

#define VMAX                  300

// Backgrounds
#define BG_WIDTH              932L

#define BG_CLOUDS_WIDTH       932L
#define BG_CLOUDS_HEIGHT      196L
#define BG_CLOUDS_LAYER       1
#define BG_CLOUDS_POSY        0

#define BG_MOUNTAIN_WIDTH     932L
#define BG_MOUNTAIN_HEIGHT    98L
#define BG_MOUNTAIN_LAYER     2
#define BG_MOUNTAIN_POSY      BG_CLOUDS_HEIGHT-BG_MOUNTAIN_HEIGHT

#define BG_FOREST_WIDTH       932L
#define BG_FOREST_HEIGHT      80L
#define BG_FOREST_LAYER       3
#define BG_FOREST_POSY        BG_CLOUDS_HEIGHT-24

#define BG_GRASS_WIDTH        932L
#define BG_GRASS_HEIGHT       76L
#define BG_GRASS_LAYER        4
#define BG_GRASS_POSY         BG_FOREST_POSY+BG_FOREST_HEIGHT

// Road types
#define ROAD_TYPE0            0       // Red/white rumble start line
#define ROAD_TYPE1            1       // Red/white rumble no line
#define ROAD_TYPE2            2       // Red/white rumble one line
#define ROAD_TYPE3            3       // Red/white rumble two lines
#define ROAD_TYPE4            4       // Yellow rumble no line
#define ROAD_TYPE5            5       // Yellow rumble one line

// Sprites
#define SPR_BANK              0
#define NB_SPRITES            22
#define TRANSP_COLOR          0xFF00FF

#define SPR_CARWIDTH          160
#define SPR_CARPOSX           SCREEN_WIDTH/2
#define SPR_CARPOSY           SCREEN_HEIGHT-8

#define SPR_CARM              0
#define SPR_CARL              1
#define SPR_CARR              2
#define SPR_CARBLUE           3
#define SPR_CARPINK           4
#define SPR_CARBROWN          5
#define SPR_CARTRUCK          6
#define SPR_CARSPORT          7
#define SPR_SIGNM             8
#define SPR_BUSH              9
#define SPR_CACTUS            10
#define SPR_TREE              11
#define SPR_SIGNC             12
#define SPR_ROCK              13
#define SPR_SEGA              14
#define SPR_TOWER             15
#define SPR_BUISSON           16
#define SPR_COCONUT           17
#define SPR_START             18
#define SPR_LSTART            19
#define SPR_RSTART            20
#define SPR_ARCHE             21

#define MAIN_MUSIC            0

// Engine constant params
LONG road_width = 1000;       // Width of the road
LONG segment_len = 200;       // Road segment length
LONG deep_view = 100;         // Deep of player view (how many segment are displayed)
FLOAT camera_depth = 0.84;    // Camera depth (distance between projection plane and user eye)
LONG playerH = 700;           // Height of player view
LONG playerAccel = 2;         // Player acceleration value (in reality it's not constant)
LONG playerDecel = 1;         // Player deceleration value (no brakes, no accel)
LONG playerBrake = 4;         // Player brake value (the car brakes more than it accelerate)
LONG playerSlow = 3;          // Player slow value (when car is out of road)
LONG playerSprite = SPR_CARM; // Player car sprite
FLOAT centrifugal = 0.0001;   // Centrifugal force ratio

// Engine data
LONG startPos = 0;

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
  1080, 576, 200, 156,
  1080, 740, 160, 112,
  180, 0, 232, 220,
  180, 220, 240, 156,
  180, 376, 236, 118,
  180, 496, 196, 140,
  440, 0, 216, 220,
  440, 226, 176, 248,
  440, 476, 268, 170,
  680, 0, 204, 314,
  680, 316, 232, 152,
  1064, 0, 216, 540,
  0, 660, 640, 92,
  740, 472, 132, 288,
  890, 472, 132, 288,
  0, 770, 784, 92
};

// Cars data
#define NB_CARS               40
#define NO_CAR                -1

// Road data
#define ROAD_GRASS1           0x00CC00
#define ROAD_GRASS2           0x00AA00
#define ROAD_RUMBLE1          0xEE0000
#define ROAD_RUMBLE2          0xEEEEEE
#define ROAD_RUMBLE3          0xEEEE00
#define ROAD_RUBBER1          0xCCCCCC
#define ROAD_RUBBER2          0xAAAAAA
#define ROAD_RUBBER3          0x888888
#define ROAD_LINE1            0xFFFFFF
#define ROAD_LINE2            0xFFFF00
#define ROAD_MUD1             0xFF8800
#define ROAD_MUD2             0xFF6600

LONG segment_color[12];

struct Element
{
  UWORD sprite;             // Element sprite
  FLOAT offset, height;     // Offset from center of road, height from ground
  LONG x, width;            // X/Y pos and offset of element, used for collision test
};

struct Car
{
  UWORD sprite;             // Car sprite
  LONG speed, pos;          // Car speed and position
  FLOAT offset;             // Offset for center of road
  LONG x, width;            // X pos and offset of car, used for collision test
};
struct Car cars[NB_CARS];

struct RoadLine
{
  FLOAT x,y,z;      // 3D center of line
  FLOAT X,Y,W;      // Screen coord
  FLOAT curve, scale, zoom, clip;
  struct Element sprLeft, sprRight, sprMid;
  WORD cars[NB_CARS+1], type;
  BOOL hasCar;
};
struct RoadLine road[ROAD_SIZE];

// Controls
#define KEY_NBR               8
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_SPACE             4
#define KEY_D                 5
#define KEY_P                 6
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

// Render data
SAGE_Screen * screen;
ULONG screen_bpr;
UWORD * screen_buffer;
LONG left_grass_crd[SCREEN_HEIGHT];
LONG left_rumble_crd[SCREEN_HEIGHT];
LONG left_road_crd[SCREEN_HEIGHT];
LONG left_line_crd[SCREEN_HEIGHT];
LONG right_grass_crd[SCREEN_HEIGHT];
LONG right_rumble_crd[SCREEN_HEIGHT];
LONG right_road_crd[SCREEN_HEIGHT];
LONG right_line_crd[SCREEN_HEIGHT];

// Game data
BOOL finish = FALSE;
UBYTE string_buffer[256];

// Big debug function
LONG rendered_segment;
VOID DumpDebugInfos(VOID)
{
  struct RoadLine *l, *p;
  SAGE_Sprite *spr;
  WORD *segCars, carIdx;
  FLOAT ratio;
  LONG n, c, y;

  printf("********** DEBUG DUMP **********\n");
  printf("Player pos=%d, speed=%d, x=%f\n", playerPos, playerSpeed, playerX);
  printf("Background x=%f\n", backX);
  printf("Start position=%d, deep=%d\n", startPos, deep_view);
  printf("** Segments :\n");
  for (n = startPos+1;n <= (startPos + deep_view);n++) {
    l = &(road[n%ROAD_SIZE]);
    printf(" - %d world x/y/z %f/%f/%f , screen X/Y/W %f/%f/%f , curve %f, scale %f, zoom %f, clip %f\n",
      n, l->x, l->y, l->z, l->X, l->Y, l->W, l->curve, l->scale, l->zoom, l->clip);
  }
  printf("Rendered %d\n", rendered_segment);
  printf("** Sprites :\n");
  for (n = (startPos + (deep_view-1));n > startPos;n--) {
    l = &(road[n%ROAD_SIZE]);
    if (l->sprLeft.sprite != 0 || l->sprRight.sprite != 0 || l->sprMid.sprite != 0) {
      printf(" - %d Left sprite %d, offset %f, x %d, y %d, width %d | Right sprite %d, offset %f, x %d, y %d, width %d | Mid sprite %d, offset %f, x %d, y %d, width %d\n",
        n, l->sprLeft.sprite, l->sprLeft.offset, l->sprLeft.x, (LONG)l->Y, l->sprLeft.width,
        l->sprRight.sprite, l->sprRight.offset, l->sprRight.x, (LONG)l->Y, l->sprRight.width,
        l->sprMid.sprite, l->sprMid.offset, l->sprMid.x, (LONG)l->Y, l->sprMid.width);
    }
    if (l->hasCar) {
      p = &(road[(n+1)%ROAD_SIZE]);
      ratio = (l->Y - p->Y) / segment_len;
      c = 0;
      segCars = l->cars;
      while ((carIdx = segCars[c++]) != NO_CAR) {
        if (n != startPos || cars[carIdx].pos >= playerPos) {
          spr = SAGE_GetSprite(SPR_BANK, cars[carIdx].sprite);
          y = (LONG)(l->Y - ((cars[carIdx].pos - l->z) * ratio));
          printf(" - %d Car %d sprite %d, speed %d, pos %d, offset %f, x %d, width %d, zoom %f, Y %d, z %d, nY %d, ratio %f, real y %d\n",
            n, carIdx, cars[carIdx].sprite, cars[carIdx].speed, cars[carIdx].pos, cars[carIdx].offset, cars[carIdx].x, cars[carIdx].width,
            spr->horizontal_zoom, (LONG)l->Y, (LONG)l->z, (LONG)p->Y, ratio, y);
        }
      }
    }
  }
  printf("** Collisions :\n");
  l = &(road[startPos%ROAD_SIZE]);
  if (l->sprLeft.sprite != 0) {
    n = SPR_CARPOSX - l->sprLeft.x;
    printf(" - Left n %d, x %d, width %d, car %d, diff %d\n", n, l->sprLeft.x, l->sprLeft.width, SPR_CARWIDTH, l->sprLeft.width + (SPR_CARWIDTH / 2));
  } else {
    printf(" - Left no sprite\n", n);
  }
  if (l->sprRight.sprite != 0) {
    n = SPR_CARPOSX - l->sprRight.x;
    printf(" - Right n %d, x %d, width %d, car %d, diff %d\n", n, l->sprRight.x, l->sprRight.width, SPR_CARWIDTH, l->sprRight.width + (SPR_CARWIDTH / 2));
  } else {
    printf(" - Right no sprite\n", n);
  }
  if (l->hasCar) {
    c = 0;
    segCars = l->cars;
    while ((carIdx = segCars[c++]) != NO_CAR) {
      n = SPR_CARPOSX - cars[carIdx].x;
      printf(" - Car id %d, x %d, width %d, car %d, diff %d\n", carIdx, cars[carIdx].x, cars[carIdx].width, SPR_CARWIDTH, cars[carIdx].width + (SPR_CARWIDTH / 2));
    }
  }
  printf("********************************\n");
}

// Demo starts here

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    screen = SAGE_GetScreen();
    SAGE_SetColor(0, 0x0);
    SAGE_SetColor(1, 0xffffff);
    SAGE_RefreshColors(0, 2);
    SAGE_SetTextColor(1, 0);
    segment_color[0] = SAGE_RemapColor(ROAD_GRASS1);
    segment_color[1] = SAGE_RemapColor(ROAD_GRASS2);
    segment_color[2] = SAGE_RemapColor(ROAD_RUMBLE1);
    segment_color[3] = SAGE_RemapColor(ROAD_RUMBLE2);
    segment_color[4] = SAGE_RemapColor(ROAD_RUBBER1);
    segment_color[5] = SAGE_RemapColor(ROAD_RUBBER2);
    segment_color[6] = SAGE_RemapColor(ROAD_LINE1);
    segment_color[7] = SAGE_RemapColor(ROAD_LINE2);
    segment_color[8] = SAGE_RemapColor(ROAD_RUMBLE3);
    segment_color[9] = SAGE_RemapColor(ROAD_RUBBER3);
    segment_color[10] = SAGE_RemapColor(ROAD_MUD1);
    segment_color[11] = SAGE_RemapColor(ROAD_MUD2);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackgroundLayer(VOID)
{
  SAGE_Picture *picture;

  SAGE_AppliLog("Load background pictures");
  if ((picture = SAGE_LoadPicture("data/bg_clouds.png")) != NULL) {
    SAGE_AppliLog("Create clouds layer");
    if (SAGE_CreateLayer(BG_CLOUDS_LAYER, BG_CLOUDS_WIDTH, BG_CLOUDS_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, BG_CLOUDS_WIDTH, BG_CLOUDS_HEIGHT, BG_CLOUDS_LAYER, 0, 0);
    }
    SAGE_ReleasePicture(picture);
  } else {
    SAGE_DisplayError();
    return FALSE;
  }
  if ((picture = SAGE_LoadPicture("data/bg_mountain.png")) != NULL) {
    SAGE_AppliLog("Create mountain layer");
    if (SAGE_CreateLayer(BG_MOUNTAIN_LAYER, BG_MOUNTAIN_WIDTH, BG_MOUNTAIN_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, BG_MOUNTAIN_WIDTH, BG_MOUNTAIN_HEIGHT, BG_MOUNTAIN_LAYER, 0, 0);
      SAGE_SetLayerTransparency(BG_MOUNTAIN_LAYER, TRANSP_COLOR);
    }
    SAGE_ReleasePicture(picture);
  } else {
    SAGE_DisplayError();
    return FALSE;
  }
  if ((picture = SAGE_LoadPicture("data/bg_forest.png")) != NULL) {
    SAGE_AppliLog("Create forest layer");
    if (SAGE_CreateLayer(BG_FOREST_LAYER, BG_FOREST_WIDTH, BG_FOREST_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, BG_FOREST_WIDTH, BG_FOREST_HEIGHT, BG_FOREST_LAYER, 0, 0);
      SAGE_SetLayerTransparency(BG_FOREST_LAYER, TRANSP_COLOR);
    }
    SAGE_ReleasePicture(picture);
  } else {
    SAGE_DisplayError();
    return FALSE;
  }
  if ((picture = SAGE_LoadPicture("data/bg_grass.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayer(BG_GRASS_LAYER, BG_GRASS_WIDTH, BG_GRASS_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, BG_GRASS_WIDTH, BG_GRASS_HEIGHT, BG_GRASS_LAYER, 0, 0);
    }
    SAGE_ReleasePicture(picture);
  } else {
    SAGE_DisplayError();
    return FALSE;
  }
  return TRUE;
}

FLOAT Random(VOID)
{
  return (FLOAT)(rand()/(RAND_MAX+1.0));
}

ULONG Rand(ULONG max)
{
  return (ULONG)(Random() * max);
}

BOOL InitSprites(VOID)
{
  SAGE_Picture *picture;
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

  for (i = 0; i < ROAD_SIZE;i++) {
    // Default segment
    road[i].x = 0;
    road[i].y = 0;
    road[i].z = i * segment_len;
    road[i].curve = 0;
    road[i].sprLeft.sprite = road[i].sprRight.sprite = road[i].sprMid.sprite = 0;
    road[i].sprLeft.offset = -2.0;
    road[i].sprRight.offset = 2.0;
    road[i].sprMid.offset = 0.0;
    road[i].cars[0] = NO_CAR;
    road[i].hasCar = FALSE;
    road[i].type = ROAD_TYPE1;
    // Add some sprites (coconut by default)
    if (i%20==0) {
      road[i].sprLeft.sprite = SPR_COCONUT;
      road[i].sprRight.sprite = SPR_COCONUT;
    }
    if (i==10) {
      road[i].type = ROAD_TYPE0;
      road[i].sprLeft.sprite = SPR_LSTART;
      road[i].sprLeft.offset = -1.0;
      road[i].sprRight.sprite = SPR_RSTART;
      road[i].sprRight.offset = 1.0;
      road[i].sprMid.sprite = SPR_START;
      road[i].sprMid.height = 1000.0;
    }
    if (i>100 && i<=1000) {
      road[i].type = ROAD_TYPE3;
      if (i%20==0) {
        road[i].sprLeft.sprite = SPR_BUSH;
        if (i<800) {
          road[i].sprRight.sprite = SPR_BUSH;
        } else {
          road[i].sprRight.sprite = SPR_COCONUT;
        }
      }
    }
    if (i>1000 && i<=2000) {
      road[i].type = ROAD_TYPE2;
      if (i%20==0) {
        road[i].sprLeft.sprite = SPR_COCONUT;
        if (i<1900) {
          road[i].sprRight.sprite = SPR_COCONUT;
        } else {
          road[i].sprRight.sprite = SPR_ROCK;
        }
      }
    }
    if (i>2000 && i<=3000) {
      road[i].type = ROAD_TYPE4;
      if (i%20==0) {
        road[i].sprLeft.sprite = SPR_CACTUS;
        road[i].sprRight.sprite = SPR_BUSH;
      }
    }
    if (i>3000 && i<=4000) {
      road[i].type = ROAD_TYPE5;
      if (i%20==0) {
        if (i<3200) {
          road[i].sprLeft.sprite = SPR_ROCK;
        } else {
          road[i].sprLeft.sprite = SPR_TREE;
        }
        road[i].sprRight.sprite = SPR_BUISSON;
      }
    }
    if (i>4200 && i<4500) {
      if (i%10==0) {
        road[i].sprLeft.sprite = SPR_TOWER;
        road[i].sprLeft.offset = -1.3;
        road[i].sprRight.sprite = SPR_TOWER;
        road[i].sprRight.offset = 1.3;
        road[i].sprMid.sprite = SPR_ARCHE;
        road[i].sprMid.height = 1000.0;
      }
    }
    if (i==1000) road[i].sprLeft.sprite = SPR_SIGNM;
    if (i==1300) road[i].sprRight.sprite = SPR_SIGNM;
    if (i==1400) road[i].sprRight.sprite = SPR_SEGA;
    if (i==2500) road[i].sprRight.sprite = SPR_SIGNC;
    if (i==3000) road[i].sprLeft.sprite = SPR_SIGNM;
    if (i==3040) road[i].sprLeft.sprite = SPR_SEGA;
    if (i==3300) road[i].sprLeft.sprite = SPR_SIGNM;
    if (i==3500) road[i].sprRight.sprite = SPR_SIGNC;
    // Add curves
    if (i>300 && i<700) road[i].curve = 0.5;
    if (i>1100 && i<2000) road[i].curve = -0.7;
    if (i>2500 && i<2800) road[i].curve = 0.9;
    if (i>2800 && i<3200) road[i].curve -= 0.9;
    if (i>3200 && i<3400) road[i].curve = 1.5;
    if (i>3600 && i<3800) road[i].curve = -1.9;
    if (i>4100 && i<4600) road[i].curve = 0.4;
    // Add hills
    if (i>750 && i<1600) road[i].y = sin((i-750)/30.0)*1500;
    if (i>3150 && i<4000) road[i].y = sin((i-3150)/30.0)*3000;
  }
}

VOID InitCars(VOID)
{
  LONG i;
  
  srand(time(NULL));
  for (i = 0;i < NB_CARS;i++) {
    cars[i].sprite = SPR_CARBLUE + Rand(5);
    cars[i].speed = (VMAX / 4) + Rand(VMAX / 2);
    cars[i].pos = Rand(ROAD_SIZE * segment_len);
    cars[i].offset = -0.8 + (Random() * 1.6);
  }
}

BOOL InitMusic(VOID)
{
  SAGE_Music *music = NULL;

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
  SAGE_ReleaseLayer(BG_CLOUDS_LAYER);
  SAGE_ReleaseLayer(BG_MOUNTAIN_LAYER);
  SAGE_ReleaseLayer(BG_FOREST_LAYER);
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
    keyboard_state[KEY_D] = keys[KEY_D].key_pressed;
    keyboard_state[KEY_P] = keys[KEY_P].key_pressed;
    keyboard_state[KEY_QUIT] = keys[KEY_QUIT].key_pressed;
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
    while (road[s].cars[c] != NO_CAR) { c++; }
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
    playerX -= 0.03;
    playerSprite = SPR_CARL;
  } else if (keyboard_state[KEY_RIGHT]) {
    playerX += 0.03;
    playerSprite = SPR_CARR;
  } else {
    playerSprite = SPR_CARM;
  }
  if (playerX < -2.0) playerX = -2.0;
  if (playerX > 2.0) playerX = 2.0;
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
  playerPos += playerSpeed;
  while (playerPos >= (ROAD_SIZE * segment_len)) playerPos -= (ROAD_SIZE * segment_len);
  startPos = playerPos / segment_len;
  // Update other cars
  UpdateCars();
}

VOID Project(struct RoadLine *line, LONG camX, LONG camY, LONG camZ)
{
  line->scale = camera_depth / (line->z - camZ);
  line->X = (1 + line->scale * (line->x - camX)) * (SCREEN_WIDTH / 2);
  line->Y = (1 - line->scale * (line->y - camY)) * (SCREEN_HEIGHT / 2);
  line->W = line->scale * road_width  * (SCREEN_WIDTH / 2);
  line->zoom = line->scale * 1000;
  if (line->zoom >= 2.0) line->zoom = 2.0;
}

VOID DrawBackground(VOID)
{
  struct RoadLine *l;

  l = &(road[startPos%ROAD_SIZE]);
  if (playerSpeed > 0) {
    backX += (l->curve*(1.0+(playerSpeed/(VMAX/4))));
  }
  if (backX < 0.0) backX += BG_WIDTH;
  if (backX >= BG_WIDTH) backX -= BG_WIDTH;
  SAGE_SetLayerView(BG_CLOUDS_LAYER, (ULONG)backX, 0, SCREEN_WIDTH, BG_CLOUDS_HEIGHT);
  SAGE_BlitLayerToScreen(BG_CLOUDS_LAYER, 0, BG_CLOUDS_POSY);
  SAGE_SetLayerView(BG_MOUNTAIN_LAYER, (ULONG)(backX*2), 0, SCREEN_WIDTH, BG_MOUNTAIN_HEIGHT);
  SAGE_BlitLayerToScreen(BG_MOUNTAIN_LAYER, 0, BG_MOUNTAIN_POSY);
  SAGE_SetLayerView(BG_FOREST_LAYER, (ULONG)(backX*3), 0, SCREEN_WIDTH, BG_FOREST_HEIGHT);
  SAGE_BlitLayerToScreen(BG_FOREST_LAYER, 0, BG_FOREST_POSY);
  SAGE_SetLayerView(BG_GRASS_LAYER, (ULONG)(backX*4), 0, SCREEN_WIDTH, BG_GRASS_HEIGHT);
  SAGE_BlitLayerToScreen(BG_GRASS_LAYER, 0, BG_GRASS_POSY);
}

VOID DrawRoadType0(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD *buffer;
  LONG gcolor, bcolor, rcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  rendered_segment++;         // DEBUG !!!!
  end = (LONG) y2;
  row = SAGE_ClippedEdgeCalc(left_grass_crd, 0, start, 0, end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, clip.top, clip.bottom);
  gcolor = (segment>>2)%2 ? segment_color[0]:segment_color[1];
  bcolor = (segment>>3)%2 ? segment_color[2]:segment_color[3];
  rcolor = segment_color[6];
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor, clip.left, clip.right);
}

VOID DrawRoadType1(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD *buffer;
  LONG gcolor, bcolor, rcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  rendered_segment++;         // DEBUG !!!!
  end = (LONG) y2;
  row = SAGE_ClippedEdgeCalc(left_grass_crd, 0, start, 0, end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, clip.top, clip.bottom);
  gcolor = (segment>>2)%2 ? segment_color[0]:segment_color[1];
  bcolor = (segment>>3)%2 ? segment_color[2]:segment_color[3];
  rcolor = (segment>>2)%2 ? segment_color[4]:segment_color[5];
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor, clip.left, clip.right);
}

VOID DrawRoadType2(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD *buffer;
  LONG gcolor, bcolor, rcolor, lcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  rendered_segment++;         // DEBUG !!!!
  end = (LONG) y2;
  row = SAGE_ClippedEdgeCalc(left_grass_crd, 0, start, 0, end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, clip.top, clip.bottom);
  gcolor = (segment>>2)%2 ? segment_color[0]:segment_color[1];
  bcolor = (segment>>3)%2 ? segment_color[2]:segment_color[3];
  rcolor = (segment>>2)%2 ? segment_color[4]:segment_color[5];
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  // Add middle line
  if ((segment>>3)%2 == 0) {
    lcolor = segment_color[6];
    SAGE_ClippedEdgeCalc(left_line_crd, (LONG)(x1 - (w1*0.05)), start, (LONG)(x2 - (w2*0.05)), end, clip.top, clip.bottom);
    SAGE_ClippedEdgeCalc(right_line_crd, (LONG)(x1 + (w1*0.05)), start, (LONG)(x2 + (w2*0.05)), end, clip.top, clip.bottom);
    SAGE_DrawClippedFlatQuad16Bits(buffer, left_line_crd, right_line_crd, row, screen_bpr, lcolor, clip.left, clip.right);
  }
}

VOID DrawRoadType3(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD *buffer;
  LONG gcolor, bcolor, rcolor, lcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  rendered_segment++;         // DEBUG !!!!
  end = (LONG) y2;
  row = SAGE_ClippedEdgeCalc(left_grass_crd, 0, start, 0, end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, clip.top, clip.bottom);
  gcolor = (segment>>2)%2 ? segment_color[0]:segment_color[1];
  bcolor = (segment>>3)%2 ? segment_color[2]:segment_color[3];
  rcolor = (segment>>2)%2 ? segment_color[4]:segment_color[5];
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  // Add two lines
  if ((segment>>3)%2 == 0) {
    lcolor = segment_color[6];
    SAGE_ClippedEdgeCalc(left_line_crd, (LONG)(x1 - (w1*0.4)), start, (LONG)(x2 - (w2*0.4)), end, clip.top, clip.bottom);
    SAGE_ClippedEdgeCalc(right_line_crd, (LONG)(x1 - (w1*0.35)), start, (LONG)(x2 - (w2*0.35)), end, clip.top, clip.bottom);
    SAGE_DrawClippedFlatQuad16Bits(buffer, left_line_crd, right_line_crd, row, screen_bpr, lcolor, clip.left, clip.right);
    SAGE_ClippedEdgeCalc(left_line_crd, (LONG)(x1 + (w1*0.35)), start, (LONG)(x2 + (w2*0.35)), end, clip.top, clip.bottom);
    SAGE_ClippedEdgeCalc(right_line_crd, (LONG)(x1 + (w1*0.4)), start, (LONG)(x2 + (w2*0.4)), end, clip.top, clip.bottom);
    SAGE_DrawClippedFlatQuad16Bits(buffer, left_line_crd, right_line_crd, row, screen_bpr, lcolor, clip.left, clip.right);
  }
}

VOID DrawRoadType4(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD *buffer;
  LONG gcolor, bcolor, rcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  rendered_segment++;         // DEBUG !!!!
  end = (LONG) y2;
  row = SAGE_ClippedEdgeCalc(left_grass_crd, 0, start, 0, end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, clip.top, clip.bottom);
  gcolor = (segment>>2)%2 ? segment_color[0]:segment_color[1];
  rcolor = (segment>>2)%2 ? segment_color[4]:segment_color[5];
  bcolor = (segment>>3)%2 ? segment_color[8]:rcolor;
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor, clip.left, clip.right);
}

VOID DrawRoadType5(LONG segment, FLOAT x1, FLOAT y1, FLOAT w1, FLOAT x2, FLOAT y2, FLOAT w2)
{
  UWORD *buffer;
  LONG gcolor, bcolor, rcolor, lcolor, start, end, row;
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};

  start = (LONG) y1;
  if (start >= SCREEN_HEIGHT) return;
  rendered_segment++;         // DEBUG !!!!
  end = (LONG) y2;
  row = SAGE_ClippedEdgeCalc(left_grass_crd, 0, start, 0, end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_rumble_crd, (LONG)(x1 - (w1*1.2)), start, (LONG)(x2 - (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(left_road_crd, (LONG)(x1 - w1), start, (LONG)(x2 - w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_road_crd, (LONG)(x1 + w1), start, (LONG)(x2 + w2), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_rumble_crd, (LONG)(x1 + (w1*1.2)), start, (LONG)(x2 + (w2*1.2)), end, clip.top, clip.bottom);
  SAGE_ClippedEdgeCalc(right_grass_crd, SCREEN_WIDTH-1, start, SCREEN_WIDTH-1, end, clip.top, clip.bottom);
  gcolor = (segment>>2)%2 ? segment_color[10]:segment_color[11];
  rcolor = segment_color[9];
  bcolor = (segment>>3)%2 ? segment_color[8]:rcolor;
  buffer = screen_buffer + (start*SCREEN_WIDTH);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_grass_crd, left_rumble_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_rumble_crd, left_road_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, left_road_crd, right_road_crd, row, screen_bpr, rcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_road_crd, right_rumble_crd, row, screen_bpr, bcolor, clip.left, clip.right);
  SAGE_DrawClippedFlatQuad16Bits(buffer, right_rumble_crd, right_grass_crd, row, screen_bpr, gcolor, clip.left, clip.right);
  // Add middle line
  if ((segment>>3)%2 == 0) {
    lcolor = segment_color[7];
    SAGE_ClippedEdgeCalc(left_line_crd, (LONG)(x1 - (w1*0.05)), start, (LONG)(x2 - (w2*0.05)), end, clip.top, clip.bottom);
    SAGE_ClippedEdgeCalc(right_line_crd, (LONG)(x1 + (w1*0.05)), start, (LONG)(x2 + (w2*0.05)), end, clip.top, clip.bottom);
    SAGE_DrawClippedFlatQuad16Bits(buffer, left_line_crd, right_line_crd, row, screen_bpr, lcolor, clip.left, clip.right);
  }
}

VOID DrawRoad(VOID)
{
  SAGE_Bitmap *bitmap;
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
  rendered_segment = 0;         // DEBUG !!!!
  for (n = startPos+1;n <= (startPos + deep_view);n++) {
    l = &(road[n%ROAD_SIZE]);
    roadW = (LONG) (playerX * road_width - x);
    Project(l, roadW, camH, startPos * segment_len - ( n>=ROAD_SIZE ? ROAD_SIZE*segment_len:0));
    x += dx;
    dx += l->curve;
    l->clip = maxy;
    if (l->Y >= maxy) continue;
    maxy = l->Y;
    p = &(road[(n-1)%ROAD_SIZE]);     // Previous line
    switch (l->type) {
      case ROAD_TYPE1:
        DrawRoadType1(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
        break;
      case ROAD_TYPE2:
        DrawRoadType2(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
        break;
      case ROAD_TYPE3:
        DrawRoadType3(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
        break;
      case ROAD_TYPE4:
        DrawRoadType4(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
        break;
      case ROAD_TYPE5:
        DrawRoadType5(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
        break;
      default:
        DrawRoadType0(n, l->X, l->Y, l->W, p->X, p->Y, p->W);
    }
  }
}

VOID DrawSprites(VOID)
{
  struct RoadLine *l, *p;
  LONG n, c, y;
  WORD *segCars, carIdx;
  FLOAT ratio;
  SAGE_Sprite *spr;

  for (n = (startPos + (deep_view-1));n > startPos;n--) {
    l = &(road[n%ROAD_SIZE]);
    screen->clipping.bottom = (LONG)l->clip;
    if (l->sprLeft.sprite != 0) {
      SAGE_SetSpriteZoom(SPR_BANK, l->sprLeft.sprite, l->zoom, l->zoom);
      spr = SAGE_GetSprite(SPR_BANK, l->sprLeft.sprite);
      l->sprLeft.x = (LONG)(l->X + (l->W*l->sprLeft.offset));
      l->sprLeft.width = spr->width / 2;
      SAGE_BlitSpriteToScreen(SPR_BANK, l->sprLeft.sprite, l->sprLeft.x, (LONG)l->Y);
    }
    if (l->sprRight.sprite != 0) {
      SAGE_SetSpriteZoom(SPR_BANK, l->sprRight.sprite, l->zoom, l->zoom);
      spr = SAGE_GetSprite(SPR_BANK, l->sprRight.sprite);
      l->sprRight.x = (LONG)(l->X + (l->W*l->sprRight.offset));
      l->sprRight.width = spr->width / 2;
      SAGE_BlitSpriteToScreen(SPR_BANK, l->sprRight.sprite, l->sprRight.x, (LONG)l->Y);
    }
    if (l->sprMid.sprite != 0) {
      SAGE_SetSpriteZoom(SPR_BANK, l->sprMid.sprite, l->zoom, l->zoom);
      l->sprMid.x = (LONG)(l->X + (l->W*l->sprMid.offset));
      y = (LONG)((1 - l->scale * (l->sprMid.height - (road[startPos].y + playerH))) * (SCREEN_HEIGHT / 2));
      SAGE_BlitSpriteToScreen(SPR_BANK, l->sprMid.sprite, l->sprMid.x, y);
    }
    if (l->hasCar) {
      p = &(road[(n+1)%ROAD_SIZE]);
      ratio = (l->Y - p->Y) / segment_len;
      c = 0;
      segCars = l->cars;
      while ((carIdx = segCars[c++]) != NO_CAR) {
        if (n != startPos || cars[carIdx].pos >= playerPos) {
          SAGE_SetSpriteZoom(SPR_BANK, cars[carIdx].sprite, l->zoom, l->zoom);
          spr = SAGE_GetSprite(SPR_BANK, cars[carIdx].sprite);
          cars[carIdx].x = (LONG)(l->X + (l->W*cars[carIdx].offset));
          cars[carIdx].width = spr->width / 2;
          y = (LONG)(l->Y - ((cars[carIdx].pos - l->z) * ratio));
          SAGE_BlitSpriteToScreen(SPR_BANK, cars[carIdx].sprite, cars[carIdx].x, y);
        }
      }
    }
  }
  screen->clipping.bottom = SCREEN_HEIGHT;
}

VOID DrawPlayer(VOID)
{
  struct RoadLine *l, *p;
  LONG n, c;
  WORD *segCars, carIdx;

  // Get segment of car
  l = &(road[startPos%ROAD_SIZE]);
  // Move car on curve (centrifugal force)
  playerX -= (l->curve * playerSpeed * centrifugal);
  // Get segment in front of car
  p = &(road[(startPos+4)%ROAD_SIZE]);
  // Check player collision with elements
  if (p->sprLeft.sprite != 0) {
    n = SPR_CARPOSX - p->sprLeft.x;
    if (n < (p->sprLeft.width + (SPR_CARWIDTH / 2)) && n > -(p->sprLeft.width + (SPR_CARWIDTH / 2))) {
      playerSpeed = 0;
    }
  }
  if (p->sprRight.sprite != 0) {
    n = SPR_CARPOSX - p->sprRight.x;
    if (n < (p->sprRight.width + (SPR_CARWIDTH / 2)) && n > -(p->sprRight.width + (SPR_CARWIDTH / 2))) {
      playerSpeed = 0;
    }
  }
  // Check player collision with car
  if (l->hasCar) {
    c = 0;
    segCars = l->cars;
    while ((carIdx = segCars[c++]) != NO_CAR) {
      // Simple collision test
      n = SPR_CARPOSX - cars[carIdx].x;
      if (n < (cars[carIdx].width + (SPR_CARWIDTH / 2)) && n > -(cars[carIdx].width + (SPR_CARWIDTH / 2))) {
        playerPos = cars[carIdx].pos;
        playerSpeed = 0;
      }
    }
  }
  SAGE_BlitSpriteToScreen(SPR_BANK, playerSprite, SPR_CARPOSX, SPR_CARPOSY);
}

VOID _Render(VOID)
{
  // Draw background
  DrawBackground();
  // Draw road
  DrawRoad();
  // Draw sprites
  DrawSprites();
  // Draw the car sprite
  DrawPlayer();
  // Draw the car speed
  SAGE_PrintFText(10, 10, "Speed %d", playerSpeed);
  // Draw the fps counter
  SAGE_PrintFText(580, 10, "%d fps", SAGE_GetFps());
  if (keyboard_state[KEY_D]) DumpDebugInfos();
}

void main(void)
{
//  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Runner demo V1.5 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloCore()) {
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
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
