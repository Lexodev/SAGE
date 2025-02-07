/**
 * sage_vampire.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Vampire & AMMX support
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <exec/execbase.h>
#include <exec/types.h>

#include <stdio.h>

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_vampire.h>

/** @var Intuition library */
extern struct ExecBase *SysBase;

/**
 * Check if Apollo core is available
 *
 * @return Apollo core is available
 */
BOOL SAGE_ApolloCore()
{
  if (SysBase->AttnFlags & AFF_68080) {
    SD(SAGE_DebugLog("Core 68080 detected");)
    return TRUE;
  }
  return FALSE;
}

/**
 * Check for Vampire V4 card
 *
 * @return V4 is available
 */
BOOL SAGE_VampireV4()
{
  volatile UWORD *card_version = (UWORD *)SAPO_VCARD;
  UWORD vampire;

  if (SAGE_ApolloCore()) {
    vampire = *card_version;
    SD(SAGE_DebugLog("Vampire card version is 0x%X", vampire);)
    vampire >>= 8;
    vampire &= 0xff;
    if (vampire == SAPO_V4500 || vampire == SAPO_V4600 || vampire == SAPO_V41200 || vampire == SAPO_V4SA) {
      SD(SAGE_DebugLog("Vampire V4 card detected");)
      return TRUE;
    }
  }
#if _SAGE_DEBUG_MODE_ == 1
  SD(SAGE_DebugLog("Simulate Vampire V4 card");)
  return TRUE;
#else
  return FALSE;
#endif
}

