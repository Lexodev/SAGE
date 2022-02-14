/**
 * sage_3dskybox.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D skybox management
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
#include "sage_3dentity.h"
#include "sage_3dtexture.h"
#include "sage_3dengine.h"

/** Engine data */
extern SAGE_3DWorld sage_world;

/**
 * Set a skybox vertex
 */
VOID SAGE_SetSkyboxVertex(ULONG vertex, FLOAT x, FLOAT y, FLOAT z)
{
  sage_world.skybox->vertices[vertex].x = x;
  sage_world.skybox->vertices[vertex].y = y;
  sage_world.skybox->vertices[vertex].z = z;
}

/**
 * Set the world skybox radius
 */
VOID SAGE_SetSkyboxRadius(FLOAT radius)
{
  FLOAT suidar;
  
  suidar = (FLOAT) -radius;
  sage_world.skybox->radius = radius;
  SAGE_SetSkyboxVertex(0, suidar, radius, radius);
  SAGE_SetSkyboxVertex(1, radius, radius, radius);
  SAGE_SetSkyboxVertex(2, radius, suidar, radius);
  SAGE_SetSkyboxVertex(3, suidar, suidar, radius);
  SAGE_SetSkyboxVertex(4, suidar, radius, suidar);
  SAGE_SetSkyboxVertex(5, radius, radius, suidar);
  SAGE_SetSkyboxVertex(6, radius, suidar, suidar);
  SAGE_SetSkyboxVertex(7, suidar, suidar, suidar);
}

/**
 * Setup a skybox plane
 */
VOID SAGE_SetSkyboxPlane(ULONG plane, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG plane_tex)
{
  SAGE_3DTexture * texture;
  FLOAT size;

  sage_world.skybox->faces[plane].is_quad = TRUE;;
  sage_world.skybox->faces[plane].p1 = p1;
  sage_world.skybox->faces[plane].p2 = p2;
  sage_world.skybox->faces[plane].p3 = p3;
  sage_world.skybox->faces[plane].p4 = p4;
  texture = SAGE_GetTexture(plane_tex);
  if (texture != NULL) {
    size = texture->size;
  } else {
    size = 128;
  }
  sage_world.skybox->faces[plane].u1 = 0;
  sage_world.skybox->faces[plane].v1 = 0;
  sage_world.skybox->faces[plane].u2 = size - 1;
  sage_world.skybox->faces[plane].v2 = 0;
  sage_world.skybox->faces[plane].u3 = size - 1;
  sage_world.skybox->faces[plane].v3 = size - 1;
  sage_world.skybox->faces[plane].u4 = 0;
  sage_world.skybox->faces[plane].v4 = size - 1;
  sage_world.skybox->faces[plane].texture = plane_tex;
}

/**
 * Set the world skybox textures
 */
VOID SAGE_SetSkyboxTextures(ULONG tex_front, ULONG tex_back, ULONG tex_left, ULONG tex_right, ULONG tex_top, ULONG tex_bottom)
{
  // Setup front plane
  SAGE_SetSkyboxPlane(S3DE_SKYBOX_FRONT, 0, 1, 2, 3, tex_front);
  // Setup back plane
  SAGE_SetSkyboxPlane(S3DE_SKYBOX_BACK, 5, 4, 7, 6, tex_back);
  // Setup left plane
  SAGE_SetSkyboxPlane(S3DE_SKYBOX_LEFT, 4, 0, 3, 7, tex_left);
  // Setup right plane
  SAGE_SetSkyboxPlane(S3DE_SKYBOX_RIGHT, 1, 5, 6, 2, tex_right);
  // Setup top plane
  SAGE_SetSkyboxPlane(S3DE_SKYBOX_TOP, 4, 5, 1, 0, tex_top);
  // Setup bottom plane
  SAGE_SetSkyboxPlane(S3DE_SKYBOX_BOTTOM, 3, 2, 6, 7, tex_bottom);
}

/**
 * Enable/disable the world skybox
 */
VOID SAGE_EnableSkybox(BOOL flag)
{
  sage_world.active_skybox = flag;
}
