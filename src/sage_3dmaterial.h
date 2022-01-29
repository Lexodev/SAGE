/**
 * sage_3dmaterial.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D material management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_3DMATERIAL_H_
#define _SAGE_3DMATERIAL_H_

#include <exec/types.h>

#define S3DE_MAX_MATERIALS    64                    // Max materials in the world

typedef struct
{
  STRPTR file, name;
  LONG left, top, size, index;
} SAGE_Material;

/** Release a material */
VOID SAGE_ReleaseMaterial(SAGE_Material *);

/** Add a single material to the world */
BOOL SAGE_AddMaterial(STRPTR, STRPTR, LONG, LONG, LONG, LONG);

/** Add multiple materials to the world */
BOOL SAGE_AddMaterials(SAGE_Material *, LONG);

/** Load all materials */
BOOL SAGE_LoadMaterials(VOID);

/** Flush all materials */
VOID SAGE_FlushMaterials(VOID);

#endif