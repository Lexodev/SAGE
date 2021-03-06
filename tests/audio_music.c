/**
 * audio_music.c
 *
 * SAGE (Simple Amiga Game Engine) project
 * Test music player
 *
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include <exec/interrupts.h>
#include <hardware/cia.h>
#include <resources/cia.h>

#include <clib/exec_protos.h>
#include <clib/cia_protos.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define MUSIC_BUBBLE          1
#define MUSIC_MANDO           2

extern struct Library * PTReplayBase;

VOID ASM MyInterrupt(REG(a1, APTR data)) {}

void ShowTimer(void)
{
  struct CIABase * ciaabase, * ciabbase;
  struct Interrupt timerint;

  printf(" --- ShowTimer ---\n");
  /* get pointers to both resource bases */
  ciaabase = OpenResource(CIAANAME);
  ciabbase = OpenResource(CIABNAME);

  timerint.is_Node.ln_Type = NT_INTERRUPT;
  timerint.is_Node.ln_Pri  = 0;
  timerint.is_Node.ln_Name = "cia_check";
  timerint.is_Data         = NULL;
  timerint.is_Code         = (APTR) MyInterrupt;

  // Check CIA-A / Timer A
  if (!(AddICRVector((struct Library *)ciaabase, CIAICRB_TA, &timerint))) {
    printf("CIA-A / Timer A is free\n");
    RemICRVector((struct Library *)ciaabase, CIAICRB_TA, &timerint);
  } else {
    printf("CIA-A / Timer A is busy\n");
  }
  // Check CIA-A / Timer B
  if (!(AddICRVector((struct Library *)ciaabase, CIAICRB_TB, &timerint))) {
    printf("CIA-A / Timer B is free\n");
    RemICRVector((struct Library *)ciaabase, CIAICRB_TB, &timerint);
  } else {
    printf("CIA-A / Timer B is busy\n");
  }
  // Check CIA-B / Timer A
  if (!(AddICRVector((struct Library *)ciabbase, CIAICRB_TA, &timerint))) {
    printf("CIA-B / Timer A is free\n");
    RemICRVector((struct Library *)ciabbase, CIAICRB_TA, &timerint);
  } else {
    printf("CIA-B / Timer A is busy\n");
  }
  // Check CIA-B / Timer B
  if (!(AddICRVector((struct Library *)ciabbase, CIAICRB_TB, &timerint))) {
    printf("CIA-B / Timer B is free\n");
    RemICRVector((struct Library *)ciabbase, CIAICRB_TB, &timerint);
  } else {
    printf("CIA-B / Timer B is busy\n");
  }

  printf(" --- ShowTimer ---\n");
}

/**
 * Test music replay
 *
 * F1/F2 to change music
 */
void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Music * music1 = NULL, * music2 = NULL;
  BOOL finish = FALSE, ok = TRUE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library AUDIO test (MUSIC) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  printf("Timer before init\n");
  ShowTimer();
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
    printf("Timer after init\n");
    ShowTimer();
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_DOUBLEBUF)) {
      printf("Loading modules\n");
      if ((music1 = SAGE_LoadMusic("/data/bubble.mod")) != NULL
          && (music2 = SAGE_LoadMusic("/data/mando.aiff")) != NULL) {
        printf("Adding musics\n");
        if (!SAGE_AddMusic(MUSIC_BUBBLE, music1)) {
          printf("Add bubble module error !\n");
          SAGE_DisplayError();
          finish = TRUE;
          ok = FALSE;
        }
        if (!SAGE_AddMusic(MUSIC_MANDO, music2)) {
          printf("Add mando music error !\n");
          SAGE_DisplayError();
          finish = TRUE;
          ok = FALSE;
        }
      } else {
        SAGE_DisplayError();
        finish = TRUE;
        ok = FALSE;
      }
      if (ok) {
        SAGE_PrintText("PRESS F1 FOR MUSIC 1", 20, 20);
        SAGE_PrintText("      F2 FOR MUSIC 2", 20, 40);
        SAGE_PrintText("      F5 FOR MUSIC PAUSE", 20, 60);
        SAGE_PrintText("      F6 FOR MUSIC RESUME", 20, 80);
        SAGE_PrintText("      MOUSE FOR EXIT", 20, 100);
        SAGE_RefreshScreen();
        printf("Playing runner\n");
        if (!SAGE_PlayMusic(MUSIC_BUBBLE)) {
          SAGE_DisplayError();
          finish = TRUE;
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
              if (event->code == SKEY_FR_F1) {
                printf("Playing bubble\n");
                SAGE_PlayMusic(MUSIC_BUBBLE);
              }
              if (event->code == SKEY_FR_F2) {
                printf("Playing mando\n");
                SAGE_PlayMusic(MUSIC_MANDO);
              }
              if (event->code == SKEY_FR_F5) {
                printf("Pause music\n");
                SAGE_PauseMusic();
              }
              if (event->code == SKEY_FR_F6) {
                printf("Resume music\n");
                SAGE_ResumeMusic();
              }
            }
          }
        }
      }
      printf("Closing music\n");
      SAGE_StopMusic();
      SAGE_ClearMusic();
      printf("Closing screen\n");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
