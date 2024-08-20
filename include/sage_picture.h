/**
 * sage_picture.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Picture management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#ifndef _SAGE_PICTURE_H_
#define _SAGE_PICTURE_H_

#include <exec/exec.h>

#include <sage/sage_bitmap.h>

// Max colors in colormap
#define SPIC_MAXCOLORS        256L

typedef struct {
  /** Picture color map */
  ULONG color_map[SPIC_MAXCOLORS];
  /** Picture bitmap */
  SAGE_Bitmap *bitmap;
} SAGE_Picture;

/** Allocate picture structure */
SAGE_Picture *SAGE_AllocPicture(VOID);

/** Reslease picture resources */
VOID SAGE_ReleasePicture(SAGE_Picture *);

/** Load a picture using system datatypes */
SAGE_Picture *SAGE_LoadPicture(STRPTR);

/** Save a picture into a file */
//BOOL SAGE_SavePicture(SAGE_Picture *, STRPTR);

/** Set the picture auto remap feature */
VOID SAGE_AutoRemapPicture(BOOL);

/** Set the picture transparency */
BOOL SAGE_SetPictureTransparency(SAGE_Picture *, ULONG);

/** Load a picture color map into screen */
BOOL SAGE_LoadPictureColorMap(SAGE_Picture *);

/** Remap a picture to the screen pixel format */
BOOL SAGE_RemapPicture(SAGE_Picture *);

/** Blit a picture part to a bitmap */
BOOL SAGE_BlitPictureToBitmap(SAGE_Picture *, ULONG, ULONG, ULONG, ULONG, SAGE_Bitmap *, ULONG, ULONG);

/** Blit a picture or a part of it to the screen */
BOOL SAGE_BlitPictureToScreen(SAGE_Picture *, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);

#endif
