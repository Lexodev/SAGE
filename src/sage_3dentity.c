/**
 * sage_3dentity.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D entity management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#include <string.h>
#include <math.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_loadlwo.h"
#include "sage_3dentity.h"
#include "sage_3dengine.h"

/** Engine data */
extern FLOAT Cosinus[];
extern FLOAT Sinus[];
extern SAGE_3DWorld sage_world;

/*****************************************************************************/
//            DEBUG ONLY
/*****************************************************************************/

VOID SAGE_DumpEntity(SAGE_Entity * entity)
{
  WORD index;
  
  SAGE_DebugLog("Dump entity");
  SAGE_DebugLog(" => ax=%d  ay=%d  az=%d", entity->anglex, entity->angley, entity->anglez);
  SAGE_DebugLog(" => px=%f  py=%f  pz=%f", entity->posx, entity->posy, entity->posz);
  SAGE_DebugLog(" => radius=%f", entity->radius);
  SAGE_DebugLog(" => disabled=%d  clipped=%d", (entity->disabled ? 1 : 0), (entity->clipped ? 1 : 0));
  SAGE_DebugLog(" => nbvertices=%d  nbfaces=%d  rendering=%d", entity->nb_vertices, entity->nb_faces, entity->rendering);
  SAGE_DebugLog("-- Vertices");
  for (index = 0;index < entity->nb_vertices;index++) {
    SAGE_DebugLog(" => vertex %d : x=%f  y=%f  z=%f", index, entity->vertices[index].x, entity->vertices[index].y, entity->vertices[index].z);
  }
  SAGE_DebugLog("-- Transformed vertices");
  for (index = 0;index < entity->nb_vertices;index++) {
    SAGE_DebugLog(" => vertex %d : x=%f  y=%f  z=%f", index, entity->trans_vertices[index].x, entity->trans_vertices[index].y, entity->trans_vertices[index].z);
  }
  SAGE_DebugLog("-- Faces");
  for (index = 0;index < entity->nb_faces;index++) {
    if (entity->faces[index].is_quad) {
      SAGE_DebugLog(" => face %d : p1=%d  p2=%d  p3=%d  p4=%d  color=%d  tex=%d  %s  clipped=%d",
        index, entity->faces[index].p1, entity->faces[index].p2, entity->faces[index].p3, entity->faces[index].p4, entity->faces[index].color,
        entity->faces[index].texture, (entity->faces[index].culled ? "culled" : "not culled"), entity->faces[index].clipped
      );
      SAGE_DebugLog("             u1,v1=%f,%f  u2,v2=%f,%f  u3,v3=%f,%f  u4,v4=%f,%f",
        entity->faces[index].u1, entity->faces[index].v1, entity->faces[index].u2, entity->faces[index].v2,
        entity->faces[index].u3, entity->faces[index].v3, entity->faces[index].u4, entity->faces[index].v4,
        entity->faces[index].texture
      );
    } else {
      SAGE_DebugLog(" => face %d : p1=%d  p2=%d  p3=%d  color=%d  tex=%d  %s  clipped=%d",
        index, entity->faces[index].p1, entity->faces[index].p2, entity->faces[index].p3, entity->faces[index].color,
        entity->faces[index].texture, (entity->faces[index].culled ? "culled" : "not culled"), entity->faces[index].clipped
      );
      SAGE_DebugLog("             u1,v1=%f,%f  u2,v2=%f,%f  u3,v3=%f,%f",
        entity->faces[index].u1, entity->faces[index].v1, entity->faces[index].u2, entity->faces[index].v2,
        entity->faces[index].u3, entity->faces[index].v3
      );
    }
    SAGE_DebugLog("             normal(%f, %f, %f)", entity->faces[index].normal.x, entity->faces[index].normal.y, entity->faces[index].normal.z);
  }
}

/*****************************************************************************/

/**
 * Create an empty entity 
 */
SAGE_Entity * SAGE_CreateEntity(WORD nb_vertices, WORD nb_faces)
{
  SAGE_Entity * entity;

  SD(SAGE_DebugLog("Create entity (%d, %d)", nb_vertices, nb_faces));
  entity = (SAGE_Entity *)SAGE_AllocMem(sizeof(SAGE_Entity));
  if (entity != NULL) {
    entity->nb_vertices = nb_vertices;
    entity->nb_faces = nb_faces;
    entity->rendering = S3DE_RENDER_TEXT;
    entity->vertices = (SAGE_EntityVertex *)SAGE_AllocMem(sizeof(SAGE_EntityVertex)*nb_vertices);
    entity->trans_vertices = (SAGE_EntityVertex *)SAGE_AllocMem(sizeof(SAGE_EntityVertex)*nb_vertices);
    entity->faces = (SAGE_EntityFace *)SAGE_AllocMem(sizeof(SAGE_EntityFace)*nb_faces);
    if (entity->vertices != NULL && entity->trans_vertices != NULL && entity->faces != NULL) {
      return entity;
    }
    SAGE_ReleaseEntity(entity);
  }
  return NULL;
}

/**
 * Release an entity
 */
VOID SAGE_ReleaseEntity(SAGE_Entity * entity)
{
  SD(SAGE_DebugLog("Release entity"));
  if (entity != NULL) {
    if (entity->vertices != NULL) {
      SAGE_FreeMem(entity->vertices);
    }
    if (entity->trans_vertices != NULL) {
      SAGE_FreeMem(entity->trans_vertices);
    }
    if (entity->faces != NULL) {
      SAGE_FreeMem(entity->faces);
    }
    SAGE_FreeMem(entity);
  }
}

/**
 *  Get the type of an entity file
 * 
 *  @param file_handle Handle on a file
 * 
 *  @return Type of entity file
 */
UWORD SAGE_GetEntityFileType(BPTR file_handle)
{
  LONG bytes_read, entity_tag;

  // Check for Ligthwave object
  bytes_read = Seek(file_handle, S3DE_LWOBOFFSET, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &entity_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return S3DE_UNDEFINED;
  }
  if (entity_tag == S3DE_LWOBTAG) {
    SD(SAGE_DebugLog("This is a Ligthwave object"));
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return S3DE_LWOB;
  }
  return S3DE_UNDEFINED;
}

/**
 * Load an entity from a file
 */
SAGE_Entity * SAGE_LoadEntity(STRPTR filename)
{
  SAGE_Entity * entity;
  BPTR file_handle;
  UWORD type;

  SD(SAGE_DebugLog("Load entity %s", filename));
  entity = NULL;
  file_handle = Open(filename, MODE_OLDFILE);
  if (file_handle != 0) {
    type = SAGE_GetEntityFileType(file_handle);
    if (type == S3DE_LWOB) {
      entity = SAGE_LoadLWO(file_handle);
    } else {
      SAGE_SetError(SERR_FILEFORMAT);
    }
    Close(file_handle);
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  SD(SAGE_DumpEntity(entity));
  return entity;
}

/**
 * Calculate the entity radius
 */
VOID SAGE_SetEntityRadius(SAGE_Entity * entity)
{
  FLOAT radius, x, y, z;
  UWORD idx;

  entity->radius = 0.0;
  for (idx = 0;idx < entity->nb_vertices;idx++) {
    x = entity->vertices[idx].x;
    y = entity->vertices[idx].y;
    z = entity->vertices[idx].z;
    radius = sqrt((x*x) + (y*y) + (z*z));
    if (radius > entity->radius) {
      entity->radius = radius;
    }
  }
}

/**
 * Calculate entity faces normal
 */
VOID SAGE_SetEntityNormals(SAGE_Entity * entity)
{
  UWORD idx, p1, p2, p3;
  SAGE_Vector u, v;

  for (idx = 0;idx < entity->nb_faces;idx++) {
    p1 = entity->faces[idx].p1;
    p2 = entity->faces[idx].p2;
    p3 = entity->faces[idx].p3;
    u.x = entity->vertices[p2].x - entity->vertices[p1].x;
    u.y = entity->vertices[p2].y - entity->vertices[p1].y;
    u.z = entity->vertices[p2].z - entity->vertices[p1].z;
    v.x = entity->vertices[p3].x - entity->vertices[p1].x;
    v.y = entity->vertices[p3].y - entity->vertices[p1].y;
    v.z = entity->vertices[p3].z - entity->vertices[p1].z;
    // Calculate the normal
    SAGE_CrossProduct(&(entity->faces[idx].normal), &u, &v);
    // Normalize vector
    SAGE_Normalize(&(entity->faces[idx].normal));
  }
}

/**
 * Add an entity to the world
 */
BOOL SAGE_AddEntity(ULONG index, SAGE_Entity * entity)
{
  SAGE_Entity * new_entity;
  UWORD idx;

  SD(SAGE_DebugLog("Add entity #%d", index));
  if (index >= S3DE_MAX_ENTITIES) {
    SAGE_SetError(SERR_ENTITY_INDEX);
    return FALSE;
  }
  // Clean the place
  SAGE_RemoveEntity(index);
  new_entity = SAGE_CreateEntity(entity->nb_vertices, entity->nb_faces);
  if (new_entity != NULL) {
    new_entity->anglex = entity->anglex;
    new_entity->angley = entity->angley;
    new_entity->anglez = entity->anglez;
    new_entity->posx = entity->posx;
    new_entity->posy = entity->posy;
    new_entity->posz = entity->posz;
    new_entity->disabled = entity->disabled;
    new_entity->nb_vertices = entity->nb_vertices;
    new_entity->nb_faces = entity->nb_faces;
    new_entity->rendering = entity->rendering;
    // Copy vertices
    for (idx = 0;idx < entity->nb_vertices;idx++) {
      new_entity->vertices[idx].x = entity->vertices[idx].x;
      new_entity->vertices[idx].y = entity->vertices[idx].y;
      new_entity->vertices[idx].z = entity->vertices[idx].z;
    }
    // Copy faces
    for (idx = 0;idx < entity->nb_faces;idx++) {
      memcpy(&(new_entity->faces[idx]), &(entity->faces[idx]), sizeof(SAGE_EntityFace));
    }
    SAGE_SetEntityRadius(new_entity);
    SAGE_SetEntityNormals(new_entity);
    SD(SAGE_DumpEntity(new_entity));
    sage_world.entities[index] = new_entity;
    return TRUE;
  }
  return FALSE;
}

/**
 * Remove entity from the world
 */
VOID SAGE_RemoveEntity(ULONG index)
{
  SAGE_Entity * entity;

  SD(SAGE_DebugLog("Remove entity #%d", index));
  if (index < S3DE_MAX_ENTITIES) {
    entity = sage_world.entities[index];
    SAGE_ReleaseEntity(entity);
    sage_world.entities[index] = NULL;
  } else {
    SAGE_SetError(SERR_ENTITY_INDEX);
  }
}

/**
 * Release all entities
 */
VOID SAGE_FlushEntities()
{
  ULONG index;
  
  for (index = 0;index < S3DE_MAX_ENTITIES;index++) {
    SAGE_RemoveEntity(index);
  }
}

/**
 * Get an entity from her index
 */
SAGE_Entity * SAGE_GetEntity(ULONG index)
{
  if (index < S3DE_MAX_ENTITIES) {
    if (sage_world.entities[index] == NULL) {
      SAGE_SetError(SERR_NO_ENTITY);
    }
    return sage_world.entities[index];
  }
  SAGE_SetError(SERR_ENTITY_INDEX);
  return NULL;
}

/**
 * Set the entity angle
 */
BOOL SAGE_SetEntityAngle(ULONG index, WORD ax, WORD ay, WORD az)
{
  SAGE_Entity * entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->anglex = ax;
    entity->angley = ay;
    entity->anglez = az;
    return TRUE;
  }
  return FALSE;
}

/**
 * Rotate the entity
 */
BOOL SAGE_RotateEntity(ULONG index, WORD dax, WORD day, WORD daz)
{
  SAGE_Entity * entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->anglex += dax;
    entity->angley += day;
    entity->anglez += daz;
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the entity position
 */
BOOL SAGE_SetEntityPosition(ULONG index, FLOAT posx, FLOAT posy, FLOAT posz)
{
  SAGE_Entity * entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->posx = posx;
    entity->posy = posy;
    entity->posz = posz;
    return TRUE;
  }
  return FALSE;
}

/**
 * Move the entity
 */
BOOL SAGE_MoveEntity(ULONG index, FLOAT dx, FLOAT dy, FLOAT dz)
{
  SAGE_Entity * entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->posx += dx;
    entity->posy += dy;
    entity->posz += dz;
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the entity rendering mode
 */
BOOL SAGE_SetEntityRenderMode(ULONG index, UWORD mode)
{
  SAGE_Entity * entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->rendering = mode;
    return TRUE;
  }
  return FALSE;
}
