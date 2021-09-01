/**
 * 3dengine_3dengine.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test 3D Engine module initialization
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

void main(void)
{
  W3D_Context * context;
  W3D_Triangle triangle;
  
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library 3DENGINE test (3DENGINE) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_3DENGINE)) {
    printf("Initialization successfull\n");
    context = SAGE_Get3DContext();
    if (context != NULL) {
      
      /*if (W3D_LockHardware(context) == W3D_SUCCESS) {
        W3D_DrawTriangle(context, &triangle);
        W3D_UnLockHardware(context);
      }*/

    } else {
      printf("Issue with context !\n");
    }
  } else {
    printf("Initialization failed\n");
  }
  SAGE_Exit();
  printf("End of test\n");
}
