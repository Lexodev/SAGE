/**
 * core_memory.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test memory management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

void main(void)
{
  APTR bloc1, bloc2, bloc3, bloc4, bloc5;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (MEMORY) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    SAGE_AppliLog("Available memory %d", SAGE_AvailMem());
    SAGE_AppliLog("Allocating public memory 5000");
    bloc1 = SAGE_AllocMem(5000);
    SAGE_AppliLog("Allocating fast memory 15000");
    bloc2 = SAGE_AllocFastMem(15000);
    SAGE_AppliLog("Allocating public memory 8000");
    bloc3 = SAGE_AllocMem(8000);
    SAGE_AppliLog("Allocating chip memory 25000");
    bloc4 = SAGE_AllocChipMem(25000);
    SAGE_AppliLog("Allocating aligned (32) public memory 10500");
    bloc5 = SAGE_AllocAlignMem(10500, 35);
    SAGE_AppliLog("Available memory %d", SAGE_AvailMem());
    SAGE_DumpMemory();
    if (bloc1 && bloc2 && bloc3 && bloc4) {
      SAGE_AppliLog("Allocation successfull");
      SAGE_AppliLog("Removing bloc3");
      SAGE_FreeMem(bloc3);
      SAGE_DumpMemory();
    }
    SAGE_AppliLog("Freeing all blocs");
    SAGE_ReleaseMem();
    SAGE_AppliLog("Available memory %d", SAGE_AvailMem());
    SAGE_DumpMemory();
    SAGE_AppliLog("End of test");
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
