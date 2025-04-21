/**
 * input_joyport.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test input joyport
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <libraries/lowlevel.h>

#include <sage/sage.h>

void main(void)
{
  SAGE_PortScan port1, port2;
  BOOL finish = FALSE;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library INPUT test (JOYPORT) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_INPUT)) {
    SAGE_AppliLog("Get port 1 type = %d", SAGE_GetPortType(SINP_JOYPORT1));
    SAGE_AppliLog("Get port 2 type = %d", SAGE_GetPortType(SINP_JOYPORT2));
    while (!finish) {
      if (SAGE_ScanPort(&port1, SINP_JOYPORT1)) {
        SAGE_AppliLog(" Port 1 event 0x%X", port1.scan);
      }
      if (SAGE_ScanPort(&port2, SINP_JOYPORT2)) {
        SAGE_AppliLog(" Port 2 event 0x%X", port2.scan);
        if (port2.type == SINP_GAMEPAD || port2.type == SINP_JOYSTICK) {
          if (port2.up) {
            SAGE_AppliLog(" Joystick up");
          } else if (port2.down) {
            SAGE_AppliLog(" Joystick down");
          }
          if (port2.left) {
            SAGE_AppliLog(" Joystick left");
          } else if (port2.right) {
            SAGE_AppliLog(" Joystick right");
          }
          if (port2.fire1 || port2.red) {
            SAGE_AppliLog(" Joystick fire, quit");
            finish = TRUE;
          }
        } else {
          SAGE_AppliLog(" Not a joystick or gamepad !");
        }
      }
      SAGE_Pause(50);
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
