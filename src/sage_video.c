/**
 * sage_video.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Video module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

/** @todo : add methods for queying video card and modes */

#include <exec/execbase.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuitionbase.h>

#include <proto/exec.h>
#include <proto/cybergraphics.h>

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_vampire.h>
#include <sage/sage_video.h>
#include <sage/sage_context.h>

#define INTUITIONVERSION      39L
#define CYBERGFXVERSION       41L
#define DATATYPEVERSION       0L

/** @var Intuition library */
struct IntuitionBase *IntuitionBase = NULL;

/** @var CybergraphX library */
struct Library *CyberGfxBase = NULL;

/** @var Datatypes library */
struct Library *DataTypesBase = NULL;

/** SAGE context */
extern SAGE_Context SageContext;

/*****************************************************************************
 *                   START DEBUG
 *****************************************************************************/

#if _SAGE_DEBUG_MODE_ == 1
/**
 * Dump available RTG video modes
 */
VOID SAGE_DumpVideoModes()
{
  SAGE_VideoModeList *video_modes;
  UWORD cgx_mode;

  video_modes = SAGE_GetVideoModes();
  if (video_modes == NULL) {
    SAGE_ErrorLog("No video mode available");
    return;
  }
  SAGE_DebugLog("Total available modes %d", video_modes->nb_modes);
  for (cgx_mode = 0; cgx_mode < video_modes->nb_modes;cgx_mode++) {
    SAGE_DebugLog(
      "Video mode 0x%X is %s (%dx%dx%d)",
      video_modes->modes[cgx_mode].mode_id,
      video_modes->modes[cgx_mode].name,
      video_modes->modes[cgx_mode].width,
      video_modes->modes[cgx_mode].height,
      video_modes->modes[cgx_mode].depth
    );
  }
}
#endif

/*****************************************************************************
 *                   END DEBUG
 *****************************************************************************/

/**
 * Initialize video module
 * Open librairies
 *
 * @return Operation success
 */
BOOL SAGE_InitVideoModule()
{
  SD(SAGE_DebugLog("Init Video module");)
  if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", INTUITIONVERSION)) == NULL) {
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
  SD(SAGE_DebugLog("Release Video module");)
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
 * List available video modes
 *
 * @param video Video device
 *
 * @return Operation success
 */
BOOL SAGE_AllocVideoModes(SAGE_VideoDevice *video)
{
  UWORD nb_mode;
  struct CyberModeNode *cgx_node;
  struct List *video_modes;

  SD(SAGE_DebugLog("Gather video modes"));
  video_modes = (struct List *)AllocCModeListTagList(NULL);
  if (video_modes == NULL) {
    SAGE_SetError(SERR_NO_MODE);
    return FALSE;
  }
  nb_mode = 0;
  for (cgx_node = (struct CyberModeNode *)video_modes->lh_Head; cgx_node->Node.ln_Succ;cgx_node = (struct CyberModeNode *)cgx_node->Node.ln_Succ) {
    nb_mode++;
  }
  if ((video->video_modes.modes = SAGE_AllocMem(sizeof(SAGE_VideoMode) * nb_mode)) == NULL) {
    FreeCModeList(video_modes);
    return FALSE;
  }
  video->video_modes.nb_modes = nb_mode;
  nb_mode = 0;
  for (cgx_node = (struct CyberModeNode *)video_modes->lh_Head; cgx_node->Node.ln_Succ;cgx_node = (struct CyberModeNode *)cgx_node->Node.ln_Succ) {
    video->video_modes.modes[nb_mode].mode_id = cgx_node->DisplayID;
    strcpy(video->video_modes.modes[nb_mode].name, cgx_node->ModeText);
    video->video_modes.modes[nb_mode].width = cgx_node->Width;
    video->video_modes.modes[nb_mode].height = cgx_node->Height;
    video->video_modes.modes[nb_mode].depth = cgx_node->Depth;
    nb_mode++;
  }
  FreeCModeList(video_modes);
}

/**
 * Allocate video device
 *
 * @return Operation success
 */
BOOL SAGE_AllocVideoDevice()
{
  SAGE_VideoDevice *video;

  SD(SAGE_DebugLog("Allocate video device");)
  if ((video = SAGE_AllocMem(sizeof(SAGE_VideoDevice))) == NULL) {
    return FALSE;
  }
  if (SAGE_VampireV4()) {
    video->SAGAReady = TRUE;
  } else {
    video->SAGAReady = FALSE;
  }
  SAGE_AllocVideoModes(video);
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
  SAGE_VideoDevice *video;
  UWORD index;
  
  SD(SAGE_DebugLog("Release video device");)
  video = SageContext.SageVideo;
  if (video == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  // Release video modes
  if (video->video_modes.modes != NULL) {
    SAGE_FreeMem(video->video_modes.modes);
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
 * Get available video modes
 *
 * @return Available video modes
 */
SAGE_VideoModeList *SAGE_GetVideoModes()
{
  SAGE_VideoDevice *video;
  
  video = SageContext.SageVideo;
  if (video == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  }
  return &(video->video_modes);
}
