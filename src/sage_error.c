/**
 * sage_error.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Errors management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <stdio.h>

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>

/** @var Errors collection */
SAGE_Error SAGE_errors[] = {
  {SERR_NO_ERROR, "No error"},
  {SERR_NOT_AVAILABLE, "Function not available"},
  {SERR_INTUITION_LIB, "Can't open Intuition library"},
  {SERR_CYBERGFX_LIB, "Can't open CybergraphX library"},
  {SERR_DATATYPES_LIB, "Can't open Datatypes library"},
  {SERR_WARP3D_LIB, "Can't open Warp3D library"},
  {SERR_MAGGIE3D_LIB, "Can't open Maggie3D library"},
  {SERR_SOCKET_LIB, "Can't open bsdsocket library"},
  {SERR_NO_VIDEODEVICE, "No video device"},
  {SERR_NO_AUDIODEVICE, "No audio device"},
  {SERR_NO_INPUTDEVICE, "No input device"},
  {SERR_NO_3DDEVICE, "No 3D device"},
  {SERR_NO_NETDEVICE, "No network device"},
  {SERR_OPENDEVICE, "Can't open device"},
  {SERR_NO_RESOLUTION, "No resolution available"},
  {SERR_NO_SCREEN, "Can't open screen"},
  {SERR_NO_WINDOW, "Can't open window"},
  {SERR_NO_BITMAP, "Can't allocate bitmap"},
  {SERR_NO_FONT, "Can't load font"},
  {SERR_NO_SCREENTIMER, "No timer available"},
  {SERR_NO_MODE, "No video mode"},
  {SERR_OPENFILE, "Can't open file"},
  {SERR_READFILE, "Can't read file"},
  {SERR_WRITEFILE, "Can't write file"},
  {SERR_FILEFORMAT, "Bad file format"},
  {SERR_BITMAPFORMAT, "Bad bitmap format"},
  {SERR_BITMAPTYPE, "Not a CGX bitmap"},
  {SERR_PIXFORMAT, "Pixel format not supported"},
  {SERR_UNKNOWN_DEPTH, "Unknown bitmap depth"},
  {SERR_VBLANK, "Can't allocate VBL interrupt"},
  {SERR_REFRESH, "Can't refresh the screen"},
  {SERR_DBLBUF, "Can't allocate double buffering"},
  {SERR_BM_MAPPING, "Unsupported bitmap mapping"},
  {SERR_BM_BLITFMT, "Can't blit on bitmap with another pixel format"},
  {SERR_UNDEF_PIXFMT, "Undefined pixel format"},
  {SERR_LOCKBITMAP, "Can't lock bitmap"},
  {SERR_NO_MEMORY, "Can't allocate memory"},
  {SERR_PICMAPPING, "Picture mapping"},
  {SERR_NULL_POINTER, "Null pointer"},
  {SERR_LAYER_SIZE, "Layer size too big"},
  {SERR_LAYER_INDEX, "Layer index out of bounds"},
  {SERR_SPRITE_SIZE, "Sprite size too big"},
  {SERR_SPRITE_INDEX, "Sprite index out of bounds"},
  {SERR_SPRBANK_INDEX, "Sprite bank index out of bounds"},
  {SERR_TILE_SIZE, "Tile width not a multiple of 16"},
  {SERR_TILEBANK_INDEX, "Tile bank index out of bounds"},
  {SERR_TILE_INDEX, "Tile index out of bounds"},
  {SERR_TILE_POS, "Tile position out of bounds"},
  {SERR_TILEMAP_INDEX, "Tilemap index out of bounds"},
  {SERR_TILEMAP_FILE, "Tilemap file not found"},
  {SERR_PICTURE_SIZE, "Picture size too big"},
  {SERR_LOWLEVEL_LIB, "Can't open lowlevel library"},
  {SERR_AHI_LIB, "Can't open AHI library"},
  {SERR_AUDIOALLOC, "Can't allocate audio"},
  {SERR_SOUNDLOAD, "Can't load sound"},
  {SERR_SOUNDADD, "Can't add sound"},
  {SERR_SOUNDPLAY, "Can't play sound"},
  {SERR_NOMUSIC, "Music not found"},
  {SERR_SOUND_INDEX, "Sound index out of bounds"},
  {SERR_LOADMUSIC, "Can't load music"},
  {SERR_PLAYMUSIC, "Can't play music"},
  {SERR_MUSIC_INDEX, "Music index out of bounds"},
  {SERR_DRAWMOUSE, "Draw mouse error"},
  {SERR_BITMAP_SIZE, "Unsupported bitmap width or depth"},
  {SERR_ALLOCTIMER, "Can't allocate timer"},
  {SERR_AREASIZE, "Bad area size"},
  {SERR_COLORINDEX, "Bad color index"},
  {SERR_BAD_PORT, "Bad port number"},
  {SERR_BAD_PORTTYPE, "Undefined port type"},
  {SERR_BAD_KEYCODE, "Bad keycode number"},
  {SERR_IT_USED, "IT already used"},
  {SERR_IT_INDEX, "IT index out of bounds"},
  {SERR_NO_THREAD, "No thread available in the pool"},
  {SERR_FINDTASK, "Can't find task"},
  {SERR_NEWPROCESS, "Unable to start new thread"},
  {SERR_NO_PORT, "Can't create message port"},
  {SERR_NO_IOREQUEST, "Can't create IO request"},
  {SERR_NO_3DDRIVER, "No 3D driver available"},
  {SERR_NO_3DCONTEXT, "Can't create 3D context"},
  {SERR_NO_MAGGIE, "Maggie not available"},
  {SERR_TEX_INDEX, "Texture index out of bounds"},
  {SERR_TEX_ALLOC, "Can't allocate texture"},
  {SERR_LOCKHARDWARE, "Can't lock 3D hardware"},
  {SERR_DRAWTRIANGLE, "Failed to render triangle"},
  {SERR_ZBUFFER, "Z buffer not abailable"},
  {SERR_CAMERA_INDEX, "Camera index out of bounds"},
  {SERR_NO_CAMERA, "No camera at this index"},
  {SERR_ENTITY_INDEX, "Entity index out of bounds"},
  {SERR_NO_ENTITY, "No entity at this index"},
  {SERR_TERRAIN_SIZE, "Terrain size not supported"},
  {SERR_TEXTURE_SIZE, "Texture size not supported"},
  {SERR_ENTITY_SIZE, "Entity has to much vertices"},
  {SERR_NO_SOCKET, "Failed to create socket"},
  {SERR_BIND_SOCKET, "Failed to bind socket"},
  {SERR_RESOLVE_HOST, "Failed to resolve hostname"},
  {SERR_CONNECT_SOCKET, "Failed to connect socket"},
  {SERR_LISTEN_SOCKET, "Failed to listen to client"},
  {SERR_SELECT_SOCKET,  "Failed to select socket state"},
  {SERR_IOCTL_SOCKET,  "Failed to set ioctl socket state"},
  {SERR_ACCEPT_SOCKET, "Failed to accept client"},
  {SERR_SEND_SOCKET, "Socket send error"},
  {SERR_RECEIVE_SOCKET, "Socket receive error"},
  {SERR_NOT_SERVERSOCK, "Not a server socket"},
  {SERR_NOT_CLIENTSOCK, "Not a client socket"},
  {SERR_FILENOTFOUND, "File not found"},
  {SERR_NOSECTION, "Section not found"},
  {SERR_NOPARAMETER, "Parameter not found"},
  {SERR_BUFFERSIZE, "Buffer is too small"},
  {SERR_ENDOF_ERROR, "End mark"}
};

/** @var Last error */
LONG SAGE_last_error = SERR_NO_ERROR;

/**
 * Set the last error code
 * 
 * @param error Error code
 */
VOID SAGE_SetError(LONG error)
{
  SD(SAGE_DebugLog("Setting error %d", error);)
  SAGE_last_error = error;
}

/**
 * Get the last error code
 * 
 * @return Error code
 */
LONG SAGE_GetErrorCode()
{
  return SAGE_last_error;
}

/**
 * Get the last error string
 * 
 * @return Error string
 */
STRPTR SAGE_GetErrorString()
{
  LONG index = 0;

  while (SAGE_errors[index].error_code != SERR_ENDOF_ERROR) {
    if (SAGE_errors[index].error_code == SAGE_last_error) {
      return SAGE_errors[index].error_string;
    }
    index++;
  }
  return "Undefined error code !";
}

/**
 * Display error message
 */
VOID SAGE_DisplayError()
{
  LONG index = 0;

  while (SAGE_errors[index].error_code != SERR_ENDOF_ERROR) {
    if (SAGE_errors[index].error_code == SAGE_last_error) {
      SAGE_ErrorLog("(%d) %s", SAGE_errors[index].error_code, SAGE_errors[index].error_string);
      SAGE_last_error = SERR_NO_ERROR;
      return;
    }
    index++;
  }
  SAGE_ErrorLog("Undefined error code %d !", SAGE_last_error);
}
