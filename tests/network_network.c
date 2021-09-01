/**
 * network_network.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test network module initialization
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

#include <stdio.h>

#include "/src/sage.h"

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library NETWORK test (NETWORK) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NETWORK)) {
    printf("Initialization successfull\n");
  } else {
    printf("Initialization failed\n");
  }
  SAGE_Exit();
  printf("End of test\n");
}
