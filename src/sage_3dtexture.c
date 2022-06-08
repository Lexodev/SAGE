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
 * Check if texture size is valid
 */
BOOL SAGE_CheckTextureSize(ULONG width, ULONG height)
{
  if (width != height) {
    return FALSE;
  }
  if (width != STEX_SIZE64 && width != STEX_SIZE128 && width != STEX_SIZE256 && width != STEX_SIZE512) {
    return FALSE;
  }
  return TRUE;
}

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
  // Check for size compliance
  if (size == STEX_FULLSIZE) {
    if (!SAGE_CheckTextureSize(picture->bitmap->width, picture->bitmap->height)) {
      SAGE_SetError(SERR_TEXTURE_SIZE);
      return NULL;
    }
    size = picture->bitmap->width;
  }
  if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return NULL;
  }
  if (SageContext.Sage3D->textures[index] != NULL) {
    SAGE_ReleaseTexture(index);
  }
  // Allocate and init texture
  texture = (SAGE_3DTexture *) SAGE_AllocMem(sizeof(SAGE_3DTexture));
  if (texture != NULL) {
    texture->size = size;
    if ((texture->bitmap = SAGE_AllocBitmap(texture->size, texture->size, screen->depth, screen->pixformat, NULL)) != NULL) {
      if (SAGE_BlitPictureToBitmap(picture, left, top, texture->size, texture->size, texture->bitmap, 0, 0)) {
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
  // Check for 3d device
  SAFE(if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
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
  // Check for 3d device
  SAFE(if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return NULL;
  })
  SAFE(if (index >= STEX_MAX_TEXTURES) {
    SAGE_SetError(SERR_TEX_INDEX);
    return NULL;
  })
  return SageContext.Sage3D->textures[index]->w3dtex;
}

/**
 * Return the first free slot in texture array
 *
 * @return Free texture index or -1 when no slot is available
 */
WORD SAGE_GetFreeTextureIndex()
{
  WORD index;
  
  // Check for 3d device
  SAFE(if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return NULL;
  })
  for (index = 0;index < STEX_MAX_TEXTURES;index++) {
    if (SageContext.Sage3D->textures[index] == NULL) {
      return index;
    }
  }
  return -1;
}

/**
 * Get a texture size by his index
 *
 * @param index Texture index
 *
 * @return Texture size
 */
UWORD SAGE_GetTextureSize(UWORD index)
{
  // Check for 3d device
  SAFE(if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return 0;
  })
  SAFE(if (index >= STEX_MAX_TEXTURES) {
    SAGE_SetError(SERR_TEX_INDEX);
    return 0;
  })
  return SageContext.Sage3D->textures[index]->size;
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
  // Check for 3d device
  SAFE(if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
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
  
  SD(SAGE_DebugLog("Add texture #%d", index));
  device = SageContext.Sage3D;
  if (device == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
  }
  texture = SAGE_GetTexture(index);
  if (texture == NULL) {
    return FALSE;
  }
  // Don't have to push texture on card when we are not using Warp3D
  if (device->render_system != S3DD_W3DRENDER) {
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
  
  SD(SAGE_DebugLog("Remove texture #%d", index));
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
 * Remove all textures from card memory and relesae ressources
 * 
 * @return Operation success
 */
BOOL SAGE_FlushTextures()
{
  UWORD index;

  SD(SAGE_DebugLog("Flush textures (%d)", STEX_MAX_TEXTURES));
  for (index = 0;index < STEX_MAX_TEXTURES;index++) {
    SAGE_ReleaseTexture(index);
  }
  return TRUE;
}
