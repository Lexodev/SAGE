/**
 * sage_3dmaterial.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D material management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#include <string.h>
#include <math.h>

#include <proto/exec.h>

#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_3dtexture.h>
#include <sage/sage_3dmaterial.h>
#include <sage/sage_3dengine.h>

#include <sage/sage_debug.h>

/** Engine data */
extern FLOAT Cosinus[];
extern FLOAT Sinus[];
extern SAGE_3DWorld sage_world;

/**
 * Release a material
 */
VOID SAGE_ReleaseMaterial(SAGE_Material *material)
{
  SD(SAGE_DebugLog("SAGE_ReleaseMaterial");)
  if (material != NULL) {
    if (material->file != NULL) {
      SAGE_FreeMem(material->file);
    }
    if (material->name != NULL) {
      SAGE_FreeMem(material->name);
    }
    SAGE_FreeMem(material);
  }
}

/**
 * Add a single material to the world
 */
BOOL SAGE_AddMaterial(STRPTR file, STRPTR name, LONG left, LONG top, LONG size, LONG index)
{
  SAGE_Material *material;
  
  SD(SAGE_DebugLog("Add material %s (%d)", name, index);)
  material = (SAGE_Material *)SAGE_AllocMem(sizeof(SAGE_Material));
  if (material == NULL) {
    return FALSE;
  }
  material->file = (STRPTR)SAGE_AllocMem(strlen(file)+1);
  material->name = (STRPTR)SAGE_AllocMem(strlen(name)+1);
  if (material->file == NULL || material->name == NULL) {
    SAGE_ReleaseMaterial(material);
    return FALSE;
  }
  strcpy(material->file, file);
  strcpy(material->name, name);
  material->left = left;
  material->top = top;
  material->size = size;
  material->index = index;
  sage_world.materials[sage_world.nb_materials] = material;
  sage_world.nb_materials++;
  return TRUE;
}

/**
 * Add multiple materials to the world
 */
BOOL SAGE_AddMaterialList(SAGE_Material *materials, LONG number)
{
  LONG idx;
  
  SD(SAGE_DebugLog("SAGE_AddMaterialList (%d)", number);)
  for (idx = 0;idx < number;idx++) {
    if (!SAGE_AddMaterial(materials[idx].file, materials[idx].name, materials[idx].left, materials[idx].top, materials[idx].size, materials[idx].index)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Load all materials
 */
BOOL SAGE_LoadMaterials()
{
  SAGE_Picture *picture;
  SAGE_Material *material;
  STRPTR file;
  LONG idx;
  
  SD(SAGE_DebugLog("SAGE_LoadMaterials");)
  file = NULL;
  picture = NULL;
  for (idx = 0;idx < sage_world.nb_materials;idx++) {
    material = sage_world.materials[idx];
    if (material != NULL) {
      // Load a new picture ?
      if (file == NULL || strcmp(file, material->file) != 0) {
        if (picture != NULL) {
          SAGE_ReleasePicture(picture);
        }
        file = material->file;
        if ((picture = SAGE_LoadPicture(file)) == NULL) {
          return FALSE;
        }
      }
      // Create a new texture
      if (!SAGE_CreateTextureFromPicture(material->index, material->left, material->top, material->size, picture)) {
        SAGE_ReleasePicture(picture);
        return FALSE;
      }
      // Add texture to card
      if (!SAGE_AddTexture(material->index)) {
        SAGE_ReleasePicture(picture);
        return FALSE;
      }
    }
  }
  if (picture != NULL) {
    SAGE_ReleasePicture(picture);
  }
  return TRUE;
}

/**
 * Flush all materials
 */
VOID SAGE_FlushMaterials()
{
  LONG idx;
  SAGE_Material *material;
  
  SD(SAGE_DebugLog("SAGE_FlushMaterials");)
  SAGE_FlushTextures();
  for (idx = 0;idx < sage_world.nb_materials;idx++) {
    material = sage_world.materials[idx];
    SAGE_ReleaseMaterial(material);
    sage_world.materials[idx] = NULL;
  }
  sage_world.nb_materials = 0;
}
