/**
 * voxel.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Voxel effect
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 November 2020
 */

// Port of example voxel code by Sebastian Macke https://github.com/s-macke/VoxelSpace
//  & Gustavo Pezzi https://github.com/gustavopezzi/voxelspace

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define MAP_SIZE              1024
#define SCALE_FACTOR          70.0

#define PI_VAL                3.14159265358979323846f

struct _camera {
  FLOAT x;         // x position on the map
  FLOAT y;         // y position on the map
  FLOAT height;    // height of the camera
  FLOAT horizon;   // offset of the horizon position (looking up-down)
  FLOAT zfar;      // distance of the camera looking forward
  FLOAT angle;     // camera angle (radians, clockwise)
} camera = { 512.0, 512.0, 70., 60.0, 600.0, (1.5 * PI_VAL) };

struct _datacast {
  FLOAT plx, ply, dix, diy, scale;
  LONG screen_width, screen_height, map_size;
  UBYTE * framebuffer, * heightmap, * colormap;
} datacast = { 0.0, 0.0, 0.0, 0.0, SCALE_FACTOR, SCREEN_WIDTH, SCREEN_HEIGHT, MAP_SIZE, NULL, NULL, NULL};

//extern VOID __asm fast_raycast(register __a0 APTR cam, register __a1 APTR cast);

// Controls
#define KEY_NBR               9
#define KEY_UP                0
#define KEY_DOWN              1
#define KEY_LEFT              2
#define KEY_RIGHT             3
#define KEY_A                 4
#define KEY_Q                 5
#define KEY_Z                 6
#define KEY_S                 7
#define KEY_QUIT              8

UBYTE keyboard_state[KEY_NBR];

SAGE_KeyScan keys[KEY_NBR] = {
  { SKEY_FR_UP, FALSE },
  { SKEY_FR_DOWN, FALSE },
  { SKEY_FR_LEFT, FALSE },
  { SKEY_FR_RIGHT, FALSE },
  { SKEY_FR_A, FALSE },
  { SKEY_FR_Q, FALSE },
  { SKEY_FR_Z, FALSE },
  { SKEY_FR_S, FALSE },
  { SKEY_FR_ESC, FALSE }
};

UBYTE string_buffer[256];

BOOL finish;

void keyboard(void)
{
  SAGE_ScanKeyboard(keys, KEY_NBR);
  if (keys[KEY_QUIT].key_pressed) {
    SAGE_AppliLog("Exit loop");
    finish = TRUE;
  }
  if (keys[KEY_LEFT].key_pressed) {
    camera.angle -= 0.02f;
  } else if (keys[KEY_RIGHT].key_pressed) {
    camera.angle += 0.02f;
  }
  if (keys[KEY_UP].key_pressed) {
    camera.x += (FLOAT)cos(camera.angle);
    camera.y += (FLOAT)sin(camera.angle);
  } else if (keys[KEY_DOWN].key_pressed) {
    camera.x -= (FLOAT)cos(camera.angle);
    camera.y -= (FLOAT)sin(camera.angle);
  }
  if (keys[KEY_A].key_pressed) {
    camera.height++;
  } else if(keys[KEY_Q].key_pressed) {
    camera.height--;
  }
  if (keys[KEY_Z].key_pressed) {
    camera.horizon += 1.5f;
  } else if (keys[KEY_S].key_pressed) {
    camera.horizon -= 1.5f;
  }
}

void raycast(struct _camera * cam, struct _datacast * cast)
{
  LONG x, y, mapoffset, tallestheight, projheight;
  FLOAT deltax, deltay, deltaz, rx, ry, z;

  // Loop 320 rays from left to right
  for (x = 0; x < cast->screen_width; x++) {
    deltax = (cast->plx + cast->dix * x) / cam->zfar;
    deltay = (cast->ply + cast->diy * x) / cam->zfar;
    deltaz = 1.0f;

    // Ray (x,y) coords
    rx = cam->x;
    ry = cam->y;

    // Store the tallest projected height per-ray
    tallestheight = cast->screen_height;

    // Loop all depth units until the zfar distance limit
    for (z = 1.0; z < cam->zfar; z += deltaz) {
      rx += deltax;
      ry += deltay;

      // Find the offset that we have to go and fetch values from the heightmap
      mapoffset = ((cast->map_size * ((int)(ry) & (cast->map_size - 1))) + ((int)(rx) & (cast->map_size - 1)));

      // Project height values and find the height on-screen
      projheight = (int)((cam->height - cast->heightmap[mapoffset]) / z * cast->scale + cam->horizon);

      // Only draw pixels if the new projected height is taller than the previous tallest height
      if (projheight < tallestheight) {
        // Draw pixels from previous max-height until the new projected height
        for (y = projheight; y < tallestheight; y++) {
          if (y >= 0) {
            cast->framebuffer[(cast->screen_width * y) + x] = cast->colormap[mapoffset];
          }
        }
        tallestheight = projheight;
      }
      deltaz += 0.005f;
    }
  }
}

void voxel(UBYTE * framebuffer, UBYTE * heightmap, UBYTE * colormap)
{
  FLOAT sinangle, cosangle, prx, pry;

  sinangle = sin(camera.angle);
  cosangle = cos(camera.angle);

  // Left-most point of the FOV
  datacast.plx = cosangle * camera.zfar + sinangle * camera.zfar;
  datacast.ply = sinangle * camera.zfar - cosangle * camera.zfar;

  // Right-most point of the FOV
  prx = cosangle * camera.zfar - sinangle * camera.zfar;
  pry = sinangle * camera.zfar + cosangle * camera.zfar;

  datacast.dix = (prx - datacast.plx) / SCREEN_WIDTH;
  datacast.diy = (pry - datacast.ply) / SCREEN_WIDTH;

  datacast.scale = SCALE_FACTOR;
  datacast.screen_width = SCREEN_WIDTH;
  datacast.screen_height = SCREEN_HEIGHT;
  datacast.map_size = MAP_SIZE;

  datacast.framebuffer = framebuffer;
  datacast.heightmap = heightmap;
  datacast.colormap = colormap;

  raycast(&camera, &datacast);
//  fast_raycast(&camera, &datacast);
  
}

void main(void)
{
  SAGE_Picture * picheight, * piccolor;
  UBYTE * heightmap, * colormap;
  

  SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("SAGE library voxel demo V1.0");
  SAGE_AppliLog("Initialize SAGE");
  if (SAGE_Init(SMOD_VIDEO|SMOD_INPUT|SMOD_INTERRUPTION)) {
    SAGE_AppliLog("Opening screen");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      SAGE_HideMouse();

      if (!SAGE_EnableFrameCount(TRUE)) {
        SAGE_ErrorLog("Can't activate frame rate counter !");
      }

      SAGE_AppliLog("Load height map");
      picheight = SAGE_LoadPicture("data/D1.gif");

      SAGE_AppliLog("Load color map");
      piccolor = SAGE_LoadPicture("data/C1W.gif");
      SAGE_LoadPictureColorMap(piccolor);
      SAGE_RefreshColors(0, 256);

      heightmap = picheight->bitmap->bitmap_buffer;
      colormap = piccolor->bitmap->bitmap_buffer;

      SAGE_AppliLog("Running main loop");
      finish = FALSE;
      while (!finish) {

        SAGE_ClearScreen();
        
        keyboard();

        voxel((UBYTE *) SAGE_GetBackBitmap()->bitmap_buffer, heightmap, colormap);

        sprintf(string_buffer, "%d fps", SAGE_GetFps());
        SAGE_PrintText(string_buffer, 10, 10);

        if (!SAGE_RefreshScreen()) {
          SAGE_DisplayError();
        }
        
      }
      SAGE_ReleasePicture(picheight);
      SAGE_ReleasePicture(piccolor);
      SAGE_ShowMouse();
      SAGE_AppliLog("Closing screen");
      SAGE_CloseScreen();
    }
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}

