/**
 * sage_tilemap.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Tilemap management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#include <proto/dos.h>

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_context.h>
#include <sage/sage_tile.h>
#include <sage/sage_tilemap.h>

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Create a tilemap
 *
 * @param index  Tilemap index
 * @param width  Map width
 * @param height Map height
 * @param bpt    Bytes per tile index
 * 
 * @return Operation success
 */
BOOL SAGE_CreateTileMap(UWORD index, UWORD cols, UWORD rows, UBYTE bpt)
{
  SAGE_TileMap *tilemap;

  SD(SAGE_DebugLog("Create tilemap #%d %dx%d (%d)", index, cols, rows, bpt);)
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  })
  if (index >= STIL_MAX_TILEMAP) {
    SAGE_SetError(SERR_TILEMAP_INDEX);
    return FALSE;
  }
  if (SageContext.SageVideo->tilemaps[index] != NULL) {
    SAGE_ReleaseLayer(index);
  }
  // Allocate and init tilemap
  tilemap = (SAGE_TileMap *)SAGE_AllocMem(sizeof(SAGE_TileMap));
  if (tilemap != NULL) {
    tilemap->cols = cols;
    tilemap->rows = rows;
    tilemap->bytespertile = bpt;
    tilemap->map = SAGE_AllocMem(cols * rows * bpt);
    if (tilemap->map != NULL) {
      SageContext.SageVideo->tilemaps[index] = tilemap;
      return TRUE;
    }
    SAGE_FreeMem(tilemap);
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Get a tile map his index
 *
 * @param index Tilemap index
 *
 * @return TileMap structure
 */
SAGE_TileMap *SAGE_GetTileMap(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  if (index >= STIL_MAX_TILEMAP) {
    SAGE_SetError(SERR_TILEMAP_INDEX);
    return NULL;
  }
  return SageContext.SageVideo->tilemaps[index];
}

/**
 * Release a tilemap resources
 * 
 * @param index Tilemap index
 * 
 * @return Operation success
 */
BOOL SAGE_ReleaseTileMap(UWORD index)
{
  SAGE_TileMap *tilemap;

  SD(SAGE_DebugLog("Release tilemap #%d", index);)
  tilemap = SAGE_GetTileMap(index);
  if (tilemap != NULL) {
    if (tilemap->map != NULL) {
      SAGE_FreeMem(tilemap->map);
    }
    SAGE_FreeMem(tilemap);
    SageContext.SageVideo->tilemaps[index] = NULL;
    return TRUE;
  }
  return FALSE;
}

/**
 * Load a tile map file
 *
 * @param index   Tilemap index
 * @param mapfile Map file name
 * 
 * @return Operation success
 */
BOOL SAGE_LoadTileMap(UWORD index, STRPTR mapfile)
{
  SAGE_TileMap *tilemap;
  BPTR file_handle;
  LONG bytes_read, file_size, map_size;

  tilemap = SAGE_GetTileMap(index);
  SAFE(if (tilemap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  map_size = tilemap->cols * tilemap->rows * tilemap->bytespertile;
  file_handle = Open(mapfile, MODE_OLDFILE);
  if (file_handle != 0) {
    bytes_read = Seek(file_handle, 0, OFFSET_END);
    file_size = Seek(file_handle, 0, OFFSET_BEGINNING);
    // Be sure that map file fits into our map buffer
    if (file_size > map_size) {
      file_size = map_size;
    }
    bytes_read = Read(file_handle, tilemap->map, file_size);
    if (bytes_read == file_size) {
      Close(file_handle);
      return TRUE;
    }
    Close(file_handle);
  }
  SAGE_SetError(SERR_TILEMAP_FILE);
  return FALSE;
}

/**
 * Get tile map as UBYTE array
 *
 * @param index Tilemap index
 *
 * @return Tilemap array
 */
UBYTE *SAGE_GetTileMapB(UWORD index)
{
  SAGE_TileMap *tilemap;

  tilemap = SAGE_GetTileMap(index);
  SAFE(if (tilemap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return NULL;
  })
  return (UBYTE *)tilemap->map;
}

/**
 * Get tile map as UWORD array
 *
 * @param index Tilemap index
 *
 * @return Tilemap array
 */
UWORD *SAGE_GetTileMapW(UWORD index)
{
  SAGE_TileMap *tilemap;

  tilemap = SAGE_GetTileMap(index);
  SAFE(if (tilemap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return NULL;
  })
  return (UWORD *)tilemap->map;
}

/** WORK IN PROGRESS !!! */

/** Get tile UBYTE value at map position */
UBYTE SAGE_GetTileValueB(UWORD index, UWORD col, UWORD row);

/** Update a tile UBYTE value at map position */
BOOL SAGE_UpdateTileValueB(UWORD index, UWORD col, UWORD row, UBYTE value);

/** Get tile UWORD value at map position */
UWORD SAGE_GetTileValueW(UWORD index, UWORD col, UWORD row);

/** Update a tile UWORD value at map position */
BOOL SAGE_UpdateTileValueW(UWORD index, UWORD col, UWORD row, UWORD value);
