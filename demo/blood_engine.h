/**
 * blood_engine.h
 * 
 * Blood project
 * Raycast engine
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#ifndef   __BLOOD_ENGINE_H__
#define   __BLOOD_ENGINE_H__

#include <exec/types.h>

#define WIDTH_SCREEN          320
#define HEIGHT_SCREEN         200
#define WIDTH_VIEW            60
#define WIDTH_BLOC            64
#define MAX_HEIGHT            1024
#define MIN_HEIGHT            8

#define ANGLE_0               0
#define ANGLE_30              WIDTH_SCREEN*30/WIDTH_VIEW
#define ANGLE_90              WIDTH_SCREEN*90/WIDTH_VIEW
#define ANGLE_180             WIDTH_SCREEN*180/WIDTH_VIEW
#define ANGLE_270             WIDTH_SCREEN*270/WIDTH_VIEW
#define ANGLE_360             WIDTH_SCREEN*360/WIDTH_VIEW

#define RAD(x)                ((x)*PI/180.0)

#define STEP_ROT              12
#define STEP_AVANCE           8
#define DEEP                  8

#define FLOOR_ID              0
#define CEIL_OFFSET           10
#define WALL_ID               20
#define DOOR_ID               50

#define INFINY                65536
#define DECAL                 14
#define MULTIPLY              16384
#define WALL_DECAL            6

#define BLOCM_WIDTH           5

#define COLOR_BLACK           0
#define COLOR_WHITE           63
#define COLOR_RED             18

VOID Engine(VOID);
BOOL InitEngine(VOID);

#endif
