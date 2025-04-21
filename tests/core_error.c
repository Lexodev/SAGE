/**
 * core_error.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test error display
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

#define CORE_ERROR_MAX    200

void main(void)
{
  LONG i;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (ERROR) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    for (i = 0;i < CORE_ERROR_MAX;i++) {
      SAGE_AppliLog("Setting error %d", i);
      SAGE_SetError(i);
      SAGE_AppliLog("Error code is => %d <= and text is => %s <=", SAGE_GetErrorCode(), SAGE_GetErrorString());
      SAGE_AppliLog("Display the error");
      SAGE_DisplayError();
      SAGE_AppliLog("----");
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
