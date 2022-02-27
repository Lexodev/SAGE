/**
 * sage_tilemap.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Tilemap management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 November 2020
 */

#ifndef _SAGE_TILEMAP_H_
#define _SAGE_TILEMAP_H_

#define STIL_MAX_TILEMAP      16

#define STIL_MAPBPT_BYTE      1
#define STIL_MAPBPT_WORD      2

/** SAGE tilemap structure */
typedef struct {
  /** Size of the tilemap */
  UWORD cols, rows;
  /** Tile map */
  APTR map;
  /** Bytes per tile index */
  UBYTE bytespertile;
} SAGE_TileMap;

/** Create a tilemap */
BOOL SAGE_CreateTileMap(UWORD, UWORD, UWORD, UBYTE);

/** Get a tile map his index */
SAGE_TileMap * SAGE_GetTileMap(UWORD);

/** Release a tilemap resources */
BOOL SAGE_ReleaseTileMap(UWORD);

/** Load a tilemap file */
BOOL SAGE_LoadTileMap(UWORD, STRPTR);

/** Get tile map as UBYTE array */
UBYTE * SAGE_GetTileMapB(UWORD);

/** Get tile map as UWORD array */
UWORD * SAGE_GetTileMapW(UWORD);

/** Get tile UBYTE value at map position */
UBYTE SAGE_GetTileValueB(UWORD, UWORD, UWORD);

/** Update a tile UBYTE value at map position */
BOOL SAGE_UpdateTileValueB(UWORD, UWORD, UWORD, UBYTE);

/** Get tile UWORD value at map position */
UWORD SAGE_GetTileValueW(UWORD, UWORD, UWORD);

/** Update a tile UWORD value at map position */
BOOL SAGE_UpdateTileValueW(UWORD, UWORD, UWORD, UWORD);

#endif
