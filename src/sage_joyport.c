/**
 * sage_joyport.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Joyport input management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 September 2021
 */

#include <exec/exec.h>
#include <libraries/lowlevel.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_context.h"
#include "sage_joyport.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/lowlevel.h>

/** @var Lowlevel library */
extern struct Library * LowLevelBase;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Get the type of device connected to a port
 *
 * @param port Number of the port (0-4)
 *
 * @return Type of device connected
 */
UWORD SAGE_GetPortType(UWORD port)
{
  ULONG port_type;
  
  if (port < SINP_NB_JOYPORT) {
    // Tricks to have a valid result
    ReadJoyPort(port);
    Delay(1);
    ReadJoyPort(port);
    Delay(1);
    ReadJoyPort(port);
    Delay(1);
    port_type = ReadJoyPort(port);
    switch (port_type & JP_TYPE_MASK) {
      case JP_TYPE_NOTAVAIL:
        return SINP_NOTAVAIL;
        break;
      case JP_TYPE_GAMECTLR:
        return SINP_GAMEPAD;
        break;
      case JP_TYPE_MOUSE:
        return SINP_MOUSE;
        break;
      case JP_TYPE_JOYSTK:
        return SINP_JOYSTICK;
        break;
      case JP_TYPE_UNKNOWN:
        return SINP_UNKNOWN;
        break;
    }
  }
  return SINP_NOTAVAIL;
}

/**
 * Get a port status
 *
 * @param scan Port scan pointer
 * @param port Port number
 *
 * @return Operation success
 */
BOOL SAGE_ScanPort(SAGE_PortScan * scan, UWORD port)
{
  ULONG port_scan;

  if (scan == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (port < SINP_NB_JOYPORT) {
    port_scan = ReadJoyPort(port);
    switch (port_scan & JP_TYPE_MASK) {
      case JP_TYPE_GAMECTLR:
        scan->scan = port_scan;
        scan->type = SINP_GAMEPAD;
        scan->up = (port_scan & JPF_JOY_UP) ? TRUE : FALSE;
        scan->down = (port_scan & JPF_JOY_DOWN) ? TRUE : FALSE;
        scan->left = (port_scan & JPF_JOY_LEFT) ? TRUE : FALSE;
        scan->right = (port_scan & JPF_JOY_RIGHT) ? TRUE : FALSE;
        scan->red = (port_scan & JPF_BUTTON_RED) ? TRUE : FALSE;
        scan->blue = (port_scan & JPF_BUTTON_BLUE) ? TRUE : FALSE;
        scan->yellow = (port_scan & JPF_BUTTON_YELLOW) ? TRUE : FALSE;
        scan->green = (port_scan & JPF_BUTTON_GREEN) ? TRUE : FALSE;
        scan->forward = (port_scan & JPF_BUTTON_FORWARD) ? TRUE : FALSE;
        scan->reverse = (port_scan & JPF_BUTTON_REVERSE) ? TRUE : FALSE;
        scan->play = (port_scan & JPF_BUTTON_PLAY) ? TRUE : FALSE;
        scan->fire1 = scan->red;
        scan->fire2 = scan->blue;
        scan->lbutton = scan->red;
        scan->rbutton = scan->blue;
        break;
      case JP_TYPE_MOUSE:
        scan->scan = port_scan;
        scan->type = SINP_MOUSE;
        scan->lbutton = (port_scan & JPF_BUTTON_RED) ? TRUE : FALSE;
        scan->rbutton = (port_scan & JPF_BUTTON_BLUE) ? TRUE : FALSE;
        scan->mbutton = (port_scan & JPF_BUTTON_PLAY) ? TRUE : FALSE;
        scan->mousev = port_scan & JP_MVERT_MASK;
        scan->mouseh = port_scan & JP_MHORZ_MASK;
        scan->fire1 = scan->lbutton;
        scan->fire2 = scan->rbutton;
        scan->red = scan->lbutton;
        scan->blue = scan->rbutton;
        break;
      case JP_TYPE_JOYSTK:
        scan->scan = port_scan;
        scan->type = SINP_JOYSTICK;
        scan->up = (port_scan & JPF_JOY_UP) ? TRUE : FALSE;
        scan->down = (port_scan & JPF_JOY_DOWN) ? TRUE : FALSE;
        scan->left = (port_scan & JPF_JOY_LEFT) ? TRUE : FALSE;
        scan->right = (port_scan & JPF_JOY_RIGHT) ? TRUE : FALSE;
        scan->fire1 = (port_scan & JPF_BUTTON_RED) ? TRUE : FALSE;
        scan->fire2 = (port_scan & JPF_BUTTON_BLUE) ? TRUE : FALSE;
        scan->red = scan->fire1;
        scan->blue = scan->fire2;
        scan->lbutton = scan->fire1;
        scan->rbutton = scan->fire2;
        break;
      case JP_TYPE_UNKNOWN:
        scan->scan = port_scan;
        scan->type = SINP_UNKNOWN;
        break;
      default:
        SAGE_SetError(SERR_BAD_PORTTYPE);
        return FALSE;
    }
    return TRUE;
  }
  SAGE_SetError(SERR_BAD_PORT);
  return FALSE;  
}

/**
 * Add a joyport handler
 *
 * @return Operation success
 */
BOOL SAGE_AddJoyportHandler(UWORD port, VOID (*handler)(SAGE_PortScan *))
{
  SAGE_InputDevice * input;

  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  if (port >= SINP_NB_JOYPORT) {
    SAGE_SetError(SERR_BAD_PORT);
    return FALSE;
  }
  input->joyport_handler[port] = handler;
  return TRUE;
}

/**
 * Remove a joyport handler
 *
 * @return Operation success
 */
BOOL SAGE_RemoveJoyportHandler(UWORD port)
{
  SAGE_InputDevice * input;

  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  if (port >= SINP_NB_JOYPORT) {
    SAGE_SetError(SERR_BAD_PORT);
    return FALSE;
  }
  input->joyport_handler[port] = NULL;
  return TRUE;
}

/**
 * Clear all joyport handlers
 *
 * @return Operation success
 */
BOOL SAGE_ClearJoyportHandlers()
{
  SAGE_InputDevice * input;
  UWORD port;

  // Check for input device
  input = SageContext.SageInput;
  if (input == NULL) {
    SAGE_SetError(SERR_NO_INPUTDEVICE);
    return FALSE;
  }
  for (port = 0;port < SINP_NB_JOYPORT;port++) {
    input->joyport_handler[port] = NULL;
  }
  return TRUE;
}
