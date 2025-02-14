/**
 * audio_music.c
 *
 * SAGE (Simple Amiga Game Engine) project
 * Test music player
 *
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <exec/interrupts.h>
#include <hardware/cia.h>
#include <resources/cia.h>

#include <clib/exec_protos.h>
#include <clib/cia_protos.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define MUSIC_BUBBLE          1
#define MUSIC_MANDO           2

extern struct Library *PTReplayBase;

VOID ASM MyInterrupt(REG(a1, APTR data)) {}

void ShowTimer(void)
{
  struct CIABase *ciaabase, *ciabbase;
  struct Interrupt timerint;

  SAGE_AppliLog(" --- ShowTimer ---");
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
    SAGE_AppliLog("CIA-A / Timer A is free");
    RemICRVector((struct Library *)ciaabase, CIAICRB_TA, &timerint);
  } else {
    SAGE_AppliLog("CIA-A / Timer A is busy");
  }
  // Check CIA-A / Timer B
  if (!(AddICRVector((struct Library *)ciaabase, CIAICRB_TB, &timerint))) {
    SAGE_AppliLog("CIA-A / Timer B is free");
    RemICRVector((struct Library *)ciaabase, CIAICRB_TB, &timerint);
  } else {
    SAGE_AppliLog("CIA-A / Timer B is busy");
  }
  // Check CIA-B / Timer A
  if (!(AddICRVector((struct Library *)ciabbase, CIAICRB_TA, &timerint))) {
    SAGE_AppliLog("CIA-B / Timer A is free");
    RemICRVector((struct Library *)ciabbase, CIAICRB_TA, &timerint);
  } else {
    SAGE_AppliLog("CIA-B / Timer A is busy");
  }
  // Check CIA-B / Timer B
  if (!(AddICRVector((struct Library *)ciabbase, CIAICRB_TB, &timerint))) {
    SAGE_AppliLog("CIA-B / Timer B is free");
    RemICRVector((struct Library *)ciabbase, CIAICRB_TB, &timerint);
  } else {
    SAGE_AppliLog("CIA-B / Timer B is busy");
  }

  SAGE_AppliLog(" --- ShowTimer ---");
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

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library AUDIO test (MUSIC) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("Timer before init");
  ShowTimer();
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
    SAGE_AppliLog("Timer after init");
    ShowTimer();
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_NOFLAG)) {
      SAGE_AppliLog("Loading modules");
      if ((music1 = SAGE_LoadMusic("/data/bubble.mod")) != NULL
          && (music2 = SAGE_LoadMusic("/data/mando.aiff")) != NULL) {
        SAGE_AppliLog("Adding musics");
        if (!SAGE_AddMusic(MUSIC_BUBBLE, music1)) {
          SAGE_AppliLog("Add bubble module error !");
          SAGE_DisplayError();
          finish = TRUE;
          ok = FALSE;
        }
        if (!SAGE_AddMusic(MUSIC_MANDO, music2)) {
          SAGE_AppliLog("Add mando music error !");
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
        SAGE_AppliLog("Playing runner");
        if (!SAGE_PlayMusic(MUSIC_BUBBLE)) {
          SAGE_DisplayError();
          finish = TRUE;
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
              if (event->code == SKEY_FR_F1) {
                SAGE_AppliLog("Playing bubble");
                SAGE_PlayMusic(MUSIC_BUBBLE);
              }
              if (event->code == SKEY_FR_F2) {
                SAGE_AppliLog("Playing mando");
                SAGE_PlayMusic(MUSIC_MANDO);
              }
              if (event->code == SKEY_FR_F5) {
                SAGE_AppliLog("Pause music");
                SAGE_PauseMusic();
              }
              if (event->code == SKEY_FR_F6) {
                SAGE_AppliLog("Resume music");
                SAGE_ResumeMusic();
              }
            }
          }
        }
      }
      SAGE_AppliLog("Closing music");
      SAGE_StopMusic();
      SAGE_ClearMusic();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
