/**
 * render3d_3dtexture.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D texture management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          16L

#define TEX_VAMPIRE           1

void main(void)
{
  SAGE_Picture * picture = NULL;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library 3D test (3DTEXTURE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_AppliLog("Load texture picture");
      if ((picture = SAGE_LoadPicture("/data/vampire.bmp")) != NULL) {
        SAGE_AppliLog("Create texture from picture");
        if (SAGE_CreateTextureFromPicture(TEX_VAMPIRE, 0, 0, STEX_SIZE256, picture)) {
          SAGE_AppliLog("Add texture to card memory");
          if (SAGE_AddTexture(TEX_VAMPIRE)) {
            SAGE_AppliLog("All done !");
          } else {
            SAGE_DisplayError();
          }
          SAGE_ReleaseTexture(TEX_VAMPIRE);
        } else {
          SAGE_DisplayError();
        }
        SAGE_ReleasePicture(picture);
      } else {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Load DDS texture");
      if (SAGE_CreateTextureFromFile(TEX_VAMPIRE, "/data/testtex.dds")) {
        SAGE_AppliLog("OK");
        SAGE_ReleaseTexture(TEX_VAMPIRE);
      } else {
        SAGE_DisplayError();
      }
      SAGE_Pause(50);
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
