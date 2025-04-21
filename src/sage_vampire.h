/**
 * sage_vampire.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Vampire & AMMX support
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#ifndef _SAGE_VAMPIRE_H_
#define _SAGE_VAMPIRE_H_

#ifndef AFB_68080
#define AFB_68080             10
#endif
#ifndef AFF_68080
#define AFF_68080             (1<<AFB_68080)
#endif

#define SAPO_VCARD            0xdff3fc
#define SAPO_V600             1
#define SAPO_V500             2
#define SAPO_V4500            3
#define SAPO_V41200           4
#define SAPO_V4SA             5
#define SAPO_V1200            6
#define SAPO_V4600            7

#define SAPO_UNDEFINED        0
#define SAPO_ENABLE           1
#define SAPO_DISABLE          2

/** Check for 68080 support */
BOOL SAGE_ApolloCore(VOID);

/** Check for Vampire V4 card */
BOOL SAGE_VampireV4(VOID);

#endif
