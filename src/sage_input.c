/**
 * sage_input.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Input devices management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <exec/exec.h>
#include <libraries/lowlevel.h>

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_context.h>
#include <sage/sage_input.h>

#include <proto/exec.h>
#include <proto/lowlevel.h>

#define LOWLEVELVERSION       0L

/** @var Lowlevel library */
struct Library *LowLevelBase = NULL;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Init the input module
 * Open librairies
 *
 * @return Operation success
 */
BOOL SAGE_InitInputModule()
{
  SD(SAGE_DebugLog("Init Input module");)
  if ((LowLevelBase = OpenLibrary("lowlevel.library", LOWLEVELVERSION)) == NULL) {
    SAGE_SetError(SERR_LOWLEVEL_LIB);
    return FALSE;
  }
  if (!SAGE_AllocInputDevice()) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Release the input module
 * Close librairies
 *
 * @return Operation success
 */
BOOL SAGE_ReleaseInputModule()
{
  SD(SAGE_DebugLog("Release Input module");)
  if (SageContext.SageInput != NULL) {
    SAGE_FreeInputDevice();
  }
  if (LowLevelBase != NULL) {
    CloseLibrary(LowLevelBase);
    LowLevelBase = NULL;
  }
  return TRUE;
}

/**
 * Allocate the input device
 *
 * @return Operation success
 */
BOOL SAGE_AllocInputDevice()
{
  SAGE_InputDevice *device;
  
  if ((device = SAGE_AllocMem(sizeof(SAGE_InputDevice))) == NULL) {
    return FALSE;
  }
  device->nb_handlers = 0;
  device->active_joyport = 0;
  if (SAGE_GetPortType(SINP_JOYPORT1) != SINP_NOTAVAIL) {
    device->active_joyport += SINP_ACTIVE_PORT0;
  }
  if (SAGE_GetPortType(SINP_JOYPORT2) != SINP_NOTAVAIL) {
    device->active_joyport += SINP_ACTIVE_PORT1;
  }
  if (SAGE_GetPortType(SINP_JOYPORT3) != SINP_NOTAVAIL) {
    device->active_joyport += SINP_ACTIVE_PORT2;
  }
  if (SAGE_GetPortType(SINP_JOYPORT4) != SINP_NOTAVAIL) {
    device->active_joyport += SINP_ACTIVE_PORT3;
  }
  SD(SAGE_DebugLog(
    "Active joyport %d %d %d %d",
    device->active_joyport & SINP_ACTIVE_PORT0,
    (device->active_joyport & SINP_ACTIVE_PORT1) >> 1,
    (device->active_joyport & SINP_ACTIVE_PORT2) >> 2,
    (device->active_joyport & SINP_ACTIVE_PORT3) >> 3
  );)
  SageContext.SageInput = device;
  return TRUE;
}

/**
 * Free the input device
 *
 * @return Operation success
 */
BOOL SAGE_FreeInputDevice()
{
  SAGE_InputDevice *device;

  device = SageContext.SageInput;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  SAGE_FreeMem(device);
  SageContext.SageInput = NULL;
  return TRUE;
}

/**
 * Call the input handlers
 *
 * @return Operation success
 */
BOOL SAGE_HandleInputEvents()
{
  SAGE_InputDevice *input;
  SAGE_PortScan scan;
  UWORD handler;
  
  // Check for input device
  input = SageContext.SageInput;
  SAFE(if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  })
  if (input->nb_handlers > 0) {
    QueryKeys((struct KeyQuery *)input->key_scan, input->nb_handlers);
    for (handler = 0;handler < input->nb_handlers;handler++) {
      (*input->keyboard_handlers[input->key_scan[handler].key_code])(input->key_scan[handler].key_pressed);
    }
  }
  if ((input->active_joyport & SINP_ACTIVE_PORT0) && input->joyport_handler[SINP_JOYPORT1] != NULL) {
    if (SAGE_ScanPort(&scan, SINP_JOYPORT1)) {
      (*input->joyport_handler[SINP_JOYPORT1])(&scan);
    }
  }
  if ((input->active_joyport & SINP_ACTIVE_PORT1) && input->joyport_handler[SINP_JOYPORT2] != NULL) {
    if (SAGE_ScanPort(&scan, SINP_JOYPORT2)) {
      (*input->joyport_handler[SINP_JOYPORT2])(&scan);
    }
  }
  if ((input->active_joyport & SINP_ACTIVE_PORT2) && input->joyport_handler[SINP_JOYPORT3] != NULL) {
    if (SAGE_ScanPort(&scan, SINP_JOYPORT3)) {
      (*input->joyport_handler[SINP_JOYPORT3])(&scan);
    }
  }
  if ((input->active_joyport & SINP_ACTIVE_PORT3) && input->joyport_handler[SINP_JOYPORT4] != NULL) {
    if (SAGE_ScanPort(&scan, SINP_JOYPORT4)) {
      (*input->joyport_handler[SINP_JOYPORT4])(&scan);
    }
  }
  return TRUE;
}
