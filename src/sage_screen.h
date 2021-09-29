/**
 * sage_screen.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Screen management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#ifndef _SAGE_SCREEN_H_
#define _SAGE_SCREEN_H_

#include <exec/exec.h>
#include <graphics/gfx.h>
#include <intuition/intuitionbase.h>

#include "sage_event.h"
#include "sage_bitmap.h"
#include "sage_timer.h"
#include "sage_vblint.h"

// Colors constants
#define SSCR_MAXCOLORS        256
#define SSCR_COLORMASK        0xFF
#define SSCR_RGBCOLOR         1
#define SSCR_ARGBCOLOR        2
#define SSCR_RGB32COLOR       3

#define SSCR_MAP32TO15(color) ((color&0xF80000)>>9)+((color&0xF800)>>6)+((color&0xF8)>>3)
#define SSCR_MAP32TO16(color) ((color&0xF80000)>>8)+((color&0xFC00)>>5)+((color&0xF8)>>3)

// Screen flags constants
#define SSCR_DOUBLEBUF        0
#define SSCR_TRIPLEBUF        1
#define SSCR_STRICTRES        2
#define SSCR_NOWINDOWEVT      4
#define SSCR_TRACKMOUSE       8

// Text drawing mode
#define SSCR_TXTTRANSP        JAM1
#define SSCR_TXTREPLACE       JAM2

// Frame rate interrupt
#define SSCR_FPS_INTERRUPT    0

/** SAGE double buffer structure */
typedef struct {
  BOOL safe_display, safe_change;
  struct MsgPort * display_port, * safe_port;
  ULONG safe_sigbit, display_sigbit;
  struct ScreenBuffer * front_buffer, * back_buffer, * wait_buffer;
  struct RastPort work_rastport;
} SAGE_ScreenBuffer;

/** SAGE clipping structure */
typedef struct {
  /** Top/left corner */
  LONG left, top;
  /** Bottom/right corner */
  LONG right, bottom;
} SAGE_Clipping;

/** FPS counter */
typedef struct {
  BOOL enable;
  UWORD fps;
  ULONG frame_count;
} SAGE_FpsCounter;

/** SAGE screen structure */
typedef struct {
  /** Amiga screen */
  struct Screen * system_screen;
  /** Amiga window */
  struct Window * system_window;
  /** Amiga font */
  struct TextFont * system_font;
  /** Event container */
  SAGE_Event * event;
  /** Screen colors */
  ULONG color_map[SSCR_MAXCOLORS], back_color, front_color;
  /** Screen dimension & bytes per row */
  LONG width, height, depth, bpr;
  /** Pixel format */
  ULONG pixformat;
  /** Screen flags */
  LONG flags;
  /** Screen clipping */
  SAGE_Clipping clipping;
  /** Screen buffer management */
  SAGE_ScreenBuffer screen_buffer;
  /** Screen bitmaps */
  SAGE_Bitmap * front_bitmap, * back_bitmap, * wait_bitmap;
  /** Drawing mode and text color */
  UBYTE drawing_mode, frontpen, backpen;
  /** Mouse status */
  BOOL hidden_mouse;
  /** Timer instance */
  SAGE_Timer * timer;
  /** Enable vertical synchro */
  BOOL vertical_synchro;
  /** Maximum fps and frame time */
  ULONG max_fps, frame_time;
  /** Framerate counter */
  SAGE_FpsCounter frame_rate;
} SAGE_Screen;

/** Check supported pixel format */
BOOL SAGE_IsSupportedPixFormat(ULONG);

/** Open SAGE screen */
BOOL SAGE_OpenScreen(LONG, LONG, LONG, LONG);

/** Get the screen structure */
SAGE_Screen * SAGE_GetScreen(VOID);

/** Close SAGE screen */
BOOL SAGE_CloseScreen(VOID);

/** Clear full screen */
BOOL SAGE_ClearScreen(VOID);

/** Set screen clipping */
BOOL SAGE_SetScreenClip(ULONG, ULONG, ULONG, ULONG);

/** Clear the active view clipping */
BOOL SAGE_ClearView(VOID);

/** Clear only a part of the screen */
BOOL SAGE_ClearArea(ULONG, ULONG, ULONG, ULONG);

/** Enable vertical synchro */
BOOL SAGE_VerticalSynchro(BOOL);

/** Set the maximum FPS */
BOOL SAGE_MaximumFPS(ULONG);

/** Switch the screen buffers */
BOOL SAGE_RefreshScreen(VOID);

/** Wait for vertical blank */
BOOL SAGE_WaitVBlank(VOID);

/** Get the last intuition event */
SAGE_Event * SAGE_GetEvent(VOID);

/** Check if our screen is in front */
BOOL SAGE_IsFrontMostScreen(VOID);

/** Get the screen front bitmap */
SAGE_Bitmap * SAGE_GetFrontBitmap(VOID);

/** Get the screen back bitmap */
SAGE_Bitmap * SAGE_GetBackBitmap(VOID);

/** Get the screen pixel format */
ULONG SAGE_GetPixelFormat(VOID);

/** Refresh the screen colors */
BOOL SAGE_RefreshColors(UWORD, UWORD);

/** Set a screen color */
BOOL SAGE_SetColor(UWORD, ULONG);

/** Set multiple screen colors */
BOOL SAGE_SetColorMap(ULONG *, UWORD, UWORD);

/** Get a color from an index */
ULONG SAGE_GetColor(UWORD);

/** Remap a 32bits color into screen pixel color format */
ULONG SAGE_RemapColor(ULONG);

/** Set the drawing colors */
BOOL SAGE_SetDrawColor(ULONG, ULONG);

/** Load a color map from a file */
BOOL SAGE_LoadFileColorMap(STRPTR, UWORD, UWORD, UWORD);

/** Hide the screen mouse */
BOOL SAGE_HideMouse(VOID);

/** Show the screen mouse */
BOOL SAGE_ShowMouse(VOID);

/** Set a new cursor for the mouse */
BOOL SAGE_SetMouseCursor(UWORD *, WORD, WORD, WORD);

/** Reset the mouse cursor */
BOOL SAGE_ResetMouse(VOID);

/** Enable/disable the mouse movement tracking */
BOOL SAGE_TrackMouse(BOOL);

/** Set the screen text font */
BOOL SAGE_SetFont(STRPTR, UWORD);

/** Set the text colors */
BOOL SAGE_SetTextColor(UBYTE, UBYTE);

/** Set the text drawing mode */
BOOL SAGE_SetDrawingMode(UBYTE);

/** Print a text on screen */
BOOL SAGE_PrintText(STRPTR, UWORD, UWORD);

/** Print a text directly on screen */
BOOL SAGE_PrintDirectText(STRPTR, UWORD, UWORD);

/** Enable frame rate counter */
BOOL SAGE_EnableFrameCount(BOOL);

/** Get frame rate */
UWORD SAGE_GetFps(VOID);

#endif
