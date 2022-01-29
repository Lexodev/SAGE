/**
 * sage_3dengine.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D engine functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#include <string.h>
#include <math.h>

#include <proto/exec.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_draw.h"
#include "sage_screen.h"
#include "sage_loadlwo.h"
#include "sage_3drender.h"
#include "sage_3dskybox.h"
#include "sage_3dengine.h"

/** Precalculs */
FLOAT Sinus[360*S3DE_PRECISION];
FLOAT Cosinus[360*S3DE_PRECISION];
FLOAT Tangente[360*S3DE_PRECISION];

/** Buffer for projected vertices */
SAGE_EntityVertex projected_vertices[S3DE_MAX_VERTICES+S3DE_CLIP_VERTICES];

/** Transformation matrix */
SAGE_Matrix EntityMatrix;
SAGE_Matrix CameraMatrix;

/** Our 3D world */
SAGE_3DWorld sage_world;

extern BOOL debug;

/*****************************************************************************/
//            DEBUG ONLY
/*****************************************************************************/

VOID SAGE_DumpEntityMatrix(VOID)
{
  SAGE_DebugLog("Dump entity matrix");
  SAGE_DebugLog(" => %f\t%f\t%f", EntityMatrix.m11, EntityMatrix.m12, EntityMatrix.m13);
  SAGE_DebugLog(" => %f\t%f\t%f", EntityMatrix.m21, EntityMatrix.m22, EntityMatrix.m23);
  SAGE_DebugLog(" => %f\t%f\t%f", EntityMatrix.m31, EntityMatrix.m32, EntityMatrix.m33);
}

VOID SAGE_DumpCameraMatrix(VOID)
{
  SAGE_DebugLog("Dump camera matrix");
  SAGE_DebugLog(" => %f\t%f\t%f", CameraMatrix.m11, CameraMatrix.m12, CameraMatrix.m13);
  SAGE_DebugLog(" => %f\t%f\t%f", CameraMatrix.m21, CameraMatrix.m22, CameraMatrix.m23);
  SAGE_DebugLog(" => %f\t%f\t%f", CameraMatrix.m31, CameraMatrix.m32, CameraMatrix.m33);
}

/*****************************************************************************/

/**
 * Setup the camera matrix
 */
VOID SAGE_SetupCameraMatrix(SAGE_Camera * camera)
{
  WORD ax, ay, az;

  ax = -camera->anglex;
  while (ax < 0) ax += S3DE_ANGLE_360;
  while (ax >= S3DE_ANGLE_360) ax-= S3DE_ANGLE_360;
  ay = -camera->angley;
  while (ay < 0) ay += S3DE_ANGLE_360;
  while (ay >= S3DE_ANGLE_360) ay-= S3DE_ANGLE_360;
  az = -camera->anglez;
  while (az < 0) az += S3DE_ANGLE_360;
  while (az >= S3DE_ANGLE_360) az-= S3DE_ANGLE_360;
  CameraMatrix.m11 = Cosinus[ay]*Cosinus[az];
  CameraMatrix.m12 = Cosinus[ay]*Sinus[az];
  CameraMatrix.m13 = -Sinus[ay];
  CameraMatrix.m21 = (Sinus[ax]*Sinus[ay]*Cosinus[az]) - (Cosinus[ax]*Sinus[az]);
  CameraMatrix.m22 = (Sinus[ax]*Sinus[ay]*Sinus[az]) + (Cosinus[ax]*Cosinus[az]);
  CameraMatrix.m23 = Sinus[ax]*Cosinus[ay];
  CameraMatrix.m31 = (Cosinus[ax]*Sinus[ay]*Cosinus[az]) + (Sinus[ax]*Sinus[az]);
  CameraMatrix.m32 = (Cosinus[ax]*Sinus[ay]*Sinus[az]) - (Sinus[ax]*Cosinus[az]);
  CameraMatrix.m33 = Cosinus[ax]*Cosinus[ay];
}

 /**
 * Tell if the entity is in the camera view, partially clipped or totally culled
 */
BOOL SAGE_EntityVisibility(SAGE_Entity * entity, SAGE_Camera * camera)
{
  FLOAT cx, cy, cz, x, y ,z;
  FLOAT radius, xplane, yplane;

  cx = entity->posx - camera->posx;
  cy = entity->posy - camera->posy;
  cz = entity->posz - camera->posz;
  x = cx*CameraMatrix.m11 + cy*CameraMatrix.m21 + cz*CameraMatrix.m31;
  y = cx*CameraMatrix.m12 + cy*CameraMatrix.m22 + cz*CameraMatrix.m32;
  z = cx*CameraMatrix.m13 + cy*CameraMatrix.m23 + cz*CameraMatrix.m33;
  radius = entity->radius;
  if (debug) SAGE_DebugLog("Visibility (%d)  x=%f  y=%f  z=%f  radius=%f", entity->nb_faces, x, y ,z ,radius);
  // Check against Z planes
  if (((z-radius) > camera->far_plane) || ((z+radius) < camera->near_plane)) {
    return FALSE;
  }
  // Check against X planes
  xplane = (camera->centerx * z) / camera->view_dist;
  if (debug) SAGE_DebugLog("  xplane=%f", xplane);
  if (((x-radius) > xplane) || ((x+radius) < -xplane)) {
    return FALSE;
  }
  // Check against Y planes
  yplane = (camera->centery * z) / camera->view_dist;
  if (debug) SAGE_DebugLog("  yplane=%f", yplane);
  if (((y-radius) > yplane) || ((y+radius) < -yplane)) {
    return FALSE;
  }
  // Check for partial clipping
  if (debug) SAGE_DebugLog("  partial ?");
  if (((z+radius) > camera->far_plane) || ((z-radius) < camera->near_plane)) {
    entity->clipped = TRUE;
  } else if (((x+radius) > xplane) || ((x-radius) < -xplane)) {
    entity->clipped = TRUE;
  } else if (((y+radius) > yplane) || ((y-radius) < -yplane)) {
    entity->clipped = TRUE;
  } else {
    entity->clipped = FALSE;
  }
  // Set the entity as visible
  sage_world.ordering[sage_world.visible_entities].entity = entity;
  sage_world.ordering[sage_world.visible_entities].posz = z;
  sage_world.visible_entities++;
  return TRUE;
}

/**
 * Setup the entity matrix
 */
VOID SAGE_SetupEntityMatrix(SAGE_Entity * entity)
{
  WORD ax, ay, az;

  ax = entity->anglex;
  while (ax < 0) ax += S3DE_ANGLE_360;
  while (ax >= S3DE_ANGLE_360) ax-= S3DE_ANGLE_360;
  ay = entity->angley;
  while (ay < 0) ay += S3DE_ANGLE_360;
  while (ay >= S3DE_ANGLE_360) ay-= S3DE_ANGLE_360;
  az = entity->anglez;
  while (az < 0) az += S3DE_ANGLE_360;
  while (az >= S3DE_ANGLE_360) az-= S3DE_ANGLE_360;
  EntityMatrix.m11 = Cosinus[ay]*Cosinus[az];
  EntityMatrix.m12 = Cosinus[ay]*Sinus[az];
  EntityMatrix.m13 = -Sinus[ay];
  EntityMatrix.m21 = (Sinus[ax]*Sinus[ay]*Cosinus[az]) - (Cosinus[ax]*Sinus[az]);
  EntityMatrix.m22 = (Sinus[ax]*Sinus[ay]*Sinus[az]) + (Cosinus[ax]*Cosinus[az]);
  EntityMatrix.m23 = Sinus[ax]*Cosinus[ay];
  EntityMatrix.m31 = (Cosinus[ax]*Sinus[ay]*Cosinus[az]) + (Sinus[ax]*Sinus[az]);
  EntityMatrix.m32 = (Cosinus[ax]*Sinus[ay]*Sinus[az]) - (Sinus[ax]*Cosinus[az]);
  EntityMatrix.m33 = Cosinus[ax]*Cosinus[ay];
}

/**
 * Transform the entity to the world coordinates
 */
VOID SAGE_EntityLocalToWorld(SAGE_Entity * entity)
{
  WORD index;
  FLOAT x, y, z;
  
  for (index = 0;index < entity->nb_vertices;index++) {
    x = entity->vertices[index].x;
    y = entity->vertices[index].y;
    z = entity->vertices[index].z;
    entity->trans_vertices[index].x = x*EntityMatrix.m11 + y*EntityMatrix.m21 + z*EntityMatrix.m31 + entity->posx;
    entity->trans_vertices[index].y = x*EntityMatrix.m12 + y*EntityMatrix.m22 + z*EntityMatrix.m32 + entity->posy;
    entity->trans_vertices[index].z = x*EntityMatrix.m13 + y*EntityMatrix.m23 + z*EntityMatrix.m33 + entity->posz;
  }
}

/**
 * Remove not visible faces
 */
VOID SAGE_EntityBackfaceCulling(SAGE_Entity * entity, SAGE_Camera * camera)
{
  UWORD index, p1;
  FLOAT res, x, y ,z;
  SAGE_Vector sight, normal;

  for (index = 0;index < entity->nb_faces;index++) {
    // Transform face normal to world space
    x = entity->faces[index].normal.x;
    y = entity->faces[index].normal.y;
    z = entity->faces[index].normal.z;
    normal.x = x*EntityMatrix.m11 + y*EntityMatrix.m21 + z*EntityMatrix.m31;
    normal.y = x*EntityMatrix.m12 + y*EntityMatrix.m22 + z*EntityMatrix.m32;
    normal.z = x*EntityMatrix.m13 + y*EntityMatrix.m23 + z*EntityMatrix.m33;
    p1 = entity->faces[index].p1;
    // Build the camera sight
    sight.x = camera->posx - entity->trans_vertices[p1].x;
    sight.y = camera->posy - entity->trans_vertices[p1].y;
    sight.z = camera->posz - entity->trans_vertices[p1].z;
    // Check face visibility (u*v = xu*xv + yu*yv + zu*zv)
    res = (normal.x*sight.x) + (normal.y*sight.y) + (normal.z*sight.z);
    if (res > 0.0) {
      entity->faces[index].culled = FALSE;
    } else {
      entity->faces[index].culled = TRUE;
    }
  }
}

/**
 * Transform the entity to the camera coordinates
 */
VOID SAGE_EntityWorldToCamera(SAGE_Entity * entity, SAGE_Camera * camera)
{
  WORD index;
  FLOAT x, y, z;
  
  for (index = 0;index < entity->nb_vertices;index++) {
    x = entity->trans_vertices[index].x - camera->posx;
    y = entity->trans_vertices[index].y - camera->posy;
    z = entity->trans_vertices[index].z - camera->posz;
    entity->trans_vertices[index].x = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
    entity->trans_vertices[index].y = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
    entity->trans_vertices[index].z = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
  }
}

/**
 * Check if entity faces are clipped
 */
VOID SAGE_EntityFaceClipping(SAGE_Entity * entity, SAGE_Camera * camera)
{
  WORD index, p1, p2, p3, p4;
  FLOAT nearp, farp, x1plane, y1plane, x2plane, y2plane, x3plane, y3plane, x4plane, y4plane;
  FLOAT x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;

  nearp = camera->near_plane;
  farp = camera->far_plane;
  for (index = 0;index < entity->nb_faces;index++) {
    if (!entity->faces[index].culled) {
      p1 = entity->faces[index].p1;
      x1 = entity->trans_vertices[p1].x; y1 = entity->trans_vertices[p1].y; z1 = entity->trans_vertices[p1].z;
      p2 = entity->faces[index].p2;
      x2 = entity->trans_vertices[p2].x; y2 = entity->trans_vertices[p2].y; z2 = entity->trans_vertices[p2].z;
      p3 = entity->faces[index].p3;
      x3 = entity->trans_vertices[p3].x; y3 = entity->trans_vertices[p3].y; z3 = entity->trans_vertices[p3].z;
      if (entity->faces[index].is_quad) {
        p4 = entity->faces[index].p4;
        x4 = entity->trans_vertices[p4].x; y4 = entity->trans_vertices[p4].y; z4 = entity->trans_vertices[p4].z;
      } else {
        p4 = p3;
        x4 = x3; y4 = y3; z4 = z3;
      }
      // Check if the face is outside of X planes
      x1plane = (camera->centerx * z1) / camera->view_dist;
      x2plane = (camera->centerx * z2) / camera->view_dist;
      x3plane = (camera->centerx * z3) / camera->view_dist;
      x4plane = (camera->centerx * z4) / camera->view_dist;
      if ((x1>x1plane && x2>x2plane && x3>x3plane && x4>x4plane) || (x1<-x1plane && x2<-x2plane && x3<-x3plane && x4<-x4plane)) {
        entity->faces[index].culled = TRUE;
      } else {
        // Check if the face is outside of Y planes
        y1plane = (camera->centery * z1) / camera->view_dist;
        y2plane = (camera->centery * z2) / camera->view_dist;
        y3plane = (camera->centery * z3) / camera->view_dist;
        y4plane = (camera->centery * z4) / camera->view_dist;
        if ((y1>y1plane && y2>y2plane && y3>y3plane && y4>y4plane) || (y1<-y1plane && y2<-y2plane && y3<-y3plane && y4<-y4plane)) {
          entity->faces[index].culled = TRUE;
        } else {
          // Check if the face is totally or partially outside of Z planes
          if ((z1<nearp && z2<nearp && z3<nearp && z4<nearp) || (z1>farp && z2>farp && z3>farp && z4>farp)) {
            entity->faces[index].culled = TRUE;
          } else {
            entity->faces[index].clipped = S3DE_NOCLIP;
            if (z1 < nearp) entity->faces[index].clipped |= S3DE_P1CLIP;
            if (z2 < nearp) entity->faces[index].clipped |= S3DE_P2CLIP;
            if (z3 < nearp) entity->faces[index].clipped |= S3DE_P3CLIP;
            if (z4 < nearp) entity->faces[index].clipped |= S3DE_P4CLIP;
          }
        }
      }
    }
  }
}

/**
 * Render an entity in wireframe mode
 */
VOID SAGE_DrawEntityWireFrame(SAGE_Entity * entity)
{
  WORD index;

  for (index = 0;index < entity->nb_faces;index++) {
    if (entity->faces[index].is_quad) {
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p1].x),
        (LONG)(projected_vertices[entity->faces[index].p1].y),
        (LONG)(projected_vertices[entity->faces[index].p2].x),
        (LONG)(projected_vertices[entity->faces[index].p2].y),
        entity->faces[index].color
      );
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p2].x),
        (LONG)(projected_vertices[entity->faces[index].p2].y),
        (LONG)(projected_vertices[entity->faces[index].p3].x),
        (LONG)(projected_vertices[entity->faces[index].p3].y),
        entity->faces[index].color
      );
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p3].x),
        (LONG)(projected_vertices[entity->faces[index].p3].y),
        (LONG)(projected_vertices[entity->faces[index].p4].x),
        (LONG)(projected_vertices[entity->faces[index].p4].y),
        entity->faces[index].color
      );
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p1].x),
        (LONG)(projected_vertices[entity->faces[index].p1].y),
        (LONG)(projected_vertices[entity->faces[index].p4].x),
        (LONG)(projected_vertices[entity->faces[index].p4].y),
        entity->faces[index].color
      );
    } else {
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p1].x),
        (LONG)(projected_vertices[entity->faces[index].p1].y),
        (LONG)(projected_vertices[entity->faces[index].p2].x),
        (LONG)(projected_vertices[entity->faces[index].p2].y),
        entity->faces[index].color
      );
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p2].x),
        (LONG)(projected_vertices[entity->faces[index].p2].y),
        (LONG)(projected_vertices[entity->faces[index].p3].x),
        (LONG)(projected_vertices[entity->faces[index].p3].y),
        entity->faces[index].color
      );
      SAGE_DrawClippedLine(
        (LONG)(projected_vertices[entity->faces[index].p1].x),
        (LONG)(projected_vertices[entity->faces[index].p1].y),
        (LONG)(projected_vertices[entity->faces[index].p3].x),
        (LONG)(projected_vertices[entity->faces[index].p3].y),
        entity->faces[index].color
      );
    }
  }
}

/**
 * Render an entity in flat mode
 */
VOID SAGE_DrawEntityFlat(SAGE_Entity * entity)
{
  WORD index;

  for (index = 0;index < entity->nb_faces;index++) {
    if (!entity->faces[index].culled) {
      SAGE_DrawClippedTriangle(
        (LONG)(projected_vertices[entity->faces[index].p1].x),
        (LONG)(projected_vertices[entity->faces[index].p1].y),
        (LONG)(projected_vertices[entity->faces[index].p2].x),
        (LONG)(projected_vertices[entity->faces[index].p2].y),
        (LONG)(projected_vertices[entity->faces[index].p3].x),
        (LONG)(projected_vertices[entity->faces[index].p3].y),
        entity->faces[index].color
      );
      if (entity->faces[index].is_quad) {
        SAGE_DrawClippedTriangle(
          (LONG)(projected_vertices[entity->faces[index].p1].x),
          (LONG)(projected_vertices[entity->faces[index].p1].y),
          (LONG)(projected_vertices[entity->faces[index].p4].x),
          (LONG)(projected_vertices[entity->faces[index].p4].y),
          (LONG)(projected_vertices[entity->faces[index].p3].x),
          (LONG)(projected_vertices[entity->faces[index].p3].y),
          entity->faces[index].color
        );
      }
    }
  }
}

/**
 * Add a textured triangle to render list (first part)
 */
VOID SAGE_AddTexturedTriangleP1(SAGE_EntityFace * face)
{
  SAGE_3DTriangle triangle;

  if (debug) SAGE_DebugLog("Add triangle 1");
  triangle.x1 = projected_vertices[face->p1].x;
  triangle.y1 = projected_vertices[face->p1].y;
  triangle.z1 = projected_vertices[face->p1].z;
  triangle.u1 = face->u1;
  triangle.v1 = face->v1;
  triangle.x2 = projected_vertices[face->p2].x;
  triangle.y2 = projected_vertices[face->p2].y;
  triangle.z2 = projected_vertices[face->p2].z;
  triangle.u2 = face->u2;
  triangle.v2 = face->v2;
  triangle.x3 = projected_vertices[face->p3].x;
  triangle.y3 = projected_vertices[face->p3].y;
  triangle.z3 = projected_vertices[face->p3].z;
  triangle.u3 = face->u3;
  triangle.v3 = face->v3;
  triangle.texture = face->texture;
  if (debug) SAGE_Dump3DTriangle(&triangle);
  SAGE_Push3DTriangle(&triangle);
}

/**
 * Add a textured triangle to render list (second part)
 */
VOID SAGE_AddTexturedTriangleP2(SAGE_EntityFace * face)
{
  SAGE_3DTriangle triangle;

  if (debug) SAGE_DebugLog("Add triangle 2");
  triangle.x1 = projected_vertices[face->p1].x;
  triangle.y1 = projected_vertices[face->p1].y;
  triangle.z1 = projected_vertices[face->p1].z;
  triangle.u1 = face->u1;
  triangle.v1 = face->v1;
  triangle.x2 = projected_vertices[face->p4].x;
  triangle.y2 = projected_vertices[face->p4].y;
  triangle.z2 = projected_vertices[face->p4].z;
  triangle.u2 = face->u4;
  triangle.v2 = face->v4;
  triangle.x3 = projected_vertices[face->p3].x;
  triangle.y3 = projected_vertices[face->p3].y;
  triangle.z3 = projected_vertices[face->p3].z;
  triangle.u3 = face->u3;
  triangle.v3 = face->v3;
  triangle.texture = face->texture;
  if (debug) SAGE_Dump3DTriangle(&triangle);
  SAGE_Push3DTriangle(&triangle);
}

/**
 * Init the 3D engine
 */
BOOL SAGE_Init3DEngine(VOID)
{
  FLOAT angle, step;
  LONG i;

  angle = 0.0;
  step = 1.0 / (FLOAT) S3DE_PRECISION;
  for (i = 0;i < (360*S3DE_PRECISION);i++) {
    Sinus[i] = sin(DEGTORAD(angle));
    Cosinus[i] = cos(DEGTORAD(angle));
    Tangente[i] = tan(DEGTORAD(angle));
    angle += step;
  }
  sage_world.nb_materials = 0;
  sage_world.visible_entities = 0;
  sage_world.active_camera = 0;
  sage_world.active_skybox = FALSE;
  sage_world.skybox = SAGE_CreateEntity(8, 6);
  if (sage_world.skybox != NULL) {
    SAGE_SetSkyboxRadius((FLOAT)20.0);
    SAGE_SetSkyboxTextures(0, 0, 0, 0, 0, 0);
    SAGE_SetEntityRadius(sage_world.skybox);
    SAGE_SetEntityNormals(sage_world.skybox);
    SD(SAGE_DumpEntity(sage_world.skybox));
  }
  return TRUE;
}

/**
 * Release the 3D engine
 */
VOID SAGE_Release3DEngine()
{
  if (sage_world.skybox != NULL) {
    SAGE_ReleaseEntity(sage_world.skybox);
  }
}

/**
 * Transform skybox to camera view
 */
VOID SAGE_TransformSkybox(SAGE_Camera * camera)
{
  SAGE_Entity * entity;

  if (debug) SAGE_DebugLog("Transform skybox");
  entity = sage_world.skybox;
  if (entity != NULL && !entity->disabled) {
    if (SAGE_EntityVisibility(entity, camera)) {
      SAGE_SetupEntityMatrix(entity);
      SAGE_EntityLocalToWorld(entity);
      SAGE_EntityBackfaceCulling(entity, camera);
      SAGE_EntityWorldToCamera(entity, camera);
      if (entity->clipped) {
        SAGE_EntityFaceClipping(entity, camera);
      }
      if (debug) SAGE_DumpEntity(entity);
    }
  }
}

/**
 * Transform entities to camera view
 */
VOID SAGE_TransformEntities(SAGE_Camera * camera)
{
  SAGE_Entity * entity;
  ULONG index;

  if (debug) SAGE_DebugLog("Transform entities");
  for (index = 0;index < S3DE_MAX_ENTITIES;index++) {
    entity = sage_world.entities[index];
    if (entity != NULL && !entity->disabled) {
      if (SAGE_EntityVisibility(entity, camera)) {
        SAGE_SetupEntityMatrix(entity);
        SAGE_EntityLocalToWorld(entity);
        SAGE_EntityBackfaceCulling(entity, camera);
        SAGE_EntityWorldToCamera(entity, camera);
        if (entity->clipped) {
          SAGE_EntityFaceClipping(entity, camera);
        }
        if (debug) SAGE_DumpEntity(entity);
      }
    }
  }
}

/**
 * Sort visible entities
 */
VOID SAGE_SortEntities(SAGE_SortedEntity * entities, LONG low, LONG high)
{
  SAGE_SortedEntity temp;
  FLOAT pivot;
  LONG idx_low, idx_high;

  if (low >= high) return;
  idx_low = low+1;
  idx_high = high;
  pivot = entities[low].posz;
  while (idx_low <= idx_high) {
    while (entities[idx_low].posz >= pivot && idx_low <= high) idx_low++;
    while (entities[idx_high].posz < pivot && idx_high >= low) idx_high--;
    if (idx_low < idx_high) {
      temp.entity = entities[idx_low].entity;
      temp.posz = entities[idx_low].posz;
      entities[idx_low].entity = entities[idx_high].entity;
      entities[idx_low].posz = entities[idx_high].posz;
      entities[idx_high].entity = temp.entity;
      entities[idx_high].posz = temp.posz;
      idx_low++;
      idx_high--;
    }
  }
  temp.entity = entities[low].entity;
  temp.posz = entities[low].posz;
  entities[low].entity = entities[idx_high].entity;
  entities[low].posz = entities[idx_high].posz;
  entities[idx_high].entity = temp.entity;
  entities[idx_high].posz = temp.posz;
  SAGE_SortEntities(entities, low, idx_high-1);
  SAGE_SortEntities(entities, idx_high+1, high);
}

/**
 * Calculate perspective for all entity vertices
 */
VOID SAGE_EntityProjection(SAGE_Entity * entity, SAGE_Camera * camera)
{
  WORD index;
  
  for (index = 0;index < entity->nb_vertices;index++) {
    if (entity->trans_vertices[index].z > 0.0) {
      projected_vertices[index].x = (entity->trans_vertices[index].x * camera->view_dist / entity->trans_vertices[index].z) + camera->centerx;
      projected_vertices[index].y = (-entity->trans_vertices[index].y * camera->view_dist / entity->trans_vertices[index].z) + camera->centery;
      projected_vertices[index].z = entity->trans_vertices[index].z;
    } else {
      projected_vertices[index].x = 0.0;
      projected_vertices[index].y = 0.0;
      projected_vertices[index].z = 0.0;
    }
  }
}

/**
 * Clip the first point of the face against near plane
 */
VOID SAGE_ClipOneFacePoint(SAGE_EntityFace * face, SAGE_EntityVertex * vertices, SAGE_Camera * camera)
{
  ULONG p1, p2, p3;
  FLOAT u1, v1, nearp, clip_inter1, clip_inter2, cx, cy;

  if (debug) SAGE_DebugLog("Clipping one point");

  p1 = face->p1;
  p2 = face->p2;
  p3 = face->p3;
  u1 = face->u1;
  v1 = face->v1;
  nearp = camera->near_plane;

  if (debug) SAGE_DebugLog(" => vertex p1 x=%f  y=%f  z=%f  u=%f  v=%f", vertices[p1].x, vertices[p1].y, vertices[p1].z, face->u1, face->v1);

  if (debug) SAGE_DebugLog(" => vertex p2 x=%f  y=%f  z=%f  u=%f  v=%f", vertices[p2].x, vertices[p2].y, vertices[p2].z, face->u2, face->v2);
  clip_inter1 = (nearp - vertices[p1].z) / (vertices[p2].z - vertices[p1].z);
  cx = vertices[p1].x + (vertices[p2].x - vertices[p1].x) * clip_inter1;
  cy = vertices[p1].y + (vertices[p2].y - vertices[p1].y) * clip_inter1;
  face->u1 = u1 + (face->u2 - u1) * clip_inter1;
  face->v1 = v1 + (face->v2 - v1) * clip_inter1;
  if (debug) SAGE_DebugLog(" => clip1=%f  cx=%f  cy=%f  cu=%f  cv=%f", clip_inter1, cx, cy, face->u1, face->v1);

  projected_vertices[S3DE_VERTEX_CLIP1].x = (cx * camera->view_dist / nearp) + camera->centerx;
  projected_vertices[S3DE_VERTEX_CLIP1].y = (-cy * camera->view_dist / nearp) + camera->centery;
  projected_vertices[S3DE_VERTEX_CLIP1].z = nearp;
  if (debug) SAGE_DebugLog(" => px=%f  py=%f  pz=%f", projected_vertices[S3DE_VERTEX_CLIP1].x, projected_vertices[S3DE_VERTEX_CLIP1].y, projected_vertices[S3DE_VERTEX_CLIP1].z);

  face->p1 = S3DE_VERTEX_CLIP1;
  SAGE_AddTexturedTriangleP1(face);

  if (debug) SAGE_DebugLog(" => vertex p3 x=%f  y=%f  z=%f  u=%f  v=%f", vertices[p3].x, vertices[p3].y, vertices[p3].z, face->u3, face->v3);
  clip_inter2 = (nearp - vertices[p1].z) / (vertices[p3].z - vertices[p1].z);
  cx = vertices[p1].x + (vertices[p3].x - vertices[p1].x) * clip_inter2;
  cy = vertices[p1].y + (vertices[p3].y - vertices[p1].y) * clip_inter2;
  face->u4 = u1 + (face->u3 - u1) * clip_inter2;
  face->v4 = v1 + (face->v3 - v1) * clip_inter2;
  if (debug) SAGE_DebugLog(" => clip2=%f  cx=%f  cy=%f  cu=%f  cv=%f", clip_inter2, cx, cy, face->u4, face->v4);

  projected_vertices[S3DE_VERTEX_CLIP2].x = (cx * camera->view_dist / nearp) + camera->centerx;
  projected_vertices[S3DE_VERTEX_CLIP2].y = (-cy * camera->view_dist / nearp) + camera->centery;
  projected_vertices[S3DE_VERTEX_CLIP2].z = nearp;
  if (debug) SAGE_DebugLog(" => px=%f  py=%f  pz=%f", projected_vertices[S3DE_VERTEX_CLIP2].x, projected_vertices[S3DE_VERTEX_CLIP2].y, projected_vertices[S3DE_VERTEX_CLIP2].z);

  face->p4 = S3DE_VERTEX_CLIP2;
  SAGE_AddTexturedTriangleP2(face);
}

/**
 * Clip the two first points of the face against near plane
 */
VOID SAGE_ClipTwoFacePoint(SAGE_EntityFace * face, SAGE_EntityVertex * vertices, SAGE_Camera * camera)
{
  ULONG p1, p2, p3;
  FLOAT u1, v1, u2, v2, nearp, clip_inter1, clip_inter2, cx, cy;

  if (debug) SAGE_DebugLog("Clipping two points");

  p1 = face->p1;
  p2 = face->p2;
  p3 = face->p3;
  u1 = face->u1;
  v1 = face->v1;
  u2 = face->u2;
  v2 = face->v2;
  nearp = camera->near_plane;

  if (debug) SAGE_DebugLog(" => vertex p1 x=%f  y=%f  z=%f  u=%f  v=%f", vertices[p1].x, vertices[p1].y, vertices[p1].z, face->u1, face->v1);
  if (debug) SAGE_DebugLog(" => vertex p2 x=%f  y=%f  z=%f  u=%f  v=%f", vertices[p2].x, vertices[p2].y, vertices[p2].z, face->u2, face->v2);

  if (debug) SAGE_DebugLog(" => vertex p3 x=%f  y=%f  z=%f  u=%f  v=%f", vertices[p3].x, vertices[p3].y, vertices[p3].z, face->u3, face->v3);
  clip_inter1 = (nearp - vertices[p1].z) / (vertices[p3].z - vertices[p1].z);
  cx = vertices[p1].x + (vertices[p3].x - vertices[p1].x) * clip_inter1;
  cy = vertices[p1].y + (vertices[p3].y - vertices[p1].y) * clip_inter1;
  face->u1 = u1 + (face->u3 - u1) * clip_inter1;
  face->v1 = v1 + (face->v3 - v1) * clip_inter1;
  if (debug) SAGE_DebugLog(" => clip1=%f  cx=%f  cy=%f  cu=%f  cv=%f", clip_inter1, cx, cy, face->u1, face->v1);

  projected_vertices[S3DE_VERTEX_CLIP1].x = (cx * camera->view_dist / nearp) + camera->centerx;
  projected_vertices[S3DE_VERTEX_CLIP1].y = (-cy * camera->view_dist / nearp) + camera->centery;
  projected_vertices[S3DE_VERTEX_CLIP1].z = nearp;
  if (debug) SAGE_DebugLog(" => px=%f  py=%f  pz=%f", projected_vertices[S3DE_VERTEX_CLIP1].x, projected_vertices[S3DE_VERTEX_CLIP1].y, projected_vertices[S3DE_VERTEX_CLIP1].z);

  clip_inter2 = (nearp - vertices[p2].z) / (vertices[p3].z - vertices[p2].z);
  cx = vertices[p2].x + (vertices[p3].x - vertices[p2].x) * clip_inter2;
  cy = vertices[p2].y + (vertices[p3].y - vertices[p2].y) * clip_inter2;
  face->u2 = u2 + (face->u3 - u2) * clip_inter2;
  face->v2 = v2 + (face->v3 - v2) * clip_inter2;
  if (debug) SAGE_DebugLog(" => clip2=%f  cx=%f  cy=%f  cu=%f  cv=%f", clip_inter2, cx, cy, face->u2, face->v2);

  projected_vertices[S3DE_VERTEX_CLIP2].x = (cx * camera->view_dist / nearp) + camera->centerx;
  projected_vertices[S3DE_VERTEX_CLIP2].y = (-cy * camera->view_dist / nearp) + camera->centery;
  projected_vertices[S3DE_VERTEX_CLIP2].z = nearp;
  if (debug) SAGE_DebugLog(" => px=%f  py=%f  pz=%f", projected_vertices[S3DE_VERTEX_CLIP2].x, projected_vertices[S3DE_VERTEX_CLIP2].y, projected_vertices[S3DE_VERTEX_CLIP2].z);

  face->p1 = S3DE_VERTEX_CLIP1;
  face->p2 = S3DE_VERTEX_CLIP2;
  SAGE_AddTexturedTriangleP1(face);
}

/**
 * Set the list of faces to render and clip them against near plane if necessary
 */
VOID SAGE_SetClippedFaceList(SAGE_Entity * entity, SAGE_Camera * camera)
{
  WORD index;
  SAGE_EntityFace * face, clipped_face;

  for (index = 0;index < entity->nb_faces;index++) {
    face = &(entity->faces[index]);
    if (!face->culled) {
      if (face->clipped == S3DE_NOCLIP) {
        SAGE_AddTexturedTriangleP1(face);
        if (face->is_quad) {
          SAGE_AddTexturedTriangleP2(face);
        }
      } else {
        if (debug) SAGE_DebugLog("Clipping first triangle");
        // Check for points 1, 2 and 3
        switch (face->clipped & S3DE_MASKP4) {
          case S3DE_NOCLIP:
            SAGE_AddTexturedTriangleP1(face);
            break;
          case S3DE_P1CLIP:
            clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
            clipped_face.p2 = face->p2; clipped_face.u2 = face->u2; clipped_face.v2 = face->v2;
            clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
            clipped_face.texture = face->texture;
            SAGE_ClipOneFacePoint(&clipped_face, entity->trans_vertices, camera);
            break;
          case S3DE_P2CLIP:
            clipped_face.p1 = face->p2; clipped_face.u1 = face->u2; clipped_face.v1 = face->v2;
            clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
            clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
            clipped_face.texture = face->texture;
            SAGE_ClipOneFacePoint(&clipped_face, entity->trans_vertices, camera);
            break;
          case S3DE_P3CLIP:
            clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
            clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
            clipped_face.p3 = face->p2; clipped_face.u3 = face->u2; clipped_face.v3 = face->v2;
            clipped_face.texture = face->texture;
            SAGE_ClipOneFacePoint(&clipped_face, entity->trans_vertices, camera);
            break;
          case S3DE_P1CLIP|S3DE_P2CLIP:
            clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
            clipped_face.p2 = face->p2; clipped_face.u2 = face->u2; clipped_face.v2 = face->v2;
            clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
            clipped_face.texture = face->texture;
            SAGE_ClipTwoFacePoint(&clipped_face, entity->trans_vertices, camera);
            break;
          case S3DE_P1CLIP|S3DE_P3CLIP:
            clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
            clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
            clipped_face.p3 = face->p2; clipped_face.u3 = face->u2; clipped_face.v3 = face->v2;
            clipped_face.texture = face->texture;
            SAGE_ClipTwoFacePoint(&clipped_face, entity->trans_vertices, camera);
            break;
          case S3DE_P2CLIP|S3DE_P3CLIP:
            clipped_face.p1 = face->p2; clipped_face.u1 = face->u2; clipped_face.v1 = face->v2;
            clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
            clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
            clipped_face.texture = face->texture;
            SAGE_ClipTwoFacePoint(&clipped_face, entity->trans_vertices, camera);
            break;
        }
        if (face->is_quad) {
          if (debug) SAGE_DebugLog("Clipping second triangle");
          // Check for points 1,3 and 4
          switch (face->clipped & S3DE_MASKP2) {
            case S3DE_NOCLIP:
              SAGE_AddTexturedTriangleP2(face);
              break;
            case S3DE_P1CLIP:
              clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
              clipped_face.p2 = face->p4; clipped_face.u2 = face->u4; clipped_face.v2 = face->v4;
              clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
              clipped_face.texture = face->texture;
              SAGE_ClipOneFacePoint(&clipped_face, entity->trans_vertices, camera);
              break;
            case S3DE_P4CLIP:
              clipped_face.p1 = face->p4; clipped_face.u1 = face->u4; clipped_face.v1 = face->v4;
              clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
              clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
              clipped_face.texture = face->texture;
              SAGE_ClipOneFacePoint(&clipped_face, entity->trans_vertices, camera);
              break;
            case S3DE_P3CLIP:
              clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
              clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
              clipped_face.p3 = face->p4; clipped_face.u3 = face->u4; clipped_face.v3 = face->v4;
              clipped_face.texture = face->texture;
              SAGE_ClipOneFacePoint(&clipped_face, entity->trans_vertices, camera);
              break;
            case S3DE_P1CLIP|S3DE_P4CLIP:
              clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
              clipped_face.p2 = face->p4; clipped_face.u2 = face->u4; clipped_face.v2 = face->v4;
              clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
              clipped_face.texture = face->texture;
              SAGE_ClipTwoFacePoint(&clipped_face, entity->trans_vertices, camera);
              break;
            case S3DE_P1CLIP|S3DE_P3CLIP:
              clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
              clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
              clipped_face.p3 = face->p4; clipped_face.u3 = face->u4; clipped_face.v3 = face->v4;
              clipped_face.texture = face->texture;
              SAGE_ClipTwoFacePoint(&clipped_face, entity->trans_vertices, camera);
              break;
            case S3DE_P4CLIP|S3DE_P3CLIP:
              clipped_face.p1 = face->p4; clipped_face.u1 = face->u4; clipped_face.v1 = face->v4;
              clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
              clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
              clipped_face.texture = face->texture;
              SAGE_ClipTwoFacePoint(&clipped_face, entity->trans_vertices, camera);
              break;
          }
        }
      }
    }
  }
}

/**
 * Render skybox
 */
VOID SAGE_RenderSkybox(SAGE_Camera * camera)
{
  SAGE_Entity * entity;

  if (debug) SAGE_DebugLog("Rendering skybox");
  entity = sage_world.skybox;
  SAGE_EntityProjection(entity, camera);
  SAGE_SetClippedFaceList(entity, camera);
  SAGE_Render3DTriangles();
  sage_world.visible_entities = 0;
}

/**
 * Render ordered entities
 */
VOID SAGE_RenderEntities(SAGE_Camera * camera)
{
  SAGE_Entity * entity;
  WORD index;

  if (debug) SAGE_DebugLog("Render entities");
  if (sage_world.visible_entities > 1) {
    SAGE_SortEntities(sage_world.ordering, 0, sage_world.visible_entities-1);
  }
  for (index = 0;index < sage_world.visible_entities;index++) {
    entity = sage_world.ordering[index].entity;
    SAGE_EntityProjection(entity, camera);
    if (entity->rendering == S3DE_RENDER_WIRE) {
      SAGE_DrawEntityWireFrame(entity);
    } else if (entity->rendering == S3DE_RENDER_FLAT) {
      SAGE_DrawEntityFlat(entity);
    } else {
      SAGE_SetClippedFaceList(entity, camera);
      SAGE_Render3DTriangles();
    }
  }
}

/**
 * Render the 3D world
 */
VOID SAGE_RenderWorld(VOID)
{
  SAGE_Camera * camera;

  sage_world.visible_entities = 0;
  camera = sage_world.cameras[sage_world.active_camera];
  if (camera != NULL) {
    SAGE_SetScreenClip(camera->view_left, camera->view_top, camera->view_width, camera->view_height);
    SAGE_SetupCameraMatrix(camera);
    if (sage_world.active_skybox) {
      SAGE_TransformSkybox(camera);
      SAGE_RenderSkybox(camera);
    }
    SAGE_TransformEntities(camera);
    SAGE_RenderEntities(camera);
  }
}
