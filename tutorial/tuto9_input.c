/**
 * tuto9_input.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Gamepad and keyboard
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 August 2024
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          8L

#define PICTURE_WIDTH         640
#define PICTURE_HEIGHT        480

// Controls
#define KEY_NBR               6
#define KEY_LEFT              0
#define KEY_RIGHT             1
#define KEY_UP                2
#define KEY_DOWN              3
#define KEY_FIRE1             4
#define KEY_FIRE2             5

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_CTRL, FALSE }
};

ULONG paddle_colors[8] = {
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
  0x00C31419, 0x0055C3C0, 0x0031A343, 0x00FFBD08
};

void main(void)
{
  SAGE_Event *event = NULL;
  SAGE_Picture *picture = NULL;
  SAGE_PortScan port2;
  BOOL finish, ok = TRUE;

  // Get rid of low level logs
  SAGE_SetLogLevel(SLOG_INFO);
  // Use the AppliLog to log text in the console, this is an unmaskable level of log
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library tutorial 9 : INPUT / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT)) {
    // Open the application screen, screen always use double buffer
    // We use SSCR_STRICTRES option to tell that we don't accept any other resolution
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      // Let's hide the mouse pointer
      SAGE_HideMouse();
      SAGE_AppliLog("Loading the background picture");
      // Load the background picture
      if ((picture = SAGE_LoadPicture("data/paddle.bmp")) != NULL) {
        // Blit the picture to the screen
        if (!SAGE_BlitPictureToScreen(picture, 0, 0, PICTURE_WIDTH, PICTURE_HEIGHT, 0, 0)) {
          SAGE_DisplayError();
        }
        // We have to set the screen colors with the picture colors
        SAGE_LoadPictureColorMap(picture);
        // Release the picture, we don't need it anymore
        SAGE_ReleasePicture(picture);
      } else {
        ok = FALSE;
        SAGE_DisplayError();
      }
      // What is plugged on port 2
      SAGE_AppliLog("Get port 2 type = %d", SAGE_GetPortType(SINP_JOYPORT2));
      // Continue if everything is OK
      if (ok) {
        // To see the colors we have to refresh the screen colors
        SAGE_RefreshColors(0, 256);
        // To see the picture we have to switch screen buffers
        SAGE_RefreshScreen();
        // Let's wait for user to click on a mouse button or press ESC key
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
            port2.fire2 = FALSE;
          }
          // Display on the paddle
          if (keys[KEY_LEFT].key_pressed || port2.left) {
            paddle_colors[0] = 0xFF0000;
          } else {
            paddle_colors[0] = 0xFFFFFF;
          }
          if (keys[KEY_RIGHT].key_pressed || port2.right) {
            paddle_colors[1] = 0xFF0000;
          } else {
            paddle_colors[1] = 0xFFFFFF;
          }
          if (keys[KEY_UP].key_pressed || port2.up) {
            paddle_colors[2] = 0xFF0000;
          } else {
            paddle_colors[2] = 0xFFFFFF;
          }
          if (keys[KEY_DOWN].key_pressed || port2.down) {
            paddle_colors[3] = 0xFF0000;
          } else {
            paddle_colors[3] = 0xFFFFFF;
          }
          // Only paddle for fire buttons
          if (keys[KEY_FIRE1].key_pressed || port2.fire1) {
            paddle_colors[4] = 0xFFFFFF;
          } else {
            paddle_colors[4] = 0xC31419;
          }
          if (keys[KEY_FIRE2].key_pressed || port2.fire2) {
            paddle_colors[5] = 0xFFFFFF;
          } else {
            paddle_colors[5] = 0x55C3C0;
          }
          // Load the colors and refresh
          SAGE_SetColorMap(paddle_colors, 248, 8);
          SAGE_RefreshColors(0, 256);
        }
      } else {
        SAGE_AppliLog("Init error");
        SAGE_DisplayError();
      }
      // Show the mouse
      SAGE_ShowMouse();
      // And close the screen
      SAGE_CloseScreen();
    }
  }
  // Release all resources
  SAGE_Exit();
  // End of tutorial
  SAGE_AppliLog("End of tutorial 9");
}
