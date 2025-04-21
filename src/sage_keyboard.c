/**
 * sage_keyboard.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Keyboard input management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <exec/exec.h>
#include <libraries/lowlevel.h>

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_context.h>
#include <sage/sage_keyboard.h>

#include <proto/exec.h>
#include <proto/lowlevel.h>

/** @var Lowlevel library */
extern struct Library *LowLevelBase;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Scan some keys and get their state
 *
 * @param keys  Array of keys to scan
 * @param nbkey Number of keys to scan
 *
 * @return Operation success
 */
BOOL SAGE_ScanKeyboard(SAGE_KeyScan *keys, UBYTE nbkey)
{
  if (keys != NULL) {
    QueryKeys((struct KeyQuery *)keys, nbkey);
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Add a keyboard key handler
 *
 * @param key     Key code
 * @param handler Handler function
 *
 * @return Operation success
 */
BOOL SAGE_AddKeyboardHandler(UWORD key, VOID (*handler)(BOOL))
{
  SAGE_InputDevice *input;

  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  if (key >= SINP_NB_KEY) {
    SAGE_SetError(SERR_BAD_KEYCODE);
    return FALSE;
  }
  input->keyboard_handlers[key] = handler;
  return TRUE;
}

/**
 * Remove a keyboard key handler
 *
 * @param key Key code
 *
 * @return Operation success
 */
BOOL SAGE_RemoveKeyboardHandler(UWORD key)
{
  SAGE_InputDevice *input;

  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  if (key >= SINP_NB_KEY) {
    SAGE_SetError(SERR_BAD_KEYCODE);
    return FALSE;
  }
  input->keyboard_handlers[key] = NULL;
  return TRUE;
}

/**
 * Clear all keyboard handlers
 *
 * @return Operation success
 */
BOOL SAGE_ClearKeyboardHandlers()
{
  SAGE_InputDevice *input;
  UWORD key;

  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  for (key = 0;key < SINP_NB_KEY;key++) {
    input->keyboard_handlers[key] = NULL;
  }
  return TRUE;
}

/**
 * Prepare the keyboard handlers
 *
 * @return Operation success
 */
BOOL SAGE_InstallKeyboardHandlers()
{
  SAGE_InputDevice *input;
  UWORD key, handler;
  
  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  handler = 0;
  for (key = 0;key < SINP_NB_KEY;key++) {
    if (input->keyboard_handlers[key] != NULL) {
      input->key_scan[handler].key_code = key;
      handler++;
    }
  }
  input->nb_handlers = handler;
  return TRUE;
}
