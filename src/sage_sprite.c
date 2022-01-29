/**
 * sage_sprite.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Sprite management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_sprite.h"

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Create a sprite bank
 *
 * @param index   Sprite bank index
 * @param size    Number of sprites in the bank
 * @param picture SAGE Picture pointer
 *
 * @return Operation success
 */
BOOL SAGE_CreateSpriteBank(UWORD index, UWORD size, SAGE_Picture * picture)
{
  SAGE_SpriteBank * bank;
  UWORD sprite;

  SD(SAGE_DebugLog("Create sprite bank #%d (%d)", index, size));
  // Check for video device
  if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  if (index >= SSPR_MAX_SPRBANK) {
    SAGE_SetError(SERR_SPRBANK_INDEX);
    return FALSE;
  }
  if (picture == NULL || picture->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (SageContext.SageVideo->sprites[index] != NULL) {
    SAGE_ReleaseSpriteBank(index);
  }
  // Allocate and init the sprite bank
  bank = (SAGE_SpriteBank *) SAGE_AllocMem(sizeof(SAGE_SpriteBank));
  if (bank != NULL) {
    bank->bank_size = size;
    bank->sprites = (SAGE_Sprite *) SAGE_AllocMem(sizeof(SAGE_Sprite) * size);
    if (bank->sprites != NULL) {
      for (sprite = 0;sprite < size;sprite++) {
        bank->sprites[sprite].left = 0;
        bank->sprites[sprite].top = 0;
        bank->sprites[sprite].width = 0;
        bank->sprites[sprite].height = 0;
      }
      if ((bank->bitmap = SAGE_AllocBitmap(picture->bitmap->width, picture->bitmap->height, picture->bitmap->depth, picture->bitmap->pixformat, NULL)) != NULL) {
        SAGE_BlitBitmap(picture->bitmap, 0, 0, picture->bitmap->width, picture->bitmap->height, bank->bitmap, 0, 0);
        SageContext.SageVideo->sprites[index] = bank;
        return TRUE;
      }
      SAGE_FreeMem(bank->sprites);
    }
    SAGE_FreeMem(bank);
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Get a sprite bank by his index
 *
 * @param index Sprite bank index
 *
 * @return Sprite bank structure
 */
SAGE_SpriteBank * SAGE_GetSpriteBank(UWORD index)
{
  // Check for video device
  SAFE(if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  })
  if (index >= SSPR_MAX_SPRBANK) {
    SAGE_SetError(SERR_SPRBANK_INDEX);
    return NULL;
  }
  return SageContext.SageVideo->sprites[index];
}

/**
 * Release a sprite bank
 *
 * @param index Sprite bank index
 *
 * @return Operation success
 */
BOOL SAGE_ReleaseSpriteBank(UWORD index)
{
  SAGE_SpriteBank * bank;
  
  SD(SAGE_DebugLog("Release sprite bank #%d", index));
  bank = SAGE_GetSpriteBank(index);
  if (bank != NULL) {
    if (bank->bitmap != NULL) {
      SAGE_ReleaseBitmap(bank->bitmap);
    }
    if (bank->sprites != NULL) {
      SAGE_FreeMem(bank->sprites);
    }
    SAGE_FreeMem(bank);
    SageContext.SageVideo->sprites[index] = NULL;
    return TRUE;
  }
  return FALSE;
}

/**
 * Define the sprite bank transparency color
 *
 * @param index Sprite bank index
 * @param color Transparent color
 *
 * @return Operation success
 */
BOOL SAGE_SetSpriteBankTransparency(UWORD index, ULONG color)
{
  SAGE_SpriteBank * bank;
  
  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  return SAGE_SetBitmapTransparency(bank->bitmap, color);
}

/**
 * Calculate the hotspot coordinate
 *
 * @param bank    Sprite bank
 * @param sprite  Sprite index
 */
VOID SAGE_CalculSpriteHotspot(SAGE_SpriteBank * bank, UWORD sprite)
{
  switch (bank->sprites[sprite].hotspot) {
    case SSPR_HS_TOPMID:
      bank->sprites[sprite].hs_x = bank->sprites[sprite].width / 2;
      bank->sprites[sprite].hs_y = 0;
      break;
    case SSPR_HS_TOPRIGHT:
      bank->sprites[sprite].hs_x = bank->sprites[sprite].width - 1;
      bank->sprites[sprite].hs_y = 0;
      break;
    case SSPR_HS_MIDLEFT:
      bank->sprites[sprite].hs_x = 0;
      bank->sprites[sprite].hs_y = bank->sprites[sprite].height / 2;
      break;
    case SSPR_HS_MIDDLE:
      bank->sprites[sprite].hs_x = bank->sprites[sprite].width / 2;
      bank->sprites[sprite].hs_y = bank->sprites[sprite].height / 2;
      break;
    case SSPR_HS_MIDRIGHT:
      bank->sprites[sprite].hs_x = bank->sprites[sprite].width - 1;
      bank->sprites[sprite].hs_y = bank->sprites[sprite].height / 2;
      break;
    case SSPR_HS_BOTLEFT:
      bank->sprites[sprite].hs_x = 0;
      bank->sprites[sprite].hs_y = bank->sprites[sprite].height - 1;
      break;
    case SSPR_HS_BOTMID:
      bank->sprites[sprite].hs_x = bank->sprites[sprite].width / 2;
      bank->sprites[sprite].hs_y = bank->sprites[sprite].height - 1;
      break;
    case SSPR_HS_BOTRIGHT:
      bank->sprites[sprite].hs_x = bank->sprites[sprite].width - 1;
      bank->sprites[sprite].hs_y = bank->sprites[sprite].height - 1;
      break;
    default:
      bank->sprites[sprite].hs_x = 0;
      bank->sprites[sprite].hs_y = 0;
  }
}

/**
 * Add a sprite to the bank
 * 
 * @param index   Sprite bank index
 * @param sprite  Sprite index
 * @param left    Left sprite position
 * @param top     Top sprite position
 * @param width   Sprite width
 * @param height  Sprite height
 * @param hotspot Sprite hot spot
 * 
 * @return Operation success
 */
BOOL SAGE_AddSpriteToBank(UWORD index, UWORD sprite, ULONG left, ULONG top, ULONG width, ULONG height, UWORD hotspot)
{
  SAGE_SpriteBank * bank;
  
  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL || bank->sprites == NULL || bank->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > sprite) {
    // Check for size constraint
    if (!SAGE_CheckSizeConstraint(width, bank->bitmap->depth)) {
      return FALSE;
    }
    if ((left + width) <= bank->bitmap->width && (top + height) <= bank->bitmap->height) {
      bank->sprites[sprite].left = left;
      bank->sprites[sprite].top = top;
      bank->sprites[sprite].width = width;
      bank->sprites[sprite].height = height;
      bank->sprites[sprite].real_width = width;
      bank->sprites[sprite].real_height = height;
      bank->sprites[sprite].horizontal_flip = FALSE;
      bank->sprites[sprite].vertical_flip = FALSE;
      bank->sprites[sprite].horizontal_zoom = 1.0;
      bank->sprites[sprite].vertical_zoom = 1.0;
      bank->sprites[sprite].flags = SSPR_STANDARD;
      bank->sprites[sprite].hotspot = hotspot;
      SAGE_CalculSpriteHotspot(bank, sprite);
      return TRUE;
    }
    SAGE_SetError(SERR_SPRITE_SIZE);
    return FALSE;
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}

/**
 * Get a sprite from a sprite bank
 *
 * @param index  Sprite bank index
 * @param sprite Sprite index
 *
 * @return Sprite structure
 */
SAGE_Sprite * SAGE_GetSprite(UWORD index, UWORD sprite)
{
  SAGE_SpriteBank * bank;

  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL || bank->sprites == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > sprite) {
    return &(bank->sprites[sprite]);
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}

/**
 *  Set the sprite flip flags
 * 
 * @param index      Sprite bank index
 * @param sprite     Sprite index
 * @param horizontal Horizontal flip
 * @param vertical   Vertical flip
 * 
 * @return Operation success
 */
BOOL SAGE_SetSpriteFlipping(UWORD index, UWORD sprite, BOOL horizontal, BOOL vertical)
{
  SAGE_SpriteBank * bank;
  
  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL || bank->sprites == NULL || bank->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > sprite) {
    bank->sprites[sprite].horizontal_flip = horizontal;
    bank->sprites[sprite].vertical_flip = vertical;
    // A flipped sprite can't be zoomed
    bank->sprites[sprite].flags &= SSPR_STMASK;
    bank->sprites[sprite].horizontal_zoom = 1.0;
    bank->sprites[sprite].vertical_zoom = 1.0;
    bank->sprites[sprite].width = bank->sprites[sprite].real_width;
    bank->sprites[sprite].height = bank->sprites[sprite].real_height;
    if (horizontal) {
      bank->sprites[sprite].flags |= SSPR_HFLIPPED;
    }
    if (vertical) {
      bank->sprites[sprite].flags |= SSPR_VFLIPPED;
    }
    if (!horizontal && !vertical) {
      bank->sprites[sprite].flags |= SSPR_STANDARD;
    }
    SAGE_CalculSpriteHotspot(bank, sprite);
    return TRUE;
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}

/**
 *  Set the sprite zoom factor
 * 
 * @param index      Sprite bank index
 * @param sprite     Sprite index
 * @param horizontal Horizontal zoom factor
 * @param vertical   Vertical zoom factor
 * 
 * @return Operation success
 */
BOOL SAGE_SetSpriteZoom(UWORD index, UWORD sprite, FLOAT horizontal, FLOAT vertical)
{
  SAGE_SpriteBank * bank;
  
  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL || bank->sprites == NULL || bank->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > sprite) {
    bank->sprites[sprite].horizontal_zoom = horizontal;
    bank->sprites[sprite].vertical_zoom = vertical;
    // A zoomed sprite can't be flipped
    bank->sprites[sprite].flags &= SSPR_STMASK;
    bank->sprites[sprite].horizontal_flip = FALSE;
    bank->sprites[sprite].vertical_flip = FALSE;
    if (horizontal == 1.0 && vertical == 1.0) {
      bank->sprites[sprite].width = bank->sprites[sprite].real_width;
      bank->sprites[sprite].height = bank->sprites[sprite].real_height;
      bank->sprites[sprite].flags |= SSPR_STANDARD;
    } else {
      bank->sprites[sprite].width = (ULONG) (horizontal * bank->sprites[sprite].real_width);
      bank->sprites[sprite].height = (ULONG) (vertical * bank->sprites[sprite].real_height);
      bank->sprites[sprite].flags |= SSPR_ZOOMED;
    }
    SAGE_CalculSpriteHotspot(bank, sprite);
    return TRUE;
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}

/**
 * Set the sprite hotspot
 *
 * @param index   Sprite bank index
 * @param sprite  Sprite index
 * @param hotspot Hotspot type
 * 
 * @return Operation success
 */
BOOL SAGE_SetSpriteHotspot(UWORD index, UWORD sprite, UWORD hotspot)
{
  SAGE_SpriteBank * bank;
  
  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL || bank->sprites == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > sprite) {
    bank->sprites[sprite].hotspot = hotspot;
    SAGE_CalculSpriteHotspot(bank, sprite);
    return TRUE;
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}

/**
 * Check for collision between two sprites
 * 
 * @param idx1 First sprite bank index
 * @param spr1 First sprite index
 * @param x1   First sprite horizontal position
 * @param y1   First sprite vertical position
 * @param idx2 Second sprite bank index
 * @param spr2 Second sprite index
 * @param x2   Second sprite horizontal position
 * @param y2   Second sprite vertical position
 *
 * @return Collision detected
 */
BOOL SAGE_SpriteCollide(UWORD idx1, UWORD spr1, LONG x1, LONG y1, UWORD idx2, UWORD spr2, LONG x2, LONG y2)
{
  SAGE_SpriteBank * bank1, * bank2;

  bank1 = SAGE_GetSpriteBank(idx1);
  bank2 = SAGE_GetSpriteBank(idx2);
  SAFE(if (bank1 == NULL || bank1->sprites == NULL || bank2 == NULL || bank2->sprites == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank1->bank_size > spr1 && bank2->bank_size > spr2) {
    // Set the real position regarding the sprite hotspot
    x1 -= bank1->sprites[spr1].hs_x;
    y1 -= bank1->sprites[spr1].hs_y;
    x2 -= bank2->sprites[spr2].hs_x;
    y2 -= bank2->sprites[spr2].hs_y;
    //printf("SpriteCollide x1 %d, y1 %d, w1 %d, h1 %d\n", x1, y1, bank1->sprites[spr1].width, bank1->sprites[spr1].height);
    //printf("              x2 %d, y2 %d, w2 %d, h2 %d\n", x2, y2, bank2->sprites[spr2].width, bank2->sprites[spr2].height);
    // Check collision
    if (x2 > (x1 + bank1->sprites[spr1].width)) return FALSE;
    if (x1 > (x2 + bank2->sprites[spr2].width)) return FALSE;
    if (y2 > (y1 + bank1->sprites[spr1].height)) return FALSE;
    if (y1 > (y2 + bank2->sprites[spr2].height)) return FALSE;
    return TRUE;
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}

/**
 * Draw the sprite on the screen at a given position inside the clipping zone
 * 
 * @param index  Sprite bank index
 * @param sprite Sprite index
 * @param x_pos  Horizontal position (can be negative)
 * @param y_pos  Vertical position (can be negative)
 *
 * @return Operation success
 */
BOOL SAGE_BlitSpriteToScreen(UWORD index, UWORD sprite, LONG x_pos, LONG y_pos)
{
  LONG left, top, width, height, rwidth, rheight, clipped;
  FLOAT hzoom, vzoom;
  SAGE_Screen * screen;
  SAGE_SpriteBank * bank;
  
  screen = SAGE_GetScreen();
  SAFE(if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  })
  bank = SAGE_GetSpriteBank(index);
  SAFE(if (bank == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  SAFE(if (bank->sprites == NULL || bank->bitmap == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  })
  if (bank->bank_size > sprite) {
    // Set the real position regarding the sprite hotspot
    x_pos -= bank->sprites[sprite].hs_x;
    y_pos -= bank->sprites[sprite].hs_y;
    // Get the sprite size
    left = (LONG) bank->sprites[sprite].left;
    top = (LONG) bank->sprites[sprite].top;
    width = (LONG) bank->sprites[sprite].width;
    height = (LONG) bank->sprites[sprite].height;
    // Remove sprite outside of the screen
    if (x_pos > screen->clipping.right || (x_pos + width) <= screen->clipping.left) {
      return TRUE;
    }
    if (y_pos > screen->clipping.bottom || (y_pos + height) <= screen->clipping.top) {
      return TRUE;
    }
    // Now we can blit the sprite to the screen
    if (bank->sprites[sprite].flags & SSPR_STANDARD)
    {
      // Standard sprite (not flipped or zoomed)
      if (x_pos < screen->clipping.left) {
        left += screen->clipping.left - x_pos;
        width -= screen->clipping.left - x_pos;
        x_pos = screen->clipping.left;
      }
      if (y_pos < screen->clipping.top) {
        top += screen->clipping.top - y_pos;
        height -= screen->clipping.top - y_pos;
        y_pos = screen->clipping.top;
      }
      if ((x_pos + width) > screen->clipping.right) {
        width = (screen->clipping.right - x_pos) + 1;
      }
      if ((y_pos + height) > screen->clipping.bottom) {
        height = (screen->clipping.bottom - y_pos) + 1;
      }
      return SAGE_BlitBitmap(bank->bitmap, left, top, width, height, screen->back_bitmap, x_pos, y_pos);
    } else if ((bank->sprites[sprite].flags & SSPR_HFLIPPED) | (bank->sprites[sprite].flags & SSPR_VFLIPPED)) {
      // Sprite is flipped
    } else if ((bank->sprites[sprite].flags & SSPR_ZOOMED)) {
      // Sprite is zoomed
      rwidth = (LONG) bank->sprites[sprite].real_width;
      rheight = (LONG) bank->sprites[sprite].real_height;
      hzoom = 1.0 / bank->sprites[sprite].horizontal_zoom;
      vzoom = 1.0 / bank->sprites[sprite].vertical_zoom;
      if (x_pos < screen->clipping.left) {
        clipped = (FLOAT)(screen->clipping.left - x_pos) * hzoom;
        left += clipped;
        rwidth -= clipped;
        width -= screen->clipping.left - x_pos;
        x_pos = screen->clipping.left;
      }
      if (y_pos < screen->clipping.top) {
        clipped = (FLOAT)(screen->clipping.top - y_pos) * vzoom;
        top += clipped;
        rheight -= clipped;
        height -= screen->clipping.top - y_pos;
        y_pos = screen->clipping.top;
      }
      if ((x_pos + width) > screen->clipping.right) {
        rwidth = (FLOAT)((screen->clipping.right - x_pos) + 1) * hzoom;
        width = (screen->clipping.right - x_pos) + 1;
      }
      if ((y_pos + height) > screen->clipping.bottom) {
        rheight = (FLOAT)((screen->clipping.bottom - y_pos) + 1) * vzoom;
        height = (screen->clipping.bottom - y_pos) + 1;
      }
      return SAGE_BlitZoomedBitmap(bank->bitmap, left, top, rwidth, rheight, screen->back_bitmap, x_pos, y_pos, width, height);
    }
  }
  SAGE_SetError(SERR_SPRITE_INDEX);
  return FALSE;
}
