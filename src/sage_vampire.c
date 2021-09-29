/**
 * sage_vampire.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Vampire & AMMX support
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#include <exec/execbase.h>
#include <exec/types.h>

#include "sage_debug.h"
#include "ext/vampire.h"
#include "sage_vampire.h"

/** @var Intuition library */
extern struct ExecBase * SysBase;

/** @var Vampire library */
struct Library * VampireBase;

/**
 * Check if Apollo core is available
 *
 * @return Apollo core is available
 */
BOOL SAGE_ApolloPresence()
{
  if (SysBase->AttnFlags & AFF_68080) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Check for AMMX2 availability
 *
 * @return AMMX2 is available
 */
BOOL SAGE_AMMX2Available()
{
  /*if (!(VampireBase = OpenResource(V_VAMPIRENAME))) {
    return FALSE;
  }
  if (VampireBase->lib_Version >= 45) {
    if (V_EnableAMMX(V_AMMX_V2) != VRES_ERROR) {
      return TRUE;
    }
  }*/
  // Let's rely on the case that if we have a 68080 we have AMMX2
  if (SysBase->AttnFlags & AFF_68080) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the Vampire frame buffer address
 *
 * @param buffer Frame buffer address
 *
 * @return Operation success
 */
BOOL SAGE_SetVampireFrameBuffer(APTR buffer)
{
  ULONG * vampire_fb = (ULONG *) SAGA_FBADDR;

  *vampire_fb = (ULONG) buffer;
  return TRUE;
}

/**
 * Get the Vampire frame buffer address
 *
 * @return Frame buffer address
 */
APTR SAGE_GetVampireFrameBuffer()
{
  ULONG * vampire_fb = (ULONG *) SAGA_FBADDR;

  return (APTR) *vampire_fb;
}
