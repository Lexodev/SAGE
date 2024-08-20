/**
 * tuto8_audio.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Sound and music
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 August 2024
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define MUSIC_SLOT            1
#define SOUND_SLOT            1

void main(void)
{
  SAGE_Event *event = NULL;
  SAGE_Music *music = NULL;
  SAGE_Sound *sound = NULL;
  BOOL finish, ok = TRUE;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 8 : AUDIO / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO|SMOD_AUDIO)) {
    // Open the application screen
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_NOFLAG)) {
      // Let's hide the mouse pointer
      SAGE_HideMouse();
      // Load the music
      SAGE_AppliLog("Load the music");
      if ((music = SAGE_LoadMusic("data/music.mod")) != NULL) {
        SAGE_AppliLog("Adding music");
        if (!SAGE_AddMusic(MUSIC_SLOT, music)) {
          ok = FALSE;
          SAGE_DisplayError();
        }
      } else {
        ok = FALSE;
        SAGE_DisplayError();
      }
      // Load the sound
      SAGE_AppliLog("Load the sound");
      if ((sound = SAGE_LoadSound("data/laser.wav")) != NULL) {
        SAGE_AppliLog("Sound volumes %d", sound->volume);
        if (!SAGE_AddSound(SOUND_SLOT, sound)) {
          ok = FALSE;
          SAGE_DisplayError();
        }
      } else {
        ok = FALSE;
        SAGE_DisplayError();
      }
      // Continue if everything is OK
      if (ok) {
        finish = FALSE;
        SAGE_PrintText("PRESS F1 FOR MUSIC PAUSE", 20, 20);
        SAGE_PrintText("      F2 FOR MUSIC RESUME", 20, 40);
        SAGE_PrintText("      F3 FOR SOUND PLAY", 20, 60);
        SAGE_PrintText("      MOUSE OR ESC FOR EXIT", 20, 80);
        SAGE_RefreshScreen();
        // Start the music
        SAGE_AppliLog("Start music");
        SAGE_PlayMusic(MUSIC_SLOT);
        while (!finish) {
          // Read all events raised by the screen
          while ((event = SAGE_GetEvent()) != NULL) {
            // Display some info about the event
            SAGE_AppliLog("Event polled type %d, code %d, mouse %d,%d", event->type, event->code, event->mousex, event->mousey);
            if (event->type == SEVT_MOUSEBT) {
              // If we click on mouse button, we stop the loop
              finish = TRUE;
            } else if (event->type == SEVT_RAWKEY) {
              if (event->code == SKEY_FR_ESC) {
                // If we press the ESC key, we stop the loop
                finish = TRUE;
              } else if (event->code == SKEY_FR_F1) {
                SAGE_AppliLog("Pause music");
                SAGE_PauseMusic();
              } else if (event->code == SKEY_FR_F2) {
                SAGE_AppliLog("Resume music");
                SAGE_ResumeMusic();
              } else if (event->code == SKEY_FR_F3) {
                SAGE_AppliLog("Play sound on channel 4");
                SAGE_PlaySound(SOUND_SLOT, SAUD_CHANNEL4);
              }
            }
          }
        }
      } else {
        SAGE_AppliLog("Init error");
        SAGE_DisplayError();
      }
      // Release resources
      SAGE_AppliLog("Clear all sounds");
      SAGE_ClearSound();
      SAGE_AppliLog("Stop & clear music");
      SAGE_StopMusic();
      SAGE_ClearMusic();
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    }
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 8");
}
