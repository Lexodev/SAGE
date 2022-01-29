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

#include <stdlib.h>
#include <math.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

#define HM_WIDTH              1024
#define HM_HEIGHT             1024

#define CM_WIDTH              1024
#define CM_HEIGHT             1024

#define PI_VAL                3.14159265358979323846f

struct {
  float x;          // x position on the map
  float y;          // y position on the map
  float height;     // height of the camera
  float angle;      // direction of the camera
  float horizon;    // horizon position (look up and down)
  float distance;   // distance of map
} camera = { 512, 800, 78, 0, 100, 800 };

int hiddeny[SCREEN_WIDTH];

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

void main(void)
{
  SAGE_Event * event = NULL;
  SAGE_Picture * picheight, * piccolor;
  int x, y, mapwidthperiod, mapheightperiod, mapshift, cameraoffs;
  int mapoffset, heightonscreen, col;
  float sinang, cosang, deltaz, z;
  float plx, ply, prx, pry, dx, dy, invz;
  unsigned char * buffer, * heightmap, * colormap;
  BOOL finish;

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

        buffer = (unsigned char *) SAGE_GetBackBitmap()->bitmap_buffer;

        mapwidthperiod = HM_WIDTH - 1;
        mapheightperiod = HM_HEIGHT - 1;
        mapshift = 10;

        // Collision detection. Don't fly below the surface.
        cameraoffs = ((((int)camera.y) & mapwidthperiod ) << mapshift ) + (((int)camera.x) & mapheightperiod);
        if ((heightmap[cameraoffs] + 10.0f ) > camera.height) camera.height = heightmap[cameraoffs] + 10.0f;

        sinang = (float)sin(camera.angle);
        cosang = (float)cos(camera.angle);

        for (x = 0;x < SCREEN_WIDTH;++x) hiddeny[x] = SCREEN_HEIGHT;
        deltaz = 1.0f;

        SAGE_ClearScreen();
        
        // Draw from front to back
        for (z = 1.0f;z < camera.distance;z += deltaz) {
          // 90 degree field of view
          plx =  -cosang * z - sinang * z;
          ply =   sinang * z - cosang * z;
          prx =   cosang * z - sinang * z;
          pry =  -sinang * z - cosang * z;

          dx = (prx - plx) / SCREEN_WIDTH;
          dy = (pry - ply) / SCREEN_HEIGHT;
          plx += camera.x;
          ply += camera.y;
          invz = 1.0f / z * 100.0f;
          for (x = 0;x < SCREEN_WIDTH;++x) {
            mapoffset = ((((int)ply) & mapwidthperiod ) << mapshift) + (((int)plx) & mapheightperiod);
            heightonscreen = (int)((camera.height - heightmap[mapoffset] ) * invz + camera.horizon);
            if (heightonscreen < 0) heightonscreen = 0;
            col = colormap[mapoffset];
            for (y = heightonscreen;y < hiddeny[x];++y) {
              buffer[x + y * SCREEN_WIDTH] = col;
            }
            if (heightonscreen < hiddeny[x] ) hiddeny[x] = heightonscreen; 
            plx += dx;
            ply += dy;
          }
          deltaz += 0.005f;
        }

        SAGE_ScanKeyboard(keys, KEY_NBR);      
        if (keys[KEY_QUIT].key_pressed) {
          SAGE_AppliLog("Exit loop");
          finish = TRUE;
        }
        if (keys[KEY_LEFT].key_pressed) {
          camera.angle += 0.02f;
        } else if (keys[KEY_RIGHT].key_pressed) {
          camera.angle -= 0.02f;
        }
        if (keys[KEY_UP].key_pressed) {
          camera.x -= (float)sin( camera.angle ) * 1.1f;
          camera.y -= (float)cos( camera.angle ) * 1.1f;
        } else if (keys[KEY_DOWN].key_pressed) {
          camera.x += (float)sin( camera.angle ) * 0.75f;
          camera.y += (float)cos( camera.angle ) * 0.75f;
        }
        if (keys[KEY_A].key_pressed) {
          camera.height += 0.5f;
        } else if(keys[KEY_Q].key_pressed) {
          camera.height -= 0.5f;
        }
        if (keys[KEY_Z].key_pressed) {
          camera.horizon += 1.5f;
        } else if (keys[KEY_S].key_pressed) {
          camera.horizon -= 1.5f;
        }

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
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("Closing SAGE");
  if (!SAGE_Exit()) {
    SAGE_DisplayError();
  }
  SAGE_AppliLog("End of demo");
}


/**

int main( int argc, char* argv[] ) {
    setvideomode( videomode_320x200 ); 

    uint8_t palette[ 768 ];
    int mapwidth, mapheight, palcount;
    uint8_t* mapcol = loadgif( "files/C1W.gif", &mapwidth, &mapheight, &palcount, palette );    
    uint8_t* mapalt = loadgif( "files/D1.gif", &mapwidth, &mapheight, NULL, NULL );    

    for( int i = 0; i < palcount; ++i ) {
        setpal(i, palette[ 3 * i + 0 ],palette[ 3 * i + 1 ], palette[ 3 * i + 2 ] );
    }
    setpal( 0, 36, 36, 56 );

    struct {
        float x;       // x position on the map
        float y;       // y position on the map
        float height;  // height of the camera
        float angle;   // direction of the camera
        float horizon; // horizon position (look up and down)
        float distance; // distance of map
    } camera = { 512, 800, 78, 0, 100, 800 };

    setdoublebuffer( 1 );
    uint8_t* screen = screenbuffer();

    while( !shuttingdown() ) {
        waitvbl();
        clearscreen();        

        if( keystate( KEY_LEFT ) ) camera.angle += 0.02f;
        if( keystate( KEY_RIGHT ) ) camera.angle -= 0.02f;
        if( keystate( KEY_UP ) ) {
            camera.x -= (float)sin( camera.angle ) * 1.1f;
            camera.y -= (float)cos( camera.angle ) * 1.1f;
        }
        if( keystate( KEY_DOWN ) ) {
            camera.x += (float)sin( camera.angle ) * 0.75f;
            camera.y += (float)cos( camera.angle ) * 0.75f;
        }
        if( keystate( KEY_R ) ) camera.height += 0.5f;
        if( keystate( KEY_F ) ) camera.height -= 0.5f;
        if( keystate( KEY_Q ) ) camera.horizon += 1.5f;
        if( keystate( KEY_W ) ) camera.horizon -= 1.5f;

        int mapwidthperiod = mapwidth - 1;
        int mapheightperiod = mapheight - 1;
        int mapshift = 10;

        // Collision detection. Don't fly below the surface.
        int cameraoffs = ( ( ((int)camera.y) & mapwidthperiod ) << mapshift ) + ( ((int)camera.x) & mapheightperiod );
        if( ( mapalt[ cameraoffs ] + 10.0f ) > camera.height ) camera.height = mapalt[ cameraoffs ] + 10.0f;

        int screenwidth = 320;
        int screenheight = 200;
        float sinang = (float)sin( camera.angle );
        float cosang = (float)cos( camera.angle );

        int hiddeny[ 320 ];
        for( int i = 0; i < screenwidth; ++i )  hiddeny[ i ] = screenheight;
        float deltaz = 1.0f;

        // Draw from front to back
        for( float z = 1.0f; z < camera.distance; z += deltaz ) {
            // 90 degree field of view
            float plx =  -cosang * z - sinang * z;
            float ply =   sinang * z - cosang * z;
            float prx =   cosang * z - sinang * z;
            float pry =  -sinang * z - cosang * z;

            float dx = ( prx - plx ) / screenwidth;
            float dy = ( pry - ply ) / screenwidth;
            plx += camera.x;
            ply += camera.y;
            float invz = 1.0f / z * 100.0f;
            for( int i = 0; i < screenwidth; ++i ) {
                int mapoffset = ( ( ((int)ply) & mapwidthperiod ) << mapshift ) + ( ((int)plx) & mapheightperiod );
                int heightonscreen = (int)( ( camera.height - mapalt[ mapoffset ] ) * invz + camera.horizon );
                if( heightonscreen < 0 ) heightonscreen = 0;
                int col = mapcol[ mapoffset ];
                for( int y = heightonscreen; y < hiddeny[ i ]; ++y ) {
                    screen[ i + y * 320 ] = (uint8_t)col;
                }
                if( heightonscreen < hiddeny[ i ] )  hiddeny[ i ] = heightonscreen; 
                plx += dx;
                ply += dy;
            }
            deltaz += 0.005f;
        }
        
        setcolor( 255 );
        outtextxy( 10, 10, "UP/DOWN/LEFT/RIGHT - move/turn" );
        outtextxy( 10, 18, "R/F - change altitude" );
        outtextxy( 10, 26, "Q/W - change pitch" );

        screen = swapbuffers();

        if( keystate( KEY_ESCAPE ) )  break;
    }

    return 0;
}

*/
