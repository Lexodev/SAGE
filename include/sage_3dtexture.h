/**
 * sage_3dtexture.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D texture management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 26/06/2024)
 */

#ifndef _SAGE_3DTEXTURE_H_
#define _SAGE_3DTEXTURE_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>
#include <Maggie3D/Maggie3D.h>

#include <sage/sage_bitmap.h>
#include <sage/sage_picture.h>

#define STEX_MAX_TEXTURES     256

#define STEX_MAXCOLORS        256

#define STEX_FREEINDEX        -1
#define STEX_NOFREEINDEX      -1
#define STEX_USECOLOR         -1

#define STEX_FULLSIZE         0
#define STEX_SIZE64           64
#define STEX_SIZE128          128
#define STEX_SIZE256          256
#define STEX_SIZE512          512

#define STEX_PIXFMT_UNKNOWN   0
#define STEX_PIXFMT_CLUT      W3D_CHUNKY
#define STEX_PIXFMT_RGB15     W3D_A1R5G5B5
#define STEX_PIXFMT_RGB16     W3D_R5G6B5
#define STEX_PIXFMT_RGB24     W3D_R8G8B8
#define STEX_PIXFMT_ARGB32    W3D_A8R8G8B8
#define STEX_PIXFMT_RGBA32    W3D_R8G8B8A8

/** SAGE 3D texture structure */
typedef struct {
  /** Texture size */
  UWORD size;
  /** Texture bitmap */
  SAGE_Bitmap *bitmap;
  /** Warp3D texture */
  W3D_Texture *w3dtex;
  /** Maggie3D texture */
  M3D_Texture *m3dtex;
  /** Texture pixel format & data size*/
  ULONG texformat, data_size;
  /** Texture palette */
  ULONG palette[STEX_MAXCOLORS];
} SAGE_3DTexture;

/** Create a texture from a file */
BOOL SAGE_CreateTextureFromFile(UWORD, STRPTR);

/** Create a texture from a picture */
BOOL SAGE_CreateTextureFromPicture(UWORD, UWORD, UWORD, UWORD, SAGE_Picture *);

/** Get a texture by his index */
SAGE_3DTexture *SAGE_GetTexture(WORD);

/** Get the Warp3D texture by his index */
W3D_Texture *SAGE_GetW3DTexture(WORD);

/** Get the Maggie3D texture by his index */
M3D_Texture *SAGE_GetM3DTexture(WORD);

/** Return the first free slot in texture array */
WORD SAGE_GetFreeTextureIndex(VOID);

/** Get a texture size by his index */
UWORD SAGE_GetTextureSize(UWORD);

/** Get texture buffer by his index */
APTR SAGE_GetTextureBuffer(UWORD);

/** Release a texture */
BOOL SAGE_ReleaseTexture(UWORD);

/** Add a texture to the card memory */
BOOL SAGE_AddTexture(UWORD);

/** Remove a texture from card memory */
BOOL SAGE_RemoveTexture(UWORD);

/** Remove all textures from card memory */
BOOL SAGE_FlushTextures(VOID);

/** Define the texture transparency color */
BOOL SAGE_SetTextureTransparency(UWORD, ULONG);

#endif
