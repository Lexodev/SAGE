/**
 * sage_memory.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Memory allocation management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#ifndef _SAGE_MEMORY_H_
#define _SAGE_MEMORY_H_

#include <exec/exec.h>

/** Memory node */
typedef struct _sage_memory_node {
  /** Base address of bloc, before alignment */
  APTR base_address;
  /** Address of memory bloc */
  APTR memory_bloc;
  /** Size of memory bloc */
  ULONG bloc_size;
  /** Previous memory node */
  struct _sage_memory_node * previous;
  /** Next memory node */
  struct _sage_memory_node * next;
} SAGE_MemoryNode;

/** Memory manager */
typedef struct {
  /** Head of memory list */
  SAGE_MemoryNode * head;
  /** Tail of memory list */
  SAGE_MemoryNode * tail;
} SAGE_MemoryManager;

/** Allocate public memory */
APTR SAGE_AllocMem(ULONG);

/** Allocate align public memory */
APTR SAGE_AllocAlignMem(ULONG, ULONG);

/** Allocate chip memory */
APTR SAGE_AllocChipMem(ULONG);

/** Allocate align chip memory */
APTR SAGE_AllocAlignChipMem(ULONG, ULONG);

/** Allocate fast memory */
APTR SAGE_AllocFastMem(ULONG);

/** Allocate align fast memory */
APTR SAGE_AllocAlignFastMem(ULONG, ULONG);

/** Free any kind of memory */
VOID SAGE_FreeMem(APTR);

/** Release all memory blocs */
VOID SAGE_ReleaseMem(VOID);

/** Dump the memory list */
VOID SAGE_DumpMemory(VOID);

/** Get the available public memory in bytes */
ULONG SAGE_AvailMem(VOID);

/** Get the available chip memory in bytes */
ULONG SAGE_AvailChipMem(VOID);

/** Get the available fast memory in bytes */
ULONG SAGE_AvailFastMem(VOID);

#endif