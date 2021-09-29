/**
 * blood_control.c
 * 
 * Blood project
 * Control management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#include "/src/sage.h"
#include "blood_control.h"

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_SCAN] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_SPACE, FALSE },
  { SKEY_FR_SHIFT, FALSE },
  { SKEY_FR_CTRL, FALSE },
  { SKEY_FR_ESC, FALSE }
};

VOID ScanKeyboard(VOID)
{
  if (SAGE_ScanKeyboard(keys, KEY_SCAN)) {
    keyboard_state[KEY_UP] = keys[0].key_pressed;
    keyboard_state[KEY_DOWN] = keys[1].key_pressed;
    keyboard_state[KEY_LEFT] = keys[2].key_pressed;
    keyboard_state[KEY_RIGHT] = keys[3].key_pressed;
    keyboard_state[KEY_SPACE] = keys[4].key_pressed;
    keyboard_state[KEY_SHIFT] = keys[5].key_pressed;
    keyboard_state[KEY_CTRL] = keys[6].key_pressed;
    keyboard_state[KEY_QUIT] = keys[7].key_pressed;
  }
}

BOOL InitInput(VOID)
{
  LONG i;
  
  SAGE_AppliLog("Init input");
  for (i = 0;i < KEY_NBR;i++) {
    keyboard_state[i] = 0;
  }
  return TRUE;
}
