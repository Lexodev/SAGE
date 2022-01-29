/**
 * sage_tile.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Tile management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 November 2020
 */

#include <proto/dos.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_tile.h"

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Create a tile bank
 *
 * @param index      Tile bank index
 * @param tilewidth  Tile width
 * @param tileheight Tile height
 * @param size       Tile bank size
 * @param picture SAGE Picture pointer
 *
 * @return Operation success
 */
BOOL SAGE_CreateTileBank(UWORD index, UWORD tilewidth, UWORD tileheight, UWORD size, SAGE_Picture * picture)
{
  SAGE_TileBank * bank;
  UWORD tile;
 
  SD(SAGE_DebugLog("Create tile bank %d %dx%dx%d", index, tilewidth, tileheight, size));
  // Check for video device
  if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  if (index >= STIL_MAX_TILEBANK) {
    SAGE_SetError(SERR_TILEBANK_INDEX);
    return FALSE;
  }
  if (tilewidth % 16 != 0) {
    SAGE_SetError(SERR_TILE_SIZE);
    return FALSE;
  }
  if (picture == NULL || picture->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (SageContext.SageVideo->tiles[index] != NULL) {
    SAGE_ReleaseTileBank(index);
  }
  bank = (SAGE_TileBank *) SAGE_AllocMem(sizeof(SAGE_TileBank));
  if (bank != NULL) {
    bank->tile_width = tilewidth;
    bank->tile_height = tileheight;
    bank->bank_size = size;
    bank->tiles = (SAGE_Tile *) SAGE_AllocMem(sizeof(SAGE_Tile) * size);
    if (bank->tiles != NULL) {
      for (tile = 0;tile < size;tile++) {
        bank->tiles[tile].left = 0;
        bank->tiles[tile].top = 0;
        bank->tiles[tile].flags = 0;
        bank->tiles[tile].user_data = NULL;
      }
      if ((bank->bitmap = SAGE_AllocBitmap(picture->bitmap->width, picture->bitmap->height, picture->bitmap->depth, picture->bitmap->pixformat, NULL)) != NULL) {
        SAGE_BlitBitmap(picture->bitmap, 0, 0, picture->bitmap->width, picture->bitmap->height, bank->bitmap, 0, 0);
        SageContext.SageVideo->tiles[index] = bank;
        return TRUE;
      }
      SAGE_FreeMem(bank->tiles);
    }
    SAGE_FreeMem(bank);
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Get a tile bank by his index
 *
 * @param index Tile bank index
 *
 * @return Tile bank structure
 */
SAGE_TileBank * SAGE_GetTileBank(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  if (index >= STIL_MAX_TILEBANK) {
    SAGE_SetError(SERR_TILEBANK_INDEX);
    return NULL;
  }
  return SageContext.SageVideo->tiles[index];
}

/**
 * Release a tile bank by his index
 *
 * @param index Tile bank index
 *
 * @param bank SAGE TileBank pointer
 */
BOOL SAGE_ReleaseTileBank(UWORD index)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  if (bank != NULL) {
    if (bank->bitmap != NULL) {
      SAGE_ReleaseBitmap(bank->bitmap);
    }
    if (bank->tiles != NULL) {
      SAGE_FreeMem(bank->tiles);
    }
    SAGE_FreeMem(bank);
    return TRUE;
  }
  return FALSE;
}

/**
 * Add a tile to the tile bank
 * 
 * @param index    Tile bank index
 * @param tile     Tile id
 * @param left     Left tile position
 * @param top      Top tile position
 * @param flags    Tile flags
 * @param userdata Tile specific user data
 * 
 * @return Operation success
 */
BOOL SAGE_AddTileToBank(UWORD index, UWORD tile, ULONG left, ULONG top, LONGBITS flags, APTR userdata)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL || bank->tiles == NULL || bank->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > tile) {
    // Check for size constraint
    if ((left + bank->tile_width) <= bank->bitmap->width && (top + bank->tile_height) <= bank->bitmap->height) {
      bank->tiles[tile].left = left;
      bank->tiles[tile].top = top;
      bank->tiles[tile].flags = flags;
      bank->tiles[tile].user_data = userdata;
      return TRUE;
    }
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return FALSE;
}

/**
 * Bulk add of tiles in a tile bank
 * 
 * @param index Tile bank index
 * 
 * @return Operation success
 */
BOOL SAGE_AddTilesToBank(UWORD index)
{
  SAGE_TileBank * bank;
  UWORD tile;
  ULONG left, top;

  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL || bank->tiles == NULL || bank->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  tile = 0;
  for (top = 0;(top + bank->tile_height) <= bank->bitmap->height;top += bank->tile_height) {
    for (left = 0;(left + bank->tile_width) <= bank->bitmap->height;left += bank->tile_width) {
      if (!SAGE_AddTileToBank(index, tile, left, top, 0, NULL)) {
        return FALSE;
      }
      tile++;
      if (tile >= bank->bank_size) {
        return TRUE;
      }
    }
  }
  return TRUE;
}

/**
 * Set flags for a tile
 * 
 * @param index Tile bank index
 * @param tile  Tile id
 * @param flags Tile flags
 * 
 * @return Operation success
 */
BOOL SAGE_SetTileFlags(UWORD index, UWORD tile, LONGBITS flags)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > tile) {
    bank->tiles[tile].flags = flags;
    return TRUE;
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return FALSE;
}

/**
 * Get flags of a tile
 * 
 * @param index Tile bank index
 * @param tile  Tile id
 * 
 * @return Tile flags
 */
LONGBITS SAGE_GetTileFlags(UWORD index, UWORD tile)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return 0;
  })
  if (bank->bank_size > tile) {
    return bank->tiles[tile].flags;
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return 0;
}

/**
 * Tell if tile has flag on
 * 
 * @param index Tile bank index
 * @param tile  Tile id
 * @param flag  Tile flag
 * 
 * @return Tile has flag setted
 */
BOOL SAGE_HasTileFlag(UWORD index, UWORD tile, LONGBITS flag)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > tile) {
    if (bank->tiles[tile].flags & flag) {
      return TRUE;
    }
    return FALSE;
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return FALSE;
}

/**
 * Set tile user data
 * 
 * @param index Tile bank index
 * @param tile  Tile id
 * 
 * @return Operation success
 */
BOOL SAGE_SetTileUserData(UWORD index, UWORD tile, APTR userdata)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > tile) {
    bank->tiles[tile].user_data = userdata;
    return TRUE;
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return FALSE;
}

/**
 * Get tile user data
 * 
 * @param index Tile bank index
 * @param tile  Tile id
 * 
 * @return Tile user data
 */
APTR SAGE_GetTileUserData(UWORD index, UWORD tile)
{
  SAGE_TileBank * bank;
  
  bank = SAGE_GetTileBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return NULL;
  })
  if (bank->bank_size > tile) {
    return bank->tiles[tile].user_data;
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return NULL;
}

/**
 * Blit a tile to a layer
 *
 * @param index Tile bank index
 * @param tile  Tile id to blit
 * @param l_idx Layer index
 * @param x_pos Horizontal position
 * @param y_pos Vertical position
 *
 * @return Operation success
 */
BOOL SAGE_BlitTileToLayer(UWORD index, UWORD tile, UWORD l_idx, ULONG x_pos, ULONG y_pos)
{
  SAGE_TileBank * bank;
  SAGE_Layer * layer;
  
  bank = SAGE_GetTileBank(index);
  layer = SAGE_GetLayer(l_idx);
  SAFE(if (bank == NULL || layer == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return NULL;
  })
  if (bank->bank_size > tile) {
    x_pos %= layer->bitmap->width;
    y_pos %= layer->bitmap->height;
    return SAGE_BlitBitmap(
      bank->bitmap,
      bank->tiles[tile].left,
      bank->tiles[tile].top,
      bank->tile_width,
      bank->tile_height,
      layer->bitmap,
      x_pos,
      y_pos
    );
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return FALSE;
}

/**
 * Blit a tile to the screen
 *
 * @param index Tile bank index
 * @param tile  Tile id to blit
 * @param x_pos Horizontal position
 * @param y_pos Vertical position
 *
 * @return Operation success
 */
BOOL SAGE_BlitTileToScreen(UWORD index, UWORD tile, ULONG x_pos, ULONG y_pos)
{
  SAGE_TileBank * bank;
  SAGE_Screen * screen;
  
  bank = SAGE_GetTileBank(index);
  screen = SAGE_GetScreen();
  SAFE(if (bank == NULL || screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  if (bank->bank_size > tile) {
    if (x_pos <= (screen->back_bitmap->width - bank->tile_width) && y_pos <= (screen->back_bitmap->height - bank->tile_height)) {
      return SAGE_BlitBitmap(
        bank->bitmap,
        bank->tiles[tile].left,
        bank->tiles[tile].top,
        bank->tile_width,
        bank->tile_height,
        screen->back_bitmap,
        x_pos,
        y_pos
      );
    }
    SAGE_SetError(SERR_TILE_POS);
    return FALSE;
  }
  SAGE_SetError(SERR_TILE_INDEX);
  return FALSE;
}
