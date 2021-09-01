/**
 * audio_sound.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test sound loading
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
#define SOUND_SHOOT           2
#define SOUND_RAGE            3

void main(void)
{
  SAGE_Event * event;
  SAGE_Sound * sound1, * sound2, * sound3;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library AUDIO test (SOUND) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_DOUBLEBUF)) {
      printf("Loading sounds\n");
      sound1 = SAGE_LoadSound("/data/evil.wav");
      sound2 = SAGE_LoadSound("/data/shoot.wav");
      sound3 = SAGE_LoadSound("/data/hiya.8svx");
      printf("Sound volumes %d / %d / %d\n", sound1->volume, sound2->volume, sound3->volume);
      if (sound1 == NULL || sound2 == NULL || sound3 == NULL) {
        finish = TRUE;
        SAGE_DisplayError();
      } else {
        if (!SAGE_AddSound(SOUND_EVIL, sound1) || !SAGE_AddSound(SOUND_SHOOT, sound2) || !SAGE_AddSound(SOUND_RAGE, sound3)) {
          printf("Add sound error !!!\n");
          finish = TRUE;
          SAGE_DisplayError();
        }
      }
      SAGE_PrintText("PRESS SPACE FOR SOUND 1", 20, 20);
      SAGE_PrintText("      ENTER FOR SOUND 2", 20, 40);
      SAGE_PrintText("      TAB   FOR SOUND 3", 20, 60);
      SAGE_PrintText("      MOUSE FOR EXIT", 20, 80);
      SAGE_RefreshScreen();
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
              if (!SAGE_PlaySound(SOUND_EVIL, 0)) {
                printf("Sound play error !\n");
              }
            } else if (event->code == SKEY_FR_ENTER) {
              if (!SAGE_PlaySound(SOUND_SHOOT, 1)) {
                printf("Sound play error !\n");
              }
            } else if (event->code == SKEY_FR_TAB) {
              if (!SAGE_PlaySound(SOUND_RAGE, 2)) {
                printf("Sound play error !\n");
              }
            }
          }
        }
      }
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
