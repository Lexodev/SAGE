/**
 * core_vampire.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test Vampire presence
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library CORE test (VAMPIRE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NONE)) {
    if (SAGE_ApolloPresence()) {
      printf("You have a Vampire inside !\n");
    } else {
      printf("You don't have a Vampire.\n");
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
