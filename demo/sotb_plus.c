/**
 * sotb_plus.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Demo of SOTB ehanced
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

#include <proto/dos.h>        // For Delay function

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define BG_WIDTH              640L
#define BG_HEIGHT             360L
#define BG_LAYER              1
#define BG_POSY               0

#define MOUNT_WIDTH           640L
#define MOUNT_HEIGHT          110L
#define MOUNT_LAYER           2
#define MOUNT_POSY            200
#define MOUNT_SPEED           1

#define GRASS0_WIDTH          640L
#define GRASS0_HEIGHT         50L
#define GRASS0_LAYER          3
#define GRASS0_POSY           274
#define GRASS0_SPEED          2

#define GRASS1_WIDTH          640L
#define GRASS1_HEIGHT         60L
#define GRASS1_LAYER          4
#define GRASS1_POSY           280
#define GRASS1_SPEED          3

#define GRASS2_WIDTH          640L
#define GRASS2_HEIGHT         58L
#define GRASS2_LAYER          5
#define GRASS2_POSY           294
#define GRASS2_SPEED          4

#define GRASS3_WIDTH          640L
#define GRASS3_HEIGHT         84L
#define GRASS3_LAYER          6
#define GRASS3_POSY           296
#define GRASS3_SPEED          5

#define GRASS4_WIDTH          640L
#define GRASS4_HEIGHT         84L
#define GRASS4_LAYER          7
#define GRASS4_POSY           324
#define GRASS4_SPEED          6

#define GRASS5_WIDTH          640L
#define GRASS5_HEIGHT         86L
#define GRASS5_LAYER          8
#define GRASS5_POSY           364
#define GRASS5_SPEED          7

#define FENCE_WIDTH           640L
#define FENCE_HEIGHT          42L
#define FENCE_LAYER           9
#define FENCE_POSY            408
#define FENCE_SPEED           8

#define CLOUD0_WIDTH          640L
#define CLOUD0_HEIGHT         42L
#define CLOUD0_LAYER          10
#define CLOUD0_POSY           0
#define CLOUD0_SPEED          6

#define CLOUD1_WIDTH          640L
#define CLOUD1_HEIGHT         80L
#define CLOUD1_LAYER          11
#define CLOUD1_POSY           44
#define CLOUD1_SPEED          5

#define CLOUD2_WIDTH          640L
#define CLOUD2_HEIGHT         38L
#define CLOUD2_LAYER          12
#define CLOUD2_POSY           126
#define CLOUD2_SPEED          4

#define CLOUD3_WIDTH          640L
#define CLOUD3_HEIGHT         18L
#define CLOUD3_LAYER          13
#define CLOUD3_POSY           166
#define CLOUD3_SPEED          3

#define CLOUD4_WIDTH          640L
#define CLOUD4_HEIGHT         12L
#define CLOUD4_LAYER          14
#define CLOUD4_POSY           188
#define CLOUD4_SPEED          2

#define NB_SPRITES            32
#define SPR_BANK              0

#define BEAST_WIDTH           88
#define BEAST_HEIGHT          128
#define BEAST_ANIM            11
#define BEAST_DELAY           5
#define BEAST_POSX            276
#define BEAST_POSY            236

#define SPR_PAUSE             6
#define SPR_PAUSEX            96
#define SPR_PAUSEY            82

// Transparent color
#define TRANSPCOLOR           0xF81F

#define SOTB_MUSIC            0

// Controls
#define KEY_NBR               7
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_SPACE             4
#define KEY_P                 5
#define KEY_QUIT              6

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_P, FALSE },
  { SKEY_FR_ESC, FALSE }
};

ULONG cloud0_x = 0, cloud1_x = 0, cloud2_x = 0, cloud3_x = 0, cloud4_x = 0, cloud5_x = 0;
ULONG mount_x = 0, grass0_x = 0, grass1_x = 0, grass2_x = 0, grass3_x = 0, grass4_x = 0, grass5_x = 0, fence_x = 0;
UWORD beast_spr = 0, beast_delay = 0;
BOOL synchro = TRUE, finish = FALSE, pause = FALSE, ispaused = FALSE;

// Render data
UBYTE string_buffer[256];

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    SAGE_SetScreenClip(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load background picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_back.png")) != NULL) {
    SAGE_AppliLog("Create back layer");
    if (SAGE_CreateLayerFromPicture(BG_LAYER, picture)) {
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCloud0Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_cloud0.png")) != NULL) {
    SAGE_AppliLog("Create cloud layer");
    if (SAGE_CreateLayerFromPicture(CLOUD0_LAYER, picture)) {
      SAGE_SetLayerTransparency(CLOUD0_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCloud1Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_cloud1.png")) != NULL) {
    SAGE_AppliLog("Create cloud layer");
    if (SAGE_CreateLayerFromPicture(CLOUD1_LAYER, picture)) {
      SAGE_SetLayerTransparency(CLOUD1_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCloud2Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_cloud2.png")) != NULL) {
    SAGE_AppliLog("Create cloud layer");
    if (SAGE_CreateLayerFromPicture(CLOUD2_LAYER, picture)) {
      SAGE_SetLayerTransparency(CLOUD2_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCloud3Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_cloud3.png")) != NULL) {
    SAGE_AppliLog("Create cloud layer");
    if (SAGE_CreateLayerFromPicture(CLOUD3_LAYER, picture)) {
      SAGE_SetLayerTransparency(CLOUD3_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCloud4Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_cloud4.png")) != NULL) {
    SAGE_AppliLog("Create cloud layer");
    if (SAGE_CreateLayerFromPicture(CLOUD4_LAYER, picture)) {
      SAGE_SetLayerTransparency(CLOUD4_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitMountainLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load mountain picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_mount.png")) != NULL) {
    SAGE_AppliLog("Create mountain layer");
    if (SAGE_CreateLayerFromPicture(MOUNT_LAYER, picture)) {
      SAGE_SetLayerTransparency(MOUNT_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass0Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_grass0.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayerFromPicture(GRASS0_LAYER, picture)) {
      SAGE_SetLayerTransparency(GRASS0_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass1Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_grass1.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayerFromPicture(GRASS1_LAYER, picture)) {
      SAGE_SetLayerTransparency(GRASS1_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass2Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_grass2.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayerFromPicture(GRASS2_LAYER, picture)) {
      SAGE_SetLayerTransparency(GRASS2_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass3Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_grass3.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayerFromPicture(GRASS3_LAYER, picture)) {
      SAGE_SetLayerTransparency(GRASS3_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass4Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_grass4.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayerFromPicture(GRASS4_LAYER, picture)) {
      SAGE_SetLayerTransparency(GRASS4_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass5Layer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_grass5.png")) != NULL) {
    SAGE_AppliLog("Create grass layer");
    if (SAGE_CreateLayerFromPicture(GRASS5_LAYER, picture)) {
      SAGE_SetLayerTransparency(GRASS5_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitFenceLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load fence picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_fence.png")) != NULL) {
    SAGE_AppliLog("Create fence layer");
    if (SAGE_CreateLayerFromPicture(FENCE_LAYER, picture)) {
      SAGE_SetLayerTransparency(FENCE_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBeastSprite(VOID)
{
  SAGE_Picture * picture;
  UWORD sprite;
  ULONG spr_y;

  SAGE_AppliLog("Load sprite picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_beast.png")) != NULL) {
    SAGE_AppliLog("Create sprite bank");
    if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
      SAGE_SetSpriteBankTransparency(SPR_BANK, TRANSPCOLOR);
      SAGE_AppliLog("Add sprites to bank");
      spr_y = 0;
      for (sprite = 0;sprite < BEAST_ANIM;sprite++) {
        if (!SAGE_AddSpriteToBank(SPR_BANK, sprite, 0, spr_y, BEAST_WIDTH, BEAST_HEIGHT, SSPR_HS_TOPLEFT)) {
          SAGE_DisplayError();
        }
        spr_y += BEAST_HEIGHT;
      }
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGraphx(VOID)
{
  SAGE_AppliLog("Init graphx");
  if (!InitBackLayer()) {
    return FALSE;
  }
  if (!InitCloud0Layer()) {
    return FALSE;
  }
  if (!InitCloud1Layer()) {
    return FALSE;
  }
  if (!InitCloud2Layer()) {
    return FALSE;
  }
  if (!InitCloud3Layer()) {
    return FALSE;
  }
  if (!InitCloud4Layer()) {
    return FALSE;
  }
  if (!InitMountainLayer()) {
    return FALSE;
  }
  if (!InitGrass0Layer()) {
    return FALSE;
  }
  if (!InitGrass1Layer()) {
    return FALSE;
  }
  if (!InitGrass2Layer()) {
    return FALSE;
  }
  if (!InitGrass3Layer()) {
    return FALSE;
  }
  if (!InitGrass4Layer()) {
    return FALSE;
  }
  if (!InitGrass5Layer()) {
    return FALSE;
  }
  if (!InitFenceLayer()) {
    return FALSE;
  }
  if (!InitBeastSprite()) {
    return FALSE;
  }
  SAGE_MaximumFPS(60);
  return TRUE;
}

VOID ReleaseGraphx(VOID)
{
  SAGE_AppliLog("Release graphx");
  SAGE_ReleaseLayer(BG_LAYER);
  SAGE_ReleaseLayer(CLOUD0_LAYER);
  SAGE_ReleaseLayer(CLOUD1_LAYER);
  SAGE_ReleaseLayer(CLOUD2_LAYER);
  SAGE_ReleaseLayer(CLOUD3_LAYER);
  SAGE_ReleaseLayer(CLOUD4_LAYER);
  SAGE_ReleaseLayer(MOUNT_LAYER);
  SAGE_ReleaseLayer(GRASS0_LAYER);
  SAGE_ReleaseLayer(GRASS1_LAYER);
  SAGE_ReleaseLayer(GRASS2_LAYER);
  SAGE_ReleaseLayer(GRASS3_LAYER);
  SAGE_ReleaseLayer(GRASS4_LAYER);
  SAGE_ReleaseLayer(GRASS5_LAYER);
  SAGE_ReleaseLayer(FENCE_LAYER);
  SAGE_ReleaseSpriteBank(SPR_BANK);
}

BOOL InitMusic(VOID)
{
  SAGE_Music * music = NULL;

  SAGE_AppliLog("Loading music");
  if ((music = SAGE_LoadMusic("/data/sotb_main.aiff")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (SAGE_AddMusic(SOTB_MUSIC, music)) {
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

BOOL _Init(VOID)
{
  SAGE_AppliLog("Init demo");
  if (!OpenScreen()) {
    return FALSE;
  }
  SAGE_HideMouse();
  if (!InitGraphx()) {
    return FALSE;
  }
  if (!InitMusic()) {
    return FALSE;
  }
  if (!SAGE_EnableFrameCount(TRUE)) {
    SAGE_ErrorLog("Can't activate frame rate counter !");
  }
  SAGE_MaximumFPS(30);
  SAGE_VerticalSynchro(synchro);
  return TRUE;
}

VOID _Restore(VOID)
{
  SAGE_AppliLog("Restore demo");
  ReleaseMusic();
  ReleaseGraphx();
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
    keyboard_state[KEY_P] = keys[KEY_P].key_pressed;
    keyboard_state[KEY_QUIT] = keys[KEY_QUIT].key_pressed;
  }
}

VOID _Update(VOID)
{
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) finish = TRUE;
  /*if (keyboard_state[KEY_SPACE]) {
    troll_x = TROLL_STARTX;
    troll_y = TROLL_STARTY;
  }
  if (keyboard_state[KEY_LEFT]) {
    troll_x -= 2;
  } else if (keyboard_state[KEY_RIGHT]) {
    troll_x += 2;
  }
  if (keyboard_state[KEY_UP]) {
    troll_y -= 2;
  } else if (keyboard_state[KEY_DOWN]) {
    troll_y += 2;
  }
  if (keyboard_state[KEY_P]) {
    pause = !pause;
  }*/
  // Animate the mountain
  mount_x += MOUNT_SPEED;
  if (mount_x >= MOUNT_WIDTH) mount_x -= MOUNT_WIDTH;
  // Animate the clouds
  cloud0_x += CLOUD0_SPEED;
  if (cloud0_x >= CLOUD0_WIDTH) cloud0_x -= CLOUD0_WIDTH;
  cloud1_x += CLOUD1_SPEED;
  if (cloud1_x >= CLOUD1_WIDTH) cloud1_x -= CLOUD1_WIDTH;
  cloud2_x += CLOUD2_SPEED;
  if (cloud2_x >= CLOUD2_WIDTH) cloud2_x -= CLOUD2_WIDTH;
  cloud3_x += CLOUD3_SPEED;
  if (cloud3_x >= CLOUD3_WIDTH) cloud3_x -= CLOUD3_WIDTH;
  cloud4_x += CLOUD4_SPEED;
  if (cloud4_x >= CLOUD4_WIDTH) cloud4_x -= CLOUD4_WIDTH;
  // Animate the grass
  grass0_x += GRASS0_SPEED;
  if (grass0_x >= GRASS0_WIDTH) grass0_x -= GRASS0_WIDTH;
  grass1_x += GRASS1_SPEED;
  if (grass1_x >= GRASS1_WIDTH) grass1_x -= GRASS1_WIDTH;
  grass2_x += GRASS2_SPEED;
  if (grass2_x >= GRASS2_WIDTH) grass2_x -= GRASS2_WIDTH;
  grass3_x += GRASS3_SPEED;
  if (grass3_x >= GRASS3_WIDTH) grass3_x -= GRASS3_WIDTH;
  grass4_x += GRASS4_SPEED;
  if (grass4_x >= GRASS4_WIDTH) grass4_x -= GRASS4_WIDTH;
  grass5_x += GRASS5_SPEED;
  if (grass5_x >= GRASS5_WIDTH) grass5_x -= GRASS5_WIDTH;
  // Animate the fence
  fence_x += FENCE_SPEED;
  if (fence_x >= FENCE_WIDTH) fence_x -= FENCE_WIDTH;
  // Animate the beast sprite
  if (++beast_delay >= BEAST_DELAY) {
    beast_delay = 0;
    if (++beast_spr >= BEAST_ANIM) beast_spr = 0;
  }
}

VOID RenderError(STRPTR text)
{
  SAGE_AppliLog(text);
  SAGE_DisplayError();
  finish = TRUE;
}

VOID PauseDemo(VOID)
{
  if (!ispaused) {
    SAGE_PauseMusic();
    if (!SAGE_BlitSpriteToScreen(SPR_BANK, SPR_PAUSE, SPR_PAUSEX, SPR_PAUSEY)) {
      RenderError("Error blitting PAUSE sprite !");
    }
    if (!SAGE_RefreshScreen()) {
      RenderError("Error on refresh screen !");
    }
  }
  ispaused = TRUE;
}

VOID _Render(VOID)
{
  if (!pause) {
    if (ispaused) {
      SAGE_ResumeMusic();
      SAGE_Pause(10);
    }
    ispaused = FALSE;
    // Draw background cloud layer
    if (!SAGE_BlitLayerToScreen(BG_LAYER, BG_POSY, 0)) {
      RenderError("Error blitting background layer !");
    }
    // Draw the clouds layers
    if (!SAGE_SetLayerView(CLOUD0_LAYER, cloud0_x, 0, CLOUD0_WIDTH, CLOUD0_HEIGHT)) {
      RenderError("Error setting cloud layer view !");
    }
    if (!SAGE_BlitLayerToScreen(CLOUD0_LAYER, 0, CLOUD0_POSY)) {
      RenderError("Error blitting cloud layer !");
    }
    if (!SAGE_SetLayerView(CLOUD1_LAYER, cloud1_x, 0, CLOUD1_WIDTH, CLOUD1_HEIGHT)) {
      RenderError("Error setting cloud layer view !");
    }
    if (!SAGE_BlitLayerToScreen(CLOUD1_LAYER, 0, CLOUD1_POSY)) {
      RenderError("Error blitting cloud layer !");
    }
    if (!SAGE_SetLayerView(CLOUD2_LAYER, cloud2_x, 0, CLOUD2_WIDTH, CLOUD2_HEIGHT)) {
      RenderError("Error setting cloud layer view !");
    }
    if (!SAGE_BlitLayerToScreen(CLOUD2_LAYER, 0, CLOUD2_POSY)) {
      RenderError("Error blitting cloud layer !");
    }
    if (!SAGE_SetLayerView(CLOUD3_LAYER, cloud3_x, 0, CLOUD3_WIDTH, CLOUD3_HEIGHT)) {
      RenderError("Error setting cloud layer view !");
    }
    if (!SAGE_BlitLayerToScreen(CLOUD3_LAYER, 0, CLOUD3_POSY)) {
      RenderError("Error blitting cloud layer !");
    }
    if (!SAGE_SetLayerView(CLOUD4_LAYER, cloud4_x, 0, CLOUD4_WIDTH, CLOUD4_HEIGHT)) {
      RenderError("Error setting cloud layer view !");
    }
    if (!SAGE_BlitLayerToScreen(CLOUD4_LAYER, 0, CLOUD4_POSY)) {
      RenderError("Error blitting cloud layer !");
    }
    // Draw the mountain layer
    if (!SAGE_SetLayerView(MOUNT_LAYER, mount_x, 0, MOUNT_WIDTH, MOUNT_HEIGHT)) {
      RenderError("Error setting mountain layer view !");
    }
    if (!SAGE_BlitLayerToScreen(MOUNT_LAYER, 0, MOUNT_POSY)) {
      RenderError("Error blitting mountain layer !");
    }
    // Draw the grass layers
    if (!SAGE_SetLayerView(GRASS0_LAYER, grass0_x, 0, GRASS0_WIDTH, GRASS0_HEIGHT)) {
      RenderError("Error setting grass layer view !");
    }
    if (!SAGE_BlitLayerToScreen(GRASS0_LAYER, 0, GRASS0_POSY)) {
      RenderError("Error blitting grass layer !");
    }
    if (!SAGE_SetLayerView(GRASS1_LAYER, grass1_x, 0, GRASS1_WIDTH, GRASS1_HEIGHT)) {
      RenderError("Error setting grass layer view !");
    }
    if (!SAGE_BlitLayerToScreen(GRASS1_LAYER, 0, GRASS1_POSY)) {
      RenderError("Error blitting grass layer !");
    }
    if (!SAGE_SetLayerView(GRASS2_LAYER, grass2_x, 0, GRASS2_WIDTH, GRASS2_HEIGHT)) {
      RenderError("Error setting grass layer view !");
    }
    if (!SAGE_BlitLayerToScreen(GRASS2_LAYER, 0, GRASS2_POSY)) {
      RenderError("Error blitting grass layer !");
    }
    // Draw the beast sprite
    if (!SAGE_BlitSpriteToScreen(SPR_BANK, beast_spr, BEAST_POSX, BEAST_POSY)) {
      RenderError("Error blitting sprite !");
    }
    // Draw the grass layers
    if (!SAGE_SetLayerView(GRASS3_LAYER, grass3_x, 0, GRASS3_WIDTH, GRASS3_HEIGHT)) {
      RenderError("Error setting grass layer view !");
    }
    if (!SAGE_BlitLayerToScreen(GRASS3_LAYER, 0, GRASS3_POSY)) {
      RenderError("Error blitting grass layer !");
    }
    if (!SAGE_SetLayerView(GRASS4_LAYER, grass4_x, 0, GRASS4_WIDTH, GRASS4_HEIGHT)) {
      RenderError("Error setting grass layer view !");
    }
    if (!SAGE_BlitLayerToScreen(GRASS4_LAYER, 0, GRASS4_POSY)) {
      RenderError("Error blitting grass layer !");
    }
    if (!SAGE_SetLayerView(GRASS5_LAYER, grass5_x, 0, GRASS5_WIDTH, GRASS5_HEIGHT)) {
      RenderError("Error setting grass layer view !");
    }
    if (!SAGE_BlitLayerToScreen(GRASS5_LAYER, 0, GRASS5_POSY)) {
      RenderError("Error blitting grass layer !");
    }
    // Draw the fence
    if (!SAGE_SetLayerView(FENCE_LAYER, fence_x, 0, FENCE_WIDTH, FENCE_HEIGHT)) {
      RenderError("Error setting fence layer view !");
    }
    if (!SAGE_BlitLayerToScreen(FENCE_LAYER, 0, FENCE_POSY)) {
      RenderError("Error blitting fence layer !");
    }
    // Draw the fps counter
    sprintf(string_buffer, "%d fps", SAGE_GetFps());
    SAGE_PrintText(string_buffer, 10, 10);
    // Refresh the screen
    if (!SAGE_RefreshScreen()) {
      RenderError("Error on refresh screen !");
    }
  } else {
    PauseDemo();
    SAGE_Pause(10);
  }
}

void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library SOTB ehanced demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    if (_Init()) {
      if (!SAGE_PlayMusic(SOTB_MUSIC)) {
        SAGE_DisplayError();
        finish = TRUE;
      }
      SAGE_AppliLog("Entering main loop");
      while (!finish) {
        if (SAGE_IsFrontMostScreen()) {

          // Update the demo data
          _Update();
          // Render the demo
          _Render();

        } else {
          pause = TRUE;
          PauseDemo();
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
