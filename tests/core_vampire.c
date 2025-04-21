/**
 * core_vampire.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test Vampire presence
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (VAMPIRE) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    if (SAGE_ApolloCore()) {
      SAGE_AppliLog("You have a 68080 inside !");
    } else {
      SAGE_AppliLog("You don't have a 68080.");
    }
    if (SAGE_VampireV4()) {
      SAGE_AppliLog(" and it's a V4 model !");
    } else {
      SAGE_AppliLog(" and it's a V2 model !");
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
