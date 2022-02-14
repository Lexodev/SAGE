/**
 * sage_vblint.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * VBL interrupt management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_VBLINT_H_
#define _SAGE_VBLINT_H_

#include <exec/types.h>

#include "sage_compiler.h"

/** External function for VBL IT installation */
extern VOID ASM SAGE_InstallVblInterrupt(VOID);

/** External function for VBL IT remove */
extern VOID ASM SAGE_RemoveVblInterrupt(VOID);

/** External function for VBL count retrieve */
extern ULONG ASM SAGE_GetVblCount(VOID);

/** External function for VBL count reset */
extern ULONG ASM SAGE_ResetVblCount(VOID);

#endif
