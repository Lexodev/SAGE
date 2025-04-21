/**
 * video_video.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test video module initialization
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

void main(void)
{
  SAGE_VideoModeList *video_modes;
  UWORD cgx_mode;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library VIDEO test (VIDEO) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_VIDEO)) {
    SAGE_AppliLog("Initialization successfull");
    video_modes = SAGE_GetVideoModes();
    if (video_modes == NULL) {
      SAGE_AppliLog("No video mode available");
    } else {
      SAGE_AppliLog("Total available video modes %d", video_modes->nb_modes);
      for (cgx_mode = 0; cgx_mode < video_modes->nb_modes;cgx_mode++) {
        SAGE_AppliLog(
          "Video mode 0x%X is %s (%dx%dx%d)",
          video_modes->modes[cgx_mode].mode_id,
          video_modes->modes[cgx_mode].name,
          video_modes->modes[cgx_mode].width,
          video_modes->modes[cgx_mode].height,
          video_modes->modes[cgx_mode].depth
        );
      }
    }
  } else {
    SAGE_AppliLog("Initialization failed");
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
