/**
 * sage_audio.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Audio module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <exec/execbase.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <devices/ahi.h>

#include <proto/exec.h>
#include <proto/ahi.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_audio.h"

#define AHIVERSION            4L
#define PTREPLAYVERSION       0L

#define MOD_TIMER_SPEED       50        /* timer speed, we need a speed of 50 Hz = 50 ticks per second */
#define MOD_TIMER_LOWER       32*2/5
#define MOD_TIMER_HIGHER      255*2/5

/** @var AHI library */
struct Library * AHIBase = NULL;

/** @var AHI message port */
struct MsgPort * AHImp = NULL;

/** @var AHI request */
struct AHIRequest * AHIio = NULL;

/** @var AHI device */
BYTE AHIDevice = -1;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * this is the interrupt timer
 */
__asm __interrupt __saveds static void SAGE_PTPlayer(
  register __a0 struct Hook * Hook,
  register __a2 struct AHIAudioCtrl * AHIAudioCtrl,
  register __a1 APTR Random
)
{
  if (Hook->h_Data != NULL) {
    mt_music(Hook->h_Data);
  }
  return;
}

/**
 * Initialize audio module
 * Open librairies
 *
 * @return Operation success
 */
BOOL SAGE_InitAudioModule(VOID)
{
  SD(SAGE_DebugLog("Init Audio module"));
  if (AHImp = CreateMsgPort()) {
    if (AHIio = (struct AHIRequest *) CreateIORequest(AHImp, sizeof(struct AHIRequest))) {
      AHIio->ahir_Version = AHIVERSION;
      if (!(AHIDevice = OpenDevice(AHINAME, AHI_NO_UNIT, (struct IORequest *) AHIio, NULL))) {
        AHIBase = (struct Library *) AHIio->ahir_Std.io_Device;
      } else {
        SAGE_SetError(SERR_AHI_LIB);
        return FALSE;
      }
    } else {
      SAGE_SetError(SERR_AHI_LIB);
      return FALSE;
    }
  } else {
    SAGE_SetError(SERR_AHI_LIB);
    return FALSE;
  }
  if (!SAGE_AllocAudioDevice()) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Release audio module
 * Close librairies
 *
 * @return Operation success
 */
BOOL SAGE_ReleaseAudioModule(VOID)
{
  SD(SAGE_DebugLog("Release Audio module"));
  if (SageContext.SageAudio != NULL) {
    SAGE_FreeAudioDevice();
  }
  if (!AHIDevice) {
    CloseDevice((struct IORequest *) AHIio);
    AHIDevice = -1;
  }
  if (AHIio != NULL) {
    DeleteIORequest((struct IORequest *) AHIio);
    AHIio = NULL;
  }
  if (AHImp != NULL) {
    DeleteMsgPort(AHImp);
    AHImp = NULL;
  }
  return TRUE;
}

/**
 * Allocate the audio device
 * 
 * @return Operation success
 */
BOOL SAGE_AllocAudioDevice(VOID)
{
  SAGE_AudioDevice * audio;

  SD(SAGE_InfoLog("Allocate audio device"));
  if ((audio = SAGE_AllocMem(sizeof(SAGE_AudioDevice))) == NULL) {
    return FALSE;
  }
  audio->actrl = NULL;
  audio->mixfreq = 0;
  audio->music_played = SMUS_MAX_MUSICS;
  SD(SAGE_DebugLog("Allocate AHI audio control"));
  audio->mod_player_hook.h_Entry = ( ULONG (*)() ) SAGE_PTPlayer;
  audio->mod_player_hook.h_Data  = NULL;
  audio->actrl = AHI_AllocAudio(
    AHIA_AudioID, AHI_DEFAULT_ID,
    AHIA_MixFreq, AHI_DEFAULT_FREQ,
    AHIA_Channels, SAUD_MAX_CHANNELS,
    AHIA_Sounds, SSND_MAX_SOUNDS,
    AHIA_PlayerFunc, &(audio->mod_player_hook),
    AHIA_PlayerFreq, MOD_TIMER_SPEED << 16,
    AHIA_MinPlayerFreq, MOD_TIMER_LOWER << 16,
    AHIA_MaxPlayerFreq, MOD_TIMER_HIGHER << 16,
    TAG_DONE
  );
  if (audio->actrl) {
    SD(SAGE_DebugLog("Audio control ready"));
    AHI_ControlAudio(audio->actrl, AHIC_MixFreq_Query, &(audio->mixfreq), TAG_DONE);
    SageContext.SageAudio = audio;
    return TRUE;
  }
  SAGE_FreeMem(audio);
  SAGE_SetError(SERR_AUDIOALLOC);
  return FALSE;
}

/**
 * Free the audio device
 *
 * @todo: release sounds and musics
 * 
 * @return Operation success
 */
BOOL SAGE_FreeAudioDevice()
{
  SAGE_AudioDevice * audio;
  
  audio = SageContext.SageAudio;
  if (audio == NULL) {
    SAGE_SetError(SERR_NO_AUDIODEVICE);
    return FALSE;
  }
  if (audio->actrl != NULL) {
    AHI_FreeAudio(audio->actrl);
  }
  SAGE_FreeMem(audio);
  SageContext.SageAudio = NULL;
  return TRUE;
}
