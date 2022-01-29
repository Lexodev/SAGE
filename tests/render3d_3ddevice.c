/**
 * render3d_3ddevice.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D render device allocation
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 December 2021
 */

#include <stdio.h>

#include <proto/dos.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          16L

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library 3D test (3DDEVICE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    printf("Initialization successfull\n");
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
