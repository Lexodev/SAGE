/**
 * render3d_3ddevice.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D render device allocation
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          16L

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library 3D test (3DDEVICE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
