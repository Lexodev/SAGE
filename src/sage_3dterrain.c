/**
 * sage_3dterrain.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D terrain management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

/**
 * How it works
 *
 * Heightmap is limited to size multiple of 8 with min of 64 and max of 248, it should be square
 * Heightmap is build from a CLUT picture but is extended to have one line and one column more (to have last point for the last zone)
 * Heightmap is divided in "zones", each zone is an entity of 9*9 vertices and 8*8*2 faces (only triangles)
 * Each zone is defined directly in the world coordinates because a zone is static (no rotation, no translation)
 * 
 */
 
#include <stdio.h>

#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_context.h>
#include <sage/sage_memory.h>
#include <sage/sage_screen.h>
#include <sage/sage_3dterrain.h>

#include <sage/sage_debug.h>

/** SAGE context */
extern SAGE_Context SageContext;

/** Engine data */
extern SAGE_3DWorld sage_world;

/*****************************************************************************
 *                   START DEBUG
 *****************************************************************************/

#if _SAGE_DEBUG_MODE_ == 1
VOID SAGE_DumpZone(SAGE_Zone *zone)
{
  UWORD index;
  SAGE_Face *faces;
  SAGE_Vector *normals;
  
  SAGE_DebugLog(" => px=%f  py=%f  pz=%f  radius=%f  lod=%d",zone->posx, zone->posy, zone->posz, zone->radius, zone->lod);
  SAGE_DebugLog(" => nbfaces=%d  disabled=%d  culled=%d  clipped=%d", zone->nb_faces, (zone->disabled ? 1 : 0), (zone->culled ? 1 : 0), (zone->clipped ? 1 : 0));
  faces = zone->faces;
  normals = zone->normals;
  for (index = 0;index < zone->nb_faces;index++) {
    SAGE_DebugLog("  . face %d : p1=%d  p2=%d  p3=%d  color=0x%X  tex=%d  culled=%d  clipped=%d",
      index, faces[index].p1, faces[index].p2, faces[index].p3, faces[index].color,
      faces[index].texture, (faces[index].culled ? 1 : 0), faces[index].clipped
    );
    SAGE_DebugLog("            u1,v1=%f,%f  u2,v2=%f,%f  u3,v3=%f,%f",
      faces[index].u1, faces[index].v1, faces[index].u2, faces[index].v2,
      faces[index].u3, faces[index].v3
    );
    SAGE_DebugLog("  . normal %d : x=%f  y=%f  z=%f", index, normals[index].x, normals[index].y, normals[index].z);
  }
}

VOID SAGE_DumpTerrain(WORD mode)
{
  UWORD width, height, index;
  UBYTE *map;
  SAGE_Vertex *vertices;
  SAGE_Zone *zone;

  if (sage_world.active_terrain) {
    SAGE_DebugLog("** Dump Terrain **");
    SAGE_DebugLog(
      " size=%d  vertices=%d  zones=%d  cellsize=%f  hzoom=%f",
      sage_world.terrain.size, sage_world.terrain.nb_vertices, sage_world.terrain.nb_zones,
      sage_world.terrain.cell_size, sage_world.terrain.height_zoom
    );
    if (mode & S3DE_DEBUG_THMAP) {
      SAGE_DebugLog("-- Heightmap");
      map = sage_world.terrain.heightmap;
      index = 0;
      for (height = 0;height <= sage_world.terrain.size;height++) {
        printf(" %02X : ", height);
        for (width = 0;width <= sage_world.terrain.size;width++) {
          printf("%02X ", map[index++]);
        }
        printf("\n");
      }
      SAGE_DebugLog("-- Colormap");
      for (index = 0;index < SPIC_MAXCOLORS;index++) {
        printf("0x%08X ", sage_world.terrain.colors[index]);
        if ((index % 16) == 15) {
          printf("\n");
        }
      }
    }
    if (mode & S3DE_DEBUG_TVERTS) {
      SAGE_DebugLog("-- Vertices");
      vertices = sage_world.terrain.vertices;
      for (index = 0;index < sage_world.terrain.nb_vertices;index++) {
        SAGE_DebugLog(" => vertex %d : x=%f  y=%f  z=%f", index, vertices[index].x, vertices[index].y, vertices[index].z);
      }
    }
    if (mode & S3DE_DEBUG_TZONES) {
      for (index = 0;index < sage_world.terrain.nb_zones;index++) {
        SAGE_DebugLog("-- Zone %d", index);
        zone = sage_world.terrain.zones[index];
        SAGE_DumpZone(zone);
      }
    }
  } else {
    SAGE_DebugLog("** Terrain is not active **");
  }
}
#endif

/*****************************************************************************
 *                   END DEBUG
 *****************************************************************************/

/**
 * Release a zone
 */
VOID SAGE_ReleaseZone(SAGE_Zone *zone)
{
  if (zone != NULL) {
    if (zone->faces != NULL) {
      SAGE_FreeMem(zone->faces);
    }
    if (zone->normals != NULL) {
      SAGE_FreeMem(zone->normals);
    }
    SAGE_FreeMem(zone);
  }
}

/**
 * Create an empty zone 
 */
SAGE_Zone *SAGE_CreateZone(UWORD nb_faces)
{
  SAGE_Zone *zone;

  SD(SAGE_DebugLog("Create zone with %d faces", nb_faces);)
  zone = (SAGE_Zone *)SAGE_AllocMem(sizeof(SAGE_Zone));
  if (zone != NULL) {
    zone->nb_faces = nb_faces;
    zone->faces = (SAGE_Face *)SAGE_AllocMem(sizeof(SAGE_Face)*nb_faces);
    zone->normals = (SAGE_Vector *)SAGE_AllocMem(sizeof(SAGE_Vector)*nb_faces);
    if (zone->faces != NULL && zone->normals != NULL) {
      return zone;
    }
    SAGE_ReleaseZone(zone);
  }
  return NULL;
}

/**
 * Check if the heightmap has the required size
 *
 * @param width  Map width
 * @param height Map height
 *
 * @return Map has required size
 */
BOOL SAGE_CheckHeightmapSize(LONG width, LONG height)
{
  if (width != height) {
    return FALSE;
  }
  if (width < S3DT_MINSIZE || width > S3DT_MAXSIZE || (width % 8) != 0) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Init the heightmap
 *
 * @param hmpic Heightmap picture
 * @param cmpic Colormap picture
 * @param tmpic Texmap picture
 *
 * @return Operation success
 */
BOOL SAGE_PrepareHeightmap(SAGE_Picture *hmpic, SAGE_Picture *cmpic, SAGE_Picture *tmpic)
{
  UWORD width, height, src_idx, dst_idx, size, color;
  ULONG *cmap;
  UBYTE *src, *dst;

  SD(SAGE_DebugLog("Prepare the heightmap");)
  width = hmpic->bitmap->width;
  height = hmpic->bitmap->height;
  if (!SAGE_CheckHeightmapSize(width, height)) {
    SAGE_SetError(SERR_TERRAIN_SIZE);
    return FALSE;
  }
  if (cmpic != NULL && cmpic->bitmap->width != width) {
    SAGE_SetError(SERR_TERRAIN_SIZE);
    return FALSE;
  }
  if (tmpic != NULL && tmpic->bitmap->width != width) {
    SAGE_SetError(SERR_TERRAIN_SIZE);
    return FALSE;
  }
  size = width;
  if ((sage_world.terrain.heightmap = (UBYTE *)SAGE_AllocMem((size+1)*(size+1))) == NULL) {
    return FALSE;
  }
  src = (UBYTE *)hmpic->bitmap->bitmap_buffer;
  dst = sage_world.terrain.heightmap;
  src_idx = 0;
  dst_idx = 0;
  for (height = 0;height < size;height++) {
    for (width = 0;width < size;width++) {
      dst[dst_idx++] = src[src_idx++];
    }
    dst[dst_idx++] = src[src_idx-1];
  }
  src_idx = dst_idx - (size+1);
  for (width = 0;width <= size;width++) {
    dst[dst_idx++] = dst[src_idx++];
  }
  sage_world.terrain.size = size;
  SD(SAGE_DebugLog(" map is %dx%d", size, size);)
  SD(SAGE_DebugLog("Prepare the terrain colors");)
  if (cmpic != NULL) {
    cmap = cmpic->color_map;
  } else {
    cmap = hmpic->color_map;
  }
  for (color = 0;color < SPIC_MAXCOLORS;color++) {
    sage_world.terrain.colors[color] = cmap[color];
  }
  return TRUE;
}

/**
 * Create heightmap vertices
 *
 * @param heightmap Heightmap buffer
 * @param size      Heightmap size
 *
 * @return Operation success
 */
BOOL SAGE_BuildHeightmapVertices(UBYTE *hmap, UWORD size)
{
  UWORD width, idx_vertex, x, y;
  
  SD(SAGE_DebugLog("Setup heightmap vertices (%d)", size);)
  width = size + 1;
  sage_world.terrain.vertices = (SAGE_Vertex *)SAGE_AllocMem(sizeof(SAGE_Vertex)*width*width);
  if (sage_world.terrain.vertices == NULL) {
    return FALSE;
  }
  idx_vertex = 0;
  for (y = 0;y < width;y++) {
    for (x = 0;x < width;x++) {
      // px = x * cell_size
      sage_world.terrain.vertices[idx_vertex].x = (FLOAT)(x * S3DT_CELL_SIZE);
      // py = map[(y * width) + x] * hzoom
      sage_world.terrain.vertices[idx_vertex].y = (FLOAT)(hmap[(y * width) + x]) * S3DT_HEIGHT_ZOOM;
      // pz = -1 * ((y * cell_size) - (width * cell_size))
      sage_world.terrain.vertices[idx_vertex].z = (FLOAT)(-1 * ((y * S3DT_CELL_SIZE) - (size * S3DT_CELL_SIZE)));
      idx_vertex++;
    }
  }
  sage_world.terrain.nb_vertices = width * width;
  return TRUE;
}

/**
 * Calculate the zone radius
 */
VOID SAGE_SetZoneRadius(SAGE_Zone *zone)
{
  FLOAT radius, x, y, z;
  UWORD idx;

  zone->radius = 0.0;
  for (idx = 0;idx < zone->nb_faces;idx++) {
    x = sage_world.terrain.vertices[zone->faces[idx].p1].x - zone->posx;
    y = sage_world.terrain.vertices[zone->faces[idx].p1].y - zone->posy;
    z = sage_world.terrain.vertices[zone->faces[idx].p1].z - zone->posz;
    radius = sqrt((x*x) + (y*y) + (z*z));
    if (radius > zone->radius) {
      zone->radius = radius;
    }
    x = sage_world.terrain.vertices[zone->faces[idx].p2].x - zone->posx;
    y = sage_world.terrain.vertices[zone->faces[idx].p2].y - zone->posy;
    z = sage_world.terrain.vertices[zone->faces[idx].p2].z - zone->posz;
    radius = sqrt((x*x) + (y*y) + (z*z));
    if (radius > zone->radius) {
      zone->radius = radius;
    }
    x = sage_world.terrain.vertices[zone->faces[idx].p3].x - zone->posx;
    y = sage_world.terrain.vertices[zone->faces[idx].p3].y - zone->posy;
    z = sage_world.terrain.vertices[zone->faces[idx].p3].z - zone->posz;
    radius = sqrt((x*x) + (y*y) + (z*z));
    if (radius > zone->radius) {
      zone->radius = radius;
    }
  }
}

/**
 * Calculate zone faces normal
 */
VOID SAGE_SetZoneNormals(SAGE_Zone *zone)
{
  UWORD idx, p1, p2, p3;
  SAGE_Vector u, v;

  for (idx = 0;idx < zone->nb_faces;idx++) {
    p1 = zone->faces[idx].p1;
    p2 = zone->faces[idx].p2;
    p3 = zone->faces[idx].p3;
    u.x = sage_world.terrain.vertices[p2].x - sage_world.terrain.vertices[p1].x;
    u.y = sage_world.terrain.vertices[p2].y - sage_world.terrain.vertices[p1].y;
    u.z = sage_world.terrain.vertices[p2].z - sage_world.terrain.vertices[p1].z;
    v.x = sage_world.terrain.vertices[p3].x - sage_world.terrain.vertices[p1].x;
    v.y = sage_world.terrain.vertices[p3].y - sage_world.terrain.vertices[p1].y;
    v.z = sage_world.terrain.vertices[p3].z - sage_world.terrain.vertices[p1].z;
    // Calculate the normal
    SAGE_CrossProduct(&(zone->normals[idx]), &u, &v);
    // Normalize vector
    SAGE_Normalize(&(zone->normals[idx]));
  }
}

/**
 * Create a new heightmap zone from height map data
 *
 * @param startx  Start point x coord
 * @param starty  Start point y coord
 * @param hmap    Height map
 * @param cmap    Color map
 * @param palette Map palette
 * @param tmap    Texture map
 * @param size    Map size
 *
 * @return New heightmap zone
 */
SAGE_Zone *SAGE_CreateHeightmapZone(UWORD startx, UWORD starty, UBYTE *hmap, UBYTE *cmap, UBYTE *tmap, UWORD size)
{
  SAGE_Zone *zone;
  UWORD width, idx_face, x, y;
  LONG color;
  WORD texture;

  SD(SAGE_DebugLog("SAGE_CreateHeightmapZone(%d, %d)", startx, starty);)
  zone = SAGE_CreateZone((S3DT_CELLS_ZONE*2)*S3DT_CELLS_ZONE);
  if (zone == NULL) {
    return NULL;
  }
  width = size + 1;
  zone->posx = (FLOAT)((startx + S3DT_CENTER_ZONE) * S3DT_CELL_SIZE);
  zone->posy = (FLOAT)(hmap[(starty + S3DT_CENTER_ZONE) * width + (startx + S3DT_CENTER_ZONE)] * S3DT_HEIGHT_ZOOM);
  zone->posz = (FLOAT)(-1 * (((starty  + S3DT_CENTER_ZONE) * S3DT_CELL_SIZE) - (width * S3DT_CELL_SIZE)));
  idx_face = 0;
  for (y = 0;y < S3DT_CELLS_ZONE;y++) {
    for (x = 0;x < S3DT_CELLS_ZONE;x++) {
      if (cmap != NULL) {
        color = sage_world.terrain.colors[cmap[(startx + x) + ((starty + y) * size)]];
      } else {
        color = sage_world.terrain.colors[hmap[(startx + x) + ((starty + y) * width)]];
      }
      if (tmap != NULL) {
        texture = tmap[(startx + x) + ((starty + y) * size)];
      } else {
        texture = STEX_USECOLOR;
      }
      zone->faces[idx_face].is_quad = FALSE;
      zone->faces[idx_face].p1 = (startx + x) + ((starty + y) * width);
      zone->faces[idx_face].p2 = (startx + x + 1) + ((starty + y) * width);
      zone->faces[idx_face].p3 = (startx + x) + ((starty + y + 1) * width);
      zone->faces[idx_face].color = color;
      zone->faces[idx_face].texture = texture;
      idx_face++;
      zone->faces[idx_face].is_quad = FALSE;
      zone->faces[idx_face].p1 = (startx + x + 1) + ((starty + y) * width);
      zone->faces[idx_face].p2 = (startx + x + 1) + ((starty + y + 1) * width);
      zone->faces[idx_face].p3 = (startx + x) + ((starty + y + 1) * width);
      zone->faces[idx_face].color = color;
      zone->faces[idx_face].texture = texture;
      idx_face++;
    }
  }
  SAGE_SetZoneRadius(zone);
  SAGE_SetZoneNormals(zone);
  return zone;
}

/**
 * Create heightmap zones
 *
 * @param heightmap Heightmap buffer
 * @param size      Heightmap size
 * @param colormap  Color map
 * @param texmap    Texture map
 *
 * @return Operation success
 */
BOOL SAGE_BuildHeightmapZones(UBYTE *heightmap, UWORD size, SAGE_Picture *colormap, SAGE_Picture *texmap)
{
  UWORD zone, idx_zone, xvertex, yvertex;
  UBYTE *cmap, *tmap;

  SD(SAGE_DebugLog("Build heightmap zones");)
  zone = size / S3DT_CELLS_ZONE;
  sage_world.terrain.nb_zones = zone * zone;
  SD(SAGE_DebugLog("Map %dx%d        Number of zones %dx%d", size, size, zone, zone);)
  if (colormap != NULL) {
    cmap = (UBYTE *)colormap->bitmap->bitmap_buffer;
  } else {
    cmap = NULL;
  }
  if (texmap != NULL) {
    tmap = (UBYTE *)texmap->bitmap->bitmap_buffer;
  } else {
    tmap = NULL;
  }
  xvertex = 0;
  yvertex = 0;
  for (idx_zone = 0;idx_zone < sage_world.terrain.nb_zones;idx_zone++) {
    sage_world.terrain.zones[idx_zone] = SAGE_CreateHeightmapZone(xvertex, yvertex, heightmap, cmap, tmap, size);
    if (sage_world.terrain.zones[idx_zone] == NULL) {
      return FALSE;
    }
    xvertex += (S3DT_VERTICES_ZONE-1);
    if ((idx_zone % zone) == (zone - 1)) {
      xvertex = 0;
      yvertex += (S3DT_VERTICES_ZONE-1);
    }
  }
  return TRUE;
}

/**
 * Load a heightmap terrain
 *
 * @param heightmap Height map file
 * @param colormap  Color map file
 * @param texmap    Texture map file
 *
 * @return Operation success
 */
BOOL SAGE_LoadHeightmapTerrain(STRPTR heightmap, STRPTR colormap, STRPTR texmap)
{
  SAGE_Picture *hmpic = NULL, *cmpic = NULL, *tmpic = NULL;
  BOOL remap;

  SD(SAGE_DebugLog("Load heightmap terrain %s", heightmap);)
  // Disable auto remap before loading heightmap, colormap & texmap
  remap = SageContext.AutoRemap;
  SageContext.AutoRemap = FALSE;
  hmpic = SAGE_LoadPicture(heightmap);
  if (hmpic == NULL) {
    return FALSE;
  }
  if (colormap != NULL) {
    cmpic = SAGE_LoadPicture(colormap);
    if (cmpic == NULL) {
      SAGE_ReleasePicture(hmpic);
      SageContext.AutoRemap = remap;
      return FALSE;
    }
  }
  if (texmap != NULL) {
    tmpic = SAGE_LoadPicture(texmap);
    if (tmpic == NULL) {
      SAGE_ReleasePicture(hmpic);
      SAGE_ReleasePicture(cmpic);
      SageContext.AutoRemap = remap;
      return FALSE;
    }
  }
  SageContext.AutoRemap = remap;
  if (!SAGE_PrepareHeightmap(hmpic, cmpic, tmpic)) {
    SAGE_ReleasePicture(tmpic);
    SAGE_ReleasePicture(cmpic);
    SAGE_ReleasePicture(hmpic);
    return FALSE;
  }
  if (!SAGE_BuildHeightmapVertices(sage_world.terrain.heightmap, sage_world.terrain.size)) {
    SAGE_ReleasePicture(tmpic);
    SAGE_ReleasePicture(cmpic);
    SAGE_ReleasePicture(hmpic);
    return FALSE;
  }
  if (!SAGE_BuildHeightmapZones(sage_world.terrain.heightmap, sage_world.terrain.size, cmpic, tmpic)) {
    SAGE_ReleasePicture(tmpic);
    SAGE_ReleasePicture(cmpic);
    SAGE_ReleasePicture(hmpic);
    return FALSE;
  }
  SAGE_ReleasePicture(tmpic);
  SAGE_ReleasePicture(cmpic);
  SAGE_ReleasePicture(hmpic);
  sage_world.active_terrain = TRUE;
  SD(SAGE_DumpTerrain(S3DE_DEBUG_THMAP|S3DE_DEBUG_TVERTS|S3DE_DEBUG_TZONES);)
//  SD(SAGE_DumpTerrain(S3DE_DEBUG_TMIN);)
  return TRUE;
}

/**
 * Release 3D terrain resources
 */
VOID SAGE_ReleaseTerrain()
{
  ULONG index;
  SAGE_Zone *zone;
  
  SD(SAGE_DebugLog("Release terrain");)
  if (sage_world.terrain.heightmap != NULL) {
    SAGE_FreeMem(sage_world.terrain.heightmap);
    sage_world.terrain.heightmap = NULL;
  }
  sage_world.terrain.size = 0;
  if (sage_world.terrain.vertices != NULL) {
    SAGE_FreeMem(sage_world.terrain.vertices);
    sage_world.terrain.vertices = NULL;
  }
  sage_world.terrain.nb_vertices = 0;
  for (index = 0;index < sage_world.terrain.nb_zones;index++) {
    zone = sage_world.terrain.zones[index];
    SAGE_ReleaseZone(zone);
    sage_world.terrain.zones[index] = NULL;
  }
  sage_world.terrain.nb_zones = 0;
  sage_world.active_terrain = FALSE;
}
