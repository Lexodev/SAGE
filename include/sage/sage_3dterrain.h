/**
 * sage_3dterrain.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D terrain management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#ifndef _SAGE_3DTERRAIN_H_
#define _SAGE_3DTERRAIN_H_

#include <exec/types.h>

#include <sage/sage_picture.h>
#include <sage/sage_3dstruct.h>

#define S3DT_MINSIZE          64
#define S3DT_MAXSIZE          128
#define S3DT_MAX_ZONES        1024
#define S3DT_VERTICES_ZONE    9
#define S3DT_CENTER_ZONE      4
#define S3DT_CELLS_ZONE       8
#define S3DT_CELL_SIZE        4.0
#define S3DT_HEIGHT_ZOOM      1.0

/** Terrain definition */
typedef struct {
  FLOAT posx, posy, posz, radius;
  BOOL disabled, culled, clipped;
  UWORD nb_faces, lod;
  SAGE_Face *faces;
  SAGE_Vector *normals;
} SAGE_Zone;

typedef struct {
  UBYTE *heightmap;
  ULONG colors[SPIC_MAXCOLORS];
  UWORD size, nb_vertices, nb_zones;
  FLOAT cell_size, height_zoom;
  SAGE_Vertex *vertices;
  SAGE_Zone *zones[S3DT_MAX_ZONES];
} SAGE_Terrain;

/** Calculate the zone radius */
VOID SAGE_SetZoneRadius(SAGE_Zone *);

/** Calculate zone faces normal */
VOID SAGE_SetZoneNormals(SAGE_Zone *);

/** Load a heightmap terrain */
BOOL SAGE_LoadHeightmapTerrain(STRPTR, STRPTR, STRPTR);

/** Set the size in unit of a cell in a zone */
VOID SAGE_SetHeighmapCellSize(FLOAT);

/** Set the zoom factor of a point height */
VOID SAGE_SetHeightmapZoom(FLOAT);

/** Release 3D terrain resources */
VOID SAGE_ReleaseTerrain(VOID);

#endif
