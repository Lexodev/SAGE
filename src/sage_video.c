/**
 * sage_video.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Video module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

/** @todo : add methods for queying video card and modes */

#include <exec/execbase.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuitionbase.h>

#include <proto/exec.h>
#include <proto/picasso96.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_vampire.h"
#include "sage_video.h"
#include "sage_context.h"

#define INTUITIONVERSION      39L
#define CYBERGFXVERSION       41L
#define DATATYPEVERSION       0L

/** @var Intuition library */
struct IntuitionBase * IntuitionBase = NULL;

/** @var CybergraphX library */
struct Library * CyberGfxBase = NULL;

/** @var Datatypes library */
struct Library * DataTypesBase = NULL;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Initialize video module
 * Open librairies
 *
 * @return Operation success
 */
BOOL SAGE_InitVideoModule()
{
  SD(SAGE_DebugLog("Init Video module"));
  if ((IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", INTUITIONVERSION)) == NULL) {
    SAGE_SetError(SERR_INTUITION_LIB);
    return FALSE;
  }
  if ((CyberGfxBase = OpenLibrary(CYBERGFXNAME, CYBERGFXVERSION)) == NULL) {
    SAGE_SetError(SERR_CYBERGFX_LIB);
    return FALSE;
  }
  if ((DataTypesBase = OpenLibrary("datatypes.library", DATATYPEVERSION)) == NULL) {
    SAGE_SetError(SERR_DATATYPES_LIB);
    return FALSE;
  }
  if (!SAGE_AllocVideoDevice()) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Release video module
 * Close librairies
 *
 * @return Operation success
 */
BOOL SAGE_ReleaseVideoModule()
{
  SD(SAGE_DebugLog("Release Video module"));
  if (SageContext.SageVideo != NULL) {
    SAGE_FreeVideoDevice();
  }
  if (DataTypesBase != NULL) {
    CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;
  }
  if (CyberGfxBase != NULL) {
    CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
  }
  if (IntuitionBase != NULL) {
    CloseLibrary(&IntuitionBase->LibNode);
    IntuitionBase = NULL;
  }
  return TRUE;
}

/**
 * Allocate video device
 *
 * @return Operation success
 */
BOOL SAGE_AllocVideoDevice()
{
  SAGE_VideoDevice * video;

  SD(SAGE_InfoLog("Allocate video device"));
  if ((video = SAGE_AllocMem(sizeof(SAGE_VideoDevice))) == NULL) {
    return FALSE;
  }
  video->video_modes = NULL;
  if (SAGE_ApolloPresence()) {
    video->SAGAReady = TRUE;
  } else {
    video->SAGAReady = FALSE;
  }
  SageContext.SageVideo = video;
  return TRUE;
}

/**
 * Release video device
 *
 * @return Operation success
 */
BOOL SAGE_FreeVideoDevice()
{
  SAGE_VideoDevice * video;
  UWORD index;
  
  SD(SAGE_InfoLog("Release video device"));
  video = SageContext.SageVideo;
  if (video == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  // Release video modes
  if (video->video_modes) {
    // Not used
  }
  // Release all layers
  for (index = 0;index < SLAY_MAX_LAYERS;index++) {
    if (video->layers[index] != NULL) {
      SAGE_ReleaseLayer(index);
      video->layers[index] = NULL;
    }
  }
  // Release all sprite banks
  for (index = 0;index < SSPR_MAX_SPRBANK;index++) {
    if (video->sprites[index] != NULL) {
      SAGE_ReleaseSpriteBank(index);
      video->sprites[index] = NULL;
    }
  }
  // Release all tile banks
  for (index = 0;index < STIL_MAX_TILEBANK;index++) {
    if (video->tiles[index] != NULL) {
      SAGE_ReleaseTileBank(index);
      video->tiles[index] = NULL;
    }
  }
  // Release all tile maps
  for (index = 0;index < STIL_MAX_TILEMAP;index++) {
    if (video->tilemaps[index] != NULL) {
      SAGE_ReleaseTileMap(index);
      video->tilemaps[index] = NULL;
    }
  }
  // Finally close screen
  if (video->screen != NULL) {
    if (!SAGE_CloseScreen()) {
      return FALSE;
    }
  }
  SAGE_FreeMem(video);
  SageContext.SageVideo = NULL;
  return TRUE;
}

/**
 * Dump available RTG vide modes
 */
VOID SAGE_DumpVideoModes()
{
  SAGE_VideoDevice * video;

  SAGE_WarningLog("Not available !");
}
