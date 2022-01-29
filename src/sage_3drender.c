/**
 * sage_3drender.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D rendering management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2021
 */

#include <exec/types.h>

#include <proto/Warp3D.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_bitmap.h"
#include "sage_screen.h"
#include "sage_3dtexture.h"
#include "sage_3dtexmap.h"
#include "sage_3drender.h"

/** SAGE context */
extern SAGE_Context SageContext;

/** Buffer of triangles */
SAGE_3DTriangle s3d_triangles[S3DR_MAX_TRIANGLES];
SAGE_SortedTriangle ordered_triangles[S3DR_MAX_TRIANGLES];

/** Number of triangles to render */
UWORD render_triangles = 0;

/*****************************************************************************/
//            DEBUG ONLY
/*****************************************************************************/

VOID SAGE_Dump3DTriangle(SAGE_3DTriangle * triangle)
{
  SAGE_DebugLog("Dump SAGE 3D triangle");
  SAGE_DebugLog(" => x1=%f  y1=%f  z1=%f  u1=%f  v1=%f", triangle->x1, triangle->y1, triangle->z1, triangle->u1, triangle->v1);
  SAGE_DebugLog(" => x2=%f  y2=%f  z2=%f  u2=%f  v2=%f", triangle->x2, triangle->y2, triangle->z2, triangle->u2, triangle->v2);
  SAGE_DebugLog(" => x3=%f  y3=%f  z3=%f  u3=%f  v3=%f", triangle->x3, triangle->y3, triangle->z3, triangle->u3, triangle->v3);
  SAGE_DebugLog(" => texture=%d", triangle->texture);
}

/*****************************************************************************/

/**
 * Initialize the 3D renderer
 */
BOOL SAGE_Init3DRender()
{
  return TRUE;
}

/**
 * Define the rendering mode
 *
 * @param mode Rendering mode
 *
 * @return Operation success
 */
BOOL SAGE_Set3DRenderMode(UWORD mode)
{
  SAGE_3DDevice * device;

  device = SageContext.Sage3D;
  SAFE(if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return FALSE;
  })
  device->render_mode = mode;
  return TRUE;
}

/**
 * Add a triangle to the rendering queue
 *
 * @param triangle Triangle to add to the queue
 *
 * @return Operation success
 */
BOOL SAGE_Push3DTriangle(SAGE_3DTriangle * triangle)
{
  memcpy(&(s3d_triangles[render_triangles]), triangle, sizeof(SAGE_3DTriangle));
  ordered_triangles[render_triangles].triangle = &(s3d_triangles[render_triangles]);
  ordered_triangles[render_triangles].avgz = (triangle->z1 + triangle->z2 + triangle->z3) / 3.0;
  render_triangles++;
  return TRUE;
}

/**
 * Quick sort the triangles in the rendering queue
 *
 * @param triangles Triangles queue
 * @param low       Lower index for sorting
 * @param high      Higher index for sorting
 */
VOID SAGE_QuicksortTriangles(SAGE_SortedTriangle * triangles, LONG low, LONG high)
{
  SAGE_SortedTriangle temp;
  DOUBLE pivot;
  LONG idx_low, idx_high;

  if (low >= high) return;
  idx_low = low+1;
  idx_high = high;
  pivot = triangles[low].avgz;
  while (idx_low <= idx_high) {
    while (triangles[idx_low].avgz >= pivot && idx_low <= high) idx_low++;
    while (triangles[idx_high].avgz < pivot && idx_high >= low) idx_high--;
    if (idx_low < idx_high) {
      temp.triangle = triangles[idx_low].triangle;
      temp.avgz = triangles[idx_low].avgz;
      triangles[idx_low].triangle = triangles[idx_high].triangle;
      triangles[idx_low].avgz = triangles[idx_high].avgz;
      triangles[idx_high].triangle = temp.triangle;
      triangles[idx_high].avgz = temp.avgz;
      idx_low++;
      idx_high--;
    }
  }
  temp.triangle = triangles[low].triangle;
  temp.avgz = triangles[low].avgz;
  triangles[low].triangle = triangles[idx_high].triangle;
  triangles[low].avgz = triangles[idx_high].avgz;
  triangles[idx_high].triangle = temp.triangle;
  triangles[idx_high].avgz = temp.avgz;
  SAGE_QuicksortTriangles(triangles, low, idx_high-1);
  SAGE_QuicksortTriangles(triangles, idx_high+1, high);
}

/**
 * Sort the triangles in the rendering queue
 */
VOID SAGE_Sort3DTriangles()
{
  SAGE_QuicksortTriangles(ordered_triangles, 0, render_triangles-1);
}

/**
 * Render all triangles in the queue
 *
 * @return Operation success
 */
BOOL SAGE_Render3DTriangles()
{
  SAGE_Screen * screen;
  SAGE_3DDevice * device;
  SAGE_3DTriangle * triangle;
  LONG index;
  W3D_Scissor scissor;
  W3D_Triangle w3d_triangle;
  S3D_Triangle s3d_triangle;

  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  })
  screen = SageContext.SageVideo->screen;
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  device = SageContext.Sage3D;
  SAFE(if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return FALSE;
  })
  if (device->render_mode == S3DR_W3DMODE) {
    // Set the W3D scissor
    scissor.left = screen->clipping.left;
    scissor.top = screen->clipping.top;
    scissor.width = screen->clipping.right - screen->clipping.left;
    scissor.height = screen->clipping.bottom - screen->clipping.top;
    // Set the drawing region
    W3D_SetDrawRegion(device->context, SAGE_GetSystemBackBitmap(), 0, &scissor);
    // Lock the hardware
    if (W3D_LockHardware(device->context) == W3D_SUCCESS) {
      // Render triangles
      for (index = 0;index < render_triangles;index++) {
        triangle = ordered_triangles[index].triangle;
        w3d_triangle.v1.x = triangle->x1;
        w3d_triangle.v1.y = triangle->y1;
        w3d_triangle.v1.z = triangle->z1;
        w3d_triangle.v1.w = 1.0 / triangle->z1;
        w3d_triangle.v1.u = triangle->u1;
        w3d_triangle.v1.v = triangle->v1;
        w3d_triangle.v1.color.a = 0.0;
        w3d_triangle.v1.color.r = 1.0;
        w3d_triangle.v1.color.g = 1.0;
        w3d_triangle.v1.color.b = 1.0;
        w3d_triangle.v2.x = triangle->x2;
        w3d_triangle.v2.y = triangle->y2;
        w3d_triangle.v2.z = triangle->z2;
        w3d_triangle.v2.w = 1.0 / triangle->z2;
        w3d_triangle.v2.u = triangle->u2;
        w3d_triangle.v2.v = triangle->v2;
        w3d_triangle.v2.color.a = 0.0;
        w3d_triangle.v2.color.r = 1.0;
        w3d_triangle.v2.color.g = 1.0;
        w3d_triangle.v2.color.b = 1.0;
        w3d_triangle.v3.x = triangle->x3;
        w3d_triangle.v3.y = triangle->y3;
        w3d_triangle.v3.z = triangle->z3;
        w3d_triangle.v3.w = 1.0 / triangle->z3;
        w3d_triangle.v3.u = triangle->u3;
        w3d_triangle.v3.v = triangle->v3;
        w3d_triangle.v3.color.a = 0.0;
        w3d_triangle.v3.color.r = 1.0;
        w3d_triangle.v3.color.g = 1.0;
        w3d_triangle.v3.color.b = 1.0;
        w3d_triangle.tex = SAGE_GetW3DTexture(triangle->texture);
        W3D_DrawTriangle(device->context, &w3d_triangle);
      }
      W3D_UnLockHardware(device->context);
    } else {
      SAGE_SetError(SERR_LOCKHARDWARE);
      render_triangles = 0;
      return FALSE;
    }
  } else {
    // Render triangles
    for (index = 0;index < render_triangles;index++) {
      triangle = ordered_triangles[index].triangle;
      s3d_triangle.x1 = triangle->x1;
      s3d_triangle.y1 = triangle->y1;
      s3d_triangle.z1 = triangle->z1;
      s3d_triangle.u1 = triangle->u1;
      s3d_triangle.v1 = triangle->v1;
      s3d_triangle.x2 = triangle->x2;
      s3d_triangle.y2 = triangle->y2;
      s3d_triangle.z2 = triangle->z2;
      s3d_triangle.u2 = triangle->u2;
      s3d_triangle.v2 = triangle->v2;
      s3d_triangle.x3 = triangle->x3;
      s3d_triangle.y3 = triangle->y3;
      s3d_triangle.z3 = triangle->z3;
      s3d_triangle.u3 = triangle->u3;
      s3d_triangle.v3 = triangle->v3;
      s3d_triangle.tex = SAGE_GetTexture(triangle->texture);
      SAGE_DrawTexturedTriangle(&s3d_triangle, screen->back_bitmap, &(screen->clipping));
    }
  }
  render_triangles = 0;
  return TRUE;
}
