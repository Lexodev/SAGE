/**
 * sage.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#include <exec/exec.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "sage_debug.h"
#include "sage_context.h"
#include "sage.h"

/** @var Library version */
STRPTR SAGE_Version = "$VER: SAGE V1.8 September 2021";

/** @var Application context */
SAGE_Context SageContext;

/**
 * Initialize SAGE modules, open libs
 *
 * @param modules Modules to activate
 * 
 * @return Operation success
 */
BOOL SAGE_Init(LONG modules)
{
  UWORD index;

  SAGE_InfoLog("SAGE Init : %s", SAGE_GetVersion());
  SAGE_DebugLog("Available memory %d KB", SAGE_AvailMem());
  // The only time where we should set the error to no error
  SAGE_SetError(SERR_NO_ERROR);
  // Init the modules
  SageContext.LoadedModules = SMOD_NONE;
  SageContext.AmmxReady = SAGE_AMMX2Available();
  SageContext.SageVideo = NULL;
  SageContext.SageAudio = NULL;
  SageContext.SageInput = NULL;
  SageContext.SageNetwork = NULL;
  SageContext.Sage3D = NULL;
  for (index = 0;index < STHD_MAX_THREAD;index++) {
    SageContext.Threads[index] = NULL;
  }
  if (modules & SMOD_VIDEO) {
    if (!SAGE_InitVideoModule()) {
      return FALSE;
    }
    SageContext.LoadedModules |= SMOD_VIDEO;
  }
  if (modules & SMOD_AUDIO) {
    if (!SAGE_InitAudioModule()) {
      return FALSE;
    }
    SageContext.LoadedModules |= SMOD_AUDIO;
  }
  if (modules & SMOD_INPUT) {
    if (!SAGE_InitInputModule()) {
      return FALSE;
    }
    SageContext.LoadedModules |= SMOD_INPUT;
  }
  if (modules & SMOD_INTERRUPTION) {
    if (!SAGE_InitInterruptionModule()) {
      return FALSE;
    }
    SageContext.LoadedModules |= SMOD_INTERRUPTION;
  }
  if (modules & SMOD_3DENGINE) {
    if (!SAGE_Init3DModule()) {
      return FALSE;
    }
    SageContext.LoadedModules |= SMOD_3DENGINE;
  }
  if (modules & SMOD_NETWORK) {
    if (!SAGE_InitNetworkModule()) {
      return FALSE;
    }
    SageContext.LoadedModules |= SMOD_NETWORK;
  }
  SAGE_DebugLog("Loaded modules %d", SageContext.LoadedModules);
  return TRUE;
}

/**
 * Release all SAGE resources
 * 
 * @return Operation success
 */
BOOL SAGE_Exit(VOID)
{
  UWORD index;
  
  SAGE_InfoLog("SAGE Exit");
  for (index = 0;index < STHD_MAX_THREAD;index++) {
    if (SageContext.Threads[index] != NULL) {
      SAGE_RemoveThread(SageContext.Threads[index]);
    }
  }
  if (SageContext.LoadedModules & SMOD_NETWORK) {
    SAGE_ReleaseNetworkModule();
  }
  if (SageContext.LoadedModules & SMOD_3DENGINE) {
    SAGE_Release3DModule();
  }
  if (SageContext.LoadedModules & SMOD_INTERRUPTION) {
    SAGE_ReleaseInterruptionModule();
  }
  if (SageContext.LoadedModules & SMOD_INPUT) {
    SAGE_ReleaseInputModule();
  }
  if (SageContext.LoadedModules & SMOD_AUDIO) {
    SAGE_ReleaseAudioModule();
  }
  if (SageContext.LoadedModules & SMOD_VIDEO) {
    SAGE_ReleaseVideoModule();
  }
  SageContext.LoadedModules = SMOD_NONE;
  // Finally clean memory
  SD(SAGE_DumpMemory());
  SAGE_ReleaseMem();  // Free all remaining memory
  SAGE_DebugLog("Available memory %d KB", SAGE_AvailMem());
  return TRUE;
}

/**
 * Tell if a module has been loaded
 * 
 * @param module Module ID
 * 
 * @return Module is loaded
 */
BOOL SAGE_LoadedModule(LONG module)
{
  if (SageContext.LoadedModules & module) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Get the libray version
 *
 * @return Library version
 */
STRPTR SAGE_GetVersion(VOID)
{
  return SAGE_Version;
}

/**
 * Enable/disable AMMX use if available
 * 
 * @param ammx Use AMMX
 */
VOID SAGE_UseAMMX(BOOL ammx)
{
  if (ammx && SAGE_AMMX2Available()) {
    SageContext.AmmxReady = TRUE;
  } else {
    SageContext.AmmxReady = FALSE;
  }
}

/**
 * Pause the program
 *
 * @param ticks Number of 1/50 of seconds
 */
VOID SAGE_Pause(ULONG ticks)
{
  Delay(ticks);
}
