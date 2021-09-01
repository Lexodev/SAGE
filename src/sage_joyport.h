/**
 * sage_joyport.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Joyport input management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_JOYPORT_H_
#define _SAGE_JOYPORT_H_

/** Port type constants */
#define SINP_NOTAVAIL         0
#define SINP_MOUSE            1
#define SINP_JOYSTICK         2
#define SINP_GAMEPAD          3
#define SINP_UNKNOWN          4

#define SINP_NB_JOYPORT       4
#define SINP_JOYPORT1         0
#define SINP_JOYPORT2         1
#define SINP_JOYPORT3         2
#define SINP_JOYPORT4         3

#define SINP_ACTIVE_PORT0     1
#define SINP_ACTIVE_PORT1     2
#define SINP_ACTIVE_PORT2     4
#define SINP_ACTIVE_PORT3     8

/** Port scan structure */
typedef struct {
  /** Port scan result */
  ULONG scan;
  /** Port type */
  UWORD type;
  /** Joystick/paddle position */
  BOOL up, down, left, right;
  /** Joystick buttons status */
  BOOL fire1, fire2;
  /** Gamepad buttons status */
  BOOL red, blue, yellow, green, forward, reverse, play;
  /** Mouse buttons status */
  BOOL lbutton, rbutton, mbutton;
  /** Mouse position */
  BYTE mousev, mouseh;
} SAGE_PortScan;

/** Get the type of a port */
UWORD SAGE_GetPortType(UWORD);

/** Scan a port and get his status */
BOOL SAGE_ScanPort(SAGE_PortScan *, UWORD);

/** Add a joyport handler */
BOOL SAGE_AddJoyportHandler(UWORD, VOID (*handler)(SAGE_PortScan *));

/** Remove a joyport handler */
BOOL SAGE_RemoveJoyportHandler(UWORD);

/** Clear all joyport handlers */
BOOL SAGE_ClearJoyportHanlders(VOID);

#endif
