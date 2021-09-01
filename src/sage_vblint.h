/**
 * sage_vblint.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * VBL interrupt management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_VBLINT_H_
#define _SAGE_VBLINT_H_

#include <exec/types.h>

/** External function for VBL IT installation */
extern VOID __asm SAGE_InstallVblInterrupt(VOID);

/** External function for VBL IT remove */
extern VOID __asm SAGE_RemoveVblInterrupt(VOID);

/** External function for VBL count retrieve */
extern ULONG __asm SAGE_GetVblCount(VOID);

/** External function for VBL count reset */
extern ULONG __asm SAGE_ResetVblCount(VOID);

#endif
