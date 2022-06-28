/**
 * troll_run.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of troll animation
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

#include <proto/dos.h>        // For Delay function

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define SKY_WIDTH             256L
#define SKY_HEIGHT            80L
#define SKY_LAYER             1
#define SKY_SPEED             1

#define CLOUD_WIDTH           384L
#define CLOUD_HEIGHT          48L
#define CLOUD_LAYER           2
#define CLOUD_SPEED           1

#define BACK_WIDTH            1024L
#define BACK_HEIGHT           192L
#define BACK_LAYER            3
#define BACK_SPEED            2

#define FRONT_WIDTH           1024L
#define FRONT_HEIGHT          192L
#define FRONT_LAYER           4
#define FRONT_SPEED           2

#define NB_SPRITES            7
#define SPR_BANK              0

#define TROLL_ANIM            6
#define TROLL_DELAY           10
#define TROLL_STARTX          90
#define TROLL_STARTY          48

#define SPR_PAUSE             6
#define SPR_PAUSEX            96
#define SPR_PAUSEY            82

#define TRANSP_COLOR          1

#define TROLL_MUSIC           0

// Controls
#define KEY_NBR               11
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_SPACE             4
#define KEY_P                 5
#define KEY_F1                6
#define KEY_F2                7
#define KEY_F3                8
#define KEY_F4                9
#define KEY_QUIT              10

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_P, FALSE },
  { SKEY_FR_F1, FALSE },
  { SKEY_FR_F2, FALSE },
  { SKEY_FR_F3, FALSE },
  { SKEY_FR_F4, FALSE },
  { SKEY_FR_ESC, FALSE }
};

// Demo data
ULONG sprites[NB_SPRITES*4] = {
  6,4,96,112,
  108,4,80,112,
  190,4,80,112,
  278,4,96,112,
  380,4,80,112,
  470,4,80,112,
  448,448,128,28
};
ULONG troll_x = TROLL_STARTX, troll_y = TROLL_STARTY, sky_x = 0, cloud_x = 0, back_x = 0, front_x = 0;
UWORD sprite = 0, delay = 0;
BOOL synchro = TRUE, finish = FALSE, pause = FALSE, ispaused = FALSE;

// Render data
UBYTE string_buffer[256];

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_TRIPLEBUF|SSCR_STRICTRES)) {
    SAGE_SetScreenClip(0, 0, SCREEN_WIDTH, BACK_HEIGHT);
    SAGE_SetTextColor(2, 1);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitSkyLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load sky picture");
  if ((picture = SAGE_LoadPicture("data/troll_sky.gif")) != NULL) {
    SAGE_AppliLog("Create sky layer");
    if (SAGE_CreateLayer(SKY_LAYER, (SKY_WIDTH+SCREEN_WIDTH), SKY_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, SKY_WIDTH, SKY_HEIGHT, SKY_LAYER, 0, 0);
      SAGE_BlitPictureToLayer(picture, 0, 0, SKY_WIDTH, SKY_HEIGHT, SKY_LAYER, SKY_WIDTH, 0);
      SAGE_BlitPictureToLayer(picture, 0, 0, (SCREEN_WIDTH-SKY_WIDTH), SKY_HEIGHT, SKY_LAYER, SKY_WIDTH*2, 0);
      SAGE_LoadPictureColorMap(picture);
      SAGE_RefreshColors(0, 256);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCloudLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load cloud picture");
  if ((picture = SAGE_LoadPicture("data/troll_cloud.gif")) != NULL) {
    SAGE_AppliLog("Create cloud layer");
    if (SAGE_CreateLayer(CLOUD_LAYER, (CLOUD_WIDTH+SCREEN_WIDTH), CLOUD_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, CLOUD_WIDTH, CLOUD_HEIGHT, CLOUD_LAYER, 0, 0);
      SAGE_BlitPictureToLayer(picture, 0, 0, SCREEN_WIDTH, CLOUD_HEIGHT, CLOUD_LAYER, CLOUD_WIDTH, 0);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load background picture");
  if ((picture = SAGE_LoadPicture("data/troll_bg.gif")) != NULL) {
    SAGE_AppliLog("Create background layer");
    if (SAGE_CreateLayer(BACK_LAYER, (BACK_WIDTH+SCREEN_WIDTH), BACK_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, BACK_WIDTH, BACK_HEIGHT, BACK_LAYER, 0, 0);
      SAGE_BlitPictureToLayer(picture, 0, 0, SCREEN_WIDTH, BACK_HEIGHT, BACK_LAYER, BACK_WIDTH, 0);
      SAGE_SetLayerTransparency(BACK_LAYER, TRANSP_COLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitFrontLayer(VOID)
{
  SAGE_Picture * picture;

  SAGE_AppliLog("Load foreground picture");
  if ((picture = SAGE_LoadPicture("data/troll_fg.gif")) != NULL) {
    SAGE_AppliLog("Create foreground layer");
    if (SAGE_CreateLayer(FRONT_LAYER, (FRONT_WIDTH+SCREEN_WIDTH), FRONT_HEIGHT)) {
      SAGE_BlitPictureToLayer(picture, 0, 0, FRONT_WIDTH, FRONT_HEIGHT, FRONT_LAYER, 0, 0);
      SAGE_BlitPictureToLayer(picture, 0, 0, SCREEN_WIDTH, FRONT_HEIGHT, FRONT_LAYER, FRONT_WIDTH, 0);
      SAGE_SetLayerTransparency(FRONT_LAYER, TRANSP_COLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTrollSprite(VOID)
{
  SAGE_Picture * picture;
  UWORD sprite;

  SAGE_AppliLog("Load sprite picture");
  if ((picture = SAGE_LoadPicture("data/troll_sprite.gif")) != NULL) {
    SAGE_AppliLog("Create sprite bank");
    if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
      SAGE_SetSpriteBankTransparency(SPR_BANK, TRANSP_COLOR);
      SAGE_AppliLog("Add sprites to bank");
      for (sprite = 0;sprite < NB_SPRITES;sprite++) {
        if (!SAGE_AddSpriteToBank(SPR_BANK, sprite, sprites[sprite*4], sprites[sprite*4+1], sprites[sprite*4+2], sprites[sprite*4+3], SSPR_HS_TOPLEFT)) {
          SAGE_DisplayError();
        }
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
  if (!InitSkyLayer()) {
    return FALSE;
  }
  if (!InitCloudLayer()) {
    return FALSE;
  }
  if (!InitBackLayer()) {
    return FALSE;
  }
  if (!InitFrontLayer()) {
    return FALSE;
  }
  if (!InitTrollSprite()) {
    return FALSE;
  }
  SAGE_MaximumFPS(30);
  return TRUE;
}

VOID ReleaseGraphx(VOID)
{
  SAGE_AppliLog("Release graphx");
  SAGE_ReleaseLayer(SKY_LAYER);
  SAGE_ReleaseLayer(CLOUD_LAYER);
  SAGE_ReleaseLayer(BACK_LAYER);
  SAGE_ReleaseLayer(FRONT_LAYER);
  SAGE_ReleaseSpriteBank(SPR_BANK);
}

BOOL InitMusic(VOID)
{
  SAGE_Music * music = NULL;

  SAGE_AppliLog("Loading music");
  if ((music = SAGE_LoadMusic("data/theme.mod")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (SAGE_AddMusic(TROLL_MUSIC, music)) {
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

VOID Instructions(VOID)
{
  SAGE_PrintText("SAGE V1.2 Troll run demo", 0, 200);
  SAGE_PrintText("Move the troll with cursor keys, SPACE to reset pos", 0, 208);
  SAGE_PrintText("Press F1 for vbl synchro on/off", 0, 216);
  SAGE_PrintText("Press F2-F4 to set max FPS", 0, 224);
  SAGE_PrintText("Press P to pause, ESC for exit", 0, 232);
}

BOOL InitText(VOID)
{
  if (!SAGE_SetFont("courier.font", 8)) {
    SAGE_DisplayError();
  }
  SAGE_SetTextColor(37, 0);
  SAGE_SetDrawingMode(SSCR_TXTTRANSP);
  Instructions();
  SAGE_RefreshScreen();
  Instructions();
  SAGE_RefreshScreen();
  Instructions();
  SAGE_SetDrawingMode(SSCR_TXTREPLACE);
  return TRUE;
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
  if (!InitText()) {
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
    keyboard_state[KEY_F1] = keys[KEY_F1].key_pressed;
    keyboard_state[KEY_F2] = keys[KEY_F2].key_pressed;
    keyboard_state[KEY_F3] = keys[KEY_F3].key_pressed;
    keyboard_state[KEY_F4] = keys[KEY_F4].key_pressed;
    keyboard_state[KEY_QUIT] = keys[KEY_QUIT].key_pressed;
  }
}

VOID _Update(VOID)
{
  ScanKeyboard();
  if (keyboard_state[KEY_QUIT]) finish = TRUE;
  if (keyboard_state[KEY_SPACE]) {
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
  }
  if (keyboard_state[KEY_F1]) {
    synchro = !synchro;
    SAGE_MaximumFPS(30);
    SAGE_VerticalSynchro(synchro);
  }
  if (keyboard_state[KEY_F2]) {
    SAGE_MaximumFPS(30);
    SAGE_VerticalSynchro(FALSE);
  }
  if (keyboard_state[KEY_F3]) {
    SAGE_MaximumFPS(60);
    SAGE_VerticalSynchro(FALSE);
  }
  if (keyboard_state[KEY_F4]) {
    SAGE_MaximumFPS(100);
    SAGE_VerticalSynchro(FALSE);
  }
  // Animate the background
  cloud_x += CLOUD_SPEED;
  if (cloud_x >= CLOUD_WIDTH) cloud_x = 0;
  sky_x += SKY_SPEED;
  if (sky_x >= SKY_WIDTH) sky_x = 0;
  back_x += BACK_SPEED;
  if (back_x >= BACK_WIDTH) back_x = 0;
  // Animate the sprite
  if (++delay >= TROLL_DELAY) {
    delay = 0;
    if (++sprite >= TROLL_ANIM) sprite = 0;
  }
  // Animate the foreground
  front_x += FRONT_SPEED;
  if (front_x >= FRONT_WIDTH) front_x = 0;
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
    // Draw the cloud layer
    if (!SAGE_SetLayerView(CLOUD_LAYER, cloud_x, 0, SCREEN_WIDTH, CLOUD_HEIGHT)) {
      RenderError("Error setting cloud layer view !");
    }
    if (!SAGE_BlitLayerToScreen(CLOUD_LAYER, 0, SKY_HEIGHT)) {
      RenderError("Error blitting cloud layer !");
    }
    // Draw the sky layer
    if (!SAGE_SetLayerView(SKY_LAYER, sky_x, 0, SCREEN_WIDTH, SKY_HEIGHT)) {
      RenderError("Error setting sky layer view !");
    }
    if (!SAGE_BlitLayerToScreen(SKY_LAYER, 0, 0)) {
      RenderError("Error blitting sky layer !");
    }
    // Draw the background layer
    if (!SAGE_SetLayerView(BACK_LAYER, back_x, 0, SCREEN_WIDTH, BACK_HEIGHT)) {
      RenderError("Error setting back layer view !");
    }
    if (!SAGE_BlitLayerToScreen(BACK_LAYER, 0, 0)) {
      RenderError("Error blitting background layer !");
    }
    // Draw the troll sprite
    if (!SAGE_BlitSpriteToScreen(SPR_BANK, sprite, troll_x, troll_y)) {
      RenderError("Error blitting sprite !");
    }
    // Draw the foreground layer
    if (!SAGE_SetLayerView(FRONT_LAYER, front_x, 0, SCREEN_WIDTH, FRONT_HEIGHT)) {
      RenderError("Error setting front layer view !");
    }
    if (!SAGE_BlitLayerToScreen(FRONT_LAYER, 0, 0)) {
      RenderError("Error blitting foreground layer !");
    }
    // Display infos
    if (synchro) {
      SAGE_PrintText("ON ", 280, 218);
    } else {
      SAGE_PrintText("OFF", 280, 218);
    }
    // Draw the fps counter
    sprintf(string_buffer, "%d fps", SAGE_GetFps());
    SAGE_PrintText(string_buffer, 280, 200);
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
  SAGE_AppliLog("** SAGE library Troll run demo V1.2 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    if (SAGE_ApolloPresence()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    if (_Init()) {
      if (!SAGE_PlayMusic(TROLL_MUSIC)) {
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
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
