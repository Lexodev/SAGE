/**
 * sage_sprite.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Sprite management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_SPRITE_H_
#define _SAGE_SPRITE_H_

#include "sage_bitmap.h"
#include "sage_picture.h"

#define SSPR_MAX_SPRBANK      32

#define SSPR_HS_TOPLEFT       0
#define SSPR_HS_TOPMID        1
#define SSPR_HS_TOPRIGHT      2
#define SSPR_HS_MIDLEFT       3
#define SSPR_HS_MIDDLE        4
#define SSPR_HS_MIDRIGHT      5
#define SSPR_HS_BOTLEFT       6
#define SSPR_HS_BOTMID        7
#define SSPR_HS_BOTRIGHT      8

#define SSPR_STANDARD         1
#define SSPR_HFLIPPED         2
#define SSPR_VFLIPPED         4
#define SSPR_ZOOMED           8
#define SSPR_STMASK           0xFFF0

/** SAGE Sprite structure */
typedef struct {
  /** Sprite flipping */
  BOOL horizontal_flip, vertical_flip;
  /** Sprite zoom factor */
  FLOAT horizontal_zoom, vertical_zoom;
  ULONG real_width, real_height;
  /** Sprite rotation angle */
  FLOAT rotation_angle;
  /** Sprite bitmap coordinates */
  ULONG left, top, width, height;
  /** Sprite hot spot */
  UWORD hotspot;
  LONG hs_x, hs_y;
  /** Sprite flags */
  LONGBITS flags;
} SAGE_Sprite;

/** SAGE Sprite bank structure */
typedef struct {
  /** Size of the bank */
  UWORD bank_size;
  /** Sprites in the bank */
  SAGE_Sprite * sprites;
  /** Sprite page bitmap */
  SAGE_Bitmap * bitmap;
} SAGE_SpriteBank;

/** Create a sprite bank */
BOOL SAGE_CreateSpriteBank(UWORD, UWORD, SAGE_Picture *);

/** Get a sprite bank by his index */
SAGE_SpriteBank * SAGE_GetSpriteBank(UWORD);

/** Release a sprite bank and all his sprite */
BOOL SAGE_ReleaseSpriteBank(UWORD);

/** Set the sprites transparency */
BOOL SAGE_SetSpriteBankTransparency(UWORD, ULONG);

/** Add a sprite to the bank */
BOOL SAGE_AddSpriteToBank(UWORD, UWORD, ULONG, ULONG, ULONG, ULONG, UWORD);

/** Get a sprite from a sprite bank */
SAGE_Sprite * SAGE_GetSprite(UWORD, UWORD);

/** Set the sprite flip flags */
BOOL SAGE_SetSpriteFlipping(UWORD, UWORD, BOOL, BOOL);

/** Set the sprite zoom factor */
BOOL SAGE_SetSpriteZoom(UWORD, UWORD, FLOAT, FLOAT);

/** Set the sprite hotspot */
BOOL SAGE_SetSpriteHotspot(UWORD, UWORD, UWORD);

/** Check for collision between two sprites */
BOOL SAGE_SpriteCollide(UWORD, UWORD, LONG, LONG, UWORD, UWORD, LONG, LONG);

/** Blit a sprite to the screen */
BOOL SAGE_BlitSpriteToScreen(UWORD, UWORD, LONG, LONG);

#endif
