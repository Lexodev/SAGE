/**
 * sage_3dengine.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D engine functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
 */

#ifndef _SAGE_3DENGINE_H_
#define _SAGE_3DENGINE_H_

#include <exec/types.h>

#include <sage/sage_maths.h>
#include <sage/sage_3dstruct.h>
#include <sage/sage_3dcamera.h>
#include <sage/sage_3dentity.h>
#include <sage/sage_3dmaterial.h>
#include <sage/sage_3dskybox.h>
#include <sage/sage_3dterrain.h>

#define S3DE_ONEDEGREE        SMTH_PRECISION        // One degree unity
#define S3DE_HAFLDEGREE       SMTH_PRECISION/2      // Half degree unity
#define S3DE_QUARTERDEGREE    SMTH_PRECISION/4      // Quarter degree unity

#define S3DE_MAX_VERTICES     65000                 // Vertices buffer size
#define S3DE_CLIP_VERTICES    2                     // Small space for clipped vertices
#define S3DE_VERTEX_CLIP1     S3DE_MAX_VERTICES     // First clipped vertex
#define S3DE_VERTEX_CLIP2     S3DE_MAX_VERTICES+1   // Second clipped vertex
#define S3DE_MAX_ELEMENTS     4096

#define S3DE_NOCLIP           0
#define S3DE_P1CLIP           1L<<0
#define S3DE_P2CLIP           1L<<1
#define S3DE_P3CLIP           1L<<2
#define S3DE_P4CLIP           1L<<3
#define S3DE_MASKP4           (S3DE_P1CLIP|S3DE_P2CLIP|S3DE_P3CLIP)
#define S3DE_MASKP2           (S3DE_P1CLIP|S3DE_P4CLIP|S3DE_P3CLIP)

#define S3DE_LOD_FULL         0                     // No LOD reduction
#define S3DE_LOD_HIGH         1                     // Small LOD reduction
#define S3DE_LOD_MEDIUM       2                     // Average LOD reduction
#define S3DE_LOD_LOW          3                     // Huge LOD reduction

#define S3DE_LOD_L1           200.0                 // Distance of level 1 reduction
#define S3DE_LOD_L2           300.0                 // Distance of level 2 reduction
#define S3DE_LOD_L3           400.0                 // Distance of level 3 reduction

#if _SAGE_DEBUG_MODE_ == 1
#define SED(x)     if (engine_debug) { x }
#else
#define SED(x)
#endif

/** Engine metrics */
typedef struct {
  ULONG rendered_planes, total_planes;        // Skybox planes
  ULONG rendered_zones, total_zones;          // Terrain zones
  ULONG rendered_entities, total_entities;    // World entities
  ULONG calculated_vertices, rendered_vertices, total_vertices;   // World vertices
  ULONG rendered_faces, total_faces;          // World faces
  ULONG rendered_elements;                    // Rendered elements
} SAGE_EngineMetrics;

/** World structure */
typedef struct {
  ULONG active_camera;
  SAGE_Camera *cameras[S3DE_MAX_CAMERAS];
  UWORD nb_materials;
  SAGE_Material *materials[S3DE_MAX_MATERIALS];
  BOOL active_skybox;
  SAGE_Skybox skybox;
  BOOL active_terrain;
  SAGE_Terrain terrain;
  UWORD nb_entities;
  SAGE_Entity *entities[S3DE_MAX_ENTITIES];
  SAGE_TransformedVertex *transformed_vertices;
  SAGE_EngineMetrics metrics;
} SAGE_3DWorld;

/** Init the 3D engine */
BOOL SAGE_Init3DEngine(VOID);

/** Release the 3D engine */
VOID SAGE_Release3DEngine(VOID);

/** Render the 3D world */
VOID SAGE_RenderWorld(VOID);

/** Get the engine metrics */
SAGE_EngineMetrics *SAGE_GetEngineMetrics(VOID);

/** Enable/Disable debug */
VOID SAGE_EngineDebug(BOOL);

#endif
