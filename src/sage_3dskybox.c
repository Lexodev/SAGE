/**
 * sage_3dskybox.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 3D skybox management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#include <string.h>
#include <math.h>

#include <proto/exec.h>

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_screen.h"
#include "sage_3dtexture.h"
#include "sage_3dcamera.h"
#include "sage_3dengine.h"
#include "sage_3dskybox.h"

/** Engine data */
extern SAGE_3DWorld sage_world;

/** Skybox reference data */
SAGE_Vertex skybox_refvertices[S3DE_SKYBOX_VERTICES] = {
  // Front 0->24
  { -1.0, 1.0, 1.0 }, { -0.5, 1.0, 1.0 }, { 0.0, 1.0, 1.0 }, { 0.5, 1.0, 1.0 }, { 1.0, 1.0, 1.0 },
  { -1.0, 0.5, 1.0 }, { -0.5, 0.5, 1.0 }, { 0.0, 0.5, 1.0 }, { 0.5, 0.5, 1.0 }, { 1.0, 0.5, 1.0 },
  { -1.0, 0.0, 1.0 }, { -0.5, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.5, 0.0, 1.0 }, { 1.0, 0.0, 1.0 },
  { -1.0, -0.5, 1.0 }, { -0.5, -0.5, 1.0 }, { 0.0, -0.5, 1.0 }, { 0.5, -0.5, 1.0 }, { 1.0, -0.5, 1.0 },
  { -1.0, -1.0, 1.0 }, { -0.5, -1.0, 1.0 }, { 0.0, -1.0, 1.0 }, { 0.5, -1.0, 1.0 }, { 1.0, -1.0, 1.0 },
  // Right 25->44
  { 1.0, 1.0, 0.5 }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0, -0.5 }, { 1.0, 1.0, -1.0 },
  { 1.0, 0.5, 0.5 }, { 1.0, 0.5, 0.0 }, { 1.0, 0.5, -0.5 }, { 1.0, 0.5, -1.0 },
  { 1.0, 0.0, 0.5 }, { 1.0, 0.0, 0.0 }, { 1.0, 0.0, -0.5 }, { 1.0, 0.0, -1.0 },
  { 1.0, -0.5, 0.5 }, { 1.0, -0.5, 0.0 }, { 1.0, -0.5, -0.5 }, { 1.0, -0.5, -1.0 },
  { 1.0, -1.0, 0.5 }, { 1.0, -1.0, 0.0 }, { 1.0, -1.0, -0.5 }, { 1.0, -1.0, -1.0 },
  // Back 45-64
  { 0.5, 1.0, -1.0 }, { 0.0, 1.0, -1.0 }, { -0.5, 1.0, -1.0 }, { -1.0, 1.0, -1.0 },
  { 0.5, 0.5, -1.0 }, { 0.0, 0.5, -1.0 }, { -0.5, 0.5, -1.0 }, { -1.0, 0.5, -1.0 },
  { 0.5, 0.0, -1.0 }, { 0.0, 0.0, -1.0 }, { -0.5, 0.0, -1.0 }, { -1.0, 0.0, -1.0 },
  { 0.5, -0.5, -1.0 }, { 0.0, -0.5, -1.0 }, { -0.5, -0.5, -1.0 }, { -1.0, -0.5, -1.0 },
  { 0.5, -1.0, -1.0 }, { 0.0, -1.0, -1.0 }, { -0.5, -1.0, -1.0 }, { -1.0, -1.0, -1.0 },
  // Left 65->79
  { -1.0, 1.0, -0.5 }, { -1.0, 1.0, 0.0 }, { -1.0, 1.0, 0.5 },
  { -1.0, 0.5, -0.5 }, { -1.0, 0.5, 0.0 }, { -1.0, 0.5, 0.5 },
  { -1.0, 0.0, -0.5 }, { -1.0, 0.0, 0.0 }, { -1.0, 0.0, 0.5 },
  { -1.0, -0.5, -0.5 }, { -1.0, -0.5, 0.0 }, { -1.0, -0.5, 0.5 },
  { -1.0, -1.0, -0.5 }, { -1.0, -1.0, 0.0 }, { -1.0, -1.0, 0.5 },
  // Top 80->88
  { -0.5, 1.0, -0.5 }, { 0.0, 1.0, -0.5 }, { 0.5, 1.0, -0.5 },
  { -0.5, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.5, 1.0, 0.0 },
  { -0.5, 1.0, 0.5 }, { 0.0, 1.0, 0.5 }, { 0.8, 1.0, 0.5 },
  // Bottom 89->97
  { -0.5, -1.0, 0.5 }, { 0.0, -1.0, 0.5 }, { 0.5, -1.0, 0.5 },
  { -0.5, -1.0, 0.0 }, { 0.0, -1.0, 0.0 }, { 0.5, -1.0, 0.0 },
  { -0.5, -1.0, -0.5 }, { 0.0, -1.0, -0.5 }, { 0.5, -1.0, -0.5 },
};

SAGE_Face skybox_reffrontfaces[S3DE_SKYBOX_FACEBYPLANE] = {
  { TRUE,FALSE,S3DE_NOCLIP, 0, 0,1,6,5, 0x80d0f0, 0.0,0.0,0.25,0.0,0.25,0.25,0.0,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 1,2,7,6, 0x80d0f0, 0.25,0.0,0.50,0.0,0.50,0.25,0.25,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 2,3,8,7, 0x80d0f0, 0.50,0.0,0.75,0.0,0.75,0.25,0.50,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 3,4,9,8, 0x80d0f0, 0.75,0.0,1.0,0.0,1.0,0.25,0.75,0.25 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 5,6,11,10, 0x80d0f0, 0.0,0.25,0.25,0.25,0.25,0.50,0.0,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 6,7,12,11, 0x80d0f0, 0.25,0.25,0.50,0.25,0.50,0.50,0.25,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 7,8,13,12, 0x80d0f0, 0.50,0.25,0.75,0.25,0.75,0.50,0.50,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 8,9,14,13, 0x80d0f0, 0.75,0.25,1.0,0.25,1.0,0.50,0.75,0.50 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 10,11,16,15, 0x80d0f0, 0.0,0.50,0.25,0.50,0.25,0.75,0.0,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 11,12,17,16, 0x80d0f0, 0.25,0.50,0.50,0.50,0.50,0.75,0.25,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 12,13,18,17, 0x80d0f0, 0.50,0.50,0.75,0.50,0.75,0.75,0.50,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 13,14,19,18, 0x80d0f0, 0.75,0.50,1.0,0.50,1.0,0.75,0.75,0.75 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 15,16,21,20, 0x80d0f0, 0.0,0.75,0.25,0.75,0.25,1.0,0.0,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 16,17,22,21, 0x80d0f0, 0.25,0.75,0.50,0.75,0.50,1.0,0.25,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 17,18,23,22, 0x80d0f0, 0.50,0.75,0.75,0.75,0.75,1.0,0.50,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 18,19,24,23, 0x80d0f0, 0.75,0.75,1.0,0.75,1.0,1.0,0.75,1.0 },
};

SAGE_Face skybox_refbackfaces[S3DE_SKYBOX_FACEBYPLANE] = {
  { TRUE,FALSE,S3DE_NOCLIP, 0, 28,45,49,32, 0x80d0f0, 0.0,0.0,0.25,0.0,0.25,0.25,0.0,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 45,46,50,49, 0x80d0f0, 0.25,0.0,0.50,0.0,0.50,0.25,0.25,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 46,47,51,50, 0x80d0f0, 0.50,0.0,0.75,0.0,0.75,0.25,0.50,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 47,48,52,51, 0x80d0f0, 0.75,0.0,1.0,0.0,1.0,0.25,0.75,0.25 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 32,49,53,36, 0x80d0f0, 0.0,0.25,0.25,0.25,0.25,0.50,0.0,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 49,50,54,53, 0x80d0f0, 0.25,0.25,0.50,0.25,0.50,0.50,0.25,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 50,51,55,54, 0x80d0f0, 0.50,0.25,0.75,0.25,0.75,0.50,0.50,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 51,52,56,55, 0x80d0f0, 0.75,0.25,1.0,0.25,1.0,0.50,0.75,0.50 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 36,53,57,40, 0x80d0f0, 0.0,0.50,0.25,0.50,0.25,0.75,0.0,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 53,54,58,57, 0x80d0f0, 0.25,0.50,0.50,0.50,0.50,0.75,0.25,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 54,55,59,58, 0x80d0f0, 0.50,0.50,0.75,0.50,0.75,0.75,0.50,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 55,56,60,59, 0x80d0f0, 0.75,0.50,1.0,0.50,1.0,0.75,0.75,0.75 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 40,57,61,44, 0x80d0f0, 0.0,0.75,0.25,0.75,0.25,1.0,0.0,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 57,58,62,61, 0x80d0f0, 0.25,0.75,0.50,0.75,0.50,1.0,0.25,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 58,59,63,62, 0x80d0f0, 0.50,0.75,0.75,0.75,0.75,1.0,0.50,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 59,60,64,63, 0x80d0f0, 0.75,0.75,1.0,0.75,1.0,1.0,0.75,1.0 },
};

SAGE_Face skybox_refleftfaces[S3DE_SKYBOX_FACEBYPLANE] = {
  { TRUE,FALSE,S3DE_NOCLIP, 0, 48,65,68,52, 0x80d0f0, 0.0,0.0,0.25,0.0,0.25,0.25,0.0,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 65,66,69,68, 0x80d0f0, 0.25,0.0,0.50,0.0,0.50,0.25,0.25,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 66,67,70,69, 0x80d0f0, 0.50,0.0,0.75,0.0,0.75,0.25,0.50,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 67,0,5,70, 0x80d0f0, 0.75,0.0,1.0,0.0,1.0,0.25,0.75,0.25 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 52,68,71,56, 0x80d0f0, 0.0,0.25,0.25,0.25,0.25,0.50,0.0,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 68,69,72,71, 0x80d0f0, 0.25,0.25,0.50,0.25,0.50,0.50,0.25,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 69,70,73,72, 0x80d0f0, 0.50,0.25,0.75,0.25,0.75,0.50,0.50,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 70,5,10,73, 0x80d0f0, 0.75,0.25,1.0,0.25,1.0,0.50,0.75,0.50 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 56,71,74,60, 0x80d0f0, 0.0,0.50,0.25,0.50,0.25,0.75,0.0,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 71,72,75,74, 0x80d0f0, 0.25,0.50,0.50,0.50,0.50,0.75,0.25,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 72,73,76,75, 0x80d0f0, 0.50,0.50,0.75,0.50,0.75,0.75,0.50,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 73,10,15,76, 0x80d0f0, 0.75,0.50,1.0,0.50,1.0,0.75,0.75,0.75 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 60,74,77,64, 0x80d0f0, 0.0,0.75,0.25,0.75,0.25,1.0,0.0,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 74,75,78,77, 0x80d0f0, 0.25,0.75,0.50,0.75,0.50,1.0,0.25,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 75,76,79,78, 0x80d0f0, 0.50,0.75,0.75,0.75,0.75,1.0,0.50,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 76,15,20,79, 0x80d0f0, 0.75,0.75,1.0,0.75,1.0,1.0,0.75,1.0 },
};

SAGE_Face skybox_refrightfaces[S3DE_SKYBOX_FACEBYPLANE] = {
  { TRUE,FALSE,S3DE_NOCLIP, 0, 4,25,29,9, 0x80d0f0, 0.0,0.0,0.25,0.0,0.25,0.25,0.0,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 25,26,30,29, 0x80d0f0, 0.25,0.0,0.50,0.0,0.50,0.25,0.25,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 26,27,31,30, 0x80d0f0, 0.50,0.0,0.75,0.0,0.75,0.25,0.50,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 27,28,32,31, 0x80d0f0, 0.75,0.0,1.0,0.0,1.0,0.25,0.75,0.25 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 9,29,33,14, 0x80d0f0, 0.0,0.25,0.25,0.25,0.25,0.50,0.0,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 29,30,34,33, 0x80d0f0, 0.25,0.25,0.50,0.25,0.50,0.50,0.25,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 30,31,35,34, 0x80d0f0, 0.50,0.25,0.75,0.25,0.75,0.50,0.50,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 31,32,36,35, 0x80d0f0, 0.75,0.25,1.0,0.25,1.0,0.50,0.75,0.50 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 14,33,37,19, 0x80d0f0, 0.0,0.50,0.25,0.50,0.25,0.75,0.0,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 33,34,38,37, 0x80d0f0, 0.25,0.50,0.50,0.50,0.50,0.75,0.25,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 34,35,39,38, 0x80d0f0, 0.50,0.50,0.75,0.50,0.75,0.75,0.50,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 35,36,40,39, 0x80d0f0, 0.75,0.50,1.0,0.50,1.0,0.75,0.75,0.75 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 19,37,41,24, 0x80d0f0, 0.0,0.75,0.25,0.75,0.25,1.0,0.0,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 37,38,42,41, 0x80d0f0, 0.25,0.75,0.50,0.75,0.50,1.0,0.25,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 38,39,43,42, 0x80d0f0, 0.50,0.75,0.75,0.75,0.75,1.0,0.50,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 39,40,44,43, 0x80d0f0, 0.75,0.75,1.0,0.75,1.0,1.0,0.75,1.0 },
};

SAGE_Face skybox_reftopfaces[S3DE_SKYBOX_FACEBYPLANE] = {
  { TRUE,FALSE,S3DE_NOCLIP, 0, 48,47,80,65, 0x2040c0, 0.0,0.0,0.25,0.0,0.25,0.25,0.0,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 47,46,81,80, 0x2040c0, 0.25,0.0,0.50,0.0,0.50,0.25,0.25,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 46,45,82,81, 0x2040c0, 0.50,0.0,0.75,0.0,0.75,0.25,0.50,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 45,28,27,82, 0x2040c0, 0.75,0.0,1.0,0.0,1.0,0.25,0.75,0.25 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 65,80,83,66, 0x2040c0, 0.0,0.25,0.25,0.25,0.25,0.50,0.0,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 80,81,84,83, 0x2040c0, 0.25,0.25,0.50,0.25,0.50,0.50,0.25,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 81,82,85,84, 0x2040c0, 0.50,0.25,0.75,0.25,0.75,0.50,0.50,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 82,27,26,85, 0x2040c0, 0.75,0.25,1.0,0.25,1.0,0.50,0.75,0.50 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 66,83,86,67, 0x2040c0, 0.0,0.50,0.25,0.50,0.25,0.75,0.0,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 83,84,87,86, 0x2040c0, 0.25,0.50,0.50,0.50,0.50,0.75,0.25,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 84,85,88,87, 0x2040c0, 0.50,0.50,0.75,0.50,0.75,0.75,0.50,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 85,26,25,88, 0x2040c0, 0.75,0.50,1.0,0.50,1.0,0.75,0.75,0.75 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 67,86,1,0, 0x2040c0, 0.0,0.75,0.25,0.75,0.25,1.0,0.0,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 86,87,2,1, 0x2040c0, 0.25,0.75,0.50,0.75,0.50,1.0,0.25,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 87,88,3,2, 0x2040c0, 0.50,0.75,0.75,0.75,0.75,1.0,0.50,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 88,25,4,3, 0x2040c0, 0.75,0.75,1.0,0.75,1.0,1.0,0.75,1.0 },
};

SAGE_Face skybox_refbottomfaces[S3DE_SKYBOX_FACEBYPLANE] = {
  { TRUE,FALSE,S3DE_NOCLIP, 0, 20,21,89,79, 0xd05010, 0.0,0.0,0.25,0.0,0.25,0.25,0.0,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 21,22,90,89, 0xd05010, 0.25,0.0,0.50,0.0,0.50,0.25,0.25,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 22,23,91,90, 0xd05010, 0.50,0.0,0.75,0.0,0.75,0.25,0.50,0.25 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 23,24,41,91, 0xd05010, 0.75,0.0,1.0,0.0,1.0,0.25,0.75,0.25 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 79,89,92,78, 0xd05010, 0.0,0.25,0.25,0.25,0.25,0.50,0.0,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 89,90,93,92, 0xd05010, 0.25,0.25,0.50,0.25,0.50,0.50,0.25,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 90,91,94,93, 0xd05010, 0.50,0.25,0.75,0.25,0.75,0.50,0.50,0.50 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 91,41,42,94, 0xd05010, 0.75,0.25,1.0,0.25,1.0,0.50,0.75,0.50 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 78,92,95,77, 0xd05010, 0.0,0.50,0.25,0.50,0.25,0.75,0.0,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 92,93,96,95, 0xd05010, 0.25,0.50,0.50,0.50,0.50,0.75,0.25,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 93,94,97,96, 0xd05010, 0.50,0.50,0.75,0.50,0.75,0.75,0.50,0.75 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 94,42,43,97, 0xd05010, 0.75,0.50,1.0,0.50,1.0,0.75,0.75,0.75 },

  { TRUE,FALSE,S3DE_NOCLIP, 0, 77,95,63,64, 0xd05010, 0.0,0.75,0.25,0.75,0.25,1.0,0.0,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 95,96,62,63, 0xd05010, 0.25,0.75,0.50,0.75,0.50,1.0,0.25,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 96,97,61,62, 0xd05010, 0.50,0.75,0.75,0.75,0.75,1.0,0.50,1.0 },
  { TRUE,FALSE,S3DE_NOCLIP, 0, 97,43,44,61, 0xd05010, 0.75,0.75,1.0,0.75,1.0,1.0,0.75,1.0 },
};

/** Skybox data */
SAGE_Vertex skybox_vertices[S3DE_SKYBOX_VERTICES];
SAGE_Face skybox_frontfaces[S3DE_SKYBOX_FACEBYPLANE];
SAGE_Face skybox_backfaces[S3DE_SKYBOX_FACEBYPLANE];
SAGE_Face skybox_leftfaces[S3DE_SKYBOX_FACEBYPLANE];
SAGE_Face skybox_rightfaces[S3DE_SKYBOX_FACEBYPLANE];
SAGE_Face skybox_topfaces[S3DE_SKYBOX_FACEBYPLANE];
SAGE_Face skybox_bottomfaces[S3DE_SKYBOX_FACEBYPLANE];
SAGE_SkyboxPlane skybox_planes[S3DE_SKYBOX_PLANES] = {
  { FALSE, { 0,4,24,20 }, skybox_frontfaces },
  { FALSE, { 28,48,64,44 }, skybox_backfaces },
  { FALSE, { 48,0,20,64 }, skybox_leftfaces },
  { FALSE, { 4,28,44,24 }, skybox_rightfaces },
  { FALSE, { 48,28,4,0 }, skybox_topfaces },
  { FALSE, { 20,24,44,64 }, skybox_bottomfaces },
};

/*****************************************************************************
 *            DEBUG ONLY
 *****************************************************************************/

VOID SAGE_DumpSkybox(WORD mode)
{
  WORD index, plane;
  SAGE_Vertex * vertices;
  SAGE_SkyboxPlane * planes;
  SAGE_Face * faces;
  
  SAGE_DebugLog("Dump skybox");
  SAGE_DebugLog(" => nbvertices=%d", S3DE_SKYBOX_VERTICES);
  if (mode & S3DE_DEBUG_SVERTS) {
    SAGE_DebugLog("-- Vertices");
    vertices = sage_world.skybox.vertices;
    for (index = 0;index < S3DE_SKYBOX_VERTICES;index++) {
      SAGE_DebugLog(" => vertex %d : x=%f  y=%f  z=%f", index, vertices[index].x, vertices[index].y, vertices[index].z);
    }
  }
  if (mode & S3DE_DEBUG_SPLANES) {
    SAGE_DebugLog("-- Planes");
    planes = sage_world.skybox.planes;
    for (plane = 0;plane < S3DE_SKYBOX_PLANES;plane++) {
      SAGE_DebugLog(
        " => #%d  culled=%d  edges(%d, %d, %d, %d)  nbfaces=%d",
        plane,(planes[plane].culled ? 1 : 0),
        planes[plane].edges[0], planes[plane].edges[1], planes[plane].edges[2], planes[plane].edges[3],
        S3DE_SKYBOX_FACEBYPLANE
      );
      SAGE_DebugLog(" - faces -");
      faces = planes[plane].faces;
      for (index = 0;index < S3DE_SKYBOX_FACEBYPLANE;index++) {
        SAGE_DebugLog(" => face %d : p1=%d  p2=%d  p3=%d  p4=%d  color=0x%06X  tex=%d  culled=%d  clipped=%d",
          index, faces[index].p1, faces[index].p2, faces[index].p3, faces[index].p4, faces[index].color,
          faces[index].texture, (faces[index].culled ? 1 : 0), faces[index].clipped
        );
        SAGE_DebugLog("             u1,v1=%f,%f  u2,v2=%f,%f  u3,v3=%f,%f  u4,v4=%f,%f",
          faces[index].u1, faces[index].v1, faces[index].u2, faces[index].v2,
          faces[index].u3, faces[index].v3, faces[index].u4, faces[index].v4
        );
      }
      SAGE_DebugLog(" - faces -");
    }
  }
}

/*****************************************************************************/

/**
 * Setup skybox vertices coordinates
 */
VOID SAGE_SetSkyboxVertices(VOID)
{
  UWORD index;
  FLOAT distance;
  SAGE_Camera * camera;
  SAGE_Vertex * vertices;
  
  camera = SAGE_GetActiveCamera();
  if (camera != NULL) {
    distance = camera->view_dist;
    vertices = sage_world.skybox.vertices;
    for (index = 0;index < S3DE_SKYBOX_VERTICES;index++) {
      vertices[index].x = distance * skybox_refvertices[index].x;
      vertices[index].y = distance * skybox_refvertices[index].y;
      vertices[index].z = distance * skybox_refvertices[index].z;
    }
  }
}

/**
 * Setup skybox plane texture
 */
VOID SAGE_SetSkyboxPlaneTexture(UWORD plane, SAGE_Face * reference)
{
  UWORD index;
  FLOAT texsize;
  SAGE_3DTexture * texture;
  SAGE_Face * faces;

  texture = SAGE_GetTexture(sage_world.skybox.textures[plane]);
  if (texture != NULL) {
    texsize = (FLOAT)texture->size;
  } else {
    texsize = STEX_SIZE64;
  }
  faces = sage_world.skybox.planes[plane].faces;
  for (index = 0;index < S3DE_SKYBOX_FACEBYPLANE;index++) {
    faces[index].is_quad = reference[index].is_quad;
    faces[index].color = SAGE_RemapColor(reference[index].color);
    faces[index].texture = sage_world.skybox.textures[plane];
    faces[index].p1 = reference[index].p1;
    faces[index].u1 = reference[index].u1 * texsize;
    faces[index].v1 = reference[index].v1 * texsize;
    faces[index].p2 = reference[index].p2;
    faces[index].u2 = (reference[index].u2 * texsize) - 1.0;
    faces[index].v2 = reference[index].v2 * texsize;
    faces[index].p3 = reference[index].p3;
    faces[index].u3 = (reference[index].u3 * texsize) - 1.0;
    faces[index].v3 = (reference[index].v3 * texsize) - 1.0;
    faces[index].p4 = reference[index].p4;
    faces[index].u4 = reference[index].u4 * texsize;
    faces[index].v4 = (reference[index].v4 * texsize) - 1.0;
  }
}

/**
 * Initialize skybox data
 */
VOID SAGE_InitSkybox(VOID)
{
  sage_world.skybox.vertices = skybox_vertices;
  sage_world.skybox.planes = skybox_planes;
  SAGE_SetSkyboxVertices();
  SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_FRONT, skybox_reffrontfaces);
  SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_BACK, skybox_refbackfaces);
  SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_LEFT, skybox_refleftfaces);
  SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_RIGHT, skybox_refrightfaces);
  SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_TOP, skybox_reftopfaces);
  SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_BOTTOM, skybox_refbottomfaces);
  SD(SAGE_DumpSkybox(S3DE_DEBUG_SALL));
}

/**
 * Set the world skybox textures
 */
VOID SAGE_SetSkyboxTextures(ULONG tex_front, ULONG tex_back, ULONG tex_left, ULONG tex_right, ULONG tex_top, ULONG tex_bottom)
{
  sage_world.skybox.textures[S3DE_SKYBOX_FRONT] = tex_front;
  sage_world.skybox.textures[S3DE_SKYBOX_BACK] = tex_back;
  sage_world.skybox.textures[S3DE_SKYBOX_LEFT] = tex_left;
  sage_world.skybox.textures[S3DE_SKYBOX_RIGHT] = tex_right;
  sage_world.skybox.textures[S3DE_SKYBOX_TOP] = tex_top;
  sage_world.skybox.textures[S3DE_SKYBOX_BOTTOM] = tex_bottom;
  if (sage_world.active_skybox) {
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_FRONT, skybox_reffrontfaces);
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_FRONT, skybox_reffrontfaces);
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_BACK, skybox_refbackfaces);
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_LEFT, skybox_refleftfaces);
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_RIGHT, skybox_refrightfaces);
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_TOP, skybox_reftopfaces);
    SAGE_SetSkyboxPlaneTexture(S3DE_SKYBOX_BOTTOM, skybox_refbottomfaces);
  }
}

/**
 * Enable/disable the world skybox
 */
VOID SAGE_EnableSkybox(BOOL flag)
{
  sage_world.active_skybox = flag;
  if (flag) {
    SAGE_InitSkybox();
  }
}
