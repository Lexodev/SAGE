/**
 * sage_vampire.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Vampire & AMMX support
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#ifndef _SAGE_VAMPIRE_H_
#define _SAGE_VAMPIRE_H_

#ifndef AFB_68080
#define AFB_68080             10
#endif
#ifndef AFF_68080
#define AFF_68080             (1<<AFB_68080)
#endif

#define SAGA_FBADDR           0xdff1ec

#define SAPO_UNDEFINED        0
#define SAPO_ENABLE           1
#define SAPO_DISABLE          2

/** Check for Vampire presence */
BOOL SAGE_ApolloPresence(VOID);

/** Check for AMMX2 availability */
BOOL SAGE_AMMX2Available(VOID);

/** Set the Vampire frame buffer address */
BOOL SAGE_SetVampireFrameBuffer(APTR);

/** Get the Vampire frame buffer address */
APTR SAGE_GetVampireFrameBuffer(VOID);

#endif
