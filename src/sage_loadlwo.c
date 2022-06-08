/**
 * sage_loadlwo.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Ligthwave object loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <dos/dos.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_screen.h"
#include "sage_loadlwo.h"

#include <proto/dos.h>
#include <proto/exec.h>

/**
 * Dump an object (DEBUG)
 */
VOID SAGE_DumpLWOB(SAGE_LightwaveObject * object)
{
  LONG idx;

  SAGE_DebugLog("Dump LWOB");
  SAGE_DebugLog("Object points %d", object->nb_points);
  for (idx = 0;idx < object->nb_points;idx++) {
    SAGE_DebugLog(" => point %d : x=%f, y=%f, z=%f", idx, object->points[idx].x, object->points[idx].y, object->points[idx].z);
  }
  SAGE_DebugLog("Object polygons %d", object->nb_polygons);
  for (idx = 0;idx < object->nb_polygons;idx++) {
    SAGE_DebugLog(
      " => polygon %d : pts=%d, p1=%d, p2=%d, p3=%d, p4=%d, surf=%d",
      idx, object->polygons[idx].nb_points,
      object->polygons[idx].p1, object->polygons[idx].p2, object->polygons[idx].p3, object->polygons[idx].p4,
      object->polygons[idx].surface
    );
  }
}

/**
 * Release a LWO object
 */
VOID SAGE_ReleaseLWO(SAGE_LightwaveObject * object)
{
  SD(SAGE_DebugLog("Release LWO"));
  if (object != NULL) {
    if (object->surfaces != NULL) {
      SAGE_FreeMem(object->surfaces);
    }
    if (object->points != NULL) {
      SAGE_FreeMem(object->points);
    }
    if (object->polygons != NULL) {
      SAGE_FreeMem(object->polygons);
    }
    SAGE_FreeMem(object);
  }
}

/**
 * Read a full chunk
 */
BOOL SAGE_ReadChunk(BPTR fd, LONG chunk_size)
{
  BYTE byte;
  LONG bytes_read;
  
  while (chunk_size--) {
    bytes_read = Read(fd, &byte, sizeof(BYTE));
    if (bytes_read != sizeof(BYTE)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Load surfaces
 */
BOOL SAGE_LoadSurfaces(BPTR fd, LONG chunk_size, SAGE_LightwaveObject * object)
{
  LONG s, octet, pos, idx;
  BYTE c, name[256];
  
  SD(SAGE_DebugLog("Loading surfaces"));
  pos = 0;
  idx = 1;
  for (s = 0;s < chunk_size;s++) {
    octet = Read(fd, &c, 1);
    if (octet == 1) {
      if (c != 0) {
        name[pos] = c;
        pos++;
      } else {
        if (pos > 0) {
          name[pos] = c;
          pos = 0;
          idx++;
        }
      }
    } else {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Load points
 */
BOOL SAGE_LoadPoints(BPTR fd, LONG chunk_size, SAGE_LightwaveObject * object)
{
  LONG idx, bytes_read;
  FLOAT points[3];

  object->nb_points = chunk_size/4/3;
  SD(SAGE_DebugLog("Loading %d points", object->nb_points));
  object->points = (SAGE_LWOPoint *)SAGE_AllocMem(sizeof(SAGE_LWOPoint)*object->nb_points);
  if (object->points == NULL) {
    return FALSE;
  }
  for (idx = 0;idx < object->nb_points;idx++) {
    bytes_read = Read(fd, &points, 12);   // 3 points as float = 12 bytes
    if (bytes_read == 12) {
      object->points[idx].x = points[0] * S3DE_LWOZOOM;
      object->points[idx].y = points[1] * S3DE_LWOZOOM;
      object->points[idx].z = points[2] * S3DE_LWOZOOM;
    } else {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Load polygons
 */
BOOL SAGE_LoadPolygons(BPTR fd, LONG chunk_size, SAGE_LightwaveObject * object)
{
  LONG idx, polygon, bytes_read;
  short * polygons, nb_points, nb_polygons;
  
  SD(SAGE_DebugLog("Loading polygons"));
  if ((polygons = SAGE_AllocMem(chunk_size)) != NULL) {
    bytes_read = Read(fd, polygons, chunk_size);
    if (bytes_read != chunk_size) {
      SAGE_FreeMem(polygons);
      return FALSE;
    }
    nb_polygons = 0;
    idx = 0;
    // let's calculate the number of polygons
    while (idx < (bytes_read/2)) {
      nb_points = polygons[idx];     // nb of points in the face
      nb_polygons++;
      idx += (nb_points+2);
    }
    object->nb_polygons = nb_polygons;
    SD(SAGE_DebugLog(" found %d polygons", object->nb_polygons));
    object->polygons = (SAGE_LWOPolygon *)SAGE_AllocMem(sizeof(SAGE_LWOPolygon)*object->nb_polygons);
    if (object->nb_polygons == NULL) {
      SAGE_FreeMem(polygons);
      return FALSE;
    }
    idx = 0;
    polygon = 0;
    while (nb_polygons--) {
      nb_points = polygons[idx++];
      object->polygons[polygon].nb_points = nb_points;
      SD(SAGE_DebugLog(" polygon %d has %d points", polygon, nb_points));
      if (nb_points > 4) {
        SD(SAGE_DebugLog(" * number of points are not supported, clipping to 4 points"));
        object->polygons[polygon].nb_points = 4;
        object->polygons[polygon].p1 = polygons[idx];
        object->polygons[polygon].p2 = polygons[idx+1];
        object->polygons[polygon].p3 = polygons[idx+2];
        object->polygons[polygon].p4 = polygons[idx+3];
        object->polygons[polygon].surface = polygons[idx+4];
      } else if (nb_points == 4) {
        object->polygons[polygon].p1 = polygons[idx];
        object->polygons[polygon].p2 = polygons[idx+1];
        object->polygons[polygon].p3 = polygons[idx+2];
        object->polygons[polygon].p4 = polygons[idx+3];
        object->polygons[polygon].surface = polygons[idx+4];
      } else {
        object->polygons[polygon].p1 = polygons[idx];
        object->polygons[polygon].p2 = polygons[idx+1];
        object->polygons[polygon].p3 = polygons[idx+2];
        object->polygons[polygon].surface = polygons[idx+3];
      }
      polygon++;
      idx += (nb_points+1);
    }
    SAGE_FreeMem(polygons);
    return TRUE;
  }
  return FALSE;
}

/**
 * Load a surface
 */
BOOL SAGE_LoadSurface(BPTR fd, LONG chunk_size, SAGE_LightwaveObject * object)
{
  SD(SAGE_DebugLog("Loading surface"));
  SAGE_ReadChunk(fd, chunk_size);
  return TRUE;
}

/**
 * Load a Lightwave object
 * 
 * @param file_handle Object file handle
 * 
 * @return SAGE entity structure
 */
SAGE_Entity * SAGE_LoadLWO(BPTR file_handle)
{
  SAGE_LightwaveObject * object;
  SAGE_Entity * entity;
  LONG bytes_read, chunk_id, chunk_size, idx;
  BOOL fin, error;

  SD(SAGE_DebugLog("Load LWO"));
  object = (SAGE_LightwaveObject *)SAGE_AllocMem(sizeof(SAGE_LightwaveObject));
  if (object == NULL) {
    return NULL;
  }
  entity = NULL;
  // Skip first data
  bytes_read = Seek(file_handle, 12, OFFSET_BEGINNING);
  fin = FALSE;
  error = FALSE;
  while (!fin) {
    bytes_read = Read(file_handle, &chunk_id, sizeof(ULONG));
    if (bytes_read != sizeof(ULONG)) {
      if (bytes_read < 0) {
        error = TRUE;
      }
      fin = TRUE;
    } else {
      bytes_read = Read(file_handle, &chunk_size, sizeof(ULONG));
      if (bytes_read != sizeof(ULONG)) {
        if (bytes_read < 0) {
          error = TRUE;
        }
        fin = TRUE;
      } else {
        switch (chunk_id) {
          case S3DE_SRFSTAG:
            if (!SAGE_LoadSurfaces(file_handle, chunk_size, object)) {
              error = TRUE;
              fin = TRUE;
            }
            break;
          case S3DE_PNTSTAG:
            if (!SAGE_LoadPoints(file_handle, chunk_size, object)) {
              error = TRUE;
              fin = TRUE;
            }
            break;
          case S3DE_POLSTAG:
            if (!SAGE_LoadPolygons(file_handle, chunk_size, object)) {
              error = TRUE;
              fin = TRUE;
            }
            break;
          case S3DE_SURFTAG:
            if (!SAGE_LoadSurface(file_handle, chunk_size, object)) {
              error = TRUE;
              fin = TRUE;
            }
            break;
          default:
            SAGE_ReadChunk(file_handle, chunk_size);
            break;
        }
      }
    }
  }
  if (!error) {
    SD(SAGE_DumpLWOB(object));
    entity = SAGE_CreateEntity(object->nb_points, object->nb_polygons);
    if (entity != NULL) {
      for (idx = 0;idx < object->nb_points;idx++) {
        entity->vertices[idx].x = object->points[idx].x;
        entity->vertices[idx].y = object->points[idx].y;
        entity->vertices[idx].z = object->points[idx].z;
      }
      for (idx = 0;idx < object->nb_polygons;idx++) {
        entity->faces[idx].p1 = object->polygons[idx].p1;
        entity->faces[idx].p2 = object->polygons[idx].p2;
        entity->faces[idx].p3 = object->polygons[idx].p3;
        if (object->polygons[idx].nb_points == 4) {
          entity->faces[idx].is_quad = TRUE;
          entity->faces[idx].p4 = object->polygons[idx].p4;
        }
        entity->faces[idx].color = SAGE_RemapColor(object->polygons[idx].surface);
        entity->faces[idx].texture = object->polygons[idx].surface;
      }
    }
  }
  SAGE_ReleaseLWO(object);
  return entity;
}
