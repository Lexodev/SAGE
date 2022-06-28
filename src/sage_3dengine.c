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

#include <proto/exec.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_maths.h"
#include "sage_draw.h"
#include "sage_screen.h"
#include "sage_loadlwo.h"
#include "sage_3drender.h"
#include "sage_3dtexture.h"
#include "sage_3dengine.h"

#define SAGE_FAST_MATRIX      1

#define SAGE_ENABLE_SKYBOX    1
#define SAGE_ENABLE_TERRAIN   1
#define SAGE_ENABLE_ENTITIES  1

/** Transformation matrix */
SAGE_Matrix CameraMatrix;
SAGE_Matrix EntityMatrix;

/** Our 3D world */
SAGE_3DWorld sage_world;

/** For debug purpose, should be removed */
BOOL engine_debug;

/*****************************************************************************
 *            DEBUG ONLY
 *****************************************************************************/

VOID SAGE_DumpCameraMatrix(VOID)
{
  SAGE_DebugLog("** Camera matrix");
  SAGE_DebugLog(" => %f\t%f\t%f", CameraMatrix.m11, CameraMatrix.m12, CameraMatrix.m13);
  SAGE_DebugLog(" => %f\t%f\t%f", CameraMatrix.m21, CameraMatrix.m22, CameraMatrix.m23);
  SAGE_DebugLog(" => %f\t%f\t%f", CameraMatrix.m31, CameraMatrix.m32, CameraMatrix.m33);
}

VOID SAGE_DumpEntityMatrix(VOID)
{
  SAGE_DebugLog("** Entity matrix");
  SAGE_DebugLog(" => %f\t%f\t%f", EntityMatrix.m11, EntityMatrix.m12, EntityMatrix.m13);
  SAGE_DebugLog(" => %f\t%f\t%f", EntityMatrix.m21, EntityMatrix.m22, EntityMatrix.m23);
  SAGE_DebugLog(" => %f\t%f\t%f", EntityMatrix.m31, EntityMatrix.m32, EntityMatrix.m33);
}

VOID SAGE_DumpTransformedVertices(UWORD nb_vertices)
{
  SAGE_TransformedVertex * tvert;
  UWORD index;
  
  SAGE_DebugLog("** Transformed vertices");
  tvert = sage_world.transformed_vertices;
  for (index = 0;index < nb_vertices;index++) {
    SAGE_DebugLog(
      " => vertex #%d : calculated=%d  visible=%d  wx=%f  wy=%f  wz=%f  cx=%f  cy=%f  cz=%f  px=%f  py=%f  pz=%f  iz=%f",
      index, (tvert[index].calculated ? 1:0), (tvert[index].visible ? 1:0),
      tvert[index].wx, tvert[index].wy, tvert[index].wz, tvert[index].cx, tvert[index].cy, tvert[index].cz,
      tvert[index].px, tvert[index].py, tvert[index].pz, tvert[index].iz
    );
  }
}

/*****************************************************************************
 *            MATRIX CALCULATION
 *****************************************************************************/

/**
 * Setup the camera matrix
 *
 * RX : 1     0    0
 *      0     Cos  -Sin
 *      0     Sin  Cos
 *
 * RY : Cos   0    Sin
 *      0     1    0
 *      -Sin  0    Cos
 *
 * RZ : Cos  -Sin  0
 *      Sin  Cos   0
 *      0    0     1
 *
 * (cosZ*cosY) ((-sinZ*cosX)+(cosZ*sinY*sinX)) ((-sinZ*-sinX)+(cosZ*sinY*cosX))
 * (sinZ*cosY) ((cosZ*cosX)+(sinZ*sinY*sinX))  ((cosZ*-sinX)+(sinZ*sinY*cosX))
 * (-sinY)     (cosY*sinX)                     (cosY*cosX)
 *
 */
VOID SAGE_SetupCameraMatrix(SAGE_Camera * camera)
{
  FLOAT sin_x, sin_y, sin_z, cos_x, cos_y, cos_z;
#if SAGE_FAST_MATRIX == 0
  SAGE_Matrix rx, ry, rz, rzy;
#endif

  sin_x = SAGE_FastSine(camera->anglex);
  sin_y = SAGE_FastSine(camera->angley);
  sin_z = SAGE_FastSine(camera->anglez);
  cos_x = SAGE_FastCosine(camera->anglex);
  cos_y = SAGE_FastCosine(camera->angley);
  cos_z = SAGE_FastCosine(camera->anglez);
#if SAGE_FAST_MATRIX == 1
  CameraMatrix.m11 = cos_z*cos_y;
  CameraMatrix.m12 = (cos_z*sin_y*sin_x) - (sin_z*cos_x);
  CameraMatrix.m13 = (sin_z*sin_x) + (cos_z*sin_y*cos_x);
  CameraMatrix.m21 = sin_z*cos_y;
  CameraMatrix.m22 = (cos_z*cos_x) + (sin_z*sin_y*sin_x);
  CameraMatrix.m23 = (sin_z*sin_y*cos_x) - (cos_z*sin_x);
  CameraMatrix.m31 = -sin_y;
  CameraMatrix.m32 = cos_y*sin_x;
  CameraMatrix.m33 = cos_y*cos_x;
#else
  SAGE_IdentityMatrix(&rx);
  rx.m22 = cos_x;
  rx.m23 = -sin_x;
  rx.m32 = sin_x;
  rx.m33 = cos_x;
  SAGE_IdentityMatrix(&ry);
  ry.m11 = cos_y;
  ry.m13 = sin_y;
  ry.m31 = -sin_y;
  ry.m33 = cos_y;
  SAGE_IdentityMatrix(&rz);
  rz.m11 = cos_z;
  rz.m12 = -sin_z;
  rz.m21 = sin_z;
  rz.m22 = cos_z;
  SAGE_MultiplyMatrix(&rzy, &rz, &ry);
  SAGE_MultiplyMatrix(&CameraMatrix, &rzy, &rx);
#endif
  SED(SAGE_DumpCameraMatrix();)
}

/**
 * Setup the entity matrix
 *
 * RX : 1     0     0
 *      0     Cos   Sin
 *      0     -Sin  Cos
 *
 * RY : Cos   0     -Sin
 *      0     1     0
 *      Sin   0     Cos
 *
 * RZ : Cos   Sin   0
 *      -Sin  Cos   0
 *      0     0     1
 *
 * (cosY*cosZ)                      (cosY*sinZ)                     (-sinY)
 * ((sinX*sinY*cosZ)+(cosX*-sinZ))  ((sinX*sinY*sinZ)+(cosX*cosZ))  (sinX*cosY)
 * ((cosX*sinY*cosZ)+(-sinX*-sinZ)) ((cosX*sinY*sinZ)+(-sinX*cosZ)) (cosX*cosY)
 *
 */
 
VOID SAGE_SetupEntityMatrix(SAGE_Entity * entity)
{
  FLOAT sin_x, sin_y, sin_z, cos_x, cos_y, cos_z;
#if SAGE_FAST_MATRIX == 0
  SAGE_Matrix rx, ry, rz , rxy;
#endif

  sin_x = SAGE_FastSine(entity->anglex);
  sin_y = SAGE_FastSine(entity->angley);
  sin_z = SAGE_FastSine(entity->anglez);
  cos_x = SAGE_FastCosine(entity->anglex);
  cos_y = SAGE_FastCosine(entity->angley);
  cos_z = SAGE_FastCosine(entity->anglez);
#if SAGE_FAST_MATRIX == 1
  EntityMatrix.m11 = cos_y*cos_z;
  EntityMatrix.m12 = cos_y*sin_z;
  EntityMatrix.m13 = -sin_y;
  EntityMatrix.m21 = (sin_x*sin_y*cos_z) - (cos_x*sin_z);
  EntityMatrix.m22 = (sin_x*sin_y*sin_z) + (cos_x*cos_z);
  EntityMatrix.m23 = sin_x*cos_y;
  EntityMatrix.m31 = (cos_x*sin_y*cos_z) + (sin_x*sin_z);
  EntityMatrix.m32 = (cos_x*sin_y*sin_z) - (sin_x*cos_z);
  EntityMatrix.m33 = cos_x*cos_y;
#else
  SAGE_IdentityMatrix(&rx);
  rx.m22 = cos_x;
  rx.m23 = sin_x;
  rx.m32 = -sin_x;
  rx.m33 = cos_x;
  SAGE_IdentityMatrix(&ry);
  ry.m11 = cos_y;
  ry.m13 = -sin_y;
  ry.m31 = sin_y;
  ry.m33 = cos_y;
  SAGE_IdentityMatrix(&rz);
  rz.m11 = cos_z;
  rz.m12 = sin_z;
  rz.m21 = -sin_z;
  rz.m22 = cos_z;
  SAGE_MultiplyMatrix(&rxy, &rx, &ry);
  SAGE_MultiplyMatrix(&EntityMatrix, &rxy, &rz);
#endif
  SED(SAGE_DumpEntityMatrix();)
}

/*****************************************************************************
 *            VERTICES CALCULATION
 *****************************************************************************/

/**
 * Set all tranformed vertices to not calculated and not visible
 */
VOID SAGE_ClearTransformedVertices(SAGE_TransformedVertex * vertices, UWORD nb_vertices)
{
  UWORD index;

  for (index = 0;index < nb_vertices;index++) {
    vertices[index].calculated = FALSE;
    vertices[index].visible = FALSE;
  }
}

/**
 * Calculate perspective projection for all visible vertices
 */
VOID SAGE_VerticesProjection(SAGE_TransformedVertex * vertices, UWORD nb_vertices, SAGE_Camera * camera)
{
  UWORD index;
  
  SED(SAGE_DebugLog("** SAGE_VerticesProjection()");)
  for (index = 0;index < nb_vertices;index++) {
    if (vertices[index].visible) {
      if (vertices[index].cz > 0.0) {
        vertices[index].px = (vertices[index].cx * camera->view_dist / vertices[index].cz) + camera->centerx;
        vertices[index].py = (-vertices[index].cy * camera->view_dist / vertices[index].cz) + camera->centery;
        vertices[index].pz = vertices[index].cz;
        vertices[index].iz = (FLOAT)1.0 / vertices[index].cz;
        SED(
          SAGE_DebugLog(" - vertex %d : px=%f  py=%f  pz=%f  iz=%f",
          index, vertices[index].px, vertices[index].py, vertices[index].pz, vertices[index].iz);
        )
      } else {
        vertices[index].px = 0.0;
        vertices[index].py = 0.0;
        vertices[index].pz = 0.0;
        vertices[index].iz = 0.0;
        SED(SAGE_DebugLog(" - vertex %d has a negative Z", index);)
      }
      sage_world.metrics.rendered_vertices++;
    }
  }
}

/*****************************************************************************
 *            TRIANGLES LIST GENERATION
 *****************************************************************************/

/**
 * Add a textured triangle to render list (first part)
 */
VOID SAGE_AddTexturedTriangleP1(SAGE_TransformedVertex * vertices, SAGE_Face * face)
{
  SAGE_3DTriangle triangle;

  SED(SAGE_DebugLog("** SAGE_AddTexturedTriangleP1()");)
  triangle.x1 = vertices[face->p1].px;
  triangle.y1 = vertices[face->p1].py;
  triangle.z1 = vertices[face->p1].pz;
  triangle.u1 = face->u1;
  triangle.v1 = face->v1;
  triangle.x2 = vertices[face->p2].px;
  triangle.y2 = vertices[face->p2].py;
  triangle.z2 = vertices[face->p2].pz;
  triangle.u2 = face->u2;
  triangle.v2 = face->v2;
  triangle.x3 = vertices[face->p3].px;
  triangle.y3 = vertices[face->p3].py;
  triangle.z3 = vertices[face->p3].pz;
  triangle.u3 = face->u3;
  triangle.v3 = face->v3;
  triangle.texture = face->texture;
  triangle.color = face->color;
  SED(SAGE_Dump3DTriangle(&triangle);)
  SAGE_Push3DTriangle(&triangle);
  sage_world.metrics.rendered_triangles++;
}

/**
 * Add a textured triangle to render list (second part)
 */
VOID SAGE_AddTexturedTriangleP2(SAGE_TransformedVertex * vertices, SAGE_Face * face)
{
  SAGE_3DTriangle triangle;

  SED(SAGE_DebugLog("** SAGE_AddTexturedTriangleP2()");)
  triangle.x1 = vertices[face->p1].px;
  triangle.y1 = vertices[face->p1].py;
  triangle.z1 = vertices[face->p1].pz;
  triangle.u1 = face->u1;
  triangle.v1 = face->v1;
  triangle.x2 = vertices[face->p4].px;
  triangle.y2 = vertices[face->p4].py;
  triangle.z2 = vertices[face->p4].pz;
  triangle.u2 = face->u4;
  triangle.v2 = face->v4;
  triangle.x3 = vertices[face->p3].px;
  triangle.y3 = vertices[face->p3].py;
  triangle.z3 = vertices[face->p3].pz;
  triangle.u3 = face->u3;
  triangle.v3 = face->v3;
  triangle.texture = face->texture;
  triangle.color = face->color;
  SED(SAGE_Dump3DTriangle(&triangle);)
  SAGE_Push3DTriangle(&triangle);
  sage_world.metrics.rendered_triangles++;
}


/*****************************************************************************
 *            FACES CLIPPING
 *****************************************************************************/

/**
 * Clip the first point of the face against near plane
 */
VOID SAGE_ClipOneFacePoint(SAGE_TransformedVertex * vertices, SAGE_Face * face, SAGE_Camera * camera)
{
  ULONG p1, p2, p3;
  FLOAT u1, v1, nearp, clip_inter1, clip_inter2, cx, cy;

  SED(SAGE_DebugLog("** SAGE_ClipOneFacePoint()");)
  p1 = face->p1;
  p2 = face->p2;
  p3 = face->p3;
  u1 = face->u1;
  v1 = face->v1;
  nearp = camera->near_plane;
  clip_inter1 = (nearp - vertices[p1].cz) / (vertices[p2].cz - vertices[p1].cz);
  cx = vertices[p1].cx + (vertices[p2].cx - vertices[p1].cx) * clip_inter1;
  cy = vertices[p1].cy + (vertices[p2].cy - vertices[p1].cy) * clip_inter1;
  face->u1 = u1 + (face->u2 - u1) * clip_inter1;
  face->v1 = v1 + (face->v2 - v1) * clip_inter1;
  vertices[S3DE_VERTEX_CLIP1].px = (cx * camera->view_dist / nearp) + camera->centerx;
  vertices[S3DE_VERTEX_CLIP1].py = (-cy * camera->view_dist / nearp) + camera->centery;
  vertices[S3DE_VERTEX_CLIP1].pz = nearp;
  face->p1 = S3DE_VERTEX_CLIP1;
  SAGE_AddTexturedTriangleP1(vertices, face);
  clip_inter2 = (nearp - vertices[p1].cz) / (vertices[p3].cz - vertices[p1].cz);
  cx = vertices[p1].cx + (vertices[p3].cx - vertices[p1].cx) * clip_inter2;
  cy = vertices[p1].cy + (vertices[p3].cy - vertices[p1].cy) * clip_inter2;
  face->u4 = u1 + (face->u3 - u1) * clip_inter2;
  face->v4 = v1 + (face->v3 - v1) * clip_inter2;
  vertices[S3DE_VERTEX_CLIP2].px = (cx * camera->view_dist / nearp) + camera->centerx;
  vertices[S3DE_VERTEX_CLIP2].py = (-cy * camera->view_dist / nearp) + camera->centery;
  vertices[S3DE_VERTEX_CLIP2].pz = nearp;
  face->p4 = S3DE_VERTEX_CLIP2;
  SAGE_AddTexturedTriangleP2(vertices, face);
}

/**
 * Clip the two first points of the face against near plane
 */
VOID SAGE_ClipTwoFacePoint(SAGE_TransformedVertex * vertices, SAGE_Face * face, SAGE_Camera * camera)
{
  ULONG p1, p2, p3;
  FLOAT u1, v1, u2, v2, nearp, clip_inter1, clip_inter2, cx, cy;

  SED(SAGE_DebugLog("** SAGE_ClipTwoFacePoint()");)
  p1 = face->p1;
  p2 = face->p2;
  p3 = face->p3;
  u1 = face->u1;
  v1 = face->v1;
  u2 = face->u2;
  v2 = face->v2;
  nearp = camera->near_plane;
  clip_inter1 = (nearp - vertices[p1].cz) / (vertices[p3].cz - vertices[p1].cz);
  cx = vertices[p1].cx + (vertices[p3].cx - vertices[p1].cx) * clip_inter1;
  cy = vertices[p1].cy + (vertices[p3].cy - vertices[p1].cy) * clip_inter1;
  face->u1 = u1 + (face->u3 - u1) * clip_inter1;
  face->v1 = v1 + (face->v3 - v1) * clip_inter1;
  vertices[S3DE_VERTEX_CLIP1].px = (cx * camera->view_dist / nearp) + camera->centerx;
  vertices[S3DE_VERTEX_CLIP1].py = (-cy * camera->view_dist / nearp) + camera->centery;
  vertices[S3DE_VERTEX_CLIP1].pz = nearp;
  clip_inter2 = (nearp - vertices[p2].cz) / (vertices[p3].cz - vertices[p2].cz);
  cx = vertices[p2].cx + (vertices[p3].cx - vertices[p2].cx) * clip_inter2;
  cy = vertices[p2].cy + (vertices[p3].cy - vertices[p2].cy) * clip_inter2;
  face->u2 = u2 + (face->u3 - u2) * clip_inter2;
  face->v2 = v2 + (face->v3 - v2) * clip_inter2;
  vertices[S3DE_VERTEX_CLIP2].px = (cx * camera->view_dist / nearp) + camera->centerx;
  vertices[S3DE_VERTEX_CLIP2].py = (-cy * camera->view_dist / nearp) + camera->centery;
  vertices[S3DE_VERTEX_CLIP2].pz = nearp;
  face->p1 = S3DE_VERTEX_CLIP1;
  face->p2 = S3DE_VERTEX_CLIP2;
  SAGE_AddTexturedTriangleP1(vertices, face);
}

/**
 * Set the list of faces to render and clip them against near plane if necessary
 */
VOID SAGE_SetClippedFaceList(SAGE_TransformedVertex * vertices, SAGE_Face * faces, UWORD nb_faces, SAGE_Camera * camera)
{
  UWORD index;
  SAGE_Face * face, clipped_face;

  SED(SAGE_DebugLog("** SAGE_SetClippedFaceList(nb_faces %d)", nb_faces);)
  for (index = 0;index < nb_faces;index++) {
    face = &(faces[index]);
    if (!face->culled) {
      if (face->clipped == S3DE_NOCLIP) {
        SAGE_AddTexturedTriangleP1(vertices, face);
        if (face->is_quad) {
          SAGE_AddTexturedTriangleP2(vertices, face);
        }
      } else {
        // Check for points 1, 2 and 3
        switch (face->clipped & S3DE_MASKP4) {
          case S3DE_NOCLIP:
            SAGE_AddTexturedTriangleP1(vertices, face);
            break;
          case S3DE_P1CLIP:
            clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
            clipped_face.p2 = face->p2; clipped_face.u2 = face->u2; clipped_face.v2 = face->v2;
            clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
            clipped_face.color = face->color; clipped_face.texture = face->texture;
            SAGE_ClipOneFacePoint(vertices, &clipped_face, camera);
            break;
          case S3DE_P2CLIP:
            clipped_face.p1 = face->p2; clipped_face.u1 = face->u2; clipped_face.v1 = face->v2;
            clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
            clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
            clipped_face.color = face->color; clipped_face.texture = face->texture;
            SAGE_ClipOneFacePoint(vertices, &clipped_face, camera);
            break;
          case S3DE_P3CLIP:
            clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
            clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
            clipped_face.p3 = face->p2; clipped_face.u3 = face->u2; clipped_face.v3 = face->v2;
            clipped_face.color = face->color; clipped_face.texture = face->texture;
            SAGE_ClipOneFacePoint(vertices, &clipped_face, camera);
            break;
          case S3DE_P1CLIP|S3DE_P2CLIP:
            clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
            clipped_face.p2 = face->p2; clipped_face.u2 = face->u2; clipped_face.v2 = face->v2;
            clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
            clipped_face.color = face->color; clipped_face.texture = face->texture;
            SAGE_ClipTwoFacePoint(vertices, &clipped_face, camera);
            break;
          case S3DE_P1CLIP|S3DE_P3CLIP:
            clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
            clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
            clipped_face.p3 = face->p2; clipped_face.u3 = face->u2; clipped_face.v3 = face->v2;
            clipped_face.color = face->color; clipped_face.texture = face->texture;
            SAGE_ClipTwoFacePoint(vertices, &clipped_face, camera);
            break;
          case S3DE_P2CLIP|S3DE_P3CLIP:
            clipped_face.p1 = face->p2; clipped_face.u1 = face->u2; clipped_face.v1 = face->v2;
            clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
            clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
            clipped_face.color = face->color; clipped_face.texture = face->texture;
            SAGE_ClipTwoFacePoint(vertices, &clipped_face, camera);
            break;
        }
        if (face->is_quad) {
          // Check for points 1,3 and 4
          switch (face->clipped & S3DE_MASKP2) {
            case S3DE_NOCLIP:
              SAGE_AddTexturedTriangleP2(vertices, face);
              break;
            case S3DE_P1CLIP:
              clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
              clipped_face.p2 = face->p4; clipped_face.u2 = face->u4; clipped_face.v2 = face->v4;
              clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
              clipped_face.color = face->color; clipped_face.texture = face->texture;
              SAGE_ClipOneFacePoint(vertices, &clipped_face, camera);
              break;
            case S3DE_P4CLIP:
              clipped_face.p1 = face->p4; clipped_face.u1 = face->u4; clipped_face.v1 = face->v4;
              clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
              clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
              clipped_face.color = face->color; clipped_face.texture = face->texture;
              SAGE_ClipOneFacePoint(vertices, &clipped_face, camera);
              break;
            case S3DE_P3CLIP:
              clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
              clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
              clipped_face.p3 = face->p4; clipped_face.u3 = face->u4; clipped_face.v3 = face->v4;
              clipped_face.color = face->color; clipped_face.texture = face->texture;
              SAGE_ClipOneFacePoint(vertices, &clipped_face, camera);
              break;
            case S3DE_P1CLIP|S3DE_P4CLIP:
              clipped_face.p1 = face->p1; clipped_face.u1 = face->u1; clipped_face.v1 = face->v1;
              clipped_face.p2 = face->p4; clipped_face.u2 = face->u4; clipped_face.v2 = face->v4;
              clipped_face.p3 = face->p3; clipped_face.u3 = face->u3; clipped_face.v3 = face->v3;
              clipped_face.color = face->color; clipped_face.texture = face->texture;
              SAGE_ClipTwoFacePoint(vertices, &clipped_face, camera);
              break;
            case S3DE_P1CLIP|S3DE_P3CLIP:
              clipped_face.p1 = face->p3; clipped_face.u1 = face->u3; clipped_face.v1 = face->v3;
              clipped_face.p2 = face->p1; clipped_face.u2 = face->u1; clipped_face.v2 = face->v1;
              clipped_face.p3 = face->p4; clipped_face.u3 = face->u4; clipped_face.v3 = face->v4;
              clipped_face.color = face->color; clipped_face.texture = face->texture;
              SAGE_ClipTwoFacePoint(vertices, &clipped_face, camera);
              break;
            case S3DE_P4CLIP|S3DE_P3CLIP:
              clipped_face.p1 = face->p4; clipped_face.u1 = face->u4; clipped_face.v1 = face->v4;
              clipped_face.p2 = face->p3; clipped_face.u2 = face->u3; clipped_face.v2 = face->v3;
              clipped_face.p3 = face->p1; clipped_face.u3 = face->u1; clipped_face.v3 = face->v1;
              clipped_face.color = face->color; clipped_face.texture = face->texture;
              SAGE_ClipTwoFacePoint(vertices, &clipped_face, camera);
              break;
          }
        }
      }
      sage_world.metrics.rendered_faces++;
    }
  }
}

/*****************************************************************************
 *            SKYBOX TRANSFORMATIONS
 *****************************************************************************/

#if SAGE_ENABLE_SKYBOX == 1

/**
 * Transform skybox planes points to camera view
 */
VOID SAGE_SkyboxPlaneWorldToCamera(SAGE_Skybox * skybox, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD plane, edge, point;
  FLOAT x, y, z;
  
  SED(SAGE_DebugLog("** SAGE_SkyboxPlaneWorldToCamera()");)
  for (plane = 0;plane < S3DE_SKYBOX_PLANES;plane++) {
    for (edge = 0;edge < S3DE_SKYBOX_PLANEEDGES;edge++) {
      point = skybox->planes[plane].edges[edge];
      SED(SAGE_DebugLog(" - plane %d  edge %d  point=%d", plane, edge, point);)
      if (!vertices[point].calculated) {
        x = skybox->vertices[point].x;
        y = skybox->vertices[point].y;
        z = skybox->vertices[point].z;
        vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
        vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
        vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
        vertices[point].calculated = TRUE;
        sage_world.metrics.calculated_vertices++;
      }
    }
  }
}

/**
 * Check for visible planes of skybox
 */
BOOL SAGE_SkyboxPlaneVisibility(SAGE_Skybox * skybox, UWORD plane, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD p1, p2, p3, p4;
  FLOAT nearp, z1, z2, z3, z4;

  SED(SAGE_DebugLog("** SAGE_SkyboxPlaneVisibility()");)
  // Check for Z culling
  nearp = camera->near_plane;
  p1 = skybox->planes[plane].edges[0];
  z1 = vertices[p1].cz;
  p2 = skybox->planes[plane].edges[1];
  z2 = vertices[p2].cz;
  p3 = skybox->planes[plane].edges[2];
  z3 = vertices[p3].cz;
  p4 = skybox->planes[plane].edges[3];
  z4 = vertices[p4].cz;
  SED(SAGE_DebugLog(" - nearp %f  p1=%d z1=%f  p2=%d z2=%f  p3=%d z3=%f  p4=%d z4=%f", nearp, p1, z1, p2, z2, p3, z3, p4, z4);)
  if (z1<nearp && z2<nearp && z3<nearp && z4<nearp) {
    return FALSE;
  }
  sage_world.metrics.rendered_planes++;
  // Set plane edges as visible
  vertices[p1].visible = TRUE;
  vertices[p2].visible = TRUE;
  vertices[p3].visible = TRUE;
  vertices[p4].visible = TRUE;
  return TRUE;
}

/**
 * Transform skybox plane faces to camera view
 */
VOID SAGE_SkyboxFaceWorldToCamera(SAGE_Skybox * skybox, UWORD plane, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, point;
  FLOAT x, y, z;

  SED(SAGE_DebugLog("** SAGE_SkyboxFaceWorldToCamera()");)
  for (index = 0;index < S3DE_SKYBOX_FACEBYPLANE;index++) {
    point = skybox->planes[plane].faces[index].p1;
    if (!vertices[point].calculated) {
      x = skybox->vertices[point].x;
      y = skybox->vertices[point].y;
      z = skybox->vertices[point].z;
      vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
      vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
      vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
      vertices[point].calculated = TRUE;
      vertices[point].visible = TRUE;
      sage_world.metrics.calculated_vertices++;
    }
    point = skybox->planes[plane].faces[index].p2;
    if (!vertices[point].calculated) {
      x = skybox->vertices[point].x;
      y = skybox->vertices[point].y;
      z = skybox->vertices[point].z;
      vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
      vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
      vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
      vertices[point].calculated = TRUE;
      vertices[point].visible = TRUE;
      sage_world.metrics.calculated_vertices++;
    }
    point = skybox->planes[plane].faces[index].p3;
    if (!vertices[point].calculated) {
      x = skybox->vertices[point].x;
      y = skybox->vertices[point].y;
      z = skybox->vertices[point].z;
      vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
      vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
      vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
      vertices[point].calculated = TRUE;
      vertices[point].visible = TRUE;
      sage_world.metrics.calculated_vertices++;
    }
    point = skybox->planes[plane].faces[index].p4;
    if (!vertices[point].calculated) {
      x = skybox->vertices[point].x;
      y = skybox->vertices[point].y;
      z = skybox->vertices[point].z;
      vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
      vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
      vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
      vertices[point].calculated = TRUE;
      vertices[point].visible = TRUE;
      sage_world.metrics.calculated_vertices++;
    }
  }
}

/**
 * Check if skybox plane faces are clipped
 */
VOID SAGE_SkyboxFaceClipping(SAGE_Skybox * skybox, UWORD plane, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, p1, p2, p3, p4;
  FLOAT nearp, farp, x1plane, y1plane, x2plane, y2plane, x3plane, y3plane, x4plane, y4plane;
  FLOAT x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;

  SED(SAGE_DebugLog("** SAGE_SkyboxFaceClipping()");)
  nearp = camera->near_plane;
  farp = camera->far_plane;
  for (index = 0;index < S3DE_SKYBOX_FACEBYPLANE;index++) {
    p1 = skybox->planes[plane].faces[index].p1;
    x1 = vertices[p1].cx; y1 = vertices[p1].cy; z1 = vertices[p1].cz;
    p2 = skybox->planes[plane].faces[index].p2;
    x2 = vertices[p2].cx; y2 = vertices[p2].cy; z2 = vertices[p2].cz;
    p3 = skybox->planes[plane].faces[index].p3;
    x3 = vertices[p3].cx; y3 = vertices[p3].cy; z3 = vertices[p3].cz;
    p4 = skybox->planes[plane].faces[index].p4;
    x4 = vertices[p4].cx; y4 = vertices[p4].cy; z4 = vertices[p4].cz;
    // Check if the face is outside of X planes
    x1plane = (camera->centerx * z1) / camera->view_dist;
    x2plane = (camera->centerx * z2) / camera->view_dist;
    x3plane = (camera->centerx * z3) / camera->view_dist;
    x4plane = (camera->centerx * z4) / camera->view_dist;
    if ((x1>x1plane && x2>x2plane && x3>x3plane && x4>x4plane) || (x1<-x1plane && x2<-x2plane && x3<-x3plane && x4<-x4plane)) {
      skybox->planes[plane].faces[index].culled = TRUE;
    } else {
      // Check if the face is outside of Y planes
      y1plane = (camera->centery * z1) / camera->view_dist;
      y2plane = (camera->centery * z2) / camera->view_dist;
      y3plane = (camera->centery * z3) / camera->view_dist;
      y4plane = (camera->centery * z4) / camera->view_dist;
      if ((y1>y1plane && y2>y2plane && y3>y3plane && y4>y4plane) || (y1<-y1plane && y2<-y2plane && y3<-y3plane && y4<-y4plane)) {
        skybox->planes[plane].faces[index].culled = TRUE;
      } else {
        // Check if the face is totally or partially outside of Z planes
        if ((z1<nearp && z2<nearp && z3<nearp && z4<nearp) || (z1>farp && z2>farp && z3>farp && z4>farp)) {
          skybox->planes[plane].faces[index].culled = TRUE;
        } else {
          skybox->planes[plane].faces[index].culled = FALSE;
          skybox->planes[plane].faces[index].clipped = S3DE_NOCLIP;
          if (z1 < nearp) skybox->planes[plane].faces[index].clipped |= S3DE_P1CLIP;
          if (z2 < nearp) skybox->planes[plane].faces[index].clipped |= S3DE_P2CLIP;
          if (z3 < nearp) skybox->planes[plane].faces[index].clipped |= S3DE_P3CLIP;
          if (z4 < nearp) skybox->planes[plane].faces[index].clipped |= S3DE_P4CLIP;
        }
      }
    }
  }
}

/**
 * Transform skybox to camera view
 */
VOID SAGE_TransformSkybox(SAGE_Camera * camera)
{
  UWORD plane;
  SAGE_Skybox * skybox;
  
  SED(SAGE_DebugLog("** SAGE_TransformSkybox()");)
  skybox = &sage_world.skybox;
  sage_world.metrics.total_planes = S3DE_SKYBOX_PLANES;
  sage_world.metrics.total_vertices += S3DE_SKYBOX_VERTICES;
  sage_world.metrics.total_faces += (S3DE_SKYBOX_FACEBYPLANE * S3DE_SKYBOX_PLANES);
  SAGE_ClearTransformedVertices(sage_world.transformed_vertices, S3DE_SKYBOX_VERTICES);
  SAGE_SkyboxPlaneWorldToCamera(skybox, camera, sage_world.transformed_vertices);
  for (plane = 0;plane < S3DE_SKYBOX_PLANES;plane++) {
    if (SAGE_SkyboxPlaneVisibility(skybox, plane, camera, sage_world.transformed_vertices)) {
      SD(if (engine_debug) SAGE_DebugLog(" - plane %d is visible", plane);)
      SAGE_SkyboxFaceWorldToCamera(skybox, plane, camera, sage_world.transformed_vertices);
      if (!skybox->planes[plane].culled) {
        SAGE_SkyboxFaceClipping(skybox, plane, camera, sage_world.transformed_vertices);
      }
    }
  }
  SAGE_VerticesProjection(sage_world.transformed_vertices, S3DE_SKYBOX_VERTICES, camera);
  for (plane = 0;plane < S3DE_SKYBOX_PLANES;plane++) {
    if (!skybox->planes[plane].culled) {
      SAGE_SetClippedFaceList(sage_world.transformed_vertices, skybox->planes[plane].faces, S3DE_SKYBOX_FACEBYPLANE, camera);
    }
  }
}

#endif

/*****************************************************************************
 *            TERRAIN TRANSFORMATIONS
 *****************************************************************************/

#if SAGE_ENABLE_TERRAIN == 1

/**
 * Tell if the zone is in the camera view, partially clipped or totally culled
 */
BOOL SAGE_TerrainZoneVisibility(SAGE_Zone * zone, SAGE_Camera * camera)
{
  FLOAT cx, cy, cz, x, y ,z;
  FLOAT radius, xplane, yplane;

  SED(SAGE_DebugLog("** SAGE_TerrainZoneVisibility()");)
  zone->culled = TRUE;
  cx = zone->posx - camera->posx;
  cy = zone->posy - camera->posy;
  cz = zone->posz - camera->posz;
  x = cx*CameraMatrix.m11 + cy*CameraMatrix.m21 + cz*CameraMatrix.m31;
  y = cx*CameraMatrix.m12 + cy*CameraMatrix.m22 + cz*CameraMatrix.m32;
  z = cx*CameraMatrix.m13 + cy*CameraMatrix.m23 + cz*CameraMatrix.m33;
  radius = zone->radius;
  SED(SAGE_DebugLog("  => x %f  y %f  z %f  r %f", x, y, z, radius);)
  // Check against Z planes
  if (((z-radius) > camera->far_plane) || ((z+radius) < camera->near_plane)) {
    SED(SAGE_DebugLog("  => this zone is outside (far or near)");)
    return FALSE;
  }
  // Check against X planes
  xplane = (camera->centerx * z) / camera->view_dist;
  if (((x-radius) > xplane) || ((x+radius) < -xplane)) {
    SED(SAGE_DebugLog("  => this zone is outside (X plane)");)
    return FALSE;
  }
  // Check against Y planes
  yplane = (camera->centery * z) / camera->view_dist;
  if (((y-radius) > yplane) || ((y+radius) < -yplane)) {
    SED(SAGE_DebugLog("  => this zone is outside (Y plane)");)
    return FALSE;
  }
  // Check for partial clipping
  if (((z+radius) > camera->far_plane) || ((z-radius) < camera->near_plane)) {
    zone->clipped = TRUE;
  } else if (((x+radius) > xplane) || ((x-radius) < -xplane)) {
    zone->clipped = TRUE;
  } else if (((y+radius) > yplane) || ((y-radius) < -yplane)) {
    zone->clipped = TRUE;
  } else {
    zone->clipped = FALSE;
  }
  zone->culled = FALSE;
  SED(SAGE_DebugLog("  => this zone is visible");)
  return TRUE;
}

/**
 * Set the level of detail of the terrain zone
 */
VOID SAGE_TerrainZoneLevelOfDetail(SAGE_Zone * zone, SAGE_Camera * camera)
{
  FLOAT distance, x, y, z;

  SED(SAGE_DebugLog("** SAGE_TerrainZoneLevelOfDetail()");)
  x = camera->posx - zone->posx;
  y = camera->posy - zone->posy;
  z = camera->posz - zone->posz;
  distance = sqrt((x*x) + (y*y) + (z*z));
  if (distance > S3DE_LOD_L3) {
    zone->lod = S3DE_LOD_LOW;
  } else if (distance > S3DE_LOD_L2) {
    zone->lod = S3DE_LOD_MEDIUM;
  } else if (distance > S3DE_LOD_L1) {
    zone->lod = S3DE_LOD_HIGH;
  } else {
    zone->lod = S3DE_LOD_FULL;
  }
  SED(SAGE_DebugLog("  => lod is %d", zone->lod);)
}

/**
 * Remove not visible faces for a zone
 */
VOID SAGE_TerrainZoneBackfaceCulling(SAGE_Terrain * terrain, SAGE_Zone * zone, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, point;
  FLOAT res;
  SAGE_Vector sight, normal;

  SED(SAGE_DebugLog("** SAGE_TerrainZoneBackfaceCulling()");)
  for (index = 0;index < zone->nb_faces;index++) {
    // Normal is already in world coordinates
    normal.x = zone->normals[index].x;
    normal.y = zone->normals[index].y;
    normal.z = zone->normals[index].z;
    point = zone->faces[index].p1;
    // Build the camera sight
    sight.x = camera->posx - terrain->vertices[point].x;
    sight.y = camera->posy - terrain->vertices[point].y;
    sight.z = camera->posz - terrain->vertices[point].z;
    // Check face visibility (u*v = xu*xv + yu*yv + zu*zv)
    res = (normal.x*sight.x) + (normal.y*sight.y) + (normal.z*sight.z);
    if (res > 0.0) {
      zone->faces[index].culled = FALSE;
      // Set all faces vertices as visible
      vertices[point].visible = TRUE;
      point = zone->faces[index].p2;
      vertices[point].visible = TRUE;
      point = zone->faces[index].p3;
      vertices[point].visible = TRUE;
      SED(SAGE_DebugLog("  => face %d is visible", index);)
    } else {
      zone->faces[index].culled = TRUE;
      SED(SAGE_DebugLog("  => face %d is culled", index);)
    }
    zone->faces[index].clipped = S3DE_NOCLIP;
  }
}

/**
 * Transform terrain zone vertices to camera view
 */
VOID SAGE_TerrainZoneWorldToCamera(SAGE_Terrain * terrain, SAGE_Zone * zone, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, point;
  FLOAT x, y, z;

  SED(SAGE_DebugLog("** SAGE_TerrainZoneWorldToCamera()");)
  for (index = 0;index < zone->nb_faces;index++) {
    if (!zone->faces[index].culled) {
      point = zone->faces[index].p1;
      if (vertices[point].visible && !vertices[point].calculated) {
        x = terrain->vertices[point].x - camera->posx;
        y = terrain->vertices[point].y - camera->posy;
        z = terrain->vertices[point].z - camera->posz;
        vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
        vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
        vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
        vertices[point].calculated = TRUE;
        sage_world.metrics.calculated_vertices++;
      }
      point = zone->faces[index].p2;
      if (vertices[point].visible && !vertices[point].calculated) {
        x = terrain->vertices[point].x - camera->posx;
        y = terrain->vertices[point].y - camera->posy;
        z = terrain->vertices[point].z - camera->posz;
        vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
        vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
        vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
        vertices[point].calculated = TRUE;
        sage_world.metrics.calculated_vertices++;
      }
      point = zone->faces[index].p3;
      if (vertices[point].visible && !vertices[point].calculated) {
        x = terrain->vertices[point].x - camera->posx;
        y = terrain->vertices[point].y - camera->posy;
        z = terrain->vertices[point].z - camera->posz;
        vertices[point].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
        vertices[point].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
        vertices[point].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
        vertices[point].calculated = TRUE;
        sage_world.metrics.calculated_vertices++;
      }
    }
  }
}

/**
 * Check if terrain zone faces are clipped
 */
VOID SAGE_TerrainZoneFaceClipping(SAGE_Terrain * terrain, SAGE_Zone * zone, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, p1, p2, p3;
  FLOAT nearp, farp, x1plane, y1plane, x2plane, y2plane, x3plane, y3plane;
  FLOAT x1, y1, z1, x2, y2, z2, x3, y3, z3;

  SED(SAGE_DebugLog("** SAGE_TerrainZoneFaceClipping()");)
  nearp = camera->near_plane;
  farp = camera->far_plane;
  for (index = 0;index < zone->nb_faces;index++) {
    p1 = zone->faces[index].p1;
    x1 = vertices[p1].cx; y1 = vertices[p1].cy; z1 = vertices[p1].cz;
    p2 = zone->faces[index].p2;
    x2 = vertices[p2].cx; y2 = vertices[p2].cy; z2 = vertices[p2].cz;
    p3 = zone->faces[index].p3;
    x3 = vertices[p3].cx; y3 = vertices[p3].cy; z3 = vertices[p3].cz;
    // Check if the face is outside of X planes
    x1plane = (camera->centerx * z1) / camera->view_dist;
    x2plane = (camera->centerx * z2) / camera->view_dist;
    x3plane = (camera->centerx * z3) / camera->view_dist;
    if ((x1>x1plane && x2>x2plane && x3>x3plane) || (x1<-x1plane && x2<-x2plane && x3<-x3plane)) {
      zone->faces[index].culled = TRUE;
    } else {
      // Check if the face is outside of Y planes
      y1plane = (camera->centery * z1) / camera->view_dist;
      y2plane = (camera->centery * z2) / camera->view_dist;
      y3plane = (camera->centery * z3) / camera->view_dist;
      if ((y1>y1plane && y2>y2plane && y3>y3plane) || (y1<-y1plane && y2<-y2plane && y3<-y3plane)) {
        zone->faces[index].culled = TRUE;
      } else {
        // Check if the face is totally or partially outside of Z planes
        if ((z1<nearp && z2<nearp && z3<nearp) || (z1>farp && z2>farp && z3>farp)) {
          zone->faces[index].culled = TRUE;
        } else {
          zone->faces[index].culled = FALSE;
          zone->faces[index].clipped = S3DE_NOCLIP;
          if (z1 < nearp) zone->faces[index].clipped |= S3DE_P1CLIP;
          if (z2 < nearp) zone->faces[index].clipped |= S3DE_P2CLIP;
          if (z3 < nearp) zone->faces[index].clipped |= S3DE_P3CLIP;
        }
      }
    }
  }
}

/**
 * Transform terrain to camera view
 */
VOID SAGE_TransformTerrain(SAGE_Camera * camera)
{
  SAGE_Zone * zone;
  UWORD index;

  SED(SAGE_DebugLog("** Transform terrain **");)
  SAGE_ClearTransformedVertices(sage_world.transformed_vertices, sage_world.terrain.nb_vertices);
  sage_world.metrics.total_vertices += sage_world.terrain.nb_vertices;
  for (index = 0;index < sage_world.terrain.nb_zones;index++) {
    zone = sage_world.terrain.zones[index];
    if (zone != NULL && !zone->disabled) {
      SED(SAGE_DebugLog("** Processing zone %d", index);)
      sage_world.metrics.total_zones++;
      sage_world.metrics.total_faces += zone->nb_faces;
      if (SAGE_TerrainZoneVisibility(zone, camera)) {
        sage_world.metrics.rendered_zones++;
        SAGE_TerrainZoneLevelOfDetail(zone, camera);
        SAGE_TerrainZoneBackfaceCulling(&sage_world.terrain, zone, camera, sage_world.transformed_vertices);
        SAGE_TerrainZoneWorldToCamera(&sage_world.terrain, zone, camera, sage_world.transformed_vertices);
        if (zone->clipped) {
          SAGE_TerrainZoneFaceClipping(&sage_world.terrain, zone, camera, sage_world.transformed_vertices);
        }
      }
    }
  }
  SED(SAGE_DumpTerrain(S3DE_DEBUG_TZONES);)
  SAGE_VerticesProjection(sage_world.transformed_vertices, sage_world.terrain.nb_vertices, camera);
  for (index = 0;index < sage_world.terrain.nb_zones;index++) {
    zone = sage_world.terrain.zones[index];
    if (zone != NULL && !zone->disabled && !zone->culled) {
      SAGE_SetClippedFaceList(sage_world.transformed_vertices, zone->faces, zone->nb_faces, camera);
    }
  }
}

#endif

/*****************************************************************************
 *            ENTITIES TRANSFORMATIONS
 *****************************************************************************/

#if SAGE_ENABLE_ENTITIES == 1

/**
 * Tell if the entity is in the camera view, partially clipped or totally culled
 */
BOOL SAGE_EntityVisibility(SAGE_Entity * entity, SAGE_Camera * camera)
{
  FLOAT cx, cy, cz, x, y ,z;
  FLOAT radius, xplane, yplane;

  SED(SAGE_DebugLog("** SAGE_EntityVisibility()");)
  entity->culled = TRUE;
  cx = entity->posx - camera->posx;
  cy = entity->posy - camera->posy;
  cz = entity->posz - camera->posz;
  x = cx*CameraMatrix.m11 + cy*CameraMatrix.m21 + cz*CameraMatrix.m31;
  y = cx*CameraMatrix.m12 + cy*CameraMatrix.m22 + cz*CameraMatrix.m32;
  z = cx*CameraMatrix.m13 + cy*CameraMatrix.m23 + cz*CameraMatrix.m33;
  radius = entity->radius;
  SED(SAGE_DebugLog("  => x %f  y %f  z %f  r %f", x, y, z, radius);)
  // Check against Z planes
  if (((z-radius) > camera->far_plane) || ((z+radius) < camera->near_plane)) {
    SED(SAGE_DebugLog("  => this entity is outside (far or near)");)
    return FALSE;
  }
  // Check against X planes
  xplane = (camera->centerx * z) / camera->view_dist;
  if (((x-radius) > xplane) || ((x+radius) < -xplane)) {
    SED(SAGE_DebugLog("  => this entity is outside (X plane)");)
    return FALSE;
  }
  // Check against Y planes
  yplane = (camera->centery * z) / camera->view_dist;
  if (((y-radius) > yplane) || ((y+radius) < -yplane)) {
    SED(SAGE_DebugLog("  => this entity is outside (Y plane)");)
    return FALSE;
  }
  // Check for partial clipping
  if (((z+radius) > camera->far_plane) || ((z-radius) < camera->near_plane)) {
    entity->clipped = TRUE;
  } else if (((x+radius) > xplane) || ((x-radius) < -xplane)) {
    entity->clipped = TRUE;
  } else if (((y+radius) > yplane) || ((y-radius) < -yplane)) {
    entity->clipped = TRUE;
  } else {
    entity->clipped = FALSE;
  }
  entity->culled = FALSE;
  SED(SAGE_DebugLog("  => this entity is visible");)
  return TRUE;
}

/**
 * Remove not visible faces for an entity and reset clipped status
 */
VOID SAGE_EntityBackfaceCulling(SAGE_Entity * entity, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, point;
  FLOAT res, x, y, z, tx, ty, tz;
  SAGE_Vector sight, normal;

  SED(SAGE_DebugLog("** SAGE_EntityBackfaceCulling()");)
  for (index = 0;index < entity->nb_faces;index++) {
    // Transform face normal to world space
    x = entity->normals[index].x;
    y = entity->normals[index].y;
    z = entity->normals[index].z;
    normal.x = x*EntityMatrix.m11 + y*EntityMatrix.m21 + z*EntityMatrix.m31;
    normal.y = x*EntityMatrix.m12 + y*EntityMatrix.m22 + z*EntityMatrix.m32;
    normal.z = x*EntityMatrix.m13 + y*EntityMatrix.m23 + z*EntityMatrix.m33;
    // Transform face vertex to world space
    point = entity->faces[index].p1;
    if (!vertices[point].calculated) {
      x = entity->vertices[point].x;
      y = entity->vertices[point].y;
      z = entity->vertices[point].z;
      vertices[point].wx = x*EntityMatrix.m11 + y*EntityMatrix.m21 + z*EntityMatrix.m31 + entity->posx;
      vertices[point].wy = x*EntityMatrix.m12 + y*EntityMatrix.m22 + z*EntityMatrix.m32 + entity->posy;
      vertices[point].wz = x*EntityMatrix.m13 + y*EntityMatrix.m23 + z*EntityMatrix.m33 + entity->posz;
      vertices[point].calculated = TRUE;
      sage_world.metrics.calculated_vertices++;
    }
    tx = vertices[point].wx;
    ty = vertices[point].wy;
    tz = vertices[point].wz;
    // Build the camera sight
    sight.x = camera->posx - tx;
    sight.y = camera->posy - ty;
    sight.z = camera->posz - tz;
    // Check face visibility (u*v = xu*xv + yu*yv + zu*zv)
    res = (normal.x*sight.x) + (normal.y*sight.y) + (normal.z*sight.z);
    if (res > 0.0) {
      entity->faces[index].culled = FALSE;
      // Set all faces vertices as visible
      vertices[point].visible = TRUE;
      point = entity->faces[index].p2;
      vertices[point].visible = TRUE;
      point = entity->faces[index].p3;
      vertices[point].visible = TRUE;
      if (entity->faces[index].is_quad) {
        point = entity->faces[index].p4;
        vertices[point].visible = TRUE;
      }
      SED(SAGE_DebugLog(" => face %d is visible", index);)
    } else {
      entity->faces[index].culled = TRUE;
      SED(SAGE_DebugLog(" =>Face %d is culled", index);)
    }
    entity->faces[index].clipped = S3DE_NOCLIP;
  }
}

/**
 * Transform the entity vertices to world coordinates
 */
VOID SAGE_EntityLocalToWorld(SAGE_Entity * entity, SAGE_TransformedVertex * vertices)
{
  UWORD index;
  FLOAT x, y, z;
  
  SED(SAGE_DebugLog("** SAGE_EntityLocalToWorld()");)
  for (index = 0;index < entity->nb_vertices;index++) {
    if (vertices[index].visible && !vertices[index].calculated) {
      x = entity->vertices[index].x;
      y = entity->vertices[index].y;
      z = entity->vertices[index].z;
      vertices[index].wx = x*EntityMatrix.m11 + y*EntityMatrix.m21 + z*EntityMatrix.m31 + entity->posx;
      vertices[index].wy = x*EntityMatrix.m12 + y*EntityMatrix.m22 + z*EntityMatrix.m32 + entity->posy;
      vertices[index].wz = x*EntityMatrix.m13 + y*EntityMatrix.m23 + z*EntityMatrix.m33 + entity->posz;
      vertices[index].calculated = TRUE;
      sage_world.metrics.calculated_vertices++;
    }
  }
}

/**
 * Transform the world vertices to camera view coordinates
 */
VOID SAGE_EntityWorldToCamera(SAGE_Entity * entity, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index;
  FLOAT x, y, z;
  
  SED(SAGE_DebugLog("** SAGE_EntityWorldToCamera()");)
  for (index = 0;index < entity->nb_vertices;index++) {
    if (vertices[index].visible) {
      x = vertices[index].wx - camera->posx;
      y = vertices[index].wy - camera->posy;
      z = vertices[index].wz - camera->posz;
      vertices[index].cx = x*CameraMatrix.m11 + y*CameraMatrix.m21 + z*CameraMatrix.m31;
      vertices[index].cy = x*CameraMatrix.m12 + y*CameraMatrix.m22 + z*CameraMatrix.m32;
      vertices[index].cz = x*CameraMatrix.m13 + y*CameraMatrix.m23 + z*CameraMatrix.m33;
    }
  }
}

/**
 * Check if entity faces are clipped
 */
VOID SAGE_EntityFaceClipping(SAGE_Entity * entity, SAGE_Camera * camera, SAGE_TransformedVertex * vertices)
{
  UWORD index, p1, p2, p3, p4;
  FLOAT nearp, farp, x1plane, y1plane, x2plane, y2plane, x3plane, y3plane, x4plane, y4plane;
  FLOAT x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;

  SED(SAGE_DebugLog("** SAGE_EntityFaceClipping()");)
  nearp = camera->near_plane;
  farp = camera->far_plane;
  for (index = 0;index < entity->nb_faces;index++) {
    if (!entity->faces[index].culled) {
      p1 = entity->faces[index].p1;
      x1 = vertices[p1].cx; y1 = vertices[p1].cy; z1 = vertices[p1].cz;
      p2 = entity->faces[index].p2;
      x2 = vertices[p2].cx; y2 = vertices[p2].cy; z2 = vertices[p2].cz;
      p3 = entity->faces[index].p3;
      x3 = vertices[p3].cx; y3 = vertices[p3].cy; z3 = vertices[p3].cz;
      if (entity->faces[index].is_quad) {
        p4 = entity->faces[index].p4;
        x4 = vertices[p4].cx; y4 = vertices[p4].cy; z4 = vertices[p4].cz;
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
 * Transform entities to camera view and build triangle list
 */
VOID SAGE_TransformEntities(SAGE_Camera * camera)
{
  SAGE_Entity * entity;
  UWORD index;

  SED(SAGE_DebugLog("** Transform entities **");)
  for (index = 0;index < S3DE_MAX_ENTITIES;index++) {
    entity = sage_world.entities[index];
    if (entity != NULL && !entity->disabled) {
      SED(SAGE_DebugLog("** Processing entity %d", index);)
      sage_world.metrics.total_entities++;
      sage_world.metrics.total_vertices += entity->nb_vertices;
      sage_world.metrics.total_faces += entity->nb_faces;
      if (SAGE_EntityVisibility(entity, camera)) {
        sage_world.metrics.rendered_entities++;
        SAGE_ClearTransformedVertices(sage_world.transformed_vertices, entity->nb_vertices);
        SAGE_SetupEntityMatrix(entity);
        SAGE_EntityBackfaceCulling(entity, camera, sage_world.transformed_vertices);
        SAGE_EntityLocalToWorld(entity, sage_world.transformed_vertices);
        SAGE_EntityWorldToCamera(entity, camera, sage_world.transformed_vertices);
        if (entity->clipped) {
          SAGE_EntityFaceClipping(entity, camera, sage_world.transformed_vertices);
        }
        SAGE_VerticesProjection(sage_world.transformed_vertices, entity->nb_vertices, camera);
        SAGE_SetClippedFaceList(sage_world.transformed_vertices, entity->faces, entity->nb_faces, camera);
      }
    }
  }
}

#endif

/*****************************************************************************
 *            3D WORLD RENDERING
 *****************************************************************************/

/**
 * Init the 3D engine
 */
BOOL SAGE_Init3DEngine(VOID)
{
  SD(SAGE_DebugLog("Init 3D engine");)
  engine_debug = FALSE;
  sage_world.nb_materials = 0;
  sage_world.active_camera = 0;
  sage_world.active_skybox = FALSE;
  sage_world.active_terrain = FALSE;
  sage_world.nb_entities = 0;
  sage_world.transformed_vertices = (SAGE_TransformedVertex *)SAGE_AllocMem(sizeof(SAGE_TransformedVertex)*(S3DE_MAX_VERTICES+S3DE_CLIP_VERTICES));
  if (sage_world.transformed_vertices == NULL) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Release the 3D engine
 */
VOID SAGE_Release3DEngine()
{
  SD(SAGE_DebugLog("Release 3D engine");)
  if (sage_world.transformed_vertices != NULL) {
    SAGE_FreeMem(sage_world.transformed_vertices);
  }
  if (sage_world.active_terrain) {
    SAGE_ReleaseTerrain();
  }
  SAGE_FlushEntities();
  SAGE_FlushCameras();
  SAGE_FlushTextures();
  SAGE_FlushMaterials();
}

/**
 * Clear engine metrics
 */
VOID SAGE_ClearEngineMetrics(VOID)
{
  sage_world.metrics.rendered_planes = 0;
  sage_world.metrics.total_planes = 0;
  sage_world.metrics.rendered_zones = 0;
  sage_world.metrics.total_zones = 0;
  sage_world.metrics.rendered_entities = 0;
  sage_world.metrics.total_entities = 0;
  sage_world.metrics.calculated_vertices = 0;
  sage_world.metrics.rendered_vertices = 0;
  sage_world.metrics.total_vertices = 0;
  sage_world.metrics.rendered_faces = 0;
  sage_world.metrics.total_faces = 0;
  sage_world.metrics.rendered_triangles = 0;
}

/**
 * Render the 3D world
 */
VOID SAGE_RenderWorld(VOID)
{
  SAGE_Camera * camera;

  SED(SAGE_DebugLog("**** Rendering 3D World ****");)
  SAGE_ClearEngineMetrics();
  camera = sage_world.cameras[sage_world.active_camera];
  SED(SAGE_DumpCamera(camera);)
  if (camera != NULL) {
    SAGE_SetScreenClip(camera->view_left, camera->view_top, camera->view_width, camera->view_height);
    SAGE_SetupCameraMatrix(camera);
#if SAGE_ENABLE_SKYBOX == 1
    if (sage_world.active_skybox) {
      SAGE_TransformSkybox(camera);
      SAGE_Render3DTriangles();
    }
#endif
#if SAGE_ENABLE_TERRAIN == 1
    if (sage_world.active_terrain) {
      SED(SAGE_DumpTerrain(S3DE_DEBUG_TZONES);)
      SAGE_TransformTerrain(camera);
    }
#endif
#if SAGE_ENABLE_ENTITIES == 1
    if (sage_world.nb_entities > 0) {
      SAGE_TransformEntities(camera);
    }
#endif
    SAGE_Render3DTriangles();
  }
}

/**
 * Get the engine metrics
 *
 * @return Engine metrics
 */
SAGE_EngineMetrics * SAGE_GetEngineMetrics()
{
  return &(sage_world.metrics);
}

/** Enable/Disable debug */
VOID SAGE_EngineDebug(BOOL flag)
{
  engine_debug = flag;
}
