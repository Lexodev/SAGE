/**
 * render3d_3deload.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test 3D entity load
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define SCREEN_WIDTH          640L
#define SCREEN_HEIGHT         480L
#define SCREEN_DEPTH          16L

#define MAIN_CAMERA           1
#define HOUSE_ENTITY          1

void main(void)
{
  SAGE_Event * event = NULL;
  BOOL finish = FALSE;
  SAGE_Entity * entity;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("    SAGE library 3D test (3DELOAD) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO|SMOD_3D)) {
    SAGE_AppliLog("Initialization successfull");
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();

      SAGE_Init3DEngine();
      SAGE_AddCamera(MAIN_CAMERA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      if ((entity = SAGE_LoadEntity("/data/house.obj")) != NULL && SAGE_AddEntity(HOUSE_ENTITY, entity)) {
        SAGE_SetActiveCamera(MAIN_CAMERA);
        SAGE_SetCameraAngle(MAIN_CAMERA, 0, 0, 0);
        SAGE_SetCameraPlane(MAIN_CAMERA, (FLOAT)10.0, (FLOAT)1000.0);
        SAGE_SetEntityPosition(HOUSE_ENTITY, (FLOAT)0.0, (FLOAT)0.0, (FLOAT)50.0);

        SAGE_AppliLog("Available memory %d KB", SAGE_AvailMem());
        SAGE_AppliLog("Entering main loop");
        while (!finish) {
          SAGE_EngineDebug(FALSE);
          while ((event = SAGE_GetEvent()) != NULL) {
            if (event->type == SEVT_RAWKEY) {
              switch (event->code) {
                case SKEY_FR_ESC:
                  SAGE_AppliLog("Exit loop");
                  finish = TRUE;
                  break;
                case SKEY_FR_D:
                  SAGE_AppliLog("Activate DEBUG mode");
                  SAGE_EngineDebug(TRUE);
                  break;
              }
            }
          }
          SAGE_ClearScreen();
          SAGE_RotateEntity(HOUSE_ENTITY, S3DE_ONEDEGREE, S3DE_ONEDEGREE, S3DE_ONEDEGREE);
          SAGE_RenderWorld();
          if (!SAGE_RefreshScreen()) {
            SAGE_ErrorLog("Error RefreshScreen !");
            SAGE_DisplayError();
            finish = TRUE;
          }
        }

        SAGE_AppliLog("All done !");
      }
      SAGE_Release3DEngine();

      SAGE_ShowMouse();
      SAGE_CloseScreen();
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
