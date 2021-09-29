/**
 * input_joyport.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test input joyport
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>
#include <libraries/lowlevel.h>

#include "/src/sage.h"

void main(void)
{
  SAGE_PortScan port1, port2;
  BOOL finish;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library INPUT test (JOYPORT) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_INPUT)) {
    printf("Get port 0 type = %d\n", SAGE_GetPortType(SINP_JOYPORT1));
    printf("Get port 1 type = %d\n", SAGE_GetPortType(SINP_JOYPORT2));
    finish = FALSE;
    while (!finish) {
      if (SAGE_ScanPort(&port1, SINP_JOYPORT1)) {
        printf(" Port 1 event 0x%X\n", port1.scan);
      }
      if (SAGE_ScanPort(&port2, SINP_JOYPORT2)) {
        printf(" Port 2 event 0x%X\n", port2.scan);
        if (port2.type == SINP_GAMEPAD || port2.type == SINP_JOYSTICK) {
          if (port2.up) {
            printf(" Joystick up\n");
          } else if (port2.down) {
            printf(" Joystick down\n");
          }
          if (port2.left) {
            printf(" Joystick left\n");
          } else if (port2.right) {
            printf(" Joystick right\n");
          }
          if (port2.fire1 || port2.red) {
            printf(" Joystick fire, quit\n");
            finish = TRUE;
          }
        } else {
          printf(" Not a joystick or gamepad !");
        }
      }
      SAGE_Pause(50);
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
