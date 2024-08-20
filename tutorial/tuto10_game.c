/**
 * tuto10_game.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of SAGE usage for a game
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 August 2024
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define BG_WIDTH              1280L
#define BG_HEIGHT             720L
#define BACK_LAYER            0

#define NB_SPRITES            2
#define SPR_BANK              0
#define SPR_TRANSP            0xff00ff
#define SHIP_NUM              0
#define SHIP_LEFT             0L
#define SHIP_TOP              0L
#define SHIP_WIDTH            360L
#define SHIP_HEIGHT           256L
#define SHIP_YMIN             250L
#define SHIP_YMAX             350L
#define SHIP_SPEED            4
#define LASER_NUM             1
#define LASER_LEFT            372L
#define LASER_TOP             0L
#define LASER_WIDTH           32L
#define LASER_HEIGHT          80L
#define LASER_SPEED           8

#define MUSIC_SLOT            1
#define SOUND_SLOT            1

// Demo data
LONG bg_xoffset, bg_yoffset, ship_y, missile_x, missile_y;
BOOL missile_on = FALSE, fc_on = TRUE;

// Controls
#define KEY_NBR               5
#define KEY_LEFT              0
#define KEY_RIGHT             1
#define KEY_UP                2
#define KEY_DOWN              3
#define KEY_FIRE              4

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_SPACE, FALSE }
};

// Init the demo, open screen, load sprites and music
BOOL Init(VOID)
{
  SAGE_Picture *picture = NULL;
  SAGE_Music *music = NULL;
  SAGE_Sound *sound = NULL;

  // Open the screen
  SAGE_AppliLog("Open the screen");
  if (!SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    return FALSE;
  }
  SAGE_HideMouse();
  // Set the color for the text
  SAGE_SetColor(0, 0x0);
  SAGE_SetColor(1, 0xffffff);
  SAGE_RefreshColors(0, 2);
  SAGE_SetTextColor(1, 0);
  // Load the background picture
  SAGE_AppliLog("Loading the background picture");
  if ((picture = SAGE_LoadPicture("data/galaxy.png")) != NULL) {
    // Let's create a background layer
    if (!SAGE_CreateLayerFromPicture(BACK_LAYER, picture)) {
      return FALSE;
    }
    // We don't need this picture anymore, release it before loading the sprite
    SAGE_ReleasePicture(picture);
  } else {
    return FALSE;
  }
  // Load the sprite picture
  SAGE_AppliLog("Loading the sprite picture");
  if ((picture = SAGE_LoadPicture("data/spaceship.png")) != NULL) {
    // Let's create a sprite bank
    if (SAGE_CreateSpriteBank(SPR_BANK, NB_SPRITES, picture)) {
      // Set the transparency color
      SAGE_SetSpriteBankTransparency(SPR_BANK, SPR_TRANSP);
      // Add our ship sprite and set the hotspot as the middle of the sprite
      if (!SAGE_AddSpriteToBank(SPR_BANK, SHIP_NUM, SHIP_LEFT, SHIP_TOP, SHIP_WIDTH, SHIP_HEIGHT, SSPR_HS_MIDDLE)) {
        return FALSE;
      }
      // Add our laser sprite and set the hotspot as the top middle of the sprite
      if (!SAGE_AddSpriteToBank(SPR_BANK, LASER_NUM, LASER_LEFT, LASER_TOP, LASER_WIDTH, LASER_HEIGHT, SSPR_HS_TOPMID)) {
        return FALSE;
      }
    } else {
      return FALSE;
    }
    // We don't need this picture anymore
    SAGE_ReleasePicture(picture);
  } else {
    return FALSE;
  }
  // Load the music
  SAGE_AppliLog("Load the music");
  if ((music = SAGE_LoadMusic("data/music.mod")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (!SAGE_AddMusic(MUSIC_SLOT, music)) {
      return FALSE;
    }
  } else {
    return FALSE;
  }
  // Load the sound
  SAGE_AppliLog("Load the sound");
  if ((sound = SAGE_LoadSound("data/laser.wav")) != NULL) {
    SAGE_AppliLog("Sound volumes %d", sound->volume);
    if (!SAGE_AddSound(SOUND_SLOT, sound)) {
      return FALSE;
    }
  } else {
    return FALSE;
  }
  // Enable the frame counter
  if (!SAGE_EnableFrameCount(TRUE)) {
    SAGE_ErrorLog("Can't activate frame rate counter !");
    fc_on = FALSE;
  }
  // Let's init our layer coordinates
  bg_xoffset = BG_WIDTH / 4;
  bg_yoffset = BG_HEIGHT - 1;
  // Ship coordinates
  ship_y = SHIP_YMAX;
  return TRUE;
}

VOID Update(VOID)
{
  SAGE_PortScan port2;

  // Now we can update the layers offset to do the scrolling effect
  bg_yoffset -= 1;  // One pixel for the background
  if (bg_yoffset < 0) {
    bg_yoffset = BG_HEIGHT - 1;
  }
  // Scan the keys
  SAGE_ScanKeyboard(keys, KEY_NBR);
  // Scan the joystick
  SAGE_ScanPort(&port2, SINP_JOYPORT2);
  // If we don't have joystick or gamepad connected just ignore
  if (port2.type != SINP_GAMEPAD || port2.type != SINP_JOYSTICK) {
    port2.left = FALSE;
    port2.right = FALSE;
    port2.up = FALSE;
    port2.down = FALSE;
    port2.fire1 = FALSE;
  }
  // Move the ship
  if (keys[KEY_LEFT].key_pressed || port2.left) {
    bg_xoffset -= SHIP_SPEED;
    if (bg_xoffset < 0) {
      bg_xoffset = 0;
    }
  } else if (keys[KEY_RIGHT].key_pressed || port2.right) {
    bg_xoffset += SHIP_SPEED;
    if (bg_xoffset > (BG_WIDTH/2)) {
      bg_xoffset = (BG_WIDTH/2);
    }
  }
  if (keys[KEY_UP].key_pressed || port2.up) {
    ship_y -= SHIP_SPEED;
    if (ship_y < SHIP_YMIN) {
      ship_y = SHIP_YMIN;
    }
  } else if (keys[KEY_DOWN].key_pressed || port2.down) {
    ship_y += SHIP_SPEED;
    if (ship_y > SHIP_YMAX) {
      ship_y = SHIP_YMAX;
    }
  }
  // Launch our missile
  if (keys[KEY_FIRE].key_pressed || port2.fire1) {
    // We can launch only one missile at a time
    if (!missile_on) {
      SAGE_PlaySound(SOUND_SLOT, SAUD_CHANNEL4);
      missile_x = SCREEN_WIDTH / 2;
      missile_y = ship_y - (LASER_HEIGHT + (SHIP_HEIGHT / 2));
      missile_on = TRUE;
    }
  }
  // Move our missile
  if (missile_on) {
    missile_y -= LASER_SPEED;
    // If the missile is out of screen, stop it
    if (missile_y < -LASER_HEIGHT) {
      missile_on = FALSE;
    }
  }
}

VOID Render(VOID)
{
  // Set the view of the background layer, the view define what part of the layer will be blit to the screen
  SAGE_SetLayerView(BACK_LAYER, bg_xoffset, bg_yoffset, SCREEN_WIDTH, SCREEN_HEIGHT);
  // First blit the background layer to the screen
  SAGE_BlitLayerToScreen(BACK_LAYER, 0, 0);
  // Then blit the sprite
  SAGE_BlitSpriteToScreen(SPR_BANK, SHIP_NUM, SCREEN_WIDTH / 2, ship_y);
  // Then the missile if active
  if (missile_on) {
    SAGE_BlitSpriteToScreen(SPR_BANK, LASER_NUM, missile_x, missile_y);
  }
  // Draw the fps counter
  if (fc_on) {
    SAGE_PrintFText(580, 10, "%d fps", SAGE_GetFps());
  }
  // To see the result we have to switch screen buffers
  SAGE_RefreshScreen();
}

VOID Restore(VOID)
{
  // Release the graphics
  SAGE_ReleaseLayer(BACK_LAYER);
  SAGE_ReleaseSpriteBank(SPR_BANK);
  // Release the audio
  SAGE_ClearSound();
  SAGE_StopMusic();
  SAGE_ClearMusic();
  // Show the mouse
  SAGE_ShowMouse();
  // And close the screen
  SAGE_CloseScreen();
}

void main(void)
{
  SAGE_Event *event = NULL;
  BOOL finish;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 10 : GAME / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    // Init all demo elements
    if (Init()) {
      SAGE_PlayMusic(MUSIC_SLOT);
      finish = FALSE;
      while (!finish) {
        // Read all events raised by the screen
        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_MOUSEBT) {
            // If we click on mouse button, we stop the loop
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY && event->code == SKEY_FR_ESC) {
            // If we press the ESC key, we stop the loop
            finish = TRUE;
          }
        }
        // Update the demo data
        Update();
        // Render the demo
        Render();
      }
    } else {
      SAGE_AppliLog("Init error");
      SAGE_DisplayError();
    }
    Restore();
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 10");
}
