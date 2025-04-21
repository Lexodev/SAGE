/**
 * sage_memory.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Memory allocation management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_memory.h>

#include <proto/exec.h>

SAGE_MemoryManager SAGE_Memory = { NULL, NULL };

/**
 * Add a memory entry to the memory list
 *
 * @param address Bloc address
 * @param size    Bloc size
 *
 * @return Operation success
 */
BOOL SAGE_AddMemoryEntry(APTR base, APTR address, ULONG size)
{
  SAGE_MemoryNode *new_node;

  new_node = (SAGE_MemoryNode *)AllocMem(sizeof(SAGE_MemoryNode), MEMF_PUBLIC);
  if (new_node) {
    new_node->base_address = base;
    new_node->memory_bloc = address;
    new_node->bloc_size = size;
    if (SAGE_Memory.head == NULL) {
      new_node->previous = NULL;
      new_node->next = NULL;
      SAGE_Memory.head = new_node;
      SAGE_Memory.tail = new_node;
    } else {
      new_node->previous = SAGE_Memory.tail;
      new_node->next = NULL;
      SAGE_Memory.tail->next = new_node;
      SAGE_Memory.tail = new_node;
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * Check if alignment is on a power 2 boundary
 *
 * @param align Alignment in bytes
 *
 * @return Valid alignment
 */
ULONG SAGE_CheckAlignment(ULONG align)
{
  ULONG limit;
  
  // Align max on 64k boundary
  limit = 65536;
  while (limit) {
    if (align >= limit) {
      return limit;
    }
    limit >>= 1;
  }
  return 0;
}

/**
 * Allocate a memory bloc and register it to the memory manager
 *
 * @param size       Bloc size
 * @param attributes Bloc attributes
 * @param align      Bloc alignment, must be a multiple of 2
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocMemoryBloc(ULONG size, ULONG attributes, ULONG align)
{
  APTR base;
  APTR memory;

  // Make sure that align is a multiple of 2
  align = SAGE_CheckAlignment(align);
  // Reserve enough space for alignment
  size += align;
  // Allocate the required bloc
  base = AllocMem(size, attributes);
  if (base == NULL) {
    SAGE_SetError(SERR_NO_MEMORY);
    return NULL;
  }
  // Align the bloc if necessary
  if (align) {
    memory = (APTR)(((ULONG)base + align) & (-align));
  } else {
    memory = base;
  }
  if (!SAGE_AddMemoryEntry(base, memory, size)) {
    FreeMem(base, size);
    SAGE_SetError(SERR_NO_MEMORY);
    return NULL;
  }
  SD(SAGE_TraceLog("Memory allocation 0x%X (0x%X) of %d bytes (align %d)", base, memory, size, align);)
  return memory;
}

/**
 * Allocate generic memory bloc
 *
 * @param size Bloc size
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocMem(ULONG size)
{
  return SAGE_AllocMemoryBloc(size, MEMF_PUBLIC|MEMF_CLEAR, 0);
}

/**
 * Allocate aligned generic memory bloc
 *
 * @param size  Bloc size
 * @param align Bloc alignment, must be a multiple of 2
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocAlignMem(ULONG size, ULONG align)
{
  return SAGE_AllocMemoryBloc(size, MEMF_PUBLIC|MEMF_CLEAR, align);
}

/**
 * Allocate chip memory bloc
 *
 * @param size Bloc size
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocChipMem(ULONG size)
{
  return SAGE_AllocMemoryBloc(size, MEMF_CHIP|MEMF_CLEAR, 0);
}

/**
 * Allocate aligned chip memory bloc
 *
 * @param size  Bloc size
 * @param align Bloc alignment, must be a multiple of 2
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocAlignChipMem(ULONG size, ULONG align)
{
  return SAGE_AllocMemoryBloc(size, MEMF_CHIP|MEMF_CLEAR, align);
}

/**
 * Allocate fast memory bloc
 *
 * @param size Bloc size
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocFastMem(ULONG size)
{
  return SAGE_AllocMemoryBloc(size, MEMF_FAST|MEMF_CLEAR, 0);
}

/**
 * Allocate aligned fast memory bloc
 *
 * @param size  Bloc size
 * @param align Bloc alignment, must be a multiple of 2
 *
 * @return Memory bloc address or NULL on error
 */
APTR SAGE_AllocAlignFastMem(ULONG size, ULONG align)
{
  return SAGE_AllocMemoryBloc(size, MEMF_FAST|MEMF_CLEAR, align);
}

/**
 * Release a memory bloc and unregister it from the memory manager
 *
 * @param address Bloc address
 */
VOID SAGE_FreeMem(APTR address)
{
  SAGE_MemoryNode *node;

  if (address == NULL) {
    return;
  }
  node = SAGE_Memory.head;
  while (node != NULL && node->memory_bloc != address) {
    node = node->next;
  }
  // Memory bloc found
  if (node != NULL) {
    SD(SAGE_TraceLog("Memory release 0x%X of %d bytes", node->base_address, node->bloc_size);)
    FreeMem(node->base_address, node->bloc_size);
    if (SAGE_Memory.head == node) {
      SAGE_Memory.head = node->next;
    }
    if (SAGE_Memory.tail == node) {
      SAGE_Memory.tail = node->previous;
    }
    if (node->previous != NULL) {
      node->previous->next = node->next;
    }
    if (node->next != NULL) {
      node->next->previous = node->previous;
    }
    FreeMem(node, sizeof(SAGE_MemoryNode));
  }
}

/**
 * Release all memory bloc and unregister them from the memory manager
 */
VOID SAGE_ReleaseMem()
{
  SAGE_MemoryNode *node;

  node = SAGE_Memory.head;
  while (node != NULL) {
    SD(SAGE_TraceLog("Releasing memory bloc 0x%X of %d bytes", node->base_address, node->bloc_size);)
    FreeMem(node->base_address, node->bloc_size);
    SAGE_Memory.head = node->next;
    FreeMem(node, sizeof(SAGE_MemoryNode));
    node = SAGE_Memory.head;
  }
  SAGE_Memory.head = NULL;
  SAGE_Memory.tail = NULL;
}

/**
 * Dump the memory list
 */
VOID SAGE_DumpMemory()
{
  SAGE_MemoryNode *node;
  WORD count = 0;

  SAGE_DebugLog("** Dumping memory list **");
  node = SAGE_Memory.head;
  if (node == NULL) {
    SAGE_DebugLog("- List is empty !");
  } else {
    while (node != NULL) {
      count++;
      SAGE_DebugLog("* Node %d :", count);
      SAGE_DebugLog(" - Base address 0x%X", node->base_address);
      SAGE_DebugLog(" - Aligned address 0x%X", node->memory_bloc);
      SAGE_DebugLog(" - Size %d", node->bloc_size);
      node = node->next;
    }
  }
  SAGE_DebugLog("** End of list **");
}

/**
 * Get the total available memory size
 *
 * @return Available memory size
 */
ULONG SAGE_AvailMem()
{
  return AvailMem(MEMF_PUBLIC);
}

/**
 * Get the total available CHIP memory size
 *
 * @return Available CHIP memory size
 */
ULONG SAGE_AvailChipMem()
{
  return AvailMem(MEMF_CHIP);
}

/**
 * Get the total available FAST memory size
 *
 * @return Available FAST memory size
 */
ULONG SAGE_AvailFastMem()
{
  return AvailMem(MEMF_FAST);
}
