/**
 * sage_debug.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Debug macro
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#ifndef _SAGE_DEBUG_H_
#define _SAGE_DEBUG_H_

#if _SAGE_DEBUG_MODE_ == 1
#define SD(x)     x
#else
#define SD(x)     ;
#endif

#if _SAGE_SAFE_MODE_ == 1
#define SAFE(x)   x
#else
#define SAFE(x)   ;
#endif

#endif
