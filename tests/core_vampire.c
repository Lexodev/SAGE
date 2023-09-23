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
    if (SAGE_ApolloCore()) {
      printf("You have a 68080 inside !\n");
      if (SAGE_VampireV4()) {
        printf(" and it's a V4 model !\n");
      } else {
        printf(" and it's a V2 model !\n");
      }
    } else {
      printf("You don't have a 68080.\n");
    }
  }
  SAGE_Exit();
  printf("End of test\n");
}
