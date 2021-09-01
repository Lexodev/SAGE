/**
 * dragon_crown.c
 *
 * Simple animation demo for Vampire & high end amiga
 * SAS/C 6.X
 *
 * Controls :
 *  - F1 : decrease number of sprites
 *  - F2 : increase number of sprites
 *  - F3 : reset sprites positions
 *  - F4 : show/hide vampire logo
 *  - F5 : show/hide crown logo
 *  - F6 : show/hide background
 *  - F7 : enable/disable AMMX
 *  - F8 : music pause/resume
 *  - F9 : play a sound
 *  - Help : show/hide help
 *  - Cursor UP/DOWN : move crown logo
 *  - ESC : quit program
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 December 2020
 */

#include <stdio.h>
#include "dragon_crown.h"

UBYTE string_buffer[256];

SpriteInfo sprite_info[NBSPRITES] = {
  {"/data/fighter.bmp", SPRFIGHTER, SPR1FRAME, SPR1WIDTH, SPR1HEIGHT, -SPR1WIDTH, SPR1POSY, SPR1STEPX, 0, 0 },
  {"/data/amazon.bmp", SPRAMAZON, SPR2FRAME, SPR2WIDTH, SPR2HEIGHT, -SPR2WIDTH, SPR2POSY, SPR2STEPX, 0, 0 },
  {"/data/wizard.bmp", SPRWIZARD, SPR3FRAME, SPR3WIDTH, SPR3HEIGHT, -SPR3WIDTH, SPR3POSY, SPR3STEPX, 0, 0 },
  {"/data/elf.bmp", SPRELF, SPR4FRAME, SPR4WIDTH, SPR4HEIGHT, -SPR4WIDTH, SPR4POSY, SPR4STEPX, 0, 0 },
  {"/data/dwarf.bmp", SPRDWARF, SPR5FRAME, SPR5WIDTH, SPR5HEIGHT, -SPR5WIDTH, SPR5POSY, SPR5STEPX, 0, 0 },
  {"/data/sorceress.bmp", SPRSORCERESS, SPR6FRAME, SPR6WIDTH, SPR6HEIGHT, -SPR6WIDTH, SPR6POSY, SPR6STEPX, 0, 0 }
};

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREENWIDTH, SCREENHEIGHT, SCREENDEPTH, SSCR_TRIPLEBUF)) {
    SAGE_SetScreenClip(0, 0, VIEWWIDTH, VIEWHEIGHT);
    SAGE_SetTextColor(2, 1);
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitBackground(VOID)
{
  SAGE_Picture * picture = NULL;

  SAGE_AppliLog("Load background picture");
  SAGE_PrintDirectText("Loading background...", 0, 20);
  if ((picture = SAGE_LoadPicture("/data/background.bmp")) != NULL) {
    SAGE_AppliLog("Create background layer");
    if (SAGE_CreateLayerFromPicture(BGLAYER, picture)) {
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitLogo(VOID)
{
  SAGE_Picture * picture = NULL;

  SAGE_AppliLog("Load logo picture");
  SAGE_PrintDirectText("Loading logo...", 0, 30);
  if ((picture = SAGE_LoadPicture("/data/crownlogo.bmp")) != NULL) {
    SAGE_DumpBitmap(picture->bitmap);
    SAGE_AppliLog("Create logo layer");
    if (SAGE_CreateLayerFromPicture(LOGOLAYER, picture)) {
      SAGE_SetLayerTransparency(LOGOLAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitVampire(VOID)
{
  SAGE_Picture * picture = NULL;

  SAGE_AppliLog("Load vampire picture");
  SAGE_PrintDirectText("Loading vampire...", 0, 40);
  if ((picture = SAGE_LoadPicture("/data/vampire.bmp")) != NULL) {
    SAGE_AppliLog("Create vampire layer");
    if (SAGE_CreateLayerFromPicture(VAMPIRELAYER, picture)) {
      SAGE_SetLayerTransparency(VAMPIRELAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitHelp(VOID)
{
  SAGE_Picture * picture = NULL;

  SAGE_AppliLog("Load help picture");
  SAGE_PrintDirectText("Loading help...", 0, 50);
  if ((picture = SAGE_LoadPicture("/data/help.bmp")) != NULL) {
    SAGE_AppliLog("Create help layer");
    if (SAGE_CreateLayerFromPicture(HELPLAYER, picture)) {
      SAGE_SetLayerTransparency(HELPLAYER, TRANSPCOLOR);
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitSprite(UWORD index)
{
  SAGE_Picture * picture = NULL;
  UWORD sprite;
  ULONG crdy = 0;

  SAGE_AppliLog("Load sprite picture %d", index);
  if (index == 0) {
    SAGE_PrintDirectText("Loading sprites...", 0, 60);
  } else {
    SAGE_PrintDirectText(".", 96+(index*6), 60);
  }
  if ((picture = SAGE_LoadPicture(sprite_info[index].filename)) != NULL) {
    SAGE_DumpBitmap(picture->bitmap);
    if (SAGE_CreateSpriteBank(sprite_info[index].bank, sprite_info[index].nbframe, picture)) {
      SAGE_SetSpriteBankTransparency(sprite_info[index].bank, TRANSPCOLOR);
      for (sprite = 0;sprite < sprite_info[index].nbframe;sprite++) {
        SAGE_AddSpriteToBank(sprite_info[index].bank, sprite, 0, crdy, sprite_info[index].width, sprite_info[index].height, SSPR_HS_TOPLEFT);
        crdy += sprite_info[index].height;
      }
      SAGE_ReleasePicture(picture);
      return TRUE;
    }
    SAGE_ReleasePicture(picture);
  }
  SAGE_DisplayError();
  return FALSE;
}

/*SAGE_SpriteBank * InitFont(VOID)
{
  SAGE_Picture * picture = NULL;
  SAGE_SpriteBank * bank = NULL;
  UWORD sprite;
  ULONG crdy = 0;

  SAGE_AppliLog("Load font and create sprite bank");
  SAGE_PrintDirectText("Loading fonts...", 0, 70);
  if ((picture = SAGE_LoadPicture("/data/fonts.bmp")) != NULL) {
    if ((bank = SAGE_CreateSpriteBank(picture, SCREENDEPTH, FONTFRAME)) != NULL) {
      SAGE_SetSpriteBankTransparency(bank, TRANSPCOLOR);
      for (sprite = 0;sprite < FONTFRAME;sprite++) {
        SAGE_AddSpriteToBank(bank, sprite, 0, crdy, FONTWIDTH, FONTHEIGHT);
        crdy += FONTHEIGHT;
      }
    } else {
      SAGE_DisplayError();
    }
    SAGE_ReleasePicture(picture);
  }
      for (index = 0;index < NBSPRITES;index++) {
        if ((sprites[index] = InitSprite(index)) == NULL) {
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
  return bank;
}*/

/*SAGE_SpriteBank * InitMouse(VOID)
{
  SAGE_Picture * picture = NULL;
  SAGE_SpriteBank * bank = NULL;
  UWORD sprite;
  ULONG crdy = 0;

  printf("Load mouse cursors and create sprite bank\n");
      SAGE_PrintText(screen, "Loading mouse pointer...", 0, 80);
  if ((picture = SAGE_LoadPicture("/data/idlesorceress.bmp")) != NULL) {
    if ((bank = SAGE_CreateSpriteBankFromPicture(picture, SCREENDEPTH, SPR0FRAME)) != NULL) {
      SAGE_SetSpriteBankTransparency(bank, TRANSPCOLOR);
      for (sprite = 0;sprite < SPR0FRAME;sprite++) {
        SAGE_AddSpriteToBank(bank, sprite, 0, crdy, SPR0WIDTH, SPR0HEIGHT);
        crdy += SPR0HEIGHT;
      }
    } else {
      SAGE_DisplayError();
    }
    SAGE_ReleasePicture(picture);
  }
  return bank;
}*/

BOOL InitGraphx(VOID)
{
  UWORD sprite;
  
  SAGE_AppliLog("Init graphx");
  if (!InitBackground()) {
    return FALSE;
  }
  if (!InitLogo()) {
    return FALSE;
  }
  if (!InitVampire()) {
    return FALSE;
  }
  if (!InitHelp()) {
    return FALSE;
  }
  for (sprite = 0;sprite < NBSPRITES;sprite++) {
    if (!InitSprite(sprite)) {
      return FALSE;
    }
  }
  if (!SAGE_EnableFrameCount(TRUE)) {
    SAGE_ErrorLog("Can't activate frame rate counter !");
  }
  SAGE_SetFont("diamond.font", 12);
  return TRUE;
}

VOID ReleaseGraphx(VOID)
{
  UWORD sprite;

  SAGE_AppliLog("Release graphx");
  SAGE_ReleaseLayer(BGLAYER);
  SAGE_ReleaseLayer(LOGOLAYER);
  SAGE_ReleaseLayer(VAMPIRELAYER);
  SAGE_ReleaseLayer(HELPLAYER);
  for (sprite = 0;sprite < NBSPRITES;sprite++) {
    SAGE_ReleaseSpriteBank(sprite_info[sprite].bank);
  }
}

BOOL InitMusic(VOID)
{
  SAGE_Music * music = NULL;

  SAGE_PrintDirectText("Loading music...", 0, 70);
  if ((music = SAGE_LoadMusic("/data/theme.mod")) != NULL) {
    SAGE_AppliLog("Adding music");
    if (SAGE_AddMusic(DRAGON_MUSIC, music)) {
      return TRUE;
    }
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitSound(VOID)
{
  SAGE_Sound * sound = NULL;

  SAGE_PrintDirectText("Loading sound...", 0, 80);
  if ((sound = SAGE_LoadSound("/data/evil.wav")) != NULL) {
    SAGE_AppliLog("Adding sound");
    if (SAGE_AddSound(SOUND_EVIL, sound)) {
      return TRUE;
    }
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitAudio(VOID)
{
  SAGE_AppliLog("Init audio");
  if (!InitMusic()) {
    return FALSE;
  }
  if (!InitSound()) {
    return FALSE;
  }
  return TRUE;
}

VOID ReleaseAudio(VOID)
{
  SAGE_AppliLog("Release audio");
  SAGE_StopMusic();
  SAGE_ClearMusic();
  SAGE_ClearSound();
}

/**
 * Vampire demo entry point
 */
void main(void)
{
  SAGE_Timer * timer = NULL;
  SAGE_Event * event = NULL;
  BOOL show_logo = TRUE, show_bg = TRUE, show_vampire = FALSE, show_help = FALSE, finish = FALSE, music = TRUE, use_ammx = FALSE;
  ULONG bg_posx = 0, logo_posy = LOGOPOSY, frame_count = 0, elapsed_time = 0, avg_render = 0;
  UWORD index = 0, show_sprite = 5; //, fps = 0, mouse_cursor = 0;

  //SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library dragon crown demo V1.2 **");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO|SMOD_INTERRUPTION)) {
    if (SAGE_AMMX2Available()) {
      SAGE_AppliLog("AMMX detected !!!");
    } else {
      SAGE_AppliLog("AMMX not detected");
    }
    SAGE_UseAMMX(FALSE);    // Don't use AMMX at start
    if (OpenScreen()) {
      SAGE_HideMouse();
      SAGE_PrintDirectText("** SAGE Dragon Crown Demo V1.2 **", 0, 10);
      if (InitGraphx() && InitAudio()) {
        if (music && !SAGE_PlayMusic(DRAGON_MUSIC)) {
          SAGE_DisplayError();
          finish = TRUE;
        }
        if ((timer = SAGE_AllocTimer()) == NULL) {
          SAGE_DisplayError();
          finish = TRUE;
        }
        SAGE_AppliLog("Entering main loop");
        //SAGE_SetCustomMouse(screen, mouses);
        while (!finish) {

          if (SAGE_IsFrontMostScreen()) {
            frame_count++;
            // Check events
            while ((event = SAGE_GetEvent()) != NULL) {
              if (event->type == SEVT_RAWKEY) {
                switch (event->code) {
                  case SKEY_FR_ESC:
                    finish = TRUE;
                    break;
                  case SKEY_FR_F1:
                    show_sprite--;
                    if (show_sprite < 1) {
                      show_sprite = 1;
                    }
                    break;
                  case SKEY_FR_F2:
                    show_sprite++;
                    if (show_sprite >= NBSPRITES) {
                      show_sprite = NBSPRITES-1;
                    }
                    break;
                  case SKEY_FR_F3:
                    for (index = 0;index < show_sprite; index++) {
                      sprite_info[index].posx = 0;
                    }
                    logo_posy = LOGOPOSY;
                    break;
                  case SKEY_FR_F4:
                    show_vampire = !show_vampire;
                    break;
                  case SKEY_FR_F5:
                    show_logo = !show_logo;
                    break;
                  case SKEY_FR_F6:
                    show_bg = !show_bg;
                    break;
                  case SKEY_FR_F7:
                    use_ammx = !use_ammx;
                    break;
                  case SKEY_FR_F8:
                    if (music) {
                      SAGE_PauseMusic();
                      music = FALSE;
                    } else {
                      SAGE_ResumeMusic();
                      music = TRUE;
                    }
                    break;
                  case SKEY_FR_F9:
                    SAGE_PlaySound(SOUND_EVIL, SAUD_CHANNEL4);
                    break;
                  case SKEY_FR_HELP:
                    show_help = !show_help;
                    break;
                  case SKEY_FR_UP:
                    logo_posy -= 4;
                    if (logo_posy < LOGOPOSY) {
                      logo_posy = LOGOPOSY;
                    }
                    break;
                  case SKEY_FR_DOWN:
                    logo_posy += 4;
                    if (logo_posy > (VIEWHEIGHT - LOGOHEIGHT - 10)) {
                      logo_posy = VIEWHEIGHT - LOGOHEIGHT - 10;
                    }
                    break;
                }
              }
            }
            SAGE_UseAMMX(use_ammx);
            elapsed_time = SAGE_ElapsedTime(timer);
            // Move & render background
            if (show_bg) {
              bg_posx = (bg_posx + 1) % BGWIDTH;
              SAGE_SetLayerView(BGLAYER, bg_posx, 0, VIEWWIDTH, VIEWHEIGHT);
              if (!SAGE_BlitLayerToScreen(BGLAYER, BGPOSX, BGPOSY)) {
                finish = TRUE;
                SAGE_DisplayError();
              }
            } else {
              SAGE_ClearView();
            }
            // Render Vampire logo
            if (show_vampire) {
              if (!SAGE_BlitLayerToScreen(VAMPIRELAYER, VAMPIREPOSX, VAMPIREPOSY)) {
                finish = TRUE;
                SAGE_DisplayError();
              }
            }
            // Render Vampire help
            if (show_help) {
              if (!SAGE_BlitLayerToScreen(HELPLAYER, HELPPOSX, HELPPOSY)) {
                finish = TRUE;
                SAGE_DisplayError();
              }
            }
            // Render sprites
            for (index = 0;index < show_sprite; index++) {
              SAGE_BlitSpriteToScreen(sprite_info[index].bank, sprite_info[index].frame, sprite_info[index].posx + BGPOSX, sprite_info[index].posy);
              if ((frame_count % SPRFRAMERATE) == 0) {
                sprite_info[index].frame = (sprite_info[index].frame + 1) % sprite_info[index].nbframe;
              }
              sprite_info[index].posx += sprite_info[index].stepx;
              if (sprite_info[index].posx > VIEWWIDTH) {
                sprite_info[index].posx = -sprite_info[index].width;
              }
            }
            // Render logo
            if (show_logo) {
              if (!SAGE_BlitLayerToScreen(LOGOLAYER, LOGOPOSX, LOGOPOSY)) {
                finish = TRUE;
                SAGE_DisplayError();
              }
            }
            // Draw the fps counter
            sprintf(string_buffer, "%d fps", SAGE_GetFps());
            SAGE_PrintText(string_buffer, 10, 10);
            // Update the mouse cursor
            /*SAGE_SetMouseSprite(screen, mouse_cursor);
            if ((frame_count % SPRFRAMERATE) == 0) {
              mouse_cursor = (mouse_cursor + 1) % SPR0FRAME;
            }*/
            elapsed_time = SAGE_ElapsedTime(timer);
            avg_render += elapsed_time & STIM_MICRO_MASK;
            // Render the screen
            if (!SAGE_RefreshScreen()) {
              finish = TRUE;
              SAGE_DisplayError();
            }
          } else {
            // Pause music if screen not in front
          }
          
        }
        SAGE_AppliLog("Average render time for a frame is %d micro seconds for a total of %d frames", avg_render / frame_count, frame_count);
      }
      SAGE_ReleaseTimer(timer);
      ReleaseAudio();
      ReleaseGraphx();
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  if (!SAGE_Exit()) {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("End of demo");
}
