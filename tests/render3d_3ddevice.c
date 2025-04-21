/**
 * render3d_3ddevice.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D render device allocation
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 26/02/2025)
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
  if (SAGE_Init(SMOD_VIDEO|SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_AppliLog("Current render system is %d", SAGE_Get3DRenderSystem());
      SAGE_Pause(50);
      SAGE_AppliLog("Switch to Warp3D");
      SAGE_Set3DRenderSystem(S3DD_W3DRENDER);
      SAGE_AppliLog("Current render system is %d", SAGE_Get3DRenderSystem());
      SAGE_Pause(50);
      SAGE_AppliLog("Switch to Maggie3D");
      SAGE_Set3DRenderSystem(S3DD_M3DRENDER);
      SAGE_AppliLog("Current render system is %d", SAGE_Get3DRenderSystem());
      SAGE_Pause(50);
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
