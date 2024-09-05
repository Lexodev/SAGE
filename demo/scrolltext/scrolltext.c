/**
 * scrolltext.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of simple text scrolling
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 September 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <dos/dos.h>
#include <clib/dos_protos.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          16L

#define FONT_WIDTH            32
#define FONT_HEIGHT           32
#define FONT_NUM              60
#define FONTPIC_WIDTH         320
#define FONTPIC_HEIGHT        192
#define FONTPIC_TRANSPARENCY  0x000000

#define TEXTFIELD_WIDTH       SCREEN_WIDTH+FONT_WIDTH
#define TEXTFIELD_HEIGHT      FONT_HEIGHT
#define TEXTFIELD_LAYER       0

#define TEXTSCROLL_SPEED      2
#define TEXTSCROLL_POSX       0

#define BACKGROUND_WIDTH      320
#define BACKGROUND_HEIGHT     240

#define RAD(x)                ((x)*PI/180.0)
#define CURVE_SCROLL          512

#define SPRITE_BANK           0
#define SPRITE_NUM            15
#define SPRITE_TRANSP         0xff00ff
#define SPRITE_WIDTH          128
#define SPRITE_HEIGHT         128
#define SPRPIC_WIDTH          640
#define SPRPIC_HEIGHT         384
#define SPRITE_ANIM           4

#define MUSIC_SLOT            1
#define SOUND_SLOT            1

SAGE_Picture *fonts = NULL, *background = NULL;

STRPTR message = NULL;
UWORD message_pos = 0, font_posx[FONT_NUM], font_posy[FONT_NUM], char_posx = SCREEN_WIDTH, char_load = 0;
UWORD layer_posx = SCREEN_WIDTH+FONT_WIDTH, layer_posy = 0, scroll_posy = 0, curve_idx = 0;
UWORD sprite_num = 0, sprite_anim = SPRITE_ANIM;
LONG sprite_posx = 0, sprite_posy = 0, sprite_dx = 1, sprite_dy = 1;

FLOAT curve[CURVE_SCROLL];

SAGE_Music *music = NULL;
SAGE_Sound *sound = NULL;

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

BOOL InitTextfieldLayer(VOID)
{
  SAGE_AppliLog("Create text field layer");
  if (SAGE_CreateLayer(TEXTFIELD_LAYER, TEXTFIELD_WIDTH, TEXTFIELD_HEIGHT)) {
    SAGE_SetLayerTransparency(TEXTFIELD_LAYER, FONTPIC_TRANSPARENCY);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackground(VOID)
{
  SAGE_AppliLog("Load background picture");
  if ((background = SAGE_LoadPicture("data/vampire.png")) != NULL) {
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitMessage(VOID)
{
  BPTR fdesc;
  LONG bytes_read;

  fdesc = Open("data/message.txt", MODE_OLDFILE);
  if (fdesc) {
    // Get the file size
    bytes_read = Seek(fdesc, 0, OFFSET_END);
    bytes_read = Seek(fdesc, 0, OFFSET_BEGINNING);
    message = (STRPTR) SAGE_AllocMem(bytes_read + 2);
    if (message != NULL) {
      if (Read(fdesc, message, bytes_read) == bytes_read) {
        Close(fdesc);
        return TRUE;
      }
    }
    Close(fdesc);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitFonts(VOID)
{
  UWORD x, y, idx, line, column;

  SAGE_AppliLog("Load fonts picture");
  if ((fonts = SAGE_LoadPicture("data/fonts_32_32.png")) != NULL) {
    SAGE_AppliLog("Precalc fonts coordinates");
    idx = 0;
    y = 0;
    for (line = 0; line < (FONTPIC_HEIGHT / FONT_HEIGHT); line++) {
      x = 0;
      for (column = 0;column < (FONTPIC_WIDTH / FONT_WIDTH); column++) {
        font_posx[idx] = x;
        font_posy[idx] = y;
        x += FONT_WIDTH;
        idx++;
      }
      y += FONT_HEIGHT;
    }
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitCurve(VOID)
{
  FLOAT angle;
  ULONG i, amplitude;

  SAGE_AppliLog("Init the scroll curve");
  angle = 0.0;
  amplitude = (SCREEN_HEIGHT - FONT_HEIGHT) / 2;
  for (i = 0;i < CURVE_SCROLL;i++) {
    curve[i] = amplitude + (sin(RAD(angle)) * amplitude);
    angle += (360.0 / CURVE_SCROLL);
  }
  return TRUE;
}

BOOL InitSprites(VOID)
{
  SAGE_Picture *picture;
  UWORD sprite_idx = 0;
  ULONG sprite_left = 0, sprite_top = 0;

  SAGE_AppliLog("Init the sprites");
  if ((picture = SAGE_LoadPicture("data/bballs.png")) != NULL) {
    // Let's create a sprite bank
    if (SAGE_CreateSpriteBank(SPRITE_BANK, SPRITE_NUM, picture)) {
      // Set the transparency color
      SAGE_SetSpriteBankTransparency(SPRITE_BANK, SPRITE_TRANSP);
      // Add our sprites and set the hotspot as the top left of the sprite
      for (sprite_idx = 0;sprite_idx < SPRITE_NUM;sprite_idx++) {
        SAGE_AddSpriteToBank(SPRITE_BANK, sprite_idx, sprite_left, sprite_top, SPRITE_WIDTH, SPRITE_HEIGHT, SSPR_HS_TOPLEFT);
        sprite_left += SPRITE_WIDTH;
        if (sprite_left >= SPRPIC_WIDTH) {
          sprite_left = 0;
          sprite_top += SPRITE_HEIGHT;
        }
      }
      // We don't need this picture anymore
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitMusic(VOID)
{
  // Load the music
  SAGE_AppliLog("Load the music");
  if ((music = SAGE_LoadMusic("data/music.mod")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (SAGE_AddMusic(MUSIC_SLOT, music)) {
      return TRUE;
    }
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitSound(VOID)
{
  // Load the sound
  SAGE_AppliLog("Load the sound");
  if ((sound = SAGE_LoadSound("data/sound.wav")) != NULL) {
    if (SAGE_AddSound(SOUND_SLOT, sound)) {
      return TRUE;
    }
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL Init(VOID)
{
  if (!OpenScreen()) {
    return FALSE;
  }
  if (!InitTextfieldLayer()) {
    return FALSE;
  }
  if (!InitBackground()) {
    return FALSE;
  }
  if (!InitMessage()) {
    return FALSE;
  }
  if (!InitFonts()) {
    return FALSE;
  }
  if (!InitCurve()) {
    return FALSE;
  }
  if (!InitSprites()) {
    return FALSE;
  }
  if (!InitMusic()) {
    return FALSE;
  }
  if (!InitSound()) {
    return FALSE;
  }
  return TRUE;
}

VOID Update(VOID)
{
  UBYTE new_char;
  UWORD char_index;
  
  // Should we load a new char from the message
  if (char_load == 0) {
    new_char = message[message_pos];
    // Have we reach the end of the message ?
    if (new_char == 0) {
      // Wrap the message;
      message_pos = 0;
      new_char = message[message_pos];
    }
    char_load = FONT_WIDTH;
    message_pos++;          // Next char
    if (new_char < ' ') {
      char_index = 0;
    } else {
      char_index = new_char - ' ';   // Space is our first char in the font picture
      // If we have some chars that are not in our fonts
      if (char_index >= FONT_NUM) {
        char_index = 0;
      }
    }
    // Copy the char to the layer
    SAGE_BlitPictureToLayer(fonts, font_posx[char_index], font_posy[char_index], FONT_WIDTH, FONT_HEIGHT, TEXTFIELD_LAYER, char_posx, 0);
    char_posx += FONT_WIDTH;
    if (char_posx > SCREEN_WIDTH) {
      char_posx = 0;
    }
  }
  char_load -= TEXTSCROLL_SPEED;
  layer_posx += TEXTSCROLL_SPEED;
  if (layer_posx >= (SCREEN_WIDTH+FONT_WIDTH)) {
    layer_posx = 0;
  }
  // Move the scroll up and down
  scroll_posy = curve[curve_idx++];
  curve_idx %= CURVE_SCROLL;
  // Animate the sprite
  sprite_anim--;
  if (sprite_anim == 0) {
    sprite_num++;
    if (sprite_num >= SPRITE_NUM) {
      sprite_num = 0;
    }
    sprite_anim = SPRITE_ANIM;
  }
  sprite_posx += sprite_dx;
  if (sprite_posx < 0) {
    sprite_dx = 1;
    sprite_posx = 0;
    SAGE_PlaySound(SOUND_SLOT, SAUD_CHANNEL4);
  } else if (sprite_posx > (SCREEN_WIDTH - SPRITE_WIDTH)) {
    sprite_dx = -1;
    sprite_posx = SCREEN_WIDTH - SPRITE_WIDTH;
    SAGE_PlaySound(SOUND_SLOT, SAUD_CHANNEL4);
  }
  sprite_posy += sprite_dy;
  if (sprite_posy < 0) {
    sprite_dy = 1;
    sprite_posy = 0;
    SAGE_PlaySound(SOUND_SLOT, SAUD_CHANNEL4);
  } else if (sprite_posy > (SCREEN_HEIGHT - SPRITE_HEIGHT)) {
    sprite_dy = -1;
    sprite_posy = SCREEN_HEIGHT - SPRITE_HEIGHT;
    SAGE_PlaySound(SOUND_SLOT, SAUD_CHANNEL4);
  }
}

VOID Render(VOID)
{
  // Blit the background to the screen
  SAGE_BlitPictureToScreen(background, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0, 0);
  // Blit the sprite
  SAGE_BlitSpriteToScreen(SPRITE_BANK, sprite_num, sprite_posx, sprite_posy);
  // Set the text layer view (using the wrapping feature of layers to simulate infite scroll)
  SAGE_SetLayerView(TEXTFIELD_LAYER, layer_posx, layer_posy, SCREEN_WIDTH, TEXTFIELD_HEIGHT);
  // Blit the text layer to the screen
  SAGE_BlitLayerToScreen(TEXTFIELD_LAYER, TEXTSCROLL_POSX, scroll_posy);
  // Switch screen buffers
  SAGE_RefreshScreen();
}

VOID Restore(VOID)
{
  // Release memory
  SAGE_FreeMem(message);
  // Stop and clear music
  SAGE_StopMusic();
  SAGE_ClearMusic();
  // Release the graphics
  SAGE_ReleaseSpriteBank(SPRITE_BANK);
  SAGE_ReleasePicture(background);
  SAGE_ReleasePicture(fonts);
  SAGE_ReleaseLayer(TEXTFIELD_LAYER);
  // Show the mouse
  SAGE_ShowMouse();
  // And close the screen
  SAGE_CloseScreen();
}

void main(void)
{
  SAGE_Event *event = NULL;
  BOOL finish;

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Scrolltext demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
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
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
