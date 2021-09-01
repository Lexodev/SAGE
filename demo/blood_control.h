/**
 * blood_control.h
 * 
 * Blood project
 * Control management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#ifndef __BLOOD_CONTROL_H__
#define __BLOOD_CONTROL_H__

#include <exec/types.h>

#define KEY_SCAN              8
#define KEY_NBR               20
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_SPACE             4
#define KEY_SHIFT             5
#define KEY_CTRL              6
#define KEY_TAB               7
#define KEY_ALT               8
#define KEY_DEBUG             18
#define KEY_QUIT              19

VOID ScanKeyboard(VOID);
BOOL InitInput(VOID);

#endif
