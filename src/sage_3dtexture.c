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
#include <dos/dos.h>

#include <proto/Warp3D.h>
#include <proto/dos.h>

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
 * Check for a DDS file
 *
 * @param file_name DDS file name
 *
 * @return File is a DDS
 */
BOOL SAGE_CheckDDSFile(STRPTR file_name)
{
  BPTR file_handle;
  LONG dds_tag, bytes_read;
  
  file_handle = Open(file_name, MODE_OLDFILE);
  if (file_handle != 0) {
    // Check if it's a DDS file
    bytes_read = Read(file_handle, &dds_tag, sizeof(dds_tag));
    if (bytes_read != sizeof(dds_tag)) {
      SAGE_SetError(SERR_READFILE);
      Close(file_handle);
      return FALSE;
    }
    Close(file_handle);
    if (dds_tag == STEX_DDSTAG) {
      return TRUE;
    }
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  return FALSE;
}

/**
 * Load a DDS texture
 *
 * @param file_name DDS file name
 *
 * @return Picture structure pointer or NULL on error
 */
SAGE_DDSFile * SAGE_LoadDDSTexture(STRPTR file_name)
{
  BPTR file_handle;
  SAGE_DDSHeader header;
  SAGE_DDSFile * dds;
  ULONG file_size, bytes_read;

  SD(SAGE_DebugLog("SAGE_LoadDDSTexture"));
  file_handle = Open(file_name, MODE_OLDFILE);
  if (file_handle != 0) {
    dds = (SAGE_DDSFile *) SAGE_AllocMem(sizeof(SAGE_DDSFile));
    if (dds == NULL) {
      Close(file_handle);
      return NULL;
    }
    // Get the file size
    bytes_read = Seek(file_handle, 0, OFFSET_END);
    file_size = Seek(file_handle, 0, OFFSET_BEGINNING);
    // Read the header
    bytes_read = Read(file_handle, &header, sizeof(header));
    if (bytes_read != sizeof(header)) {
      SAGE_SetError(SERR_READFILE);
      Close(file_handle);
      return NULL;
    }
    dds->width = SAGE_LONGTOBE(header.width);
    dds->height = SAGE_LONGTOBE(header.height);
    dds->depth = 24;
    dds->data_size = file_size - sizeof(header);
    SD(SAGE_DebugLog("DDS header => width=%d, height=%d, depth=%d, data_size=%d", dds->width, dds->height, dds->depth, dds->data_size));
    // Read the body
    if ((dds->data = SAGE_AllocAlignMem(dds->data_size, STEX_DDSALIGN)) == NULL) {
      SAGE_FreeMem(dds);
      Close(file_handle);
      return NULL;
    }
    bytes_read = Read(file_handle, dds->data, dds->data_size);
    if (bytes_read != dds->data_size) {
      SAGE_SetError(SERR_READFILE);
      SAGE_FreeMem(dds->data);
      SAGE_FreeMem(dds);
      Close(file_handle);
      return NULL;
    }
    return dds;
  } else {
    SAGE_SetError(SERR_OPENFILE);
  }
  return NULL;
}

/**
 * Get the mip size value depending on texture size
 *
 * @param size Texture size
 *
 * @return Mip size
 */
UWORD SAGE_GetTextureMipsize(UWORD size)
{
  switch (size) {
    case STEX_SIZE64:
      return 6;
    case STEX_SIZE128:
      return 7;
    case STEX_SIZE256:
      return 8;
    case STEX_SIZE512:
      return 9;
  }
  return 0;
}

/**
 * Create a texture from a file
 *
 * @param index     Texture index
 * @param file_name File name
 *
 * @return Operation success
 */
BOOL SAGE_CreateTextureFromFile(UWORD index, STRPTR file_name)
{
  SAGE_DDSFile * dds;
  SAGE_3DTexture * texture;
  SAGE_Picture * picture;

  SD(SAGE_DebugLog("Create texture #%d from file %s", index, file_name));
  if (SageContext.Sage3D == NULL) {
    SAGE_SetError(SERR_NO_3DDEVICE);
    return FALSE;
  }
  if (SAGE_CheckDDSFile(file_name)) {
    if ((dds = SAGE_LoadDDSTexture(file_name)) == NULL) {
      return FALSE;
    }
    if (!SAGE_CheckTextureSize(dds->width, dds->height)) {
      SAGE_FreeMem(dds->data);
      SAGE_FreeMem(dds);
      SAGE_SetError(SERR_TEXTURE_SIZE);
      return FALSE;
    }
    // Allocate and init texture
    if (SageContext.Sage3D->textures[index] != NULL) {
      SAGE_ReleaseTexture(index);
    }
    texture = (SAGE_3DTexture *) SAGE_AllocMem(sizeof(SAGE_3DTexture));
    if (texture != NULL) {
      texture->size = dds->width;
      texture->mipsize = SAGE_GetTextureMipsize(texture->size);
      if ((texture->bitmap = SAGE_AllocBitmap(texture->size, texture->size, 32, 0, PIXFMT_DXT1, dds->data)) != NULL) {
        texture->bitmap->properties = SBMP_COMPRESSED;
        texture->w3dtex = NULL;
        texture->texformat = STEX_PIXFMT_DXT1;
        SageContext.Sage3D->textures[index] = texture;
        SAGE_FreeMem(dds);
        return TRUE;
      }
    }
    SAGE_FreeMem(texture);
    SAGE_FreeMem(dds->data);
    SAGE_FreeMem(dds);
  } else {
    if ((picture = SAGE_LoadPicture(file_name)) != NULL) {
      // Create a new texture
      if (SAGE_CreateTextureFromPicture(index, 0, 0, STEX_FULLSIZE, picture)) {
        SAGE_ReleasePicture(picture);
        return TRUE;
      }
      SAGE_ReleasePicture(picture);
    }
  }
  return FALSE;
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
    texture->mipsize = SAGE_GetTextureMipsize(texture->size);
    if ((texture->bitmap = SAGE_AllocBitmap(texture->size, texture->size, screen->depth, 0, screen->pixformat, NULL)) != NULL) {
      texture->w3dtex = NULL;
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
SAGE_3DTexture * SAGE_GetTexture(WORD index)
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
  if (index == STEX_USECOLOR) {
    return NULL;
  }
  return SageContext.Sage3D->textures[index];
}

/**
 * Get the Warp3D texture by his index
 *
 * @param index Texture index
 *
 * @return W3D texture structure
 */
W3D_Texture * SAGE_GetW3DTexture(WORD index)
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
  if (index == STEX_USECOLOR) {
    return NULL;
  }
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

/**
 * Define the texture transparency color
 *
 * @param index Texture index
 * @param color Transparent color
 *
 * @return Operation success
 */
BOOL SAGE_SetTextureTransparency(UWORD index, ULONG color)
{
  SAGE_3DTexture * texture;

  texture = SAGE_GetTexture(index);
  if (texture == NULL) {
    return FALSE;
  }
  return SAGE_SetBitmapTransparency(texture->bitmap, color);
}
