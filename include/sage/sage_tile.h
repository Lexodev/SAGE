/**
 * sage_tile.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Tile management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#ifndef _SAGE_TILE_H_
#define _SAGE_TILE_H_

#include <sage/sage_bitmap.h>

#define STIL_MAX_TILEBANK     16

#define STIL_MAPINBYTE        1
#define STIL_MAPINWORD        2

/** SAGE tile */
typedef struct {
  /** Tile position */
  ULONG left, top;
  /** Tile flags */
  LONGBITS flags;
  /** Tile user data */
  APTR user_data;
} SAGE_Tile;

/** SAGE tile bank */
typedef struct {
  /** Size of a tile */
  UWORD tile_width, tile_height;
  /** Bank size */
  UWORD bank_size;
  /** Bank of tiles */
  SAGE_Tile *tiles;
  /** Tile bitmap */
  SAGE_Bitmap * bitmap;
} SAGE_TileBank;

/** Create a tilebank */
BOOL SAGE_CreateTileBank(UWORD, UWORD, UWORD, UWORD, SAGE_Picture *);

/** Get a tilebank by his index */
SAGE_TileBank *SAGE_GetTileBank(UWORD);

/** Release a tilebank */
BOOL SAGE_ReleaseTileBank(UWORD);

/** Add a tile to the tilebank */
BOOL SAGE_AddTileToBank(UWORD, UWORD, ULONG, ULONG, LONGBITS, APTR);

/** Add multiple tiles to the tilebank */
BOOL SAGE_AddTilesToBank(UWORD);

/** Set flags for a tile */
BOOL SAGE_SetTileFlags(UWORD, UWORD, LONGBITS);

/** Get flags of a tile */
LONGBITS SAGE_GetTileFlags(UWORD, UWORD);

/** Tell if tile has flag on */
BOOL SAGE_HasTileFlag(UWORD, UWORD, LONGBITS);

/** Set tile user data */
BOOL SAGE_SetTileUserData(UWORD, UWORD, APTR);

/** Get tile user data */
APTR SAGE_GetTileUserData(UWORD, UWORD);

/** Blit a tile to a layer */
BOOL SAGE_BlitTileToLayer(UWORD, UWORD, UWORD, ULONG, ULONG);

/** Blit a tile to the screen */
BOOL SAGE_BlitTileToScreen(UWORD, UWORD, ULONG, ULONG);

#endif
