/**
 * odysseus.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of multiscroll tiled game
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 December 2020
 */

/**
 * Odysseus :
 *   - Move with cursor keys
 *   - Shoot with CTRL
 *   - Quit with ESC
 */

#include <stdio.h>
#include <stdlib.h>

#include <sage/sage.h>

#include <proto/dos.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define PLAYFIELD_WIDTH       SCREEN_WIDTH+(TILE_WIDTH*2)
#define PLAYFIELD_HEIGHT      SCREEN_HEIGHT+(TILE_HEIGHT*2)
#define PLAYFIELD_LAYER       0

#define VIEW_WIDTH            320L
#define VIEW_HEIGHT           200L

#define TILE_WIDTH            32L
#define TILE_HEIGHT           20L
#define TILE_NB               160L
#define TILE_NBWIDTH          VIEW_WIDTH/TILE_WIDTH
#define TILE_NBHEIGHT         VIEW_HEIGHT/TILE_HEIGHT
#define TILE_BANK             0
#define TILE_NBWALLS          10
#define TILE_WALLFLAG         1

#define MAP_WIDTH             100
#define MAP_HEIGHT            200
#define MAP_STARTX            0
#define MAP_STARTY            190*TILE_HEIGHT
#define MAP_MAX_X             MAP_WIDTH*TILE_WIDTH
#define MAP_MAX_Y             MAP_HEIGHT*TILE_HEIGHT
#define MAP_BANK              0

#define SPR_BANK              0
#define NB_SPRITES            32

#define TANK_NBSPR            8
#define TANK_WIDTH            16
#define TANK_HEIGHT           16
#define TANK_TOP              0
#define TANK_LEFT             128
#define TANK_STARTX           140
#define TANK_STARTY           197*TILE_HEIGHT
#define TANK_DELTAX           (VIEW_WIDTH/2)-(TANK_WIDTH/2)
#define TANK_DELTAY           (VIEW_HEIGHT/2)-(TANK_HEIGHT/2)

#define TANK_SPRDOWN          0
#define TANK_SPRDOWNLEFT      1
#define TANK_SPRLEFT          2
#define TANK_SPRUPLEFT        3
#define TANK_SPRUP            4
#define TANK_SPRUPRIGHT       5
#define TANK_SPRRIGHT         6
#define TANK_SPRDOWNRIGHT     7

#define MISSILE_NBSPR         8
#define MISSILE_WIDTH         16
#define MISSILE_HEIGHT        16
#define MISSILE_TOP           16
#define MISSILE_LEFT          128
#define MISSILE_DELAY         16
#define MISSILE_MAX           10
#define MISSILE_SPEED         2

#define PANEL_LAYER           1
#define PANEL_LEFT            0
#define PANEL_TOP             190
#define PANEL_WIDTH           320
#define PANEL_HEIGHT          20
#define PANEL_POSX            0
#define PANEL_POSY            202

#define TRANSP_COLOR          0

#define MOVE_NONE             0
#define MOVE_LEFT             1
#define MOVE_RIGHT            2
#define MOVE_UP               4
#define MOVE_DOWN             8

#define SCAN_KEYS             7
#define KEY_QUIT              0
#define KEY_LEFT              1
#define KEY_RIGHT             2
#define KEY_UP                3
#define KEY_DOWN              4
#define KEY_FIRE              5
#define KEY_ENTER             6

#define MAIN_MUSIC            0

#define SOUND_SHOOT           1
#define SOUND_EXPLOSION       2

struct Map {
  LONG posx, posy;
};

struct Player {
  UWORD sprite, next_missile, direction;
  LONG posx, posy;
};

struct Missile {
  UWORD sprite;
  LONG posx, posy, deltax, deltay;
  BOOL active;
};

struct Map map;
struct Player player;
struct Missile missiles[MISSILE_MAX];

UWORD walls[TILE_NBWALLS] = {5, 6, 7, 16, 17, 25, 27, 94, 95, 97};

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    SAGE_SetTextColor(2, 1);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitPlayfieldLayer(VOID)
{
  SAGE_AppliLog("Create playfield layer");
  if (SAGE_CreateLayer(PLAYFIELD_LAYER, PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT)) {
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTiles(VOID)
{
  SAGE_Picture * picture = NULL;
  UWORD index;

  SAGE_AppliLog("Load tile picture");
  if ((picture = SAGE_LoadPicture("data/Odysseus_Tiles.bmp")) != NULL) {
    SAGE_LoadPictureColorMap(picture);
    SAGE_RefreshColors(0, 256);
    SAGE_AppliLog("Create tile bank");
    if (SAGE_CreateTileBank(TILE_BANK, TILE_WIDTH, TILE_HEIGHT, TILE_NB, picture)) {
      SAGE_AddTilesToBank(TILE_BANK);
      // Tell which tile is a wall
      for (index = 0;index < TILE_NBWALLS;index++) {
        SAGE_SetTileFlags(TILE_BANK, walls[index], TILE_WALLFLAG);
      }
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTileMap(VOID)
{
  SAGE_AppliLog("Create tile map");
  if (SAGE_CreateTileMap(MAP_BANK, MAP_WIDTH, MAP_HEIGHT, STIL_MAPBPT_BYTE)) {
    SAGE_AppliLog("Load map level");
    if (SAGE_LoadTileMap(MAP_BANK, "data/Odysseus_Level1.map")) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOL InitSprites(VOID)
{
  SAGE_Picture * picture;
  UWORD sprite;
  ULONG top, left;

  SAGE_AppliLog("Load sprite picture");
  if ((picture = SAGE_LoadPicture("data/Odysseus_Sprites.bmp")) != NULL) {
    SAGE_AppliLog("Create sprites bank");
    if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
      SAGE_SetSpriteBankTransparency(SPR_BANK, TRANSP_COLOR);
      SAGE_AppliLog("Add tank sprites to bank");
      left = TANK_LEFT;
      top = TANK_TOP;
      for (sprite = 0;sprite < TANK_NBSPR;sprite++) {
        if (!SAGE_AddSpriteToBank(SPR_BANK, sprite, left, top, TANK_WIDTH, TANK_HEIGHT, SSPR_HS_TOPLEFT)) {
          SAGE_DisplayError();
        }
        left += TANK_WIDTH;
      }
      SAGE_AppliLog("Add missile sprites to bank");
      left = MISSILE_LEFT;
      top = MISSILE_TOP;
      for (sprite = TANK_NBSPR;sprite < MISSILE_NBSPR+TANK_NBSPR;sprite++) {
        if (!SAGE_AddSpriteToBank(SPR_BANK, sprite, left, top, MISSILE_WIDTH, MISSILE_HEIGHT, SSPR_HS_TOPLEFT)) {
          SAGE_DisplayError();
        }
        left += MISSILE_WIDTH;
      }
      SAGE_AppliLog("Create panel layer");
      if (SAGE_CreateLayer(PANEL_LAYER, PANEL_WIDTH, PANEL_HEIGHT)) {
        SAGE_BlitPictureToLayer(picture, PANEL_LEFT, PANEL_TOP, PANEL_WIDTH, PANEL_HEIGHT, PANEL_LAYER, 0, 0);
        SAGE_ReleasePicture(picture);
        return TRUE;
      }
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGraphx(VOID)
{
  SAGE_AppliLog("Init graphx");
  SAGE_SetScreenClip(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
  if (!InitPlayfieldLayer()) {
    return FALSE;
  }
  if (!InitTiles()) {
    return FALSE;
  }
  if (!InitTileMap()) {
    return FALSE;
  }
  if (!InitSprites()) {
    return FALSE;
  }
  return TRUE;
}

VOID ReleaseGraphx(VOID)
{
  SAGE_AppliLog("Release graphx");
  SAGE_ReleaseTileMap(MAP_BANK);
  SAGE_ReleaseTileBank(TILE_BANK);
  SAGE_ReleaseLayer(PLAYFIELD_LAYER);
  SAGE_ReleaseLayer(PANEL_LAYER);
  SAGE_ReleaseSpriteBank(SPR_BANK);
}

BOOL InitMusic(VOID)
{
  SAGE_Music * music = NULL;

  SAGE_AppliLog("Loading music");
  if ((music = SAGE_LoadMusic("data/theme.mod")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (SAGE_AddMusic(MAIN_MUSIC, music)) {
      return TRUE;
    }
  }
  return FALSE;
}

VOID ReleaseMusic(VOID)
{
  SAGE_AppliLog("Release music");
  SAGE_StopMusic();
  SAGE_ClearMusic();
}

BOOL InitSound(VOID)
{
  SAGE_Sound * sound1 = NULL, * sound2 = NULL;

  SAGE_AppliLog("Loading sounds");
  sound1 = SAGE_LoadSound("data/shoot.wav");
  sound2 = SAGE_LoadSound("data/explosion.wav");
  if (sound1 != NULL && sound2 != NULL) {
    sound1->volume = 65536*2;
    sound2->volume = 65536*2;
    if (SAGE_AddSound(SOUND_SHOOT, sound1) && SAGE_AddSound(SOUND_EXPLOSION, sound2)) {
      return TRUE;
    }
  }
  return FALSE;
}

VOID ReleaseSound(VOID)
{
  SAGE_ClearSound();
}

BOOL LoadResources(VOID)
{
  if (InitGraphx() && InitMusic() && InitSound()) {
    return TRUE;
  }
  return FALSE;
}

VOID ReleaseResources(VOID)
{
  ReleaseSound();
  ReleaseMusic();
  ReleaseGraphx();
}

VOID BuildPlayfield(VOID)
{
  UBYTE * level_map;
  UWORD x_tile, y_tile, map_col, map_row;
  UBYTE index;
  LONG posx, posy;

  level_map = SAGE_GetTileMapB(MAP_BANK);
  map_col = map.posy / TILE_HEIGHT;
  map_row = map.posx / TILE_WIDTH;
  posy = map.posy;
  for (y_tile = map_col;y_tile < (map_col + TILE_NBHEIGHT);y_tile++) {
    posx = map.posx;
    for (x_tile = map_row;x_tile < (map_row + TILE_NBWIDTH);x_tile++) {
      index = level_map[(y_tile * MAP_WIDTH) + x_tile];
      SAGE_BlitTileToLayer(TILE_BANK, index, PLAYFIELD_LAYER, posx, posy);
      posx += TILE_WIDTH;
    }
    posy += TILE_HEIGHT;
  }
}

VOID InitGame(VOID)
{
  UWORD missile;
  
  map.posx = MAP_STARTX;
  map.posy = MAP_STARTY;
  player.posx = TANK_STARTX;
  player.posy = TANK_STARTY;
  player.sprite = TANK_SPRUP;
  player.next_missile = 0;
  player.direction = MOVE_UP;
  for (missile = 0;missile < MISSILE_MAX;missile++) {
    missiles[missile].active = FALSE;
  }
  BuildPlayfield();
}

UWORD GetTileAt(ULONG posx, ULONG posy)
{
  return (UWORD) ((posy / TILE_HEIGHT * MAP_WIDTH) + (posx / TILE_WIDTH));
}

VOID DrawLeftTiles(ULONG xpf, ULONG ypf)
{
  UBYTE * level_map;
  UWORD y_tile, tile;
  UBYTE index;
  ULONG posx = 0, posy = 0;

  if (xpf > 0) {
    level_map = SAGE_GetTileMapB(MAP_BANK);
    tile = GetTileAt(xpf, ypf) - 1;
    posx = xpf - (xpf % TILE_WIDTH) - TILE_WIDTH;
    posy = ypf - (ypf % TILE_HEIGHT);
    for (y_tile = 0;y_tile <= TILE_NBHEIGHT;y_tile++) {
      index = level_map[tile];
      SAGE_BlitTileToLayer(TILE_BANK, index, PLAYFIELD_LAYER, posx, posy);
      tile += MAP_WIDTH;
      posy += TILE_HEIGHT;
    }
  }
}

VOID DrawRightTiles(ULONG xpf, ULONG ypf)
{
  UBYTE * level_map;
  UWORD y_tile, tile;
  UBYTE index;
  ULONG posx = 0, posy = 0;

  if (xpf < (MAP_MAX_X-VIEW_WIDTH)) {
    level_map = SAGE_GetTileMapB(MAP_BANK);
    tile = GetTileAt(xpf, ypf) + TILE_NBWIDTH;
    posx = xpf - (xpf % TILE_WIDTH) + (TILE_WIDTH * TILE_NBWIDTH);
    posy = ypf - (ypf % TILE_HEIGHT);
    for (y_tile = 0;y_tile <= TILE_NBHEIGHT;y_tile++) {
      index = level_map[tile];
      SAGE_BlitTileToLayer(TILE_BANK, index, PLAYFIELD_LAYER, posx, posy);
      tile += MAP_WIDTH;
      posy += TILE_HEIGHT;
    }
  }
}

VOID DrawTopTiles(ULONG xpf, ULONG ypf)
{
  UBYTE * level_map;
  UWORD x_tile, tile;
  UBYTE index;
  ULONG posx = 0, posy = 0;

  if (ypf > 0) {
    level_map = SAGE_GetTileMapB(MAP_BANK);
    tile = GetTileAt(xpf, ypf) - MAP_WIDTH;
    posx = xpf - (xpf % TILE_WIDTH);
    posy = ypf - (ypf % TILE_HEIGHT) - TILE_HEIGHT;
    for (x_tile = 0;x_tile <= TILE_NBWIDTH;x_tile++) {
      index = level_map[tile];
      SAGE_BlitTileToLayer(TILE_BANK, index, PLAYFIELD_LAYER, posx, posy);
      tile++;
      posx += TILE_WIDTH;
    }
  }
}

VOID DrawBottomTiles(ULONG xpf, ULONG ypf)
{
  UBYTE * level_map;
  UWORD x_tile, tile;
  UBYTE index;
  ULONG posx = 0, posy = 0;

  if (ypf < (MAP_MAX_Y-VIEW_HEIGHT)) {
    level_map = SAGE_GetTileMapB(MAP_BANK);
    tile = GetTileAt(xpf, ypf) + (MAP_WIDTH * TILE_NBHEIGHT);
    posx = xpf - (xpf % TILE_WIDTH);
    posy = ypf - (ypf % TILE_HEIGHT) + (TILE_HEIGHT * TILE_NBHEIGHT);
    for (x_tile = 0;x_tile <= TILE_NBWIDTH;x_tile++) {
      index = level_map[tile];
      SAGE_BlitTileToLayer(TILE_BANK, index, PLAYFIELD_LAYER, posx, posy);
      tile++;
      posx += TILE_WIDTH;
    }
  }
}

BOOL FireMissile(VOID)
{
  UWORD missile;

//  SAGE_DebugLog("Firing missile from %d,%d", player.posx, player.posy);
  for (missile = 0;missile < MISSILE_MAX;missile++) {
    if (!missiles[missile].active) {
//      SAGE_DebugLog("Missile %d is available", missile);
      missiles[missile].sprite = player.sprite + TANK_NBSPR;
      switch (player.sprite) {
        case TANK_SPRUP:
          missiles[missile].posx = player.posx;
          missiles[missile].posy = player.posy - MISSILE_HEIGHT;
          missiles[missile].deltax = 0;
          missiles[missile].deltay = -MISSILE_SPEED;
          break;
        case TANK_SPRDOWN:
          missiles[missile].posx = player.posx;
          missiles[missile].posy = player.posy + TANK_HEIGHT;
          missiles[missile].deltax = 0;
          missiles[missile].deltay = MISSILE_SPEED;
          break;
        case TANK_SPRLEFT:
          missiles[missile].posx = player.posx - MISSILE_WIDTH;
          missiles[missile].posy = player.posy;
          missiles[missile].deltax = -MISSILE_SPEED;
          missiles[missile].deltay = 0;
          break;
        case TANK_SPRRIGHT:
          missiles[missile].posx = player.posx + TANK_WIDTH;
          missiles[missile].posy = player.posy;
          missiles[missile].deltax = MISSILE_SPEED;
          missiles[missile].deltay = 0;
          break;
        case TANK_SPRUPLEFT:
          missiles[missile].posx = player.posx - MISSILE_WIDTH;
          missiles[missile].posy = player.posy - TANK_HEIGHT;
          missiles[missile].deltax = -MISSILE_SPEED;
          missiles[missile].deltay = -MISSILE_SPEED;
          break;
        case TANK_SPRUPRIGHT:
          missiles[missile].posx = player.posx + MISSILE_WIDTH;
          missiles[missile].posy = player.posy - TANK_HEIGHT;
          missiles[missile].deltax = MISSILE_SPEED;
          missiles[missile].deltay = -MISSILE_SPEED;
          break;
        case TANK_SPRDOWNLEFT:
          missiles[missile].posx = player.posx - MISSILE_WIDTH;
          missiles[missile].posy = player.posy + TANK_HEIGHT;
          missiles[missile].deltax = -MISSILE_SPEED;
          missiles[missile].deltay = MISSILE_SPEED;
          break;
        case TANK_SPRDOWNRIGHT:
          missiles[missile].posx = player.posx + MISSILE_WIDTH;
          missiles[missile].posy = player.posy + TANK_HEIGHT;
          missiles[missile].deltax = MISSILE_SPEED;
          missiles[missile].deltay = MISSILE_SPEED;
          break;
        default:
          missiles[missile].posx = player.posx;
          missiles[missile].posy = player.posy;
          missiles[missile].deltax = 0;
          missiles[missile].deltay = MISSILE_SPEED;
      }
      missiles[missile].active = TRUE;
      player.next_missile = MISSILE_DELAY;
      SAGE_PlaySound(SOUND_SHOOT, 4);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL IsWall(LONG spot1x, LONG spot1y, LONG spot2x, LONG spot2y)
{
  UBYTE * level_map;
  UWORD index;

  level_map = SAGE_GetTileMapB(MAP_BANK);
  index = GetTileAt(spot1x, spot1y);
//  SAGE_DebugLog("Index spot1 %d,%d = %d", spot1x, spot1y, index);
  if (SAGE_HasTileFlag(TILE_BANK, level_map[index], TILE_WALLFLAG)) {
    return TRUE;
  }
  index = GetTileAt(spot2x, spot2y);
//  SAGE_DebugLog("Index spot2 %d,%d = %d", spot2x, spot2y, index);
  if (SAGE_HasTileFlag(TILE_BANK, level_map[index], TILE_WALLFLAG)) {
    return TRUE;
  }
  return FALSE;
}

VOID UpdateTank(VOID)
{
  // Update player position
  if (player.direction == MOVE_LEFT) {
    player.sprite = TANK_SPRLEFT;
    if (player.posx > 0 && !IsWall(player.posx-1, player.posy, player.posx-1, player.posy+(TANK_HEIGHT-1))) {
      player.posx--;
      if (map.posx % TILE_WIDTH == 0) {
        DrawLeftTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == MOVE_RIGHT) {
    player.sprite = TANK_SPRRIGHT;
    if (player.posx < (MAP_MAX_X - TANK_WIDTH) && !IsWall(player.posx+TANK_WIDTH, player.posy, player.posx+TANK_WIDTH, player.posy+(TANK_HEIGHT-1))) {
      player.posx++;
      if (map.posx % TILE_WIDTH == 0) {
        DrawRightTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == MOVE_UP) {
    player.sprite = TANK_SPRUP;
    if (player.posy > 0 && !IsWall(player.posx, player.posy-1, player.posx+(TANK_WIDTH-1), player.posy-1)) {
      player.posy--;
      if (map.posy % TILE_HEIGHT == 0) {
        DrawTopTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == MOVE_DOWN) {
    player.sprite = TANK_SPRDOWN;
    if (player.posy < (MAP_MAX_Y - TANK_HEIGHT) && !IsWall(player.posx, player.posy+TANK_HEIGHT, player.posx+(TANK_WIDTH-1), player.posy+TANK_HEIGHT)) {
      player.posy++;
      if (map.posy % TILE_HEIGHT == 0) {
        DrawBottomTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == (MOVE_UP+MOVE_LEFT)) {
    player.sprite = TANK_SPRUPLEFT;
    if (player.posx > 0 && !IsWall(player.posx-1, player.posy, player.posx-1, player.posy+(TANK_HEIGHT-1))
        && player.posy > 0 && !IsWall(player.posx, player.posy-1, player.posx+(TANK_WIDTH-1), player.posy-1)) {
      player.posx--;
      player.posy--;
      if (map.posx % TILE_WIDTH == 0) {
        DrawLeftTiles(map.posx, map.posy);
      }
      if (map.posy % TILE_HEIGHT == 0) {
        DrawTopTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == (MOVE_UP+MOVE_RIGHT)) {
    player.sprite = TANK_SPRUPRIGHT;
    if (player.posy > 0 && !IsWall(player.posx, player.posy-1, player.posx+(TANK_WIDTH-1), player.posy-1)
        && player.posx < (MAP_MAX_X - TANK_WIDTH) && !IsWall(player.posx+TANK_WIDTH, player.posy, player.posx+TANK_WIDTH, player.posy+(TANK_HEIGHT-1))) {
      player.posy--;
      player.posx++;
      if (map.posy % TILE_HEIGHT == 0) {
        DrawTopTiles(map.posx, map.posy);
      }
      if (map.posx % TILE_WIDTH == 0) {
        DrawRightTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == (MOVE_DOWN+MOVE_LEFT)) {
    player.sprite = TANK_SPRDOWNLEFT;
    if (player.posx > 0 && !IsWall(player.posx-1, player.posy, player.posx-1, player.posy+(TANK_HEIGHT-1))
        && player.posy < (MAP_MAX_Y - TANK_HEIGHT) && !IsWall(player.posx, player.posy+TANK_HEIGHT, player.posx+(TANK_WIDTH-1), player.posy+TANK_HEIGHT)) {
      player.posx--;
      player.posy++;
      if (map.posx % TILE_WIDTH == 0) {
        DrawLeftTiles(map.posx, map.posy);
      }
      if (map.posy % TILE_HEIGHT == 0) {
        DrawBottomTiles(map.posx, map.posy);
      }
    }
  } else if (player.direction == (MOVE_DOWN+MOVE_RIGHT)) {
    player.sprite = TANK_SPRDOWNRIGHT;
    if (player.posx < (MAP_MAX_X - TANK_WIDTH) && !IsWall(player.posx+TANK_WIDTH, player.posy, player.posx+TANK_WIDTH, player.posy+(TANK_HEIGHT-1))
        && player.posy < (MAP_MAX_Y - TANK_HEIGHT) && !IsWall(player.posx, player.posy+TANK_HEIGHT, player.posx+(TANK_WIDTH-1), player.posy+TANK_HEIGHT)) {
      player.posx++;
      player.posy++;
      if (map.posx % TILE_WIDTH == 0) {
        DrawRightTiles(map.posx, map.posy);
      }
      if (map.posy % TILE_HEIGHT == 0) {
        DrawBottomTiles(map.posx, map.posy);
      }
    }
  }
  // Refresh the missiles
  if (player.next_missile > 0) {
    player.next_missile--;
  }
}

VOID UpdateMap(VOID)
{
  map.posx = player.posx - TANK_DELTAX;
  if (map.posx < 0) {
    map.posx = 0;
  } else if (map.posx > (MAP_MAX_X - VIEW_WIDTH)) {
    map.posx = MAP_MAX_X - VIEW_WIDTH;
  }
  map.posy = player.posy - TANK_DELTAY;
  if (map.posy < 0) {
    map.posy = 0;
  } else if (map.posy > (MAP_MAX_Y - VIEW_HEIGHT)) {
    map.posy = MAP_MAX_Y - VIEW_HEIGHT;
  }
}

VOID UpdateMissiles(VOID)
{
  UWORD missile;

  for (missile = 0;missile < MISSILE_MAX;missile++) {
    if (missiles[missile].active) {
      missiles[missile].posx += missiles[missile].deltax;
      missiles[missile].posy += missiles[missile].deltay;
      if (missiles[missile].posx < (map.posx-MISSILE_WIDTH)
          || missiles[missile].posx > (map.posx+VIEW_WIDTH)
          || missiles[missile].posy < (map.posy-MISSILE_HEIGHT)
          || missiles[missile].posy > (map.posy+VIEW_HEIGHT)) {
//        SAGE_DebugLog("Stop missile %d", missile);
        missiles[missile].active = FALSE;
        SAGE_PlaySound(SOUND_EXPLOSION, 5);
      }
    }
  }
}

BOOL UpdateGame(VOID)
{
  SAGE_KeyScan keys[SCAN_KEYS] = {
    { SKEY_FR_ESC, FALSE },
    { SKEY_FR_LEFT, FALSE },
    { SKEY_FR_RIGHT, FALSE },
    { SKEY_FR_UP, FALSE },
    { SKEY_FR_DOWN, FALSE },
    { SKEY_FR_CTRL, FALSE },
    { SKEY_FR_ENTER, FALSE }
  };
  
  // Check player move
  player.direction = MOVE_NONE;
  if (SAGE_ScanKeyboard(keys, SCAN_KEYS)) {
    if (keys[KEY_QUIT].key_pressed) {
      SAGE_AppliLog("Exit loop");
      return FALSE;
    }
    if (keys[KEY_ENTER].key_pressed) {
      InitGame();
    } else {
      if (keys[KEY_LEFT].key_pressed) {
        player.direction += MOVE_LEFT;
      } else if (keys[KEY_RIGHT].key_pressed) {
        player.direction += MOVE_RIGHT;
      }
      if (keys[KEY_UP].key_pressed) {
        player.direction += MOVE_UP;
      } else if (keys[KEY_DOWN].key_pressed) {
        player.direction += MOVE_DOWN;
      }
      UpdateTank();
      if (keys[KEY_FIRE].key_pressed) {
        if (player.next_missile == 0) {
          FireMissile();
        }
      }
    }
  } else {
    SAGE_AppliLog("Keyboard scan error !");
    return FALSE;
  }
  UpdateMap();
  UpdateMissiles();
  return TRUE;
}

VOID DebugGame(VOID)
{
  UBYTE string_buffer[256];
  
  sprintf(string_buffer, "MAP : %d, %d    ", map.posx, map.posy);
  SAGE_PrintText(string_buffer, 0, 230);
  sprintf(string_buffer, "PLAYER : %d, %d    ", player.posx, player.posy);
  SAGE_PrintText(string_buffer, 160, 230);
}

BOOL RenderGame(VOID)
{
  UWORD missile;
  
  // Draw the playfield map
  if (!SAGE_SetLayerView(PLAYFIELD_LAYER, map.posx, map.posy, VIEW_WIDTH, VIEW_HEIGHT)) {
    SAGE_AppliLog("Error setting playfield layer view !");
    return FALSE;
  }
  if (!SAGE_BlitLayerToScreen(PLAYFIELD_LAYER, 0L, 0L)) {
    SAGE_AppliLog("Error blitting playfield layer !");
    return FALSE;
  }
  // Draw the status panel
  if (!SAGE_SetLayerView(PANEL_LAYER, 0, 0, PANEL_WIDTH, PANEL_HEIGHT)) {
    SAGE_AppliLog("Error setting panel layer view !");
    return FALSE;
  }
  if (!SAGE_BlitLayerToScreen(PANEL_LAYER, PANEL_POSX, PANEL_POSY)) {
    SAGE_AppliLog("Error blitting panel layer !");
    return FALSE;
  }
  // Draw the tank sprite
  if (!SAGE_BlitSpriteToScreen(SPR_BANK, player.sprite, player.posx - map.posx, player.posy - map.posy)) {
    SAGE_AppliLog("Error blitting tank sprite (%d) !", player.sprite);
    return FALSE;
  }
  // Draw the missiles
  for (missile = 0;missile < MISSILE_MAX;missile++) {
    if (missiles[missile].active) {
      SAGE_DebugLog("Blitting missile (%d)", missile);
      if (!SAGE_BlitSpriteToScreen(SPR_BANK, missiles[missile].sprite, missiles[missile].posx - map.posx, missiles[missile].posy - map.posy)) {
        SAGE_AppliLog("Error blitting missile sprite (%d) !", missile);
        return FALSE;
      }
    }
  }
  DebugGame();
  // Refresh the screen
  if (!SAGE_RefreshScreen()) {
    SAGE_AppliLog("Error on refresh screen !");
    return FALSE;
  }
  return TRUE;
}

void main(void)
{
  BOOL finish = FALSE;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Odysseus demo V1.2 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT)) {
    if (SAGE_ApolloCore()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    if (OpenScreen()) {
      SAGE_HideMouse();
      if (LoadResources()) {
        if (!SAGE_PlayMusic(MAIN_MUSIC)) {
          SAGE_DisplayError();
          finish = TRUE;
        }
        SAGE_AppliLog("Entering main loop");
        InitGame();
        while (!finish) {
          if (SAGE_IsFrontMostScreen()) {
            if (!UpdateGame()) {
              SAGE_DisplayError();
              finish = TRUE;
            }
            if (!RenderGame()) {
              SAGE_DisplayError();
              finish = TRUE;
            }
          }
        }
      }
      ReleaseResources();
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
