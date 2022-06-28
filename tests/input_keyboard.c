/**
 * input_keyboard.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test input keyboard
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#include <proto/dos.h>

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

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library INPUT test (KEYBOARD) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_INPUT)) {
    finish = FALSE;
    while (!finish) {
      if (SAGE_ScanKeyboard(keys, NB_KEYS)) {
        if (keys[0].key_pressed) {
          printf("ESC key pressed\n");
          finish = TRUE;
        }
        if (keys[1].key_pressed) {
          printf("F1 key pressed\n");
        }
        if (keys[2].key_pressed) {
          printf("A key pressed\n");
        }
        if (keys[3].key_pressed) {
          printf("Z key pressed\n");
        }
        if (keys[4].key_pressed) {
          printf("E key pressed\n");
        }
        if (keys[5].key_pressed) {
          printf("Q key pressed\n");
        }
        if (keys[6].key_pressed) {
          printf("S key pressed\n");
        }
        if (keys[7].key_pressed) {
          printf("D key pressed\n");
        }
        if (keys[8].key_pressed) {
          printf("SPACE key pressed\n");
          finish = TRUE;
        }
        if (keys[9].key_pressed) {
          printf("SHIFT key pressed\n");
        }
      } else {
        printf("Keyboard scan error !\n");
      }
      Delay(20);
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
