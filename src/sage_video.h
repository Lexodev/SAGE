/**
 * sage_video.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * Video module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 November 2020
 */

#ifndef _SAGE_VIDEO_H_
#define _SAGE_VIDEO_H_

#include "sage_screen.h"
#include "sage_layer.h"
#include "sage_sprite.h"
#include "sage_tile.h"
#include "sage_tilemap.h"

/** SAGE video device structure */
typedef struct {
  /** Video modes */
  struct List * video_modes;
  /** SAGA chipset */
  BOOL SAGAReady;
  /** Application screen */
  SAGE_Screen * screen;
  /** Layers */
  SAGE_Layer * layers[SLAY_MAX_LAYERS];
  /** Sprite bank */
  SAGE_SpriteBank * sprites[SSPR_MAX_SPRBANK];
  /** Tile bank */
  SAGE_TileBank * tiles[STIL_MAX_TILEBANK];
  /** Tile map */
  SAGE_TileMap * tilemaps[STIL_MAX_TILEMAP];
} SAGE_VideoDevice;

/** Init the video module */
BOOL SAGE_InitVideoModule(VOID);

/** Release the video module */
BOOL SAGE_ReleaseVideoModule(VOID);

/** Allocate video device */
BOOL SAGE_AllocVideoDevice(VOID);

/** Release video device */
BOOL SAGE_FreeVideoDevice(VOID);

/** Dump available RTG vide modes */
VOID SAGE_DumpVideoModes(VOID);

#endif
