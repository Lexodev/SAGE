/**
 * sotb_ehanced.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Demo of SOTB ehanced
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 September 2021
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

#define MOUNTAIN_WIDTH        640L
#define MOUNTAIN_HEIGHT       110L
#define MOUNTAIN_LAYER        2
#define MOUNTAIN_POSY         200
#define MOUNTAIN_SPEED        1

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

#define SPR_BEASTRIGHT        0
#define SPR_NBRIGHT           15
#define SPR_BEASTLEFT         1
#define SPR_NBLEFT            15
#define SPR_BEASTTURN         2
#define SPR_NBTURN            4

#define BEAST_WIDTH           88
#define BEAST_HEIGHT          128

#define BEAST_SPRSTAY         0
#define BEAST_SPRDOWN         1
#define BEAST_SPRJUMP         3
#define BEAST_SPRRUN          4

#define BEAST_RUNANIM         11
#define BEAST_RUNDELAY        5

#define BEAST_DOWNANIM        2
#define BEAST_DOWNDELAY       8

#define BEAST_TURNANIM        4
#define BEAST_TURNDELAY       8

#define BEAST_POSX            276
#define BEAST_POSY            236

#define BEAST_RIGHTDIR        0
#define BEAST_LEFTDIR         1

#define BEAST_STAY            0
#define BEAST_RUN             1
#define BEAST_DOWN            2
#define BEAST_JUMP            3

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

LONG cloud0_x = 0, cloud1_x = 0, cloud2_x = 0, cloud3_x = 0, cloud4_x = 0, cloud5_x = 0;
LONG grass0_x = 0, grass1_x = 0, grass2_x = 0, grass3_x = 0, grass4_x = 0, grass5_x = 0;
LONG mountain_x = 0, fence_x = 0;
UWORD beast_bank = 0, beast_spr = 0, beast_anim = 0, beast_delay = 0, beast_direction = 0, beast_status = 0;
BOOL synchro = TRUE, finish = FALSE, pause = FALSE, ispaused = FALSE;

// Render data
UBYTE string_buffer[256];

/**
 * Open the main screen
 */
BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    SAGE_SetScreenClip(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SAGE_SetTextColor(2, 1);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

/**
 * Create the background layer
 */
BOOL InitBackground(VOID)
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

/**
 * Creat the clouds layers
 */
BOOL InitCloudLayer(STRPTR filename, UWORD layer_idx, ULONG transparent)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture(filename)) != NULL) {
    SAGE_AppliLog("Create cloud layer %d", layer_idx);
    if (SAGE_CreateLayerFromPicture(layer_idx, picture)) {
      SAGE_SetLayerTransparency(layer_idx, transparent);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitClouds(VOID)
{
  BOOL result;
  
  result = InitCloudLayer("/data/SOTB_cloud0.png", CLOUD0_LAYER, TRANSPCOLOR)
         & InitCloudLayer("/data/SOTB_cloud1.png", CLOUD1_LAYER, TRANSPCOLOR)
         & InitCloudLayer("/data/SOTB_cloud2.png", CLOUD2_LAYER, TRANSPCOLOR)
         & InitCloudLayer("/data/SOTB_cloud3.png", CLOUD3_LAYER, TRANSPCOLOR)
         & InitCloudLayer("/data/SOTB_cloud4.png", CLOUD4_LAYER, TRANSPCOLOR);
  return result;
}

/**
 * Create the mountain layer
 */
BOOL InitMountain(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load mountain picture");
  if ((picture = SAGE_LoadPicture("/data/SOTB_mount.png")) != NULL) {
    SAGE_AppliLog("Create mountain layer");
    if (SAGE_CreateLayerFromPicture(MOUNTAIN_LAYER, picture)) {
      SAGE_SetLayerTransparency(MOUNTAIN_LAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

/**
 * Create the grass layers
 */
BOOL InitGrassLayer(STRPTR filename, UWORD layer_idx, ULONG transparent)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load grass picture");
  if ((picture = SAGE_LoadPicture(filename)) != NULL) {
    SAGE_AppliLog("Create grass layer %d", layer_idx);
    if (SAGE_CreateLayerFromPicture(layer_idx, picture)) {
      SAGE_SetLayerTransparency(layer_idx, transparent);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitGrass(VOID)
{
  BOOL result;
  
  result = InitGrassLayer("/data/SOTB_grass0.png", GRASS0_LAYER, TRANSPCOLOR)
         & InitGrassLayer("/data/SOTB_grass1.png", GRASS1_LAYER, TRANSPCOLOR)
         & InitGrassLayer("/data/SOTB_grass2.png", GRASS2_LAYER, TRANSPCOLOR)
         & InitGrassLayer("/data/SOTB_grass3.png", GRASS3_LAYER, TRANSPCOLOR)
         & InitGrassLayer("/data/SOTB_grass4.png", GRASS4_LAYER, TRANSPCOLOR)
         & InitGrassLayer("/data/SOTB_grass5.png", GRASS5_LAYER, TRANSPCOLOR);
  return result;
}

/**
 * Create the fence layer
 */
BOOL InitFence(VOID)
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

/**
 * Create the sprites for the beast
 */
BOOL InitBeastSprite(STRPTR filename, UWORD bank_idx, UWORD nb_sprites, ULONG transparent)
{
  SAGE_Picture * picture;
  UWORD sprite;
  ULONG spr_y;

  SAGE_AppliLog("Load beast sprite picture");
  if ((picture = SAGE_LoadPicture(filename)) != NULL) {
    SAGE_AppliLog("Create sprite bank %d", bank_idx);
    if (SAGE_CreateSpriteBank(bank_idx, nb_sprites, picture)) {
      SAGE_SetSpriteBankTransparency(bank_idx, transparent);
      SAGE_AppliLog("Add sprites to bank");
      spr_y = 0;
      for (sprite = 0;sprite < nb_sprites;sprite++) {
        if (!SAGE_AddSpriteToBank(bank_idx, sprite, 0, spr_y, BEAST_WIDTH, BEAST_HEIGHT, SSPR_HS_TOPLEFT)) {
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

BOOL InitBeast(VOID)
{
  BOOL result;
  
  result = InitBeastSprite("/data/SOTB_beastright.png", SPR_BEASTRIGHT, SPR_NBRIGHT, TRANSPCOLOR)
         & InitBeastSprite("/data/SOTB_beastleft.png", SPR_BEASTLEFT, SPR_NBLEFT, TRANSPCOLOR)
         & InitBeastSprite("/data/SOTB_beastturn.png", SPR_BEASTTURN, SPR_NBTURN, TRANSPCOLOR);
  return result;
}

BOOL InitGraphx(VOID)
{
  SAGE_AppliLog("Init graphx");
  SAGE_PrintDirectText("Loading background...", 0, 16);
  if (!InitBackground()) {
    return FALSE;
  }
  SAGE_PrintDirectText("Loading clouds...", 0, 32);
  if (!InitClouds()) {
    return FALSE;
  }
  SAGE_PrintDirectText("Loading mountain...", 0, 48);
  if (!InitMountain()) {
    return FALSE;
  }
  SAGE_PrintDirectText("Loading grass...", 0, 64);
  if (!InitGrass()) {
    return FALSE;
  }
  SAGE_PrintDirectText("Loading fence...", 0, 80);
  if (!InitFence()) {
    return FALSE;
  }
  SAGE_PrintDirectText("Loading beast sprites...", 0, 96);
  if (!InitBeast()) {
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
  SAGE_ReleaseLayer(MOUNTAIN_LAYER);
  SAGE_ReleaseLayer(GRASS0_LAYER);
  SAGE_ReleaseLayer(GRASS1_LAYER);
  SAGE_ReleaseLayer(GRASS2_LAYER);
  SAGE_ReleaseLayer(GRASS3_LAYER);
  SAGE_ReleaseLayer(GRASS4_LAYER);
  SAGE_ReleaseLayer(GRASS5_LAYER);
  SAGE_ReleaseLayer(FENCE_LAYER);
  SAGE_ReleaseSpriteBank(SPR_BEASTRIGHT);
  SAGE_ReleaseSpriteBank(SPR_BEASTLEFT);
  SAGE_ReleaseSpriteBank(SPR_BEASTTURN);
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

/**
 * Animate the clouds
 */
VOID AnimateClouds(VOID)
{
  if (beast_direction == BEAST_RIGHTDIR) {
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
  } else {
    cloud0_x -= CLOUD0_SPEED;
    if (cloud0_x < 0) cloud0_x += CLOUD0_WIDTH;
    cloud1_x -= CLOUD1_SPEED;
    if (cloud1_x < 0) cloud1_x += CLOUD1_WIDTH;
    cloud2_x -= CLOUD2_SPEED;
    if (cloud2_x < 0) cloud2_x += CLOUD2_WIDTH;
    cloud3_x -= CLOUD3_SPEED;
    if (cloud3_x < 0) cloud3_x += CLOUD3_WIDTH;
    cloud4_x -= CLOUD4_SPEED;
    if (cloud4_x < 0) cloud4_x += CLOUD4_WIDTH;
  }
}

/**
 * Animate the mountain
 */
VOID AnimateMountain(VOID)
{
  if (beast_direction == BEAST_RIGHTDIR) {
    mountain_x += MOUNTAIN_SPEED;
    if (mountain_x >= MOUNTAIN_WIDTH) mountain_x -= MOUNTAIN_WIDTH;
  } else {
    mountain_x -= MOUNTAIN_SPEED;
    if (mountain_x < 0) mountain_x += MOUNTAIN_WIDTH;
  }
}

/**
 * Animate the grass
 */
VOID AnimateGrass(VOID)
{
  if (beast_direction == BEAST_RIGHTDIR) {
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
  } else {
    grass0_x -= GRASS0_SPEED;
    if (grass0_x < 0) grass0_x += GRASS0_WIDTH;
    grass1_x -= GRASS1_SPEED;
    if (grass1_x < 0) grass1_x += GRASS1_WIDTH;
    grass2_x -= GRASS2_SPEED;
    if (grass2_x < 0) grass2_x += GRASS2_WIDTH;
    grass3_x -= GRASS3_SPEED;
    if (grass3_x < 0) grass3_x += GRASS3_WIDTH;
    grass4_x -= GRASS4_SPEED;
    if (grass4_x < 0) grass4_x += GRASS4_WIDTH;
    grass5_x -= GRASS5_SPEED;
    if (grass5_x < 0) grass5_x += GRASS5_WIDTH;
  }
}

VOID AnimateFence(VOID)
{
  if (beast_direction == BEAST_RIGHTDIR) {
    fence_x += FENCE_SPEED;
    if (fence_x >= FENCE_WIDTH) fence_x -= FENCE_WIDTH;
  } else {
    fence_x -= FENCE_SPEED;
    if (fence_x < 0) fence_x += FENCE_WIDTH;
  }
}

VOID AnimateBeast(VOID)
{
  // beast_bank = 0, beast_spr = 0, beast_delay = 0, beast_direction = 0, beast_status = 0, beast_anim = 0
  if (beast_direction == BEAST_RIGHTDIR) {
    beast_bank = SPR_BEASTRIGHT;
  } else {
    beast_bank = SPR_BEASTLEFT;
  }
  if (beast_status == BEAST_STAY) {
    beast_spr = BEAST_SPRSTAY;
    beast_anim = 0;
  } else if (beast_status == BEAST_RUN) {
    if (++beast_delay >= BEAST_RUNDELAY) {
      beast_delay = 0;
      if (++beast_anim >= BEAST_RUNANIM) beast_anim = 0;
    }
    beast_spr = beast_anim + BEAST_SPRRUN;
  }
}

VOID CheckKeyboard(VOID)
{
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) {
    finish = TRUE;
  } else if (keyboard_state[KEY_P]) {
    pause = !pause;
  } else {
    beast_status = BEAST_STAY;
    if (keyboard_state[KEY_SPACE]) {
      beast_status = BEAST_JUMP;
    } else {
      if (keyboard_state[KEY_LEFT]) {
        beast_direction = BEAST_LEFTDIR;
        beast_status = BEAST_RUN;
      } else if (keyboard_state[KEY_RIGHT]) {
        beast_direction = BEAST_RIGHTDIR;
        beast_status = BEAST_RUN;
      }
      if (keyboard_state[KEY_DOWN]) {
        beast_status = BEAST_DOWN;
      }
    }
  }
}

VOID _Update(VOID)
{
  CheckKeyboard();
  if (beast_status == BEAST_RUN || beast_status == BEAST_JUMP) {
    AnimateClouds();
    AnimateMountain();
    AnimateGrass();
    AnimateFence();
  }
  AnimateBeast();
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
    /*SAGE_PauseMusic();
    if (!SAGE_BlitSpriteToScreen(SPR_BANK, SPR_PAUSE, SPR_PAUSEX, SPR_PAUSEY)) {
      RenderError("Error blitting PAUSE sprite !");
    }
    if (!SAGE_RefreshScreen()) {
      RenderError("Error on refresh screen !");
    }*/
  }
  ispaused = TRUE;
}

/**
 * Draw a scrolling layer
 */
VOID DrawLayer(UWORD layer_idx, LONG crd_x, LONG pos_y, LONG width, LONG height)
{
  if (!SAGE_SetLayerView(layer_idx, crd_x, 0, width, height)) {
    RenderError("Error setting layer view !");
  }
  if (!SAGE_BlitLayerToScreen(layer_idx, 0, pos_y)) {
    RenderError("Error blitting layer to screen !");
  }
}

/**
 * Render elements on the screen
 */
VOID _Render(VOID)
{
  if (!pause) {
    if (ispaused) {
      SAGE_ResumeMusic();
      SAGE_Pause(10);
    }
    ispaused = FALSE;
    // Draw the background
    if (!SAGE_BlitLayerToScreen(BG_LAYER, BG_POSY, 0)) {
      RenderError("Error blitting background layer !");
    }
    // Draw the clouds
    DrawLayer(CLOUD0_LAYER, cloud0_x, CLOUD0_POSY, CLOUD0_WIDTH, CLOUD0_HEIGHT);
    DrawLayer(CLOUD1_LAYER, cloud1_x, CLOUD1_POSY, CLOUD1_WIDTH, CLOUD1_HEIGHT);
    DrawLayer(CLOUD2_LAYER, cloud2_x, CLOUD2_POSY, CLOUD2_WIDTH, CLOUD2_HEIGHT);
    DrawLayer(CLOUD3_LAYER, cloud3_x, CLOUD3_POSY, CLOUD3_WIDTH, CLOUD3_HEIGHT);
    DrawLayer(CLOUD4_LAYER, cloud4_x, CLOUD4_POSY, CLOUD4_WIDTH, CLOUD4_HEIGHT);
    // Draw the mountain
    DrawLayer(MOUNTAIN_LAYER, mountain_x, MOUNTAIN_POSY, MOUNTAIN_WIDTH, MOUNTAIN_HEIGHT);
    // Draw the first grass layers
    DrawLayer(GRASS0_LAYER, grass0_x, GRASS0_POSY, GRASS0_WIDTH, GRASS0_HEIGHT);
    DrawLayer(GRASS1_LAYER, grass1_x, GRASS1_POSY, GRASS1_WIDTH, GRASS1_HEIGHT);
    DrawLayer(GRASS2_LAYER, grass2_x, GRASS2_POSY, GRASS2_WIDTH, GRASS2_HEIGHT);
    // Draw the beast sprite
    if (!SAGE_BlitSpriteToScreen(beast_bank, beast_spr, BEAST_POSX, BEAST_POSY)) {
      RenderError("Error blitting sprite !");
    }
    // Draw the second grass layers
    DrawLayer(GRASS3_LAYER, grass3_x, GRASS3_POSY, GRASS3_WIDTH, GRASS3_HEIGHT);
    DrawLayer(GRASS4_LAYER, grass4_x, GRASS4_POSY, GRASS4_WIDTH, GRASS4_HEIGHT);
    DrawLayer(GRASS5_LAYER, grass5_x, GRASS5_POSY, GRASS5_WIDTH, GRASS5_HEIGHT);
    // Draw the fence
    DrawLayer(FENCE_LAYER, fence_x, FENCE_POSY, FENCE_WIDTH, FENCE_HEIGHT);
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

/**
 * main function, everything starts here !
 */
void main(void)
{
  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library SOTB ehanced demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_AMMX2Available()) {
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
