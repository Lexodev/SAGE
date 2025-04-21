/**
 * sage_video.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Video module management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#ifndef _SAGE_VIDEO_H_
#define _SAGE_VIDEO_H_

#include <sage/sage_screen.h>
#include <sage/sage_layer.h>
#include <sage/sage_sprite.h>
#include <sage/sage_tile.h>
#include <sage/sage_tilemap.h>

/** Video mode infos */
typedef struct {
  ULONG mode_id;
  UWORD width, height, depth;
  UBYTE name[DISPLAYNAMELEN];
} SAGE_VideoMode;

/** Video modes */
typedef struct {
  UWORD nb_modes;
  SAGE_VideoMode *modes;
} SAGE_VideoModeList;

/** SAGE video device structure */
typedef struct {
  /** Video modes */
  SAGE_VideoModeList video_modes;
  /** SAGA chipset */
  BOOL SAGAReady;
  /** Application screen */
  SAGE_Screen *screen;
  /** Layers */
  SAGE_Layer *layers[SLAY_MAX_LAYERS];
  /** Sprite bank */
  SAGE_SpriteBank *sprites[SSPR_MAX_SPRBANK];
  /** Tile bank */
  SAGE_TileBank *tiles[STIL_MAX_TILEBANK];
  /** Tile map */
  SAGE_TileMap *tilemaps[STIL_MAX_TILEMAP];
} SAGE_VideoDevice;

/** Init the video module */
BOOL SAGE_InitVideoModule(VOID);

/** Release the video module */
BOOL SAGE_ReleaseVideoModule(VOID);

/** Allocate video device */
BOOL SAGE_AllocVideoDevice(VOID);

/** Release video device */
BOOL SAGE_FreeVideoDevice(VOID);

/** Get available video modes */
SAGE_VideoModeList *SAGE_GetVideoModes(VOID);

#endif
