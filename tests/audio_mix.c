/**
 * audio_mix.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test mix sound/music
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define SOUND_EVIL            1
#define MUSIC_THEME           1

void main(void)
{
  SAGE_Event * event;
  SAGE_Music * music = NULL;
  SAGE_Sound * sound = NULL;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library AUDIO test (MIX) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_NOFLAG)) {
      SAGE_AppliLog("Loading module");
      if ((music = SAGE_LoadMusic("data/bubble.mod")) == NULL) {
        finish = TRUE;
        SAGE_DisplayError();
      } else {
        SAGE_AppliLog("Adding music");
        if (!SAGE_AddMusic(MUSIC_THEME, music)) {
          SAGE_AppliLog("Add theme module error !");
          finish = TRUE;
          SAGE_DisplayError();
        }
      }
      SAGE_AppliLog("Loading sound\n");
      sound = SAGE_LoadSound("data/evil.wav");
      if (sound == NULL) {
        finish = TRUE;
        SAGE_DisplayError();
      } else {
        if (!SAGE_AddSound(SOUND_EVIL, sound)) {
          SAGE_AppliLog("Add sound error !!!");
          finish = TRUE;
          SAGE_DisplayError();
        }
      }
      SAGE_PrintText("PRESS SPACE FOR SOUND 1 ON CHAN0", 20, 20);
      SAGE_PrintText("      ENTER FOR SOUND 1 ON CHAN4", 20, 40);
      SAGE_PrintText("      MOUSE FOR EXIT", 20, 60);
      SAGE_RefreshScreen();
      if (!SAGE_PlayMusic(MUSIC_THEME)) {
        finish = TRUE;
        SAGE_DisplayError();
      }
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          SAGE_AppliLog(
            "Event polled type %d, code %d, mouse %d,%d",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_MOUSEBT) {
            SAGE_AppliLog("Exit loop");
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY) {
            if (event->code == SKEY_FR_SPACE) {
              if (!SAGE_PlaySound(SOUND_EVIL, SAUD_CHANNEL0)) {
                SAGE_AppliLog("Sound play error !");
              }
            } else if (event->code == SKEY_FR_ENTER) {
              if (!SAGE_PlaySound(SOUND_EVIL, SAUD_CHANNEL4)) {
                SAGE_AppliLog("Sound play error !");
              }
            }
          }
        }
      }
      SAGE_AppliLog("Closing music");
      SAGE_StopMusic();
      SAGE_ClearMusic();
      SAGE_AppliLog("Clear all sounds");
      SAGE_ClearSound();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
