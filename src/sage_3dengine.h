/**
 * sage_3dengine.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * 3D engine functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 August 2021
 */

#ifndef _SAGE_3DENGINE_H_
#define _SAGE_3DENGINE_H_

#include <exec/types.h>
#include <Warp3D/Warp3D.h>

//#include "SAGE_camera.h"
//#include "SAGE_object.h"
//#include "SAGE_light.h"
//#include "SAGE_texture.h"

/** Engine constants */
#define S3DE_FOCALE		    90    // Player FOV
#define S3DE_MAX_POINT		500		// Max object points
#define DEGTORAD(x)				((x)*PI/180.0)

/** Face clipping constants */
#define S3DE_NOCLIP			0
#define S3DE_P1CLIP			(1L<<0)
#define S3DE_P2CLIP			(1L<<1)
#define S3DE_P3CLIP			(1L<<2)
#define S3DE_P4CLIP			(1L<<3)
#define S3DE_P2MASK			(S3DE_P1CLIP|S3DE_P3CLIP|S3DE_P4CLIP)
#define S3DE_P4MASK			(S3DE_P1CLIP|S3DE_P2CLIP|S3DE_P3CLIP)

/** Face clipping structure */
typedef struct {
  FLOAT t1,t2;
  FLOAT u1,v1;
  FLOAT u2,v2;
  FLOAT u3,v3;
} SAGE_FaceClip;

/** Initialize the tranformation matrix */
VOID SAGE_TrMatrix(SAGE_Matrix *, FLOAT, FLOAT, FLOAT);

/** Setup the rotation matrix */
VOID SAGE_RxMatrix(SAGE_Matrix *, FLOAT);
VOID SAGE_RyMatrix(SAGE_Matrix *, FLOAT);
VOID SAGE_RzMatrix(SAGE_Matrix *, FLOAT);

/** Transform the camera matric */
VOID SAGE_TransformCameraMatrix(SAGE_Camera *);

/** Transform the object matrix */
VOID SAGE_TransformObjectMatrix(SAGE_Object *);

/** Check if object is in the camera view */
BOOL SAGE_ObjectCulling(SAGE_Camera *, SAGE_Object *);

/** Transform an object to world system */
VOID SAGE_LocalToWorld(SAGE_Object *);

/** Remove the invisible faces of an object */
VOID SAGE_BackFaceCulling(SAGE_Camera *, SAGE_Object *);

//VOID SAGE_DirectionalLight(SAGE_Light *, SAGE_Object *);
//VOID SAGE_PointLight(SAGE_Light *, SAGE_Object *);
//VOID SAGE_FaceShading(SAGE_Object *, SAGE_Light **, UWORD);

/** Transform an object to camera system */
VOID SAGE_WorldToCamera(SAGE_Object *);

/** Clip the object faces in the camera view */
VOID SAGE_FaceClipping(SAGE_Camera *, SAGE_Object *);

/** Build the list of faces to render */
UWORD SAGE_SetClipPolyList(SAGE_Object *, SAGE_Texture **, SAGE_Color *, W3D_Triangle *, UWORD, FLOAT);

//VOID SAGE_CopyTriangle1(W3D_Triangle *,UWORD,UWORD,UWORD,UWORD,SAGE_Face *,UWORD,FLOAT,FLOAT,FLOAT,FLOAT);
//VOID SAGE_CopyTriangle2(W3D_Triangle *,UWORD,UWORD,UWORD,UWORD,SAGE_Face *,UWORD,FLOAT,FLOAT,FLOAT,FLOAT);
//VOID SAGE_ClipTriangle1(W3D_Triangle *,UWORD,SAGE_Clip *,UWORD,UWORD,UWORD,SAGE_Face *,UWORD,FLOAT,FLOAT,FLOAT,FLOAT,FLOAT);
//VOID SAGE_ClipTriangle2(W3D_Triangle *,UWORD,SAGE_Clip *,UWORD,UWORD,UWORD,SAGE_Face *,UWORD,FLOAT,FLOAT,FLOAT,FLOAT,FLOAT);

#endif
