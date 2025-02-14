/**
 * core_vampire.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test Vampire presence
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
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
      if (SAGE_VampireV4()) {
        SAGE_AppliLog(" and it's a V4 model !");
      } else {
        SAGE_AppliLog(" and it's a V2 model !");
      }
    } else {
      SAGE_AppliLog("You don't have a 68080.");
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
