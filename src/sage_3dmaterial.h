/**
 * sage_3dmaterial.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D material management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#ifndef _SAGE_3DMATERIAL_H_
#define _SAGE_3DMATERIAL_H_

#include <exec/types.h>

#define S3DE_MAX_MATERIALS    256                   // Max materials in the world

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
BOOL SAGE_AddMaterialList(SAGE_Material *, LONG);

/** Load all materials */
BOOL SAGE_LoadMaterials(VOID);

/** Flush all materials */
VOID SAGE_FlushMaterials(VOID);

#endif