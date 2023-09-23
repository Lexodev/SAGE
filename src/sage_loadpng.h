/**
 * sage_loadpng.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * PNG picture loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_LOADPNG_H_
#define _SAGE_LOADPNG_H_

#include <exec/exec.h>
#include "sage_bitmap.h"
#include "sage_picture.h"

// PNG constants
#define SPIC_PNGTAG           0x89504e47
#define SPIC_PNGTAG2          0x0d0a1a0a
#define SPIC_PNGIHDR          0x49484452
#define SPIC_PNGPLTE          0x504C5445
#define SPIC_PNGIDAT          0x49444154
#define SPIC_PNGIEND          0x49454E44
#define SPIC_PNGGRAY          0
#define SPIC_PNGTRUE          2
#define SPIC_PNGINDX          3
#define SPIC_PNGAGRAY         4
#define SPIC_PNGATRUE         6

// GIF constants
#define SPIC_GIFTAG           0x12345678

/** PNG picture header */
typedef struct {
  LONG width, height;
  BYTE depth, color, compression, filter, interleave;
} SAGE_PNGHeader;

/** Load a PNG picture file */
SAGE_Picture * SAGE_LoadPNGPicture(BPTR);

#endif
