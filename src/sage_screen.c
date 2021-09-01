/**
 * sage_screen.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Screen management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <string.h>

#include "sage_debug.h"
#include "sage_logger.h"
#include "sage_error.h"
#include "sage_memory.h"
#include "sage_vampire.h"
#include "sage_blitter.h"
#include "sage_interrupt.h"
#include "sage_context.h"
#include "sage_screen.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/diskfont.h>
#include <proto/cybergraphics.h>
#include <proto/lowlevel.h>

/** @var Blank mouse pointer */
UWORD SAGE_BlankPointer[16] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};    // In burst mode you need 4x64 bits for a blank sprite

/** @var System color map */
ULONG SystemColorMap[SSCR_MAXCOLORS * 3 + 2];

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Get the best screen mode ID for the required size
 *
 * @param width  Screen width
 * @param height Screen height
 * @param depth  Screen depth
 *
 * @return Display ID
 */
ULONG SAGE_GetBestdisplayID(LONG width, LONG height, LONG depth)
{
  ULONG display_id;

  SD(SAGE_InfoLog("Getting best display ID (%dx%dx%d)", width, height, depth));
  display_id = BestCModeIDTags(
    CYBRBIDTG_NominalWidth, width,
    CYBRBIDTG_NominalHeight, height,
    CYBRBIDTG_Depth, depth,
    TAG_DONE
  );
  if (display_id != INVALID_ID) {
    SD(SAGE_DebugLog("Found a suitable screenmode with ID 0x%X", display_id));
    if (!IsCyberModeID(display_id)) {
      SD(SAGE_DebugLog("This is not a CyberGfx mode !"));
      display_id = INVALID_ID;
    SD(} else {
      SAGE_DebugLog("This is a CyberGfx mode ID");
      SAGE_DebugLog("Width is %d", GetCyberIDAttr(CYBRIDATTR_WIDTH, display_id));
      SAGE_DebugLog("Height is %d", GetCyberIDAttr(CYBRIDATTR_HEIGHT, display_id));
      SAGE_DebugLog("Depth is %d", GetCyberIDAttr(CYBRIDATTR_DEPTH, display_id));
      SAGE_DebugLog("Bytes per pixel is %d", GetCyberIDAttr(CYBRIDATTR_BPPIX, display_id));
      SAGE_DumpPixelFormat(GetCyberIDAttr(CYBRIDATTR_PIXFMT, display_id));
    )
    }
  }
  return display_id;
}

/**
 * Open the system screen
 *
 * @param display_id Display ID
 * @param width      Screen width
 * @param height     Screen height
 * @param depth      Screen depth
 *
 * @return Screen structure pointer
 */
struct Screen * SAGE_OpenSystemScreen(ULONG display_id, LONG width, LONG height, LONG depth)
{
  struct Screen * system_screen;

  SD(SAGE_DebugLog("Opening system screen"));
  system_screen = OpenScreenTags(
    NULL,
    SA_Title, "SAGE Screen",
    SA_ShowTitle, FALSE,
    SA_Draggable, FALSE,
    SA_DisplayID, display_id,
    SA_Width, width,
    SA_Height, height,
    SA_Depth, depth,
    SA_Type, SCREENQUIET|CUSTOMSCREEN,
    TAG_DONE
  );
  return system_screen;
}

/**
 * Open the system window
 *
 * @param custom_screen Screen structure pointer
 * @param width         Window width
 * @param height        Window height
 *
 * @return Window structure pointer
 */
struct Window * SAGE_OpenSystemWindow(struct Screen * custom_screen, LONG width, LONG height)
{
  struct Window * system_window;

  SD(SAGE_DebugLog("Opening system window"));
  system_window = OpenWindowTags(
    NULL,
    WA_Title, NULL,
    WA_Flags, WFLG_ACTIVATE|WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_REPORTMOUSE,
    WA_IDCMP, IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE,
    WA_Left, 0,
    WA_Top, 0,
    WA_Width, width,
    WA_Height, height,
    WA_CustomScreen, custom_screen,
    TAG_DONE
  );
  return system_window;
}

/**
 * Release screen buffer resources
 *
 * @param custom_screen System screen structure
 * @param scr_buf       SAGE screen buffer structure
 */
VOID SAGE_ReleaseScreenBuffer(struct Screen * custom_screen, SAGE_ScreenBuffer * scr_buf)
{
  SD(SAGE_DebugLog("Release screen buffer"));
  if (scr_buf->safe_port) {
    while (GetMsg(scr_buf->safe_port) != NULL);
    DeleteMsgPort(scr_buf->safe_port);
    scr_buf->safe_port = NULL;
  }
  if (scr_buf->display_port) {
    while (GetMsg(scr_buf->display_port) != NULL);
    DeleteMsgPort(scr_buf->display_port);
    scr_buf->display_port = NULL;
  }
  if (scr_buf->wait_buffer) {
    scr_buf->wait_buffer->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
    scr_buf->wait_buffer->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = NULL;
    FreeScreenBuffer(custom_screen, scr_buf->wait_buffer);
    scr_buf->wait_buffer = NULL;
  }
  if (scr_buf->back_buffer) {
    scr_buf->back_buffer->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
    scr_buf->back_buffer->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = NULL;
    FreeScreenBuffer(custom_screen, scr_buf->back_buffer);
    scr_buf->back_buffer = NULL;
  }
  if (scr_buf->front_buffer) {
    scr_buf->front_buffer->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
    scr_buf->front_buffer->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = NULL;
    FreeScreenBuffer(custom_screen, scr_buf->front_buffer);
    scr_buf->front_buffer = NULL;
  }
}

/**
 * Setup screen buffer resources
 *
 * @param custom_screen System screen structure
 * @param scr_buf       SAGE screen buffer structure
 * @param triple        Activate triple buffer
 *
 * @return Operation success
 */
BOOL SAGE_SetupScreenBuffer(struct Screen * custom_screen, SAGE_ScreenBuffer * scr_buf, BOOL triple)
{
  SD(SAGE_DebugLog("Setup screen buffer (%s)", (triple ? "Triple":"Double")));
  scr_buf->safe_display = TRUE;
  scr_buf->safe_change = TRUE;
  scr_buf->safe_port = NULL;
  scr_buf->display_port = NULL;
  scr_buf->front_buffer = AllocScreenBuffer(custom_screen, NULL, SB_SCREEN_BITMAP);
  scr_buf->back_buffer = AllocScreenBuffer(custom_screen, NULL, 0L);
  if (triple) {
    scr_buf->wait_buffer = AllocScreenBuffer(custom_screen, NULL, 0L);
    if (scr_buf->wait_buffer == NULL) {
      SAGE_ReleaseScreenBuffer(custom_screen, scr_buf);
      return FALSE;
    }
  } else {
    scr_buf->wait_buffer = NULL;
  }
  if (scr_buf->front_buffer != NULL && scr_buf->back_buffer != NULL) {
    SD(SAGE_DebugLog("<SAGE_SetupScreenBuffer> Front screenbuffer bitmap"));
    SD(SAGE_DumpSystemBitmap(scr_buf->front_buffer->sb_BitMap));
    SD(SAGE_DebugLog("<SAGE_SetupScreenBuffer> Back screenbuffer bitmap"));
    SD(SAGE_DumpSystemBitmap(scr_buf->back_buffer->sb_BitMap));
    SD(if (triple) {
      SAGE_DebugLog("<SAGE_SetupScreenBuffer> Wait screenbuffer bitmap");
      SAGE_DumpSystemBitmap(scr_buf->wait_buffer->sb_BitMap);
    })
    scr_buf->safe_port = CreateMsgPort();
    scr_buf->display_port = CreateMsgPort();
    if (scr_buf->safe_port && scr_buf->display_port) {
      scr_buf->safe_sigbit = 1L << scr_buf->safe_port->mp_SigBit;
      scr_buf->display_sigbit = 1L << scr_buf->display_port->mp_SigBit;
      scr_buf->front_buffer->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = scr_buf->safe_port;
      scr_buf->front_buffer->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = scr_buf->display_port;
      scr_buf->back_buffer->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = scr_buf->safe_port;
      scr_buf->back_buffer->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = scr_buf->display_port;
      if (triple) {
        scr_buf->wait_buffer->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = scr_buf->safe_port;
        scr_buf->wait_buffer->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = scr_buf->display_port;
      }
      InitRastPort(&(scr_buf->work_rastport));
      scr_buf->work_rastport.BitMap = scr_buf->back_buffer->sb_BitMap;
      return TRUE;
    }
  }
  SAGE_ReleaseScreenBuffer(custom_screen, scr_buf);
  return FALSE;
}

/**
 * Get the screen bitmap address
 *
 * @return Bitmap address or NULL on error
 */
UBYTE * SAGE_GetScreenBitMapAddress(VOID)
{
  SAGE_Screen * screen;
  APTR cgx_handle = NULL;
  ULONG base_adr;

  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_ErrorLog("Screen is NULL, this is not normal !");
    SAGE_SetError(SERR_NO_SCREEN);
    return NULL;
  }
  if (GetCyberMapAttr(screen->system_screen->RastPort.BitMap, CYBRMATTR_ISCYBERGFX)) {
    cgx_handle = LockBitMapTags(
      screen->system_screen->RastPort.BitMap,
      LBMI_BASEADDRESS, &base_adr,
      TAG_DONE
    );
    if (cgx_handle != NULL) {
      UnLockBitMap(cgx_handle);
      return (UBYTE *) base_adr;
    } else {
      SAGE_ErrorLog("Lock bitmap has failed !");
      SAGE_SetError(SERR_LOCKBITMAP);
    }
  } else {
    SAGE_ErrorLog("Bitmap is not CGX !");
    SAGE_SetError(SERR_BITMAPFORMAT);
  }
  return NULL;
}

/**
 * Setup the screen frame buffers
 *
 * @param screen SAGE screen structure
 *
 * @return Operation success
 */
BOOL SAGE_SetupFrameBuffer(SAGE_Screen * screen)
{
  UBYTE * bitmap_adr;

  SD(SAGE_DebugLog("Setup screen frame buffers"));
  // To be sure our screen is installed
  WaitTOF();
  WaitTOF();
  if ((bitmap_adr = SAGE_GetScreenBitMapAddress()) == NULL) {
    return FALSE;
  }
  if ((screen->front_bitmap = SAGE_AllocBitmap(screen->width, screen->height, screen->depth, screen->pixformat, bitmap_adr)) == NULL) {
    return FALSE;
  }
  if (!SAGE_AllocateFastDrawBuffers(screen->front_bitmap)) {
    return FALSE;
  }
  SD(SAGE_DebugLog("<SAGE_SetupFrameBuffer> First screen bitmap adr 0x%X", bitmap_adr));
  SD(SAGE_DumpSystemBitmap(screen->system_screen->RastPort.BitMap));
  SD(SAGE_DumpBitmap(screen->front_bitmap));
  SAGE_ClearBitmap(screen->front_bitmap, 0, 0, screen->width, screen->height);
  /**
   *  /!\ WARNING /!\ : switch screen is needed here because we have to get
   *                    the physical bitmap address from the screen.rastport structure only
   */
  SD(SAGE_DebugLog("<SAGE_SetupFrameBuffer> Switch the screen buffers"));
  SAGE_RefreshScreen();
  // To be sure our screen is installed
  WaitTOF();
  WaitTOF();
  if ((bitmap_adr = SAGE_GetScreenBitMapAddress()) == NULL) {
    return FALSE;
  }
  if ((screen->front_bitmap = SAGE_AllocBitmap(screen->width, screen->height, screen->depth, screen->pixformat, bitmap_adr)) == NULL) {
    return FALSE;
  }
  if (!SAGE_AllocateFastDrawBuffers(screen->front_bitmap)) {
    return FALSE;
  }
  SD(SAGE_DebugLog("<SAGE_SetupFrameBuffer> Second screen bitmap adr 0x%X", bitmap_adr));
  SD(SAGE_DumpSystemBitmap(screen->system_screen->RastPort.BitMap));
  SD(SAGE_DumpBitmap(screen->front_bitmap));
  SAGE_ClearBitmap(screen->front_bitmap, 0, 0, screen->width, screen->height);
  // Triple buffer activated
  if (screen->flags & SSCR_TRIPLEBUF) {
    SD(SAGE_DebugLog("<SAGE_SetupFrameBuffer> Switch the screen buffers"));
    SAGE_RefreshScreen();
    WaitTOF();
    WaitTOF();
    if ((bitmap_adr = SAGE_GetScreenBitMapAddress()) == NULL) {
      return FALSE;
    }
    if ((screen->front_bitmap = SAGE_AllocBitmap(screen->width, screen->height, screen->depth, screen->pixformat, bitmap_adr)) == NULL) {
      return FALSE;
    }
    if (!SAGE_AllocateFastDrawBuffers(screen->front_bitmap)) {
      return FALSE;
    }
    SD(SAGE_DebugLog("<SAGE_SetupFrameBuffer> Third screen bitmap adr 0x%X", bitmap_adr));
    SD(SAGE_DumpSystemBitmap(screen->system_screen->RastPort.BitMap));
    SD(SAGE_DumpBitmap(screen->front_bitmap));
    SAGE_ClearBitmap(screen->front_bitmap, 0, 0, screen->width, screen->height);
  }
  return TRUE;
}

/**
 * Release the screen frame buffers
 *
 * @param screen SAGE screen structure
 */
VOID SAGE_ReleaseFrameBuffer(SAGE_Screen * screen)
{
  // Release SAGE bitmap
  if (screen->wait_bitmap != NULL) {
    SAGE_ReleaseBitmap(screen->wait_bitmap);
  }
  if (screen->back_bitmap != NULL) {
    SAGE_ReleaseBitmap(screen->back_bitmap);
  }
  if (screen->front_bitmap != NULL) {
    SAGE_ReleaseBitmap(screen->front_bitmap);
  }
}

/**
 * Check if the pixel format is supported
 *
 * @param pixformat Pixel format
 *
 * @return Format supported
 */
BOOL SAGE_IsSupportedPixFormat(ULONG pixformat)
{
  if (pixformat == PIXFMT_CLUT
   || pixformat == PIXFMT_RGB15
   || pixformat == PIXFMT_RGB16
   || pixformat == PIXFMT_RGB16PC
   || pixformat == PIXFMT_RGB24
   || pixformat == PIXFMT_BGR24
   || pixformat == PIXFMT_ARGB32
   || pixformat == PIXFMT_RGBA32
  ) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Open RTG screen and window and allocate the frame buffer
 *
 * @param width  Screen width
 * @param height Screen height
 * @param depth  Screen depth
 * @param flags  Screen flags
 *
 * @return Operation success
 */
BOOL SAGE_OpenScreen(LONG width, LONG height, LONG depth, LONG flags)
{
  ULONG display_id, pixformat;
  LONG display_width, display_height, display_depth;
  SAGE_Screen * screen;
  struct BitMap * bitmap;

  SD(SAGE_DebugLog("Opening a screen of %dx%dx%d", width, height, depth));
  // Check for video device
  if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return FALSE;
  }
  // Check for size constraint
  if (!SAGE_CheckSizeConstraint(width, depth)) {
    return FALSE;
  }
  // Setup screen
  screen = (SAGE_Screen *) SAGE_AllocMem(sizeof(SAGE_Screen));
  if (screen == NULL) {
    return FALSE;
  }
  screen->system_screen = NULL;
  screen->system_window = NULL;
  screen->system_font = NULL;
  screen->front_bitmap = NULL;
  screen->back_bitmap = NULL;
  screen->wait_bitmap = NULL;
  screen->drawing_mode = SSCR_TXTREPLACE;
  screen->event = NULL;
  screen->timer = NULL;
  screen->back_color = 0;
  if (depth == SBMP_DEPTH8) {
    screen->front_color = 1;
  } else if (depth == SBMP_DEPTH16) {
    screen->front_color = 0xFFFF;
  } else {
    screen->front_color = 0xFFFFFF;
  }
  screen->flags = flags;
  // Attach the screen to the context
  SageContext.SageVideo->screen = screen;
  // Find best screen mode
  if ((display_id = SAGE_GetBestdisplayID(width, height, depth)) == INVALID_ID) {
    SAGE_CloseScreen();
    SAGE_SetError(SERR_NO_RESOLUTION);
    return FALSE;
  }
  display_width = GetCyberIDAttr(CYBRIDATTR_WIDTH, display_id);
  display_height = GetCyberIDAttr(CYBRIDATTR_HEIGHT, display_id);
  display_depth = GetCyberIDAttr(CYBRIDATTR_BPPIX, display_id) * 8;
  SD(SAGE_DebugLog("Found best mode ID 0x%X (%dx%dx%d) for screen", display_id, display_width, display_height, display_depth));
  if (flags & SSCR_STRICTRES) {
    if (display_width != width || display_height != height || display_depth != depth) {
      SAGE_CloseScreen();
      SAGE_SetError(SERR_NO_RESOLUTION);
      return FALSE;
    }
  }
  // Open the screen
  if ((screen->system_screen = SAGE_OpenSystemScreen(display_id, width, height, depth)) == NULL) {
    SAGE_CloseScreen();
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  bitmap = screen->system_screen->RastPort.BitMap;
  // Check for CGX bitmap
  if (GetCyberMapAttr(bitmap, CYBRMATTR_ISCYBERGFX)) {
    screen->width = GetCyberMapAttr(bitmap, CYBRMATTR_WIDTH);
    screen->height = GetCyberMapAttr(bitmap, CYBRMATTR_HEIGHT);
    screen->depth = GetCyberMapAttr(bitmap, CYBRMATTR_DEPTH);
    screen->bpr = GetCyberMapAttr(bitmap, CYBRMATTR_XMOD);
    SD(SAGE_DebugLog("RTG screen size informations %dx%dx%d (%dBPR)", screen->width, screen->height, screen->depth, screen->bpr));
  } else {
    SAGE_CloseScreen();
    SAGE_SetError(SERR_BITMAPTYPE);
    return FALSE;
  }
  // Check for supported pixel format
  pixformat = GetCyberMapAttr(bitmap, CYBRMATTR_PIXFMT);
  if (!SAGE_IsSupportedPixFormat(pixformat)) {
    SAGE_ErrorLog("The pixel format %s is not supported yet !", SAGE_GetPixelFormatName(pixformat));
    SAGE_CloseScreen();
    SAGE_SetError(SERR_PIXFORMAT);
    return FALSE;
  }
  screen->pixformat = pixformat;
  // Open the window
  if ((screen->system_window = SAGE_OpenSystemWindow(screen->system_screen, screen->width, screen->height)) == NULL) {
    SAGE_CloseScreen();
    SAGE_SetError(SERR_NO_WINDOW);
    return FALSE;
  }
  // BOOL ModifyIDCMP( struct Window *, ULONG ); 
  // VOID ReportMouse( BOOL, struct Window * );
  // Setup the screen buffer
  if (!SAGE_SetupScreenBuffer(screen->system_screen, &(screen->screen_buffer), (screen->flags&SSCR_TRIPLEBUF))) {
    SAGE_CloseScreen();
    SAGE_SetError(SERR_DBLBUF);
    return FALSE;
  }
  // Allocate bitmap buffer
  if (!SAGE_SetupFrameBuffer(screen)) {
    SAGE_CloseScreen();
    return FALSE;
  }    
  // Allocate screen event structure
  if ((screen->event = SAGE_AllocEvent()) == NULL) {
    SAGE_CloseScreen();
    return FALSE;
  }
  // Set default clipping
  screen->clipping.left = 0;
  screen->clipping.top = 0;
  screen->clipping.right = width - 1;
  screen->clipping.bottom = height - 1;
  // Mouse status
  screen->hidden_mouse = FALSE;
  // Allocate a timer
  screen->timer = SAGE_AllocTimer();
  screen->max_fps = STIM_TICKS / 60;
  // Install a VBL handler
  SAGE_InstallVblInterrupt();
  screen->vertical_synchro = TRUE;
  // Framerate counter
  screen->frame_rate.enable = FALSE;
  // Everything is OK
  return TRUE;
}

/**
 * Get the screen structure
 *
 * @return Screen structure
 */
SAGE_Screen * SAGE_GetScreen()
{
  // Check for video device
  if (SageContext.SageVideo == NULL) {
    SAGE_SetError(SERR_NO_VIDEODEVICE);
    return NULL;
  }
  return SageContext.SageVideo->screen;
}

/**
 * Close Cybergraphics screen and window
 * and free the frame buffer
 *
 * @param screen SAGE_Screen pointer
 *
 * @return Operation success
 */
BOOL SAGE_CloseScreen()
{
  SAGE_Screen * screen;
  
  SAGE_InfoLog("Closing the screen");
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_ErrorLog("Screen is NULL, this is not normal !");
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  SAGE_EnableFrameCount(FALSE);
  SAGE_RemoveVblInterrupt();
  if (screen->timer != NULL) {
    SAGE_ReleaseTimer(screen->timer);
  }
  if (screen->event != NULL) {
    SAGE_ReleaseEvent(screen->event);
  }
  if (screen->system_font != NULL) {
    SD(SAGE_DebugLog("SAGE_CloseScreen::CloseFont"));
    CloseFont(screen->system_font);
  }
  if (screen->system_window != NULL) {
    SD(SAGE_DebugLog("SAGE_CloseScreen::CloseWindow"));
    CloseWindow(screen->system_window);
  }
  if (screen->system_screen != NULL) {
    SD(SAGE_DebugLog("SAGE_CloseScreen::CloseScreen"));
    SAGE_ReleaseFrameBuffer(screen);
    SAGE_ReleaseScreenBuffer(screen->system_screen, &(screen->screen_buffer));
    CloseScreen(screen->system_screen);
  }
  SAGE_FreeMem(screen);
  SageContext.SageVideo->screen = NULL;
  return TRUE;
}

/**
 * Clear the screen
 * 
 * @return Operation success
 */
BOOL SAGE_ClearScreen()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (screen->back_bitmap->depth == SBMP_DEPTH8) {
    SAGE_FastClearScreen(
      (ULONG)screen->back_bitmap->bitmap_buffer,
      (UWORD)screen->back_bitmap->height,
      (UWORD)screen->back_bitmap->width
    );
    return TRUE;
  } else if (screen->back_bitmap->depth == SBMP_DEPTH16) {
    SAGE_FastClearScreen(
      (ULONG)screen->back_bitmap->bitmap_buffer,
      (UWORD)screen->back_bitmap->height,
      (UWORD)(screen->back_bitmap->width * 2)
    );
    return TRUE;
  } else if (screen->back_bitmap->depth == SBMP_DEPTH24) {
    SAGE_FastClearScreen(
      (ULONG)screen->back_bitmap->bitmap_buffer,
      (UWORD)screen->back_bitmap->height,
      (UWORD)(screen->back_bitmap->width * 3)
    );
    return TRUE;
  } else if (screen->back_bitmap->depth == SBMP_DEPTH32) {
    SAGE_FastClearScreen(
      (ULONG)screen->back_bitmap->bitmap_buffer,
      (UWORD)screen->back_bitmap->height,
      (UWORD)(screen->back_bitmap->width * 4)
    );
    return TRUE;
  }
  SAGE_SetError(SERR_PIXFORMAT);
  return FALSE;
}

/**
 * Set the screen clipping
 * 
 * @param left   Clip left
 * @param top    Clip top
 * @param width  Clip width
 * @param height Clip height
 * 
 * @return Operation success
 */
BOOL SAGE_SetScreenClip(ULONG left, ULONG top, ULONG width, ULONG height)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if ((left + width) > screen->width) {
    SAGE_SetError(SERR_AREASIZE);
    return FALSE;
  }
  if ((top + height) > screen->height) {
    SAGE_SetError(SERR_AREASIZE);
    return FALSE;
  }
  screen->clipping.left = (LONG) left;
  screen->clipping.top = (LONG) top;
  screen->clipping.right = (LONG) (left + width) - 1;
  screen->clipping.bottom = (LONG) (top + height) - 1;
  return TRUE;
}

/**
 * Clear the screen view (clipping zone)
 * 
 * @return Operation success
 */
BOOL SAGE_ClearView()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  return SAGE_ClearBitmap(
    screen->back_bitmap,
    screen->clipping.left,
    screen->clipping.top,
    (screen->clipping.right - screen->clipping.left) + 1,
    (screen->clipping.bottom - screen->clipping.top) + 1
  );
}

/**
 * Clear a screen area
 * 
 * @param left   Area left
 * @param top    Area top
 * @param width  Area width
 * @param height Area height
 * 
 * @return Operation success
 */
BOOL SAGE_ClearArea(ULONG left, ULONG top, ULONG width, ULONG height)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if ((left + width) == 0 || (left + width) > screen->width) {
    SAGE_SetError(SERR_AREASIZE);
    return FALSE;
  }
  if ((top + height) == 0 || (top + height) > screen->height) {
    SAGE_SetError(SERR_AREASIZE);
    return FALSE;
  }
  return SAGE_ClearBitmap(screen->back_bitmap, left, top, width, height);
}

/**
 * Activate or deactivate the vertical synchronisation
 *
 * @param synchro Activate vertical synchro
 *
 * @return Operation success
 */
BOOL SAGE_VerticalSynchro(BOOL synchro)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  screen->vertical_synchro = synchro;
  return TRUE;
}

/**
 * Set the maximum fps limit
 *
 * @param maxfps Maximum FPS limit
 *
 * @return Operation success
 */ 
BOOL SAGE_MaximumFPS(ULONG maxfps)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (screen->timer == NULL) {
    SAGE_SetError(SERR_NO_SCREENTIMER);
    return FALSE;
  }
  screen->max_fps = STIM_TICKS / maxfps;
  return TRUE;
}

/**
 * Refresh the screen buffers
 * 
 * @return Operation success
 */
BOOL SAGE_RefreshScreen()
{
  struct ScreenBuffer * temp_screenbuffer;
  SAGE_Bitmap * temp_bitmap;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  // Wait for intuition display message that say it's ready for change
  if (!screen->screen_buffer.safe_change) {
    Wait(screen->screen_buffer.display_sigbit);
    while (GetMsg(screen->screen_buffer.display_port) != NULL);
    screen->screen_buffer.safe_change = TRUE;
  }
  // Switch to new screen buffer
  if (ChangeScreenBuffer(screen->system_screen, screen->screen_buffer.back_buffer)) {
    screen->screen_buffer.safe_change = FALSE;
    screen->screen_buffer.safe_display = FALSE;
    // Switch screen buffer & frame buffer
    if (screen->flags & SSCR_TRIPLEBUF) {
      temp_screenbuffer = screen->screen_buffer.front_buffer;
      screen->screen_buffer.front_buffer = screen->screen_buffer.back_buffer;
      screen->screen_buffer.back_buffer = screen->screen_buffer.wait_buffer;
      screen->screen_buffer.wait_buffer = temp_screenbuffer;
      screen->screen_buffer.work_rastport.BitMap = screen->screen_buffer.back_buffer->sb_BitMap;
      temp_bitmap = screen->front_bitmap;
      screen->front_bitmap = screen->back_bitmap;
      screen->back_bitmap = screen->wait_bitmap;
      screen->wait_bitmap = temp_bitmap;
    } else {
      temp_screenbuffer = screen->screen_buffer.front_buffer;
      screen->screen_buffer.front_buffer = screen->screen_buffer.back_buffer;
      screen->screen_buffer.back_buffer = temp_screenbuffer;
      screen->screen_buffer.work_rastport.BitMap = screen->screen_buffer.back_buffer->sb_BitMap;
      temp_bitmap = screen->front_bitmap;
      screen->front_bitmap = screen->back_bitmap;
      screen->back_bitmap = temp_bitmap;
    }
  } else {
    SAGE_WarningLog("System failed to change the screen buffer !");
  }
  // Wait for intuition message that say the screen buffer has changed
  if (!screen->screen_buffer.safe_display) {
    Wait(screen->screen_buffer.safe_sigbit);
    while (GetMsg(screen->screen_buffer.safe_port) != NULL);
    screen->screen_buffer.safe_display = TRUE;
  }
  // Increment the frame counter
  screen->frame_rate.frame_count++;
  // Wait for the VBL if synchro is active, else use the max fps limit
  if (screen->vertical_synchro) {
    WaitTOF();
  } else if (screen->timer != NULL) {
    screen->frame_time = SAGE_ElapsedTime(screen->timer);
    if (screen->frame_time < screen->max_fps) {
      SAGE_Delay(screen->timer, screen->max_fps - screen->frame_time);
      SAGE_ElapsedTime(screen->timer);
    }
  }
  return TRUE;
}

/**
 * Wait for the screen vertical blank
 * and increment the frame counter
 * 
 * @return Operation success
 */
BOOL SAGE_WaitVBlank()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  WaitTOF();
  return TRUE;
}

/**
 * Poll a screen event (non blocking)
 *
 * @return SAGE Event structure pointer or NULL if no event
 */
SAGE_Event * SAGE_GetEvent()
{
  struct IntuiMessage * message = NULL;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (screen->system_window != NULL) {
    message = GETIMSG(screen->system_window);
    if (message) {
      switch (message->Class) {
        case IDCMP_RAWKEY:
          if (message->Code < 128) {
            screen->event->type = SEVT_KEYDOWN;
            screen->event->code = message->Code;
          } else {
            screen->event->type = SEVT_KEYUP;
            screen->event->code = message->Code - 128;
          }
          screen->event->mousex = message->MouseX;
          screen->event->mousey = message->MouseY;
          break;
        case IDCMP_MOUSEBUTTONS:
          screen->event->type = SEVT_MOUSEBT;
          screen->event->code = message->Code;
          screen->event->mousex = message->MouseX;
          screen->event->mousey = message->MouseY;
          break;
        case IDCMP_MOUSEMOVE:
          screen->event->type = SEVT_MOUSEMV;
          screen->event->code = message->Code;
          screen->event->mousex = message->MouseX;
          screen->event->mousey = message->MouseY;
          break;
        default:
          screen->event->type = SEVT_UNKNOWN;
          screen->event->code = message->Code;
          screen->event->mousex = message->MouseX;
          screen->event->mousey = message->MouseY;
          break;
      }
      ReplyMsg(&message->ExecMessage);
      return screen->event;
    }
  }
  return NULL;
}

/**
 * Tell if the screen is the one in the front
 *
 * @return Screen is in front
 */
BOOL SAGE_IsFrontMostScreen()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (IntuitionBase->FirstScreen == screen->system_screen) {
    return TRUE;
  }
  // Small pause if we are not the front most screen
  Delay(2);
  return FALSE;
}

/**
 * Get the screen front buffer address
 * 
 * @return Front bitmap
 */
SAGE_Bitmap * SAGE_GetFrontBitmap()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return NULL;
  }
  return screen->front_bitmap;
}

/**
 * Get the screen front buffer address
 * 
 * @return Back bitmap
 */
SAGE_Bitmap * SAGE_GetBackBitmap()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return NULL;
  }
  return screen->back_bitmap;
}

/**
 * Get the screen back buffer address
 * 
 * @return Back buffer address
 */
ULONG SAGE_GetPixelFormat()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return PIXFMT_UNDEFINED;
  }
  return screen->pixformat;
}

/**
 * Refresh the screen colors
 * 
 * @param start   Color start
 * @param nbcolor Number of color to set
 * 
 * @return Operation success
 */
BOOL SAGE_RefreshColors(UWORD start, UWORD nbcolor)
{
  UWORD index, color;
  ULONG red, green, blue;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if ((start+nbcolor) > SSCR_MAXCOLORS) {
    SAGE_SetError(SERR_COLORINDEX);
    return FALSE;
  }
  index = 0;
  SystemColorMap[index++] = nbcolor << 16 + start;
  for (color = start;color < (start + nbcolor);color++) {
    red = (screen->color_map[color] >> 16) & SSCR_COLORMASK;
    SystemColorMap[index++] = red * 0x01010101;
    green = (screen->color_map[color] >> 8) & SSCR_COLORMASK;
    SystemColorMap[index++] = green * 0x01010101;
    blue = screen->color_map[color] & SSCR_COLORMASK;
    SystemColorMap[index++] = blue * 0x01010101;
  }
  SystemColorMap[index] = 0;
  LoadRGB32(&(screen->system_screen->ViewPort), (APTR)SystemColorMap);
  return TRUE;
}

/**
 * Set a screen color
 *
 * @param index Color index
 * @param color Color ARGB value
 *
 * @return Operation success
 */
BOOL SAGE_SetColor(UWORD index, ULONG color)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (index < SSCR_MAXCOLORS) {
    screen->color_map[index] = color;
    return TRUE;
  }
  SAGE_SetError(SERR_COLORINDEX);
  return FALSE;
}

/**
 * Get a the screen color
 *
 * @param index Color index
 *
 * @return Color ARGB value
 */
ULONG SAGE_GetColor(UWORD index)
{
  SAGE_Screen * screen;

  screen = SAGE_GetScreen();
  if (screen != NULL && index < SSCR_MAXCOLORS) {
    return screen->color_map[index];
  }
  return 0;
}

/**
 * Set multiple screen colors
 * 
 * @param colors  Colors array in ARGB format
 * @param start   First color index
 * @param nbcolor Number of colors to load
 *
 * @return Operation success
 */
BOOL SAGE_SetColorMap(ULONG * colors, UWORD start, UWORD nbcolor)
{
  UWORD index;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (colors == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if ((start+nbcolor) > SSCR_MAXCOLORS) {
    SAGE_SetError(SERR_COLORINDEX);
    return FALSE;
  }
  for (index = 0;index < nbcolor;index++) {
    screen->color_map[index + start] = colors[index];
  }
  return TRUE;
}

/**
 * Set the back and front color
 * 
 * @param back  Back color
 * @param front Front color
 *
 * @return Operation success
 */
BOOL SAGE_SetDrawColor(ULONG back, ULONG front)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  screen->back_color = back;
  screen->front_color = front;
  return TRUE;
}

/**
 * Set the screen color map from a file color map
 *
 * @param filepath Path to the file
 * @param start    First color index
 * @param nbcolor  Number of colors to load
 * @param format   Colors format
 *
 * @return Operation success
 */
BOOL SAGE_LoadFileColorMap(STRPTR filepath, UWORD start, UWORD nbcolor, UWORD format)
{
  BPTR file_handle;
  UBYTE alpha, red, green, blue;
  ULONG red32, green32, blue32, color;
  UWORD index;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if ((start+nbcolor) > SSCR_MAXCOLORS) {
    SAGE_SetError(SERR_COLORINDEX);
    return FALSE;
  }
  file_handle = Open(filepath, MODE_OLDFILE);
  if (file_handle != 0) {
    if (format == SSCR_RGB32COLOR) {
      if (Read(file_handle, &color, sizeof(color)) != sizeof(color)) {
        Close(file_handle);
        SAGE_SetError(SERR_READFILE);
        return FALSE;
      }
    }
    for (index = start;index < nbcolor; index++) {
      if (format == SSCR_RGBCOLOR) {
        if (Read(file_handle, &red, sizeof(red)) == sizeof(red)
            && Read(file_handle, &green, sizeof(green)) == sizeof(green)
            && Read(file_handle, &blue, sizeof(blue)) == sizeof(blue))
        {
          color = (red << 16) + (green << 8) + blue;
          screen->color_map[index] = color;
        } else {
          Close(file_handle);
          SAGE_SetError(SERR_READFILE);
          return FALSE;
        }
      } else if (format == SSCR_ARGBCOLOR) {
        if (Read(file_handle, &alpha, sizeof(alpha)) == sizeof(alpha)
            && Read(file_handle, &red, sizeof(red)) == sizeof(red)
            && Read(file_handle, &green, sizeof(green)) == sizeof(green)
            && Read(file_handle, &blue, sizeof(blue)) == sizeof(blue))
        {
          color = (alpha << 24) + (red << 16) + (green << 8) + blue;
          screen->color_map[index] = color;
        } else {
          Close(file_handle);
          SAGE_SetError(SERR_READFILE);
          return FALSE;
        }
      } else if (format == SSCR_RGB32COLOR) {
        if (Read(file_handle, &red32, sizeof(red32)) == sizeof(red32)
            && Read(file_handle, &green32, sizeof(green32)) == sizeof(green32)
            && Read(file_handle, &blue32, sizeof(blue32)) == sizeof(blue32))
        {
          color = ((red32 & SSCR_COLORMASK) << 16)
              + ((green32 & SSCR_COLORMASK) << 8)
              + (blue32 & SSCR_COLORMASK);
          screen->color_map[index] = color;
        } else {
          Close(file_handle);
          SAGE_SetError(SERR_READFILE);
          return FALSE;
        }
      }
    }
    Close(file_handle);
    return TRUE;
  }
  SAGE_SetError(SERR_OPENFILE);
  return FALSE;
}

/**
 * Hide the mouse pointer
 *
 * @return Operation success
 */
BOOL SAGE_HideMouse()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (screen->system_window != NULL) {
    SetPointer(screen->system_window, SAGE_BlankPointer, 0, 16, 0, 0);
    ReportMouse(FALSE, screen->system_window);
  }
  screen->hidden_mouse = TRUE;
  return TRUE;
}

/**
 * Show the mouse pointer
 *
 * @return Operation success
 */
BOOL SAGE_ShowMouse()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (screen->system_window != NULL) {
    ReportMouse(TRUE, screen->system_window);
    ClearPointer(screen->system_window);
  }
  screen->hidden_mouse = FALSE;
  return TRUE;
}

/**
 * Set a new cursor for the mouse
 *
 * @param cursor   Cursor definition (array of words)
 * @param height   Cursor height (width is always 16)
 * @param x_action X action point of the cursor
 * @param y_action Y action point of the cursor
 *
 * @return Operation success
 */
BOOL SAGE_SetMouseCursor(UWORD * cursor, WORD height, WORD x_action, WORD y_action)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (!screen->hidden_mouse && screen->system_window != NULL) {
    SetPointer(screen->system_window, cursor, 16, height, x_action, y_action);
  }
  return TRUE;
}

/**
 * Reset a custom mouse
 *
 * @return Operation success
 */
BOOL SAGE_ResetMouse()
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (!screen->hidden_mouse && screen->system_window != NULL) {
    ClearPointer(screen->system_window);
  }
  return TRUE;
}

/** Track mouse movement */
BOOL SAGE_TrackMouse(BOOL flag)
{
  return TRUE;
}

/**
 * Set the screen font
 * 
 * @param name Font name
 * @param size Font size
 * 
 * @return Operation success
 */
BOOL SAGE_SetFont(STRPTR name, UWORD size)
{
  struct TextAttr ta;
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (screen->system_font != NULL) {
    CloseFont(screen->system_font);
  }
  ta.ta_Name = name;
  ta.ta_YSize = size;
  ta.ta_Flags = FPB_DISKFONT | FPF_DESIGNED;
  ta.ta_Style = FS_NORMAL;
  screen->system_font = OpenDiskFont(&ta);
  if (screen->system_font != NULL) {
    SetFont(&(screen->screen_buffer.work_rastport), screen->system_font);
    return TRUE;
  }
  SAGE_SetError(SERR_NO_FONT);
  return FALSE;
}

/**
 * Set the front and background color
 * 
 * @param frontpen Front pen color index
 * @param backpen  Back pen color index
 * 
 * @return Operation success
 */
BOOL SAGE_SetTextColor(UBYTE frontpen, UBYTE backpen)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  SetAPen(&(screen->screen_buffer.work_rastport), frontpen);
  SetBPen(&(screen->screen_buffer.work_rastport), backpen);
  return TRUE;
}

/**
 * Set the text drawing mode
 * 
 * @param mode Drawing mode
 * 
 * @return Operation success
 */
BOOL SAGE_SetDrawingMode(UBYTE mode)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  screen->drawing_mode = mode;
  return TRUE;
}

/**
 * Print a text on a screen position
 * 
 * @param text Text string
 * @param posx Text X position
 * @param posy Text Y position
 * 
 * @return Operation success
 */
BOOL SAGE_PrintText(STRPTR text, UWORD posx, UWORD posy)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  Move(&(screen->screen_buffer.work_rastport), posx, posy);
  SetDrMd(&(screen->screen_buffer.work_rastport), screen->drawing_mode);
  Text(&(screen->screen_buffer.work_rastport), text, strlen(text));
  return TRUE;
}

/**
 * Print a text directly on a screen position
 * 
 * @param text Text string
 * @param posx Text X position
 * @param posy Text Y position
 * 
 * @return Operation success
 */
BOOL SAGE_PrintDirectText(STRPTR text, UWORD posx, UWORD posy)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  Move(&(screen->system_screen->RastPort), posx, posy);
  SetDrMd(&(screen->system_screen->RastPort), screen->drawing_mode);
  Text(&(screen->system_screen->RastPort), text, strlen(text));
  return TRUE;
}

/**
 * Frame rate interrupt
 */
__asm __interrupt __saveds VOID FpsHandler(
  register __a5 APTR UserData
)
{
  SAGE_FpsCounter * counter;
  
  counter = (SAGE_FpsCounter *) UserData;
  counter->fps = counter->frame_count;
  counter->frame_count = 0;
}

/**
 * Enable or disable the frame counter
 *
 * @param enable Frame counter enable
 *
 * @return Operation success
 */
BOOL SAGE_EnableFrameCount(BOOL enable)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return FALSE;
  }
  if (enable) {
    if (!screen->frame_rate.enable) {
      if (SAGE_AddInterruptionHandler(SSCR_FPS_INTERRUPT, FpsHandler, (APTR) &(screen->frame_rate))) {
        screen->frame_rate.enable = TRUE;
        screen->frame_rate.frame_count = 0;
        return SAGE_StartInterruption(SSCR_FPS_INTERRUPT, SINT_ONE_SECOND);
      }
    }
  } else {
    if (screen->frame_rate.enable) {
      if (SAGE_StopInterruption(SSCR_FPS_INTERRUPT) && SAGE_RemoveInterruptionHandler(SSCR_FPS_INTERRUPT)) {
        screen->frame_rate.enable = FALSE;
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Get the average frame per second
 *
 * @return Frame per second
 */
UWORD SAGE_GetFps(VOID)
{
  SAGE_Screen * screen;
  
  screen = SAGE_GetScreen();
  if (screen == NULL) {
    SAGE_SetError(SERR_NO_SCREEN);
    return 0;
  }
  return screen->frame_rate.fps;
}
