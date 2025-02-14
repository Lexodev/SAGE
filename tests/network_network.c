/**
 * network_network.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test network module initialization
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library NETWORK test (NETWORK) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NETWORK)) {
    SAGE_AppliLog("Initialization successfull");
  } else {
    SAGE_AppliLog("Initialization failed");
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
