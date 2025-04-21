/**
 * sage_compiler.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Compiler macros
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 * @link http://aminet.net/package/game/shoot/BlitzQuake_src
 */

#ifndef _SAGE_COMPILER_H_
#define _SAGE_COMPILER_H_

#ifdef __SASC
#   define SAVEDS       __saveds
#   define ASM          __asm
#   define REG(r,a)     register __##r a
#   define INTERRUPT    __interrupt
#elif defined(__GNUC__)
#   define SAVEDS       __saveds
#   define ASM
#   define REG(r,a)     a __asm(#r)
#   define INTERRUPT    __interrupt
#elif defined(_DCC)
#   define SAVEDS       __geta4
#   define ASM
#   define REG(r,a)     __##r a
#   define INTERRUPT    __interrupt
#elif defined(__STORM__)
#   define SAVEDS       __saveds
#   define ASM
#   define REG(r,a)     register __##r a
#   define INTERRUPT    __interrupt
#elif defined(__VBCC__)
#   define SAVEDS       __saveds
#   define ASM
#   define REG(r,a)     __reg(#r) a
#   define INTERRUPT    __interrupt
#else
#   error   add #defines for your compiler...
#endif

#endif
