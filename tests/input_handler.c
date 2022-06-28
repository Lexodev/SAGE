/**
 * input_handler.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test input handler
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

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
    printf("BOOM !\n");
  }
  if (scan->fire2) {
    printf("Goodbye !\n");
    finish = TRUE;
  }
}

void what_joyport(UWORD port)
{
  switch (SAGE_GetPortType(port)) {
    case SINP_NOTAVAIL:
      printf("Port %d is not available\n", port);
      break;
    case SINP_MOUSE:
      printf("Port %d is a mouse\n", port);
      break;
    case SINP_JOYSTICK:
      printf("Port %d is a joystick\n", port);
      break;
    case SINP_GAMEPAD:
      printf("Port %d is a gamepad\n", port);
      break;
    case SINP_UNKNOWN:
      printf("Port %d is unknown\n", port);
      break;
    default:
      printf("You should not be here !\n");
  }
}

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library INPUT test (HANDLER) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_INPUT)) {
    printf("Add keyboard handlers\n");
    SAGE_AddKeyboardHandler(SKEY_FR_ESC, key_esc);
    SAGE_AddKeyboardHandler(SKEY_FR_Z, key_z);
    SAGE_AddKeyboardHandler(SKEY_FR_Q, key_q);
    SAGE_AddKeyboardHandler(SKEY_FR_S, key_s);
    SAGE_AddKeyboardHandler(SKEY_FR_D, key_d);
    printf("Install the handlers\n");
    SAGE_InstallKeyboardHandlers();
    printf("Add joyport handlers\n");
    what_joyport(SINP_JOYPORT1);
    what_joyport(SINP_JOYPORT2);
    what_joyport(SINP_JOYPORT3);
    what_joyport(SINP_JOYPORT4);
    SAGE_AddJoyportHandler(SINP_JOYPORT2, joy_1);
    finish = FALSE;
    printf("Start the main loop with x=%d and y=%d\n", x_pos, y_pos);
    while (!finish) {
      SAGE_HandleInputEvents();
      printf("Pos x=%d and y=%d\n", x_pos, y_pos);
      SAGE_Pause(50);
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
