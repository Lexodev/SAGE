/**
 * input_keyboard.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * SAGE_AppliLog input keyboard
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define NB_KEYS     10

void main(void)
{
  SAGE_KeyScan keys[NB_KEYS] = {
    { SKEY_FR_ESC, FALSE },
    { SKEY_FR_F1, FALSE },
    { SKEY_FR_A, FALSE },
    { SKEY_FR_Z, FALSE },
    { SKEY_FR_E, FALSE },
    { SKEY_FR_Q, FALSE },
    { SKEY_FR_S, FALSE },
    { SKEY_FR_D, FALSE },
    { SKEY_FR_SPACE, FALSE },
    { SKEY_FR_SHIFT, FALSE }
  };
  BOOL finish;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library INPUT SAGE_AppliLog (KEYBOARD) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_INPUT)) {
    finish = FALSE;
    while (!finish) {
      if (SAGE_ScanKeyboard(keys, NB_KEYS)) {
        if (keys[0].key_pressed) {
          SAGE_AppliLog("ESC key pressed");
          finish = TRUE;
        }
        if (keys[1].key_pressed) {
          SAGE_AppliLog("F1 key pressed");
        }
        if (keys[2].key_pressed) {
          SAGE_AppliLog("A key pressed");
        }
        if (keys[3].key_pressed) {
          SAGE_AppliLog("Z key pressed");
        }
        if (keys[4].key_pressed) {
          SAGE_AppliLog("E key pressed");
        }
        if (keys[5].key_pressed) {
          SAGE_AppliLog("Q key pressed");
        }
        if (keys[6].key_pressed) {
          SAGE_AppliLog("S key pressed");
        }
        if (keys[7].key_pressed) {
          SAGE_AppliLog("D key pressed");
        }
        if (keys[8].key_pressed) {
          SAGE_AppliLog("SPACE key pressed");
          finish = TRUE;
        }
        if (keys[9].key_pressed) {
          SAGE_AppliLog("SHIFT key pressed");
        }
      } else {
        SAGE_AppliLog("Keyboard scan error !");
      }
      SAGE_Pause(20);
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of SAGE_AppliLog");
}
