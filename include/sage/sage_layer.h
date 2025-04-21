/**
 * sage_layer.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Layer management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#ifndef _SAGE_LAYER_H_
#define _SAGE_LAYER_H_

#include <exec/exec.h>

#include <sage/sage_bitmap.h>
#include <sage/sage_picture.h>

#define SLAY_MAX_LAYERS       32

#define SLAY_OVERNONE         0
#define SLAY_OVERWIDTH        1
#define SLAY_OVERHEIGHT       2
#define SLAY_OVERBOTH         3

/** SAGE layer view structure */
typedef struct {
  /** View dimension */
  ULONG left, top, width, height;
} SAGE_LayerView;

/** SAGE layer structure */
typedef struct {
  /** Layer views */
  SAGE_LayerView view[4];
  /** Layer view overflow */
  UWORD overflow;
  /** Layer bitmap */
  SAGE_Bitmap *bitmap;
} SAGE_Layer;

/** Create a layer */
BOOL SAGE_CreateLayer(UWORD, ULONG, ULONG);

/** Create a layer from a picture */
BOOL SAGE_CreateLayerFromPicture(UWORD, SAGE_Picture *);

/** Get a layer by his index */
SAGE_Layer *SAGE_GetLayer(UWORD);

/** Release a layer resources */
BOOL SAGE_ReleaseLayer(UWORD);

/** Clear a layer */
BOOL SAGE_ClearLayer(UWORD);

/** Fill a layer with a color */
BOOL SAGE_FillLayer(UWORD, ULONG);

/** Set the layer view */
BOOL SAGE_SetLayerView(UWORD, ULONG, ULONG, ULONG, ULONG);

/** Set the layer transparency */
BOOL SAGE_SetLayerTransparency(UWORD, ULONG);

/** Get the layer bitmap */
SAGE_Bitmap *SAGE_GetLayerBitmap(UWORD);

/** Blit a picture to layer at given position */
BOOL SAGE_BlitPictureToLayer(SAGE_Picture *, ULONG, ULONG, ULONG, ULONG, UWORD, ULONG, ULONG);

/** Blit a layer to the screen at given position */
BOOL SAGE_BlitLayerToScreen(UWORD, ULONG, ULONG);

#endif
