/**
 * video_video.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test video module initialization
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

void main(void)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (VIDEO) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Initialization successfull\n");
  } else {
    printf("Initialization failed\n");
  }
  SAGE_Exit();
  printf("End of test\n");
}
