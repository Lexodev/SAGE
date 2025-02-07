/**
 * sage_layer.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Layer management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_context.h>
#include <sage/sage_blitter.h>
#include <sage/sage_layer.h>

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Create a SAGE Layer
 * 
 * @param index  Layer index
 * @param width  Layer width
 * @param height Layer height
 * 
 * @return Operation success
 */
BOOL SAGE_CreateLayer(UWORD index, ULONG width, ULONG height)
{
  SAGE_Screen *screen;
  SAGE_Layer *layer;

  SD(SAGE_DebugLog("Create layer #%d %dx%d", index, width, height);)
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
  if (index >= SLAY_MAX_LAYERS) {
    SAGE_SetError(SERR_LAYER_INDEX);
    return FALSE;
  }
  if (SageContext.SageVideo->layers[index] != NULL) {
    SAGE_ReleaseLayer(index);
  }
  // Allocate and init layer
  layer = (SAGE_Layer *)SAGE_AllocMem(sizeof(SAGE_Layer));
  if (layer != NULL) {
    layer->view[SLAY_OVERNONE].left = 0;
    layer->view[SLAY_OVERNONE].top = 0;
    layer->view[SLAY_OVERNONE].width = width;
    layer->view[SLAY_OVERNONE].height = height;
    layer->overflow = SLAY_OVERNONE;
    if ((layer->bitmap = SAGE_AllocBitmap(width, height, screen->depth, 0, screen->pixformat, NULL)) != NULL) {
      SageContext.SageVideo->layers[index] = layer;
      return TRUE;
    }
    SAGE_FreeMem(layer);
    return FALSE;
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Create a SAGE Layer from a picture
 * 
 * @param index   Layer index
 * @param picture SAGE picture pointer
 * 
 * @return Operation success
 */
BOOL SAGE_CreateLayerFromPicture(UWORD index, SAGE_Picture *picture)
{
  if (picture == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (SAGE_CreateLayer(index, picture->bitmap->width, picture->bitmap->height)) {
    return SAGE_BlitPictureToLayer(picture, 0, 0, picture->bitmap->width, picture->bitmap->height, index, 0, 0);
  }
  return FALSE;
}

/**
 * Get a layer by his index
 *
 * @param index Layer index
 *
 * @return Layer structure
 */
SAGE_Layer *SAGE_GetLayer(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  SAFE(if (index >= SLAY_MAX_LAYERS) {
    SAGE_SetError(SERR_LAYER_INDEX);
    return NULL;
  })
  return SageContext.SageVideo->layers[index];
}

/**
 * Release SAGE Layer resource
 * 
 * @param index Layer index
 * 
 * @return Operation success
 */
BOOL SAGE_ReleaseLayer(UWORD index)
{
  SAGE_Layer *layer;

  SD(SAGE_DebugLog("Release layer #%d", index);)
  layer = SAGE_GetLayer(index);
  if (layer != NULL) {
    SAGE_ReleaseBitmap(layer->bitmap);
    SAGE_FreeMem(layer);
    SageContext.SageVideo->layers[index] = NULL;
    return TRUE;
  }
  return FALSE;
}

/**
 * Set a SAGE Layer view
 * 
 * @param index  Layer index
 * @param left   Left view start
 * @param top    Top view start
 * @param width  View width
 * @param height View height
 * 
 * @return Operation success
 */
BOOL SAGE_SetLayerView(UWORD index, ULONG left, ULONG top, ULONG width, ULONG height)
{
  SAGE_Layer *layer;

  layer = SAGE_GetLayer(index);
  SAFE(if (layer == NULL) {
    return FALSE;
  })
  // Check for size constraint
  if (!SAGE_CheckSizeConstraint(width, layer->bitmap->depth)) {
    return FALSE;
  }
  // Setup the layer view
  if (width <= layer->bitmap->width && height <= layer->bitmap->height) {
    left %= layer->bitmap->width;
    top %= layer->bitmap->height;
    layer->overflow = SLAY_OVERNONE;
    // Let's define if we have some overflow
    if ((left + width) > layer->bitmap->width) {
      layer->overflow += SLAY_OVERWIDTH;
    }
    if ((top + height) > layer->bitmap->height) {
      layer->overflow += SLAY_OVERHEIGHT;
    }
    if (layer->overflow == SLAY_OVERNONE) {
      layer->view[SLAY_OVERNONE].left = left;
      layer->view[SLAY_OVERNONE].top = top;
      layer->view[SLAY_OVERNONE].width = width;
      layer->view[SLAY_OVERNONE].height = height;
    } else if (layer->overflow == SLAY_OVERWIDTH) {
      layer->view[SLAY_OVERNONE].left = left;
      layer->view[SLAY_OVERNONE].top = top;
      layer->view[SLAY_OVERNONE].width = layer->bitmap->width - left;
      layer->view[SLAY_OVERNONE].height = height;
      layer->view[SLAY_OVERWIDTH].left = 0;
      layer->view[SLAY_OVERWIDTH].top = top;
      layer->view[SLAY_OVERWIDTH].width = width - (layer->bitmap->width - left);
      layer->view[SLAY_OVERWIDTH].height = height;
    } else if (layer->overflow == SLAY_OVERHEIGHT) {
      layer->view[SLAY_OVERNONE].left = left;
      layer->view[SLAY_OVERNONE].top = top;
      layer->view[SLAY_OVERNONE].width = width;
      layer->view[SLAY_OVERNONE].height = layer->bitmap->height - top;
      layer->view[SLAY_OVERHEIGHT].left = left;
      layer->view[SLAY_OVERHEIGHT].top = 0;
      layer->view[SLAY_OVERHEIGHT].width = width;
      layer->view[SLAY_OVERHEIGHT].height = height - (layer->bitmap->height - top);
    } else if (layer->overflow == SLAY_OVERBOTH) {
      layer->view[SLAY_OVERNONE].left = left;
      layer->view[SLAY_OVERNONE].top = top;
      layer->view[SLAY_OVERNONE].width = layer->bitmap->width - left;
      layer->view[SLAY_OVERNONE].height = layer->bitmap->height - top;
      layer->view[SLAY_OVERWIDTH].left = 0;
      layer->view[SLAY_OVERWIDTH].top = top;
      layer->view[SLAY_OVERWIDTH].width = width - (layer->bitmap->width - left);
      layer->view[SLAY_OVERWIDTH].height = layer->bitmap->height - top;
      layer->view[SLAY_OVERHEIGHT].left = left;
      layer->view[SLAY_OVERHEIGHT].top = 0;
      layer->view[SLAY_OVERHEIGHT].width = layer->bitmap->width - left;
      layer->view[SLAY_OVERHEIGHT].height = height - (layer->bitmap->height - top);
      layer->view[SLAY_OVERBOTH].left = 0;
      layer->view[SLAY_OVERBOTH].top = 0;
      layer->view[SLAY_OVERBOTH].width = width - (layer->bitmap->width - left);
      layer->view[SLAY_OVERBOTH].height = height - (layer->bitmap->height - top);
    }
    return TRUE;
  }
  SAGE_SetError(SERR_LAYER_SIZE);
  return FALSE;
}

/**
 * Define the layer transparency color
 *
 * @param index Layer index
 * @param color Transparent color
 *
 * @return Operation success
 */
BOOL SAGE_SetLayerTransparency(UWORD index, ULONG color)
{
  SAGE_Layer *layer;

  layer = SAGE_GetLayer(index);
  if (layer == NULL) {
    return FALSE;
  }
  return SAGE_SetBitmapTransparency(layer->bitmap, color);
}

/**
 * Get the layer bitmap
 *
 * @return Layer bitmap
 */
SAGE_Bitmap *SAGE_GetLayerBitmap(UWORD index)
{
  SAGE_Layer *layer;

  layer = SAGE_GetLayer(index);
  if (layer == NULL) {
    return NULL;
  }
  return layer->bitmap;
}

/**
 * Blit a picture to a layer
 *
 * @param picture SAGE Picture pointer
 * @param left    Picture left in pixel
 * @param top     Picture top in pixel
 * @param width   Picture width in pixel
 * @param height  Picture height in pixel
 * @param index   Layer index
 * @param x_pos   Horizontal position
 * @param y_pos   Vertical position
 *
 * @return Operation success
 */
BOOL SAGE_BlitPictureToLayer(SAGE_Picture *picture, ULONG left, ULONG top, ULONG width, ULONG height, UWORD index, ULONG x_pos, ULONG y_pos)
{
  SAGE_Layer *layer;

  SAFE(if (picture == NULL || picture->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  layer = SAGE_GetLayer(index);
  SAFE(if (layer == NULL || layer->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if ((left + width) <= picture->bitmap->width && (top + height) <= picture->bitmap->height) {
    if (layer->bitmap->width >= (width + x_pos) && layer->bitmap->height >= (height + y_pos)) {
      return SAGE_BlitBitmap(picture->bitmap, left, top, width, height, layer->bitmap, x_pos, y_pos);
    }
  }
  SAGE_SetError(SERR_PICTURE_SIZE);
  return FALSE;
}

/**
 * Blit a layer on the screen at a given position
 * 
 * @param index Layer index
 * @param x_pos Horizontal position
 * @param y_pos Vertical position
 * 
 * @return Operation success
 */
BOOL SAGE_BlitLayerToScreen(UWORD index, ULONG x_pos, ULONG y_pos)
{
  SAGE_Screen *screen;
  SAGE_Layer *layer;

  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  layer = SAGE_GetLayer(index);
  SAFE(if (layer == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  // Check if the layer fit in the screen
  if (screen->back_bitmap->width >= (layer->view[SLAY_OVERNONE].width + x_pos) && screen->back_bitmap->height >= (layer->view[SLAY_OVERNONE].height + y_pos)) {
    // Copy the main layer bitmap into screen bitmap
    SAGE_BlitBitmap(
      layer->bitmap,
      layer->view[SLAY_OVERNONE].left,
      layer->view[SLAY_OVERNONE].top,
      layer->view[SLAY_OVERNONE].width,
      layer->view[SLAY_OVERNONE].height,
      screen->back_bitmap,
      x_pos,
      y_pos
    );
    // Now copy the overflow parts if needed
    if (layer->overflow & SLAY_OVERWIDTH) {
      SAGE_BlitBitmap(
        layer->bitmap,
        layer->view[SLAY_OVERWIDTH].left,
        layer->view[SLAY_OVERWIDTH].top,
        layer->view[SLAY_OVERWIDTH].width,
        layer->view[SLAY_OVERWIDTH].height,
        screen->back_bitmap,
        x_pos + layer->view[SLAY_OVERNONE].width,
        y_pos
      );
    }
    if (layer->overflow & SLAY_OVERHEIGHT) {
      SAGE_BlitBitmap(
        layer->bitmap,
        layer->view[SLAY_OVERHEIGHT].left,
        layer->view[SLAY_OVERHEIGHT].top,
        layer->view[SLAY_OVERHEIGHT].width,
        layer->view[SLAY_OVERHEIGHT].height,
        screen->back_bitmap,
        x_pos,
        y_pos + layer->view[SLAY_OVERNONE].height
      );
    }
    if (layer->overflow == SLAY_OVERBOTH) {
      SAGE_BlitBitmap(
        layer->bitmap,
        layer->view[SLAY_OVERBOTH].left,
        layer->view[SLAY_OVERBOTH].top,
        layer->view[SLAY_OVERBOTH].width,
        layer->view[SLAY_OVERBOTH].height,
        screen->back_bitmap,
        x_pos + layer->view[SLAY_OVERNONE].width,
        y_pos + layer->view[SLAY_OVERNONE].height
      );
    }
    return TRUE;
  }
  SAGE_SetError(SERR_LAYER_SIZE);
  return FALSE;
}
