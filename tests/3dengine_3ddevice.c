/**
 * 3dengine_3ddevice.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D Engine device allocation
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include <proto/dos.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library 3DENGINE test (3DDEVICE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3DENGINE) && SAGE_AllocVideoDevice()) {
    printf("Initialization successfull\n");
    printf("Try to allocate 3D device without screen\n");
    if (!SAGE_Alloc3DDevice()) {
      SAGE_DisplayError();
    }
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      printf("Try to allocate 3D device with screen\n");
      if (SAGE_Alloc3DDevice()) {
        printf("3D device allocated\n");
        Delay(50*2);
      } else {
        SAGE_DisplayError();
      }
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    printf("Initialization failed\n");
  }
  SAGE_Free3DDevice();
  SAGE_FreeVideoDevice();
  SAGE_Exit();
  printf("End of test\n");
}
