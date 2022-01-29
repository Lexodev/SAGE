/**
 * audio_mix.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test mix sound/music
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

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

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library AUDIO test (MIX) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_DOUBLEBUF)) {
      printf("Loading module\n");
      if ((music = SAGE_LoadMusic("/data/bubble.mod")) == NULL) {
        finish = TRUE;
        SAGE_DisplayError();
      } else {
        printf("Adding music\n");
        if (!SAGE_AddMusic(MUSIC_THEME, music)) {
          printf("Add theme module error !\n");
          finish = TRUE;
          SAGE_DisplayError();
        }
      }
      printf("Loading sound\n");
      sound = SAGE_LoadSound("/data/evil.wav");
      if (sound == NULL) {
        finish = TRUE;
        SAGE_DisplayError();
      } else {
        if (!SAGE_AddSound(SOUND_EVIL, sound)) {
          printf("Add sound error !!!\n");
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
          printf(
            "Event polled type %d, code %d, mouse %d,%d\n",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_MOUSEBT) {
            printf("Exit loop\n");
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY) {
            if (event->code == SKEY_FR_SPACE) {
              if (!SAGE_PlaySound(SOUND_EVIL, SAUD_CHANNEL0)) {
                printf("Sound play error !\n");
              }
            } else if (event->code == SKEY_FR_ENTER) {
              if (!SAGE_PlaySound(SOUND_EVIL, SAUD_CHANNEL4)) {
                printf("Sound play error !\n");
              }
            }
          }
        }
      }
      printf("Closing music\n");
      SAGE_StopMusic();
      SAGE_ClearMusic();
      printf("Clear all sounds\n");
      SAGE_ClearSound();
      printf("Closing screen\n");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
