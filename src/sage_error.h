/**
 * sage_error.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Errors management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.1 April 2020
 */

#ifndef _SAGE_ERROR_H_
#define _SAGE_ERROR_H_

#include <exec/types.h>

// Utilities should be in another file but this one is always included
#define SAGE_WORDTOBE(value)  ((value & 0xff00) >> 8) | ((value & 0xff) << 8)
#define SAGE_LONGTOBE(value)  ((value & 0xff) << 24) | ((value & 0xff00) << 8) | ((value & 0xff000000) >> 24) | ((value & 0xff0000) >> 8)

// Error constants
#define SERR_ENDOF_ERROR      -1L
#define SERR_NO_ERROR         0L
#define SERR_ERROR            1L
#define SERR_NULL_POINTER     2L
#define SERR_NO_MEMORY        3L
#define SERR_NOT_AVAILABLE    4L
#define SERR_NO_VIDEODEVICE   5L
#define SERR_NO_AUDIODEVICE   6L
#define SERR_NO_INPUTDEVICE   7L
#define SERR_NO_3DDEVICE      8L
#define SERR_NO_NETDEVICE     9L
// Library errors
#define SERR_INTUITION_LIB    10L
#define SERR_CYBERGFX_LIB     11L
#define SERR_LOWLEVEL_LIB     12L
#define SERR_AHI_LIB          13L
#define SERR_ALLOCTIMER       14L
#define SERR_DATATYPES_LIB    15L
#define SERR_OPENDEVICE       16L
#define SERR_WARP3D_LIB       17L
#define SERR_SOCKET_LIB       18L
// Bitmap errors
#define SERR_BITMAP_SIZE      20L
#define SERR_UNKNOWN_DEPTH    21L
#define SERR_BM_MAPPING       22L
#define SERR_BM_BLITFMT       23L
#define SERR_UNDEF_PIXFMT     24L
// Screen errors
#define SERR_NO_RESOLUTION    30L
#define SERR_NO_SCREEN        31L
#define SERR_NO_WINDOW        32L
#define SERR_NO_BITMAP        33L
#define SERR_LOCKBITMAP       34L
#define SERR_DRAWMOUSE        35L
#define SERR_AREASIZE         36L
#define SERR_COLORINDEX       37L
#define SERR_BITMAPFORMAT     38L
#define SERR_BITMAPTYPE       39L
#define SERR_PIXFORMAT        40L
#define SERR_VBLANK           41L
#define SERR_REFRESH          42L
#define SERR_DBLBUF           43L
#define SERR_NO_FONT          44L
#define SERR_NO_SCREENTIMER   45L
// Picture errors
#define SERR_OPENFILE         50L
#define SERR_READFILE         51L
#define SERR_WRITEFILE        52L
#define SERR_FILEFORMAT       53L
#define SERR_PIXELFORMAT      54L
#define SERR_PICMAPPING       55L
#define SERR_PICTURE_SIZE     56L
// Layer errors
#define SERR_LAYER_SIZE       58L
#define SERR_LAYER_INDEX      59L
// Sprite errors
#define SERR_SPRITE_SIZE      60L
#define SERR_SPRITE_INDEX     61L
#define SERR_SPRBANK_INDEX    62L
// Tile errors
#define SERR_TILE_SIZE        63L
#define SERR_TILEBANK_INDEX   64L
#define SERR_TILE_INDEX       65L
#define SERR_TILE_POS         66L
#define SERR_TILEMAP_INDEX    67L
#define SERR_TILEMAP_FILE     68L
// Audio errors
#define SERR_AUDIOALLOC       70L
#define SERR_SOUNDLOAD        71L
#define SERR_SOUNDADD         72L
#define SERR_SOUNDPLAY        73L
#define SERR_NOMUSIC          74L
#define SERR_SOUND_INDEX      75L
#define SERR_LOADMUSIC        76L
#define SERR_PLAYMUSIC        77L
#define SERR_MUSIC_INDEX      78L
// Input errors
#define SERR_BAD_PORT         80L
#define SERR_BAD_PORTTYPE     81L
#define SERR_BAD_KEYCODE      82L
// Interrupt errors
#define SERR_IT_USED          90L
#define SERR_IT_INDEX         91L
// Thread errors
#define SERR_NO_THREAD        92L
#define SERR_FINDTASK         93L
#define SERR_NEWPROCESS       94L
// Timer errors
#define SERR_NO_PORT          95L
#define SERR_NO_IOREQUEST     96L
// 3D errors
#define SERR_NO_3DDRIVER      100L
#define SERR_NO_3DCONTEXT     101L
#define SERR_TEX_INDEX        105L
#define SERR_TEX_ALLOC        106L
#define SERR_LOCKHARDWARE     107L
#define SERR_DRAWTRIANGLE     108L
#define SERR_CAMERA_INDEX     110L
#define SERR_NO_CAMERA        111L
#define SERR_ENTITY_INDEX     112L
#define SERR_NO_ENTITY        113L
#define SERR_TERRAIN_SIZE     114L
#define SERR_TEXTURE_SIZE     115L
#define SERR_ENTITY_SIZE      116L
// Network errors
#define SERR_NO_SOCKET        150L
#define SERR_BIND_SOCKET      151L
#define SERR_RESOLVE_HOST     152L
#define SERR_CONNECT_SOCKET   153L
#define SERR_LISTEN_SOCKET    154L
#define SERR_SELECT_SOCKET    155L
#define SERR_IOCTL_SOCKET     156L
#define SERR_ACCEPT_SOCKET    157L
#define SERR_SEND_SOCKET      158L
#define SERR_RECEIVE_SOCKET   159L
#define SERR_NOT_SERVERSOCK   160L
#define SERR_NOT_CLIENTSOCK   161L
// Configuration file errors
#define SERR_FILENOTFOUND     170L
#define SERR_NOSECTION        171L
#define SERR_NOPARAMETER      172L
#define SERR_BUFFERSIZE       173L

/** SAGE error */
typedef struct {
  /** Error code */
  LONG error_code;
  /** Error text */
  STRPTR error_string;
} SAGE_Error;

/** Set error */
VOID SAGE_SetError(LONG);

/** Get the error code */
LONG SAGE_GetErrorCode(VOID);

/** Get the error text */
STRPTR SAGE_GetErrorString(VOID);

/** Display the error */
VOID SAGE_DisplayError(VOID);

#endif
