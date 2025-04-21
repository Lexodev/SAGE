/**
 * sage_3dentity.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D entity management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#include <string.h>
#include <math.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_loadlwo.h>
#include <sage/sage_loadobj.h>
#include <sage/sage_screen.h>
#include <sage/sage_3dtexture.h>
#include <sage/sage_3dentity.h>
#include <sage/sage_3dengine.h>

#include <sage/sage_debug.h>

/** Data for entity optimization */
typedef struct {
  UWORD new_index;
  UWORD replaced_by;
} SAGE_RemapVertex;

/** Engine data */
extern SAGE_3DWorld sage_world;

/*****************************************************************************
 *                   START DEBUG
 *****************************************************************************/

#if _SAGE_DEBUG_MODE_ == 1
VOID SAGE_DumpEntity(SAGE_Entity *entity, UWORD mode)
{
  UWORD index;
  SAGE_Vertex *vertices;
  SAGE_Face *faces;
  SAGE_Vector *normals;
  
  SAGE_DebugLog("** Dump entity **");
  if (entity == NULL) {
    SAGE_DebugLog("entity is NULL !");
    return;
  }
  SAGE_DebugLog(" => ax=%d  ay=%d  az=%d", entity->anglex, entity->angley, entity->anglez);
  SAGE_DebugLog(" => px=%f  py=%f  pz=%f  radius=%f", entity->posx, entity->posy, entity->posz, entity->radius);
  SAGE_DebugLog(" => disabled=%d  culled=%d  clipped=%d", (entity->disabled ? 1 : 0), (entity->culled ? 1 : 0), (entity->clipped ? 1 : 0));
  SAGE_DebugLog(" => nbvertices=%d  nbfaces=%d  lod=%d", entity->nb_vertices, entity->nb_faces, entity->lod);
  if (mode & S3DE_DEBUG_EVERTS) {
    SAGE_DebugLog("-- Vertices");
    vertices = entity->vertices;
    for (index = 0;index < entity->nb_vertices;index++) {
      SAGE_DebugLog(" => vertex %d : x=%f  y=%f  z=%f", index, vertices[index].x, vertices[index].y, vertices[index].z);
    }
  }
  if (mode & S3DE_DEBUG_EFACES) {
    SAGE_DebugLog("-- Faces");
    faces = entity->faces;
    for (index = 0;index < entity->nb_faces;index++) {
      if (faces[index].is_quad) {
        SAGE_DebugLog(" => face %d : p1=%d  p2=%d  p3=%d  p4=%d  color=0x%06X  tex=%d  culled=%d  clipped=%d",
          index, faces[index].p1, faces[index].p2, faces[index].p3, faces[index].p4, faces[index].color,
          faces[index].texture, (faces[index].culled ? 1 : 0), faces[index].clipped
        );
        SAGE_DebugLog("             u1,v1=%f,%f  u2,v2=%f,%f  u3,v3=%f,%f  u4,v4=%f,%f",
          faces[index].u1, faces[index].v1, faces[index].u2, faces[index].v2,
          faces[index].u3, faces[index].v3, faces[index].u4, faces[index].v4
        );
      } else {
        SAGE_DebugLog(" => face %d : p1=%d  p2=%d  p3=%d  color=0x%06X  tex=%d  culled=%d  clipped=%d",
          index, faces[index].p1, faces[index].p2, faces[index].p3, faces[index].color,
          faces[index].texture, (faces[index].culled ? 1 : 0), faces[index].clipped
        );
        SAGE_DebugLog("             u1,v1=%f,%f  u2,v2=%f,%f  u3,v3=%f,%f",
          faces[index].u1, faces[index].v1, faces[index].u2, faces[index].v2,
          faces[index].u3, faces[index].v3
        );
      }
    }
  }
  if (mode & S3DE_DEBUG_ENORMS) {
    SAGE_DebugLog("-- Normals");
    normals = entity->normals;
    for (index = 0;index < entity->nb_faces;index++) {
      SAGE_DebugLog(" => normal %d : x=%f  y=%f  z=%f", index, normals[index].x, normals[index].y, normals[index].z);
    }
  }
}

VOID SAGE_DumpRemapVertex(SAGE_RemapVertex *remap, UWORD nb_vertices)
{
  UWORD idx;
  
  SAGE_DebugLog("** Dump remap vertex **");
  for (idx = 0;idx < nb_vertices;idx++) {
    if (remap[idx].replaced_by == idx) {
      SAGE_DebugLog("- vertex #%d has new index %d", idx, remap[idx].new_index);
    } else {
      SAGE_DebugLog("- vertex #%d has new index %d and is replaced by %d", idx, remap[idx].new_index, remap[idx].replaced_by);
    }
  }
}

#endif

/*****************************************************************************
 *                   END DEBUG
 *****************************************************************************/

/**
 * Create an empty entity
 */
SAGE_Entity *SAGE_CreateEntity(UWORD nb_vertices, UWORD nb_faces)
{
  SAGE_Entity *entity;

  SD(SAGE_DebugLog("Create entity (%d, %d)", nb_vertices, nb_faces);)
  if (nb_vertices >= S3DE_MAX_VERTICES) {
    SAGE_SetError(SERR_ENTITY_SIZE);
    return NULL;
  }
  entity = (SAGE_Entity *)SAGE_AllocMem(sizeof(SAGE_Entity));
  if (entity != NULL) {
    entity->nb_vertices = nb_vertices;
    entity->nb_faces = nb_faces;
    entity->vertices = (SAGE_Vertex *)SAGE_AllocMem(sizeof(SAGE_Vertex) * nb_vertices);
    entity->faces = (SAGE_Face *)SAGE_AllocMem(sizeof(SAGE_Face) * nb_faces);
    entity->normals = (SAGE_Vector *)SAGE_AllocMem(sizeof(SAGE_Vector) * nb_faces);
    if (entity->vertices != NULL && entity->faces != NULL && entity->normals != NULL) {
      return entity;
    }
    SAGE_ReleaseEntity(entity);
  }
  return NULL;
}

/**
 * Initialize an entity (calc radius/normals)
 */
VOID SAGE_InitEntity(SAGE_Entity *entity)
{
  SD(SAGE_DebugLog("Init entity");)
  if (entity != NULL) {
    SAGE_SetEntityRadius(entity);
    SAGE_SetEntityNormals(entity);
  }
  SD(SAGE_DumpEntity(entity, S3DE_DEBUG_EALL);)
}

/**
 * Compute an array for remapping entity vertices
 */
UWORD SAGE_ComputeRemapVertex(SAGE_Entity *entity, SAGE_RemapVertex *remap)
{
  UWORD vertice_idx, search_idx, new_index, replaced_by;
  SAGE_Vertex *vertices;
  
  SD(SAGE_DebugLog("- Compute remap vertices");)
  remap[0].new_index = 0;
  remap[0].replaced_by = 0;
  new_index = 1;
  vertices = entity->vertices;
  for (vertice_idx = 1; vertice_idx < entity->nb_vertices;vertice_idx++) {
    SAGE_DebugLog(" . vertex #%d x=%f  y=%f  z=%f", vertice_idx, vertices[vertice_idx].x, vertices[vertice_idx].y, vertices[vertice_idx].z);
    replaced_by = vertice_idx;
    for (search_idx = 0;search_idx < vertice_idx;search_idx++) {
      SAGE_DebugLog("  => compare to #%d x=%f  y=%f  z=%f", search_idx, vertices[search_idx].x, vertices[search_idx].y, vertices[search_idx].z);
      if (vertices[vertice_idx].x == vertices[search_idx].x && vertices[vertice_idx].y == vertices[search_idx].y && vertices[vertice_idx].z == vertices[search_idx].z) {
        SAGE_DebugLog("  => vertex #%d will be replaced by #%d", vertice_idx, search_idx);
        replaced_by = search_idx;
        break;
      }
    }
    remap[vertice_idx].new_index = new_index;
    remap[vertice_idx].replaced_by = replaced_by;
    if (replaced_by == vertice_idx) {
      SAGE_DebugLog("  => vertex #%d will have new index #%d", vertice_idx, new_index);
      new_index++;
    }
  }
  return new_index;
}

VOID SAGE_RemapEntity(SAGE_Entity *entity, SAGE_RemapVertex *remap, UWORD new_nb_vertices)
{
  SAGE_Vertex *old_vertices, *new_vertices;
  SAGE_Face *faces;
  UWORD vertice_idx, remap_idx, face_idx, new_index, replaced_by;

  SD(SAGE_DebugLog("- Remap entity");)
  new_vertices = (SAGE_Vertex *)SAGE_AllocMem(sizeof(SAGE_Vertex) * new_nb_vertices);
  if (new_vertices != NULL) {
    SD(SAGE_DebugLog(" * Remap vertices");)
    remap_idx = 0;
    old_vertices = entity->vertices;
    for (vertice_idx = 0; vertice_idx < entity->nb_vertices;vertice_idx++) {
      SAGE_DebugLog(" . old vertex #%d x=%f  y=%f  z=%f", vertice_idx, old_vertices[vertice_idx].x, old_vertices[vertice_idx].y, old_vertices[vertice_idx].z);
      if (remap[vertice_idx].replaced_by == vertice_idx) {
        new_vertices[remap_idx].x = old_vertices[vertice_idx].x;
        new_vertices[remap_idx].y = old_vertices[vertice_idx].y;
        new_vertices[remap_idx].z = old_vertices[vertice_idx].z;
        SAGE_DebugLog(" =>  new vertex #%d x=%f  y=%f  z=%f", remap_idx, new_vertices[remap_idx].x, new_vertices[remap_idx].y, new_vertices[remap_idx].z);
        remap_idx++;
      }
    }
    entity->nb_vertices = new_nb_vertices;
    entity->vertices = new_vertices;
    SAGE_FreeMem(old_vertices);
    SD(SAGE_DebugLog(" * Remap faces");)
    faces = entity->faces;
    for (face_idx = 0;face_idx < entity->nb_faces;face_idx++) {
      replaced_by = remap[faces[face_idx].p1].replaced_by;
      new_index = remap[replaced_by].new_index;
      faces[face_idx].p1 = new_index;
      SAGE_DebugLog(" . face #%d P1 is remaped to index %d", face_idx, new_index);
      replaced_by = remap[faces[face_idx].p2].replaced_by;
      new_index = remap[replaced_by].new_index;
      faces[face_idx].p2 = new_index;
      SAGE_DebugLog(" . face #%d P2 is remaped to index %d", face_idx, new_index);
      replaced_by = remap[faces[face_idx].p3].replaced_by;
      new_index = remap[replaced_by].new_index;
      faces[face_idx].p3 = new_index;
      SAGE_DebugLog(" . face #%d P1 is remaped to index %d", face_idx, new_index);
      if (faces[face_idx].is_quad) {
        replaced_by = remap[faces[face_idx].p4].replaced_by;
        new_index = remap[replaced_by].new_index;
        faces[face_idx].p4 = new_index;
        SAGE_DebugLog(" . face #%d P1 is remaped to index %d", face_idx, new_index);
      }
    }
  }
}

/**
 * Optimize an entity, remove duplicate vertices
 */
BOOL SAGE_OptimizeEntity(SAGE_Entity *entity)
{
  SAGE_RemapVertex *remap;
  UWORD new_index;
  
  SD(SAGE_DebugLog("Optimize entity");)
  if (entity != NULL) {
    remap = (SAGE_RemapVertex *)SAGE_AllocMem(sizeof(SAGE_RemapVertex) * entity->nb_vertices);
    if (remap != NULL) {
      new_index = SAGE_ComputeRemapVertex(entity, remap);
      SD(SAGE_DumpRemapVertex(remap, entity->nb_vertices);)
      if (new_index != entity->nb_vertices) {
        SAGE_RemapEntity(entity, remap, new_index);
      }
      SAGE_FreeMem(remap);
    } else {
      return FALSE;
    }
  }
  SD(SAGE_DumpEntity(entity, S3DE_DEBUG_EALL);)
  return TRUE;
}

/**
 * Clone an entity
 */
SAGE_Entity *SAGE_CloneEntity(SAGE_Entity *entity)
{
  SAGE_Entity *new_entity;
  UWORD idx;

  SD(SAGE_DebugLog("Clone entity");)
  new_entity = SAGE_CreateEntity(entity->nb_vertices, entity->nb_faces);
  if (new_entity != NULL && entity != NULL) {
    new_entity->anglex = entity->anglex;
    new_entity->angley = entity->angley;
    new_entity->anglez = entity->anglez;
    new_entity->posx = entity->posx;
    new_entity->posy = entity->posy;
    new_entity->posz = entity->posz;
    new_entity->radius = entity->radius;
    new_entity->disabled = entity->disabled;
    new_entity->nb_vertices = entity->nb_vertices;
    new_entity->nb_faces = entity->nb_faces;
    new_entity->lod = entity->lod;
    // Copy vertices
    for (idx = 0;idx < entity->nb_vertices;idx++) {
      new_entity->vertices[idx].x = entity->vertices[idx].x;
      new_entity->vertices[idx].y = entity->vertices[idx].y;
      new_entity->vertices[idx].z = entity->vertices[idx].z;
    }
    // Copy faces & normals
    for (idx = 0;idx < entity->nb_faces;idx++) {
      new_entity->faces[idx].is_quad = entity->faces[idx].is_quad; 
      new_entity->faces[idx].p1 = entity->faces[idx].p1;
      new_entity->faces[idx].p2 = entity->faces[idx].p2;
      new_entity->faces[idx].p3 = entity->faces[idx].p3;
      new_entity->faces[idx].p4 = entity->faces[idx].p4;
      new_entity->faces[idx].color = entity->faces[idx].color;
      new_entity->faces[idx].texture = entity->faces[idx].texture;
      new_entity->faces[idx].u1 = entity->faces[idx].u1;
      new_entity->faces[idx].v1 = entity->faces[idx].v1;
      new_entity->faces[idx].u2 = entity->faces[idx].u2;
      new_entity->faces[idx].v2 = entity->faces[idx].v2;
      new_entity->faces[idx].u3 = entity->faces[idx].u3;
      new_entity->faces[idx].v3 = entity->faces[idx].v3;
      new_entity->faces[idx].u4 = entity->faces[idx].u4;
      new_entity->faces[idx].v4 = entity->faces[idx].v4;
      new_entity->normals[idx].x = entity->normals[idx].x;
      new_entity->normals[idx].y = entity->normals[idx].y;
      new_entity->normals[idx].z = entity->normals[idx].z;
    }
    return new_entity;
  }
  return NULL;
}

/**
 * Release an entity
 */
VOID SAGE_ReleaseEntity(SAGE_Entity *entity)
{
  if (entity != NULL) {
    if (entity->vertices != NULL) {
      SAGE_FreeMem(entity->vertices);
    }
    if (entity->faces != NULL) {
      SAGE_FreeMem(entity->faces);
    }
    if (entity->normals != NULL) {
      SAGE_FreeMem(entity->normals);
    }
    SAGE_FreeMem(entity);
  }
}

/**
 * Get the type of an entity file
 * 
 * @param file_handle Handle on a file
 * 
 * @return Type of entity file
 */
UWORD SAGE_GetEntityFileType(BPTR file_handle)
{
  BYTE byte;
  LONG bytes_read, entity_tag;

  // Check for Ligthwave object
  bytes_read = Seek(file_handle, S3DE_LWOBOFFSET, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &entity_tag, 4);
  if (bytes_read != 4) {
    SAGE_SetError(SERR_READFILE);
    return S3DE_UNDEFINED;
  }
  if (entity_tag == S3DE_LWOBTAG) {
    SD(SAGE_DebugLog("This is a Ligthwave object");)
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return S3DE_LWOB;
  }
  // Check for Wavefront object
  bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
  bytes_read = Read(file_handle, &byte, 1);
  if (bytes_read != 1) {
    SAGE_SetError(SERR_READFILE);
    return S3DE_UNDEFINED;
  }
  if (byte >= ' ' && byte < 'Z') {
    SD(SAGE_DebugLog("This is a Wavefront object");)
    bytes_read = Seek(file_handle, 0, OFFSET_BEGINNING);
    return S3DE_WFOB;
  }
  return S3DE_UNDEFINED;
}

/**
 * Load an entity from a file
 */
SAGE_Entity *SAGE_LoadEntity(STRPTR filename)
{
  SAGE_Entity *entity;
  BPTR file_handle;
  UWORD type;

  SD(SAGE_DebugLog("Load entity %s", filename);)
  entity = NULL;
  file_handle = Open(filename, MODE_OLDFILE);
  if (file_handle != 0) {
    type = SAGE_GetEntityFileType(file_handle);
    if (type == S3DE_LWOB) {
      entity = SAGE_LoadLWO(file_handle);
    } else if (type == S3DE_WFOB) {
      entity = SAGE_LoadOBJ(file_handle, filename);
    } else {
      SAGE_SetError(SERR_FILEFORMAT);
    }
    Close(file_handle);
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  return entity;
}

/**
 * Calculate the entity radius
 */
VOID SAGE_SetEntityRadius(SAGE_Entity *entity)
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
VOID SAGE_SetEntityNormals(SAGE_Entity *entity)
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
    SAGE_CrossProduct(&(entity->normals[idx]), &u, &v);
    // Normalize vector
    SAGE_Normalize(&(entity->normals[idx]));
  }
}

/**
 * Add an entity to the world
 */
BOOL SAGE_AddEntity(UWORD index, SAGE_Entity *entity)
{
  SD(SAGE_DebugLog("Add entity #%d", index);)
  if (index >= S3DE_MAX_ENTITIES) {
    SAGE_SetError(SERR_ENTITY_INDEX);
    return FALSE;
  }
  // Clean the place
  if (sage_world.entities[index] != NULL) {
    SAGE_RemoveEntity(index);
  }
  sage_world.entities[index] = entity;
  sage_world.nb_entities++;
  return TRUE;
}

/**
 * Remove entity from the world
 */
VOID SAGE_RemoveEntity(UWORD index)
{
  SAGE_Entity *entity;

  SD(SAGE_DebugLog("Remove entity #%d", index);)
  if (index < S3DE_MAX_ENTITIES) {
    entity = sage_world.entities[index];
    if (entity != NULL) {
      SAGE_ReleaseEntity(entity);
      sage_world.entities[index] = NULL;
      sage_world.nb_entities--;
    }
  } else {
    SAGE_SetError(SERR_ENTITY_INDEX);
  }
}

/**
 * Release all entities
 */
VOID SAGE_FlushEntities()
{
  UWORD index;
  SAGE_Entity *entity;
  
  SD(SAGE_DebugLog("Flush entities (%d)", S3DE_MAX_ENTITIES);)
  for (index = 0;index < S3DE_MAX_ENTITIES;index++) {
    entity = sage_world.entities[index];
    SAGE_ReleaseEntity(entity);
    sage_world.entities[index] = NULL;
  }
  sage_world.nb_entities = 0;
}

/**
 * Get an entity from his index
 */
SAGE_Entity *SAGE_GetEntity(UWORD index)
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
 * Keep entity angle between 0 and SMTH_ANGLE_360
 *
 * @param entity Entity
 */
VOID SAGE_ClampEntityAngle(SAGE_Entity *entity)
{
  while (entity->anglex < 0) entity->anglex += SMTH_ANGLE_360;
  while (entity->anglex >= SMTH_ANGLE_360) entity->anglex-= SMTH_ANGLE_360;
  while (entity->angley < 0) entity->angley += SMTH_ANGLE_360;
  while (entity->angley >= SMTH_ANGLE_360) entity->angley-= SMTH_ANGLE_360;
  while (entity->anglez < 0) entity->anglez += SMTH_ANGLE_360;
  while (entity->anglez >= SMTH_ANGLE_360) entity->anglez-= SMTH_ANGLE_360;
}

/**
 * Set the entity angle
 */
BOOL SAGE_SetEntityAngle(UWORD index, WORD ax, WORD ay, WORD az)
{
  SAGE_Entity *entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->anglex = ax;
    entity->angley = ay;
    entity->anglez = az;
    SAGE_ClampEntityAngle(entity);
    return TRUE;
  }
  return FALSE;
}

/**
 * Rotate the entity
 */
BOOL SAGE_RotateEntity(UWORD index, WORD dax, WORD day, WORD daz)
{
  SAGE_Entity *entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->anglex += dax;
    entity->angley += day;
    entity->anglez += daz;
    SAGE_ClampEntityAngle(entity);
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the entity position
 */
BOOL SAGE_SetEntityPosition(UWORD index, FLOAT posx, FLOAT posy, FLOAT posz)
{
  SAGE_Entity *entity;
  
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
BOOL SAGE_MoveEntity(UWORD index, FLOAT dx, FLOAT dy, FLOAT dz)
{
  SAGE_Entity *entity;
  
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
 * Hide the entity
 */
BOOL SAGE_HideEntity(UWORD index)
{
  SAGE_Entity *entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->disabled = TRUE;
    return TRUE;
  }
  return FALSE;
}

/**
 * Show the entity
 */
BOOL SAGE_ShowEntity(UWORD index)
{
  SAGE_Entity *entity;
  
  entity = SAGE_GetEntity(index);
  if (entity != NULL) {
    entity->disabled = FALSE;
    return TRUE;
  }
  return FALSE;
}

/**
 * Set the entity texture
 */
BOOL SAGE_SetEntityTexture(UWORD idx_entity, UWORD idx_mat, UWORD idx_tex, UWORD mode)
{
  SAGE_Entity *entity;
  SAGE_3DTexture *texture;
  UWORD index, size;

  entity = SAGE_GetEntity(idx_entity);
  texture = SAGE_GetTexture(idx_tex);
  if (entity != NULL && texture != NULL) {
    size = texture->size - 1;
    for (index = 0;index < entity->nb_faces;index++) {
      if (entity->faces[index].texture == idx_mat) {
        entity->faces[index].texture = idx_tex;
        if (mode == S3DE_TEXT_RECALC) {
          entity->faces[index].u1 *= size;
          entity->faces[index].v1 *= size;
          entity->faces[index].u2 *= size;
          entity->faces[index].v2 *= size;
          entity->faces[index].u3 *= size;
          entity->faces[index].v3 *= size;
          if (entity->faces[index].is_quad) {
            entity->faces[index].u4 *= size;
            entity->faces[index].v4 *= size;
          }
        }
      }
    }
    return TRUE;
  }
  return FALSE;
}
