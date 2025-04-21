/**
 * sage_debug.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Debug macro
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#ifndef _SAGE_DEBUG_H_
#define _SAGE_DEBUG_H_

#if _SAGE_DEBUG_MODE_ == 1
#define SD(x) x
#else
#define SD(x)
#endif

#if _SAGE_SAFE_MODE_ == 1
#define SAFE(x) x
#else
#define SAFE(x)
#endif

#include <exec/exec.h>

#include <sage/sage_3dcamera.h>
#include <sage/sage_3dentity.h>
#include <sage/sage_3drender.h>
#include <sage/sage_3dterrain.h>
#include <sage/sage_3dtexmap.h>

/**********************************************************
 *             DEBUG PROTOTYPES
 **********************************************************/

// sage_3dcamera
VOID SAGE_DumpCamera(SAGE_Camera *);

// sage_3dentity
#define S3DE_DEBUG_EMIN     0
#define S3DE_DEBUG_EALL     255
#define S3DE_DEBUG_EVERTS   1
#define S3DE_DEBUG_EFACES   2
#define S3DE_DEBUG_ENORMS   4
VOID SAGE_DumpEntity(SAGE_Entity *, UWORD);

// sage_3drender
VOID SAGE_Dump3DElement(SAGE_3DElement *);
VOID SAGE_DumpElementList(SAGE_SortedElement *, UWORD);
VOID SAGE_DumpRenderModeState(VOID);

// sage_3dskybox
#define S3DE_DEBUG_SALL     255
#define S3DE_DEBUG_SVERTS   1
#define S3DE_DEBUG_SPLANES  2
VOID SAGE_DumpSkybox(WORD);

// sage_3dterrain
#define S3DE_DEBUG_TMIN     0
#define S3DE_DEBUG_TALL     255
#define S3DE_DEBUG_THMAP    1
#define S3DE_DEBUG_TVERTS   2
#define S3DE_DEBUG_TZONES   4
VOID SAGE_DumpZone(SAGE_Zone *);
VOID SAGE_DumpTerrain(WORD);

// sage_3dtexmap
VOID SAGE_DumpS3DTriangle(S3D_Triangle *);

// sage_video
VOID SAGE_DumpVideoModes(VOID);

#endif
