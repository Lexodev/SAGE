/**
 * sage_loadbmp.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * BMP picture loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_LOADBMP_H_
#define _SAGE_LOADBMP_H_

#include <exec/exec.h>
#include "sage_bitmap.h"
#include "sage_picture.h"

// BMP constants
#define SPIC_BMPTAG           0x424d
#define SPIC_BMPHSIZE         14
#define SPIC_BMPWIN3          40L
#define SPIC_BMPWIN4          108L
#define SPIC_BMPWIN5          124L

/** BPM picture header */
typedef struct {
  LONG size, width, height;
  WORD planes, depth;
  LONG compression, imgsize;
  LONG hresol, vresol, colors, important;
} SAGE_BMPHeader;

/** Load the BMP picture */
SAGE_Picture * SAGE_LoadBMPPicture(STRPTR);

#endif
