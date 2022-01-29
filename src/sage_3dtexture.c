/**
 * sage_3dtexture.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture management
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
#include "sage_3dtexture.h"

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Create a texture from a picture
 *
 * @param index   Texture index
 * @param left    Left position of texture in picture
 * @param top     Top position of texture in picture
 * @param size    Texture size
 * @param picture SAGE Picture pointer
 *
 * @return Operation success
 */
BOOL SAGE_CreateTextureFromPicture(UWORD index, UWORD left, UWORD top, UWORD size, SAGE_Picture * picture)
{
  SAGE_Screen * screen;
  SAGE_3DTexture * texture;
  UWORD idxcol;

  SD(SAGE_DebugLog("Create texture #%d (%d,%d)x%d", index, left, top, size));
  // Check for video device
  if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  screen = SageContext.SageVideo->screen;
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (index >= STEX_MAX_TEXTURES) {
    SAGE_SetError(SERR_TEX_INDEX);
    return FALSE;
  }
  if (picture == NULL || picture->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (SageContext.Sage3D->textures[index] != NULL) {
    SAGE_ReleaseTexture(index);
  }
  // Allocate and init texture
  texture = (SAGE_3DTexture *) SAGE_AllocMem(sizeof(SAGE_3DTexture));
  if (texture != NULL) {
    texture->size = size;
    if ((texture->bitmap = SAGE_AllocBitmap(size, size, screen->depth, screen->pixformat, NULL)) != NULL) {
      if (SAGE_BlitPictureToBitmap(picture, left, top, size, size, texture->bitmap, 0, 0)) {
        // Set texture format
        switch (screen->pixformat) {
          case PIXFMT_CLUT:
            texture->texformat = STEX_PIXFMT_CLUT;
            for (idxcol = 0;idxcol < STEX_MAXCOLORS;idxcol++) {
              texture->palette[idxcol] = picture->color_map[idxcol];
            }
            break;
          case PIXFMT_RGB15:
            texture->texformat = STEX_PIXFMT_RGB15;
            break;
          case PIXFMT_RGB16:
            texture->texformat = STEX_PIXFMT_RGB16;
            break;
          case PIXFMT_RGB24:
            texture->texformat = STEX_PIXFMT_RGB24;
            break;
          case PIXFMT_ARGB32:
            texture->texformat = STEX_PIXFMT_ARGB32;
            break;
          case PIXFMT_RGBA32:
            texture->texformat = STEX_PIXFMT_RGBA32;
            break;
          default:
            texture->texformat = STEX_PIXFMT_UNKNOWN;
        }
        SageContext.Sage3D->textures[index] = texture;
        return TRUE;
      }
    }
    SAGE_FreeMem(texture);
    return FALSE;
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Get a texture by his index
 *
 * @param index Texture index
 *
 * @return Texture structure
 */
SAGE_3DTexture * SAGE_GetTexture(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  SAFE(if (index >= STEX_MAX_TEXTURES) {
    SAGE_SetError(SERR_TEX_INDEX);
    return NULL;
  })
  return SageContext.Sage3D->textures[index];
}

/**
 * Get the Warp3D texture by his index
 *
 * @param index Texture index
 *
 * @return W3D texture structure
 */
W3D_Texture * SAGE_GetW3DTexture(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  SAFE(if (index >= STEX_MAX_TEXTURES) {
    SAGE_SetError(SERR_TEX_INDEX);
    return NULL;
  })
  return SageContext.Sage3D->textures[index]->w3dtex;
}

/**
 * Get texture buffer by his index
 *
 * @param index Texture index
 *
 * @return Texture buffer address
 */
APTR SAGE_GetTextureBuffer(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  SAFE(if (index >= STEX_MAX_TEXTURES) {
    SAGE_SetError(SERR_TEX_INDEX);
    return NULL;
  })
  return SageContext.Sage3D->textures[index]->bitmap->bitmap_buffer;
}

/**
 * Release a texture
 *
 * @param index Texture index
 * 
 * @return Operation success
 */
BOOL SAGE_ReleaseTexture(UWORD index)
{
  SAGE_3DTexture * texture;

  SD(SAGE_DebugLog("Release texture #%d", index));
  texture = SAGE_GetTexture(index);
  if (texture != NULL) {
    SAGE_RemoveTexture(index);
    SAGE_ReleaseBitmap(texture->bitmap);
    SAGE_FreeMem(texture);
    SageContext.Sage3D->textures[index] = NULL;
    return TRUE;
  }
  return FALSE;
}

/**
 * Add a texture to the card memory
 *
 * @param index Texture index
 * 
 * @return Operation success
 */
BOOL SAGE_AddTexture(UWORD index)
{
  SAGE_3DDevice * device;
  SAGE_3DTexture * texture;
  
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
  }
  texture = SAGE_GetTexture(index);
  if (texture == NULL) {
    return FALSE;
  }
  // Don't have to push texture on card when we are in internal mode
  if (device->render_mode == S3DR_S3DMODE) {
    texture->w3dtex = NULL;
    return TRUE;
  }
  SD(SAGE_DebugLog("Allocating W3D texture #%d", index));
  texture->w3dtex = W3D_AllocTexObjTags(
    device->context,
    &(device->warp3d_error),
    W3D_ATO_IMAGE, texture->bitmap->bitmap_buffer,
    W3D_ATO_FORMAT, texture->texformat,
    W3D_ATO_WIDTH, texture->size,
    W3D_ATO_HEIGHT, texture->size,
    W3D_ATO_PALETTE, texture->palette,
    TAG_DONE
  );
  if (texture->w3dtex == NULL || device->warp3d_error != W3D_SUCCESS) {
    SD(SAGE_ErrorLog("Error : %s", SAGE_GetLast3DDeviceError()));
    SAGE_SetError(SERR_TEX_ALLOC);
    return FALSE;
  }
  W3D_SetFilter(device->context, texture->w3dtex, W3D_LINEAR, W3D_LINEAR);
  W3D_SetTexEnv(device->context, texture->w3dtex, W3D_MODULATE, NULL);
  return TRUE;
}

/**
 * Remove a texture from card memory
 *
 * @param index Texture index
 * 
 * @return Operation success
 */
BOOL SAGE_RemoveTexture(UWORD index)
{
  SAGE_3DDevice * device;
  SAGE_3DTexture * texture;
  
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
  }
  texture = SAGE_GetTexture(index);
  if (texture == NULL) {
    return FALSE;
  }
  if (texture->w3dtex != NULL) {
    SD(SAGE_DebugLog("Removing W3D texture #%d", index));
    W3D_FreeTexObj(device->context, texture->w3dtex);
    texture->w3dtex = NULL;
  }
  return TRUE;
}

/**
 * Remove all textures from card memory
 * 
 * @return Operation success
 */
BOOL SAGE_FlushTextures()
{
  UWORD index;

  for (index = 0;index < STEX_MAX_TEXTURES;index++) {
    SAGE_RemoveTexture(index);
  }
  return TRUE;
}
