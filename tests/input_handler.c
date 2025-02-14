/**
 * input_handler.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test input handler
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define NB_KEYS     10

BOOL finish = TRUE;
WORD x_pos = 0, y_pos = 0;

void key_esc(BOOL pressed)
{
  if (pressed) {
    finish = TRUE;
  }
}

void key_z(BOOL pressed)
{
  if (pressed) {
    y_pos++;
  }
}

void key_q(BOOL pressed)
{
  if (pressed) {
    x_pos--;
  }
}

void key_s(BOOL pressed)
{
  if (pressed) {
    x_pos++;
  }
}

void key_d(BOOL pressed)
{
  if (pressed) {
    y_pos--;
  }
}

void joy_1(SAGE_PortScan * scan)
{
  if (scan->fire1) {
    SAGE_AppliLog("BOOM !");
  }
  if (scan->fire2) {
    SAGE_AppliLog("Goodbye !");
    finish = TRUE;
  }
}

void what_joyport(UWORD port)
{
  switch (SAGE_GetPortType(port)) {
    case SINP_NOTAVAIL:
      SAGE_AppliLog("Port %d is not available", port);
      break;
    case SINP_MOUSE:
      SAGE_AppliLog("Port %d is a mouse", port);
      break;
    case SINP_JOYSTICK:
      SAGE_AppliLog("Port %d is a joystick", port);
      break;
    case SINP_GAMEPAD:
      SAGE_AppliLog("Port %d is a gamepad", port);
      break;
    case SINP_UNKNOWN:
      SAGE_AppliLog("Port %d is unknown", port);
      break;
    default:
      SAGE_AppliLog("You should not be here !");
  }
}

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library INPUT test (HANDLER) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_INPUT)) {
    SAGE_AppliLog("Add keyboard handlers");
    SAGE_AddKeyboardHandler(SKEY_FR_ESC, key_esc);
    SAGE_AddKeyboardHandler(SKEY_FR_Z, key_z);
    SAGE_AddKeyboardHandler(SKEY_FR_Q, key_q);
    SAGE_AddKeyboardHandler(SKEY_FR_S, key_s);
    SAGE_AddKeyboardHandler(SKEY_FR_D, key_d);
    SAGE_AppliLog("Install the handlers");
    SAGE_InstallKeyboardHandlers();
    SAGE_AppliLog("Add joyport handlers");
    what_joyport(SINP_JOYPORT1);
    what_joyport(SINP_JOYPORT2);
    what_joyport(SINP_JOYPORT3);
    what_joyport(SINP_JOYPORT4);
    SAGE_AddJoyportHandler(SINP_JOYPORT2, joy_1);
    finish = FALSE;
    SAGE_AppliLog("Start the main loop with x=%d and y=%d", x_pos, y_pos);
    while (!finish) {
      SAGE_HandleInputEvents();
      SAGE_AppliLog("Pos x=%d and y=%d", x_pos, y_pos);
      SAGE_Pause(50);
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
