/**
 * video_draw.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test drawing primitives
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include <stdio.h>

#include "/src/sage.h"

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          8L

LONG coords[SCREEN_HEIGHT+2];

void dump_coords(STRPTR item, LONG * crd, ULONG nb)
{
  LONG index;
  
  printf("** Dump '%s' line coords (%d) **\n", item, nb);
  index = 0;
  nb++;
  while (nb--) {
    printf("- point %d = %d\n", index, crd[index]);
    index++;
  }
  printf("***************************\n", nb);
}

void test_coords(void)
{
  /*printf("** Vertical line coords 100,10 -> 100,84 **\n");
  SAGE_FastLeftEdgeCalculation(coords,100,10,100,84);
  dump_coords(coords,84-10);
  printf("** Horizontal line + coords 10,50 -> 104,50 **\n");
  SAGE_FastLeftEdgeCalculation(coords,10,50,104,50);
  dump_coords(coords,50-50);
  printf("** Horizontal line - coords 115,50 -> 10,50 **\n");
  SAGE_FastLeftEdgeCalculation(coords,115,50,10,50);
  dump_coords(coords,50-50);*/
  /*printf("** Diagonal line + coords 10,10 -> 96,96 **\n");
  SAGE_FastLeftEdgeCalculation(coords,10,10,96,96);
  dump_coords(coords,96-10);
  printf("** Diagonal line - coords 96,10 -> 10,96 **\n");
  SAGE_FastLeftEdgeCalculation(coords,96,10,10,96);
  dump_coords(coords,96-10);*/
  /*printf("** Soft slope line + coords 80,10 -> 250,98 **\n");
  SAGE_FastLeftEdgeCalculation(coords,80,10,250,98);
  dump_coords("left edge",coords,98-10);
  SAGE_DumpLineCoords(80,10,250,98);
  printf("** Soft slope line - coords 250,10 -> 80,98 **\n");
  SAGE_FastLeftEdgeCalculation(coords,250,10,80,98);
  dump_coords("left edge",coords,98-10);
  SAGE_DumpLineCoords(250,10,80,98);*/
  /*printf("** Hard slope line + coords 10,10 -> 60,93 **\n");
  SAGE_FastLeftEdgeCalculation(coords,10,10,60,93);
  dump_coords("left edge",coords,93-10);
  printf("** Hard slope line - coords 60,10 -> 10,93 **\n");
  SAGE_FastLeftEdgeCalculation(coords,60,10,10,93);
  dump_coords("left edge",coords,93-10);*/

  /*printf("** Soft slope line + coords 80,10 -> 250,98 **\n");
  SAGE_DumpLineCoords(80,10,250,37);
  SAGE_FastLeftEdgeCalculation(coords,80,10,250,37);
  dump_coords("left edge",coords,37-10);
  SAGE_FastRightEdgeCalculation(coords,80,10,250,37);
  dump_coords("right edge",coords,37-10);*/
  /*printf("** Soft slope line - coords 250,10 -> 80,44 **\n");
  SAGE_DumpLineCoords(250,10,80,44);
  SAGE_FastLeftEdgeCalculation(coords,250,10,80,44);
  dump_coords("left edge",coords,44-10);
  SAGE_FastRightEdgeCalculation(coords,250,10,80,44);
  dump_coords("right edge",coords,44-10);*/

}

void test_lines8(void)
{
  SAGE_DrawLine(0,100,319,100,1);   // Horizontal x1 < x2 (dx=319, dy=0)
  SAGE_DrawLine(319,239,0,239,2);   // Horizontal x1 > x2 (dx=-319, dy=0)
  SAGE_DrawLine(160,100,160,239,3); // Vertical (dx=0, dy=139)
  SAGE_DrawLine(10,110,110,210,4);  // Diagonal x1 < x2 (dx=100, dy=100)
  SAGE_DrawLine(110,110,10,210,5);  // Diagonal x1 > x2 (dx=-100, dy=100)
  SAGE_DrawLine(180,110,310,150,6); // Pente douce x1 < x2 (dx=130, dy=40)
  SAGE_DrawLine(180,110,230,230,7); // Pente raide x1 < x2 (dx=50, dy=120)
  SAGE_DrawLine(310,110,180,150,8); // Pente douce x1 > x2 (dx=-130, dy=40)
  SAGE_DrawLine(310,110,260,230,9); // Pente raide x1 > x2 (dx=-50, dy=120)
}

void test_lines16(void)
{
  SAGE_DrawLine(0,100,319,100,0xffff);   // Horizontal x1 < x2 (dx=319, dy=0)
  SAGE_DrawLine(319,239,0,239,0x8888);   // Horizontal x1 > x2 (dx=-319, dy=0)
  SAGE_DrawLine(160,100,160,239,0x4884); // Vertical (dx=0, dy=139)
  SAGE_DrawLine(10,110,110,210,0x8080);  // Diagonal x1 < x2 (dx=100, dy=100)
  SAGE_DrawLine(110,110,10,210,0xF4F4);  // Diagonal x1 > x2 (dx=-100, dy=100)
  SAGE_DrawLine(180,110,310,150,0xF004); // Pente douce x1 < x2 (dx=130, dy=40)
  SAGE_DrawLine(180,110,230,230,0x1234); // Pente raide x1 < x2 (dx=50, dy=120)
  SAGE_DrawLine(310,110,180,150,0x5678); // Pente douce x1 > x2 (dx=-130, dy=40)
  SAGE_DrawLine(310,110,260,230,0x0808); // Pente raide x1 > x2 (dx=-50, dy=120)
}

void test_triangle8(void)
{
//  SAGE_DebugLog("Random triangle no sort");
//  SAGE_DrawTriangle(150, 10, 50, 98, 260, 80, 1);
//  SAGE_DebugLog("Flat top triangle no sort");
//  SAGE_DrawTriangle(10, 10, 50, 10, 60, 80, 2);
//  SAGE_DebugLog("Flat bottom triangle no sort");
//  SAGE_DrawTriangle(230, 10, 150, 95, 290, 95, 3);
//  SAGE_DebugLog("Flat top triangle sort");
//  SAGE_DrawTriangle(210, 10, 50, 90, 20, 10, 4);
//  SAGE_DebugLog("Random triangle sort");
//  SAGE_DrawTriangle(10, 110, 50, 80, 40, 30, 5);
}

void test_triangle16(void)
{
//  SAGE_DebugLog("Random triangle no sort");
  SAGE_DrawTriangle(150, 10, 50, 98, 260, 80, 0x8888);
}

void test_clipleftedge(void)
{
  SAGE_Clipping clip = {0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};
  LONG points;

  // Soft slope + left clipped    - OK le 26/03/2021
  /*printf("** Soft slope line + left clip coords -10,6 ->  120,80 **\n");
  SAGE_FastLeftEdgeCalculation(coords, -10, 6, 120, 80);
  dump_coords("left edge",coords,80-6);
  points = SAGE_FastClippedLeftEdgeCalc(coords, -10, 6, 120, 80, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(-10, 6, 120, 80);*/
  // Soft slope + right clipped      - OK le 26/03/2021
  /*printf("** Soft slope line + right clip coords 50,10 ->  350,110 **\n");
  SAGE_FastLeftEdgeCalculation(coords, 50, 10, 350, 110);
  dump_coords("left edge",coords,110-10);
  points = SAGE_FastClippedLeftEdgeCalc(coords, 50, 10, 350, 110, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(50, 10, 350, 110);*/
  // Hard slope + left clipped     - OK le 26/03/2021
  /*SAGE_FastLeftEdgeCalculation(coords, -4, 6, 50, 180);
  dump_coords("left edge",coords,180-6);
  points = SAGE_FastClippedLeftEdgeCalc(coords, -4, 6, 50, 180, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(-4, 6, 50, 180);*/
  // Hard slope + right clipped      - OK le 26/03/2021
  /*SAGE_FastLeftEdgeCalculation(coords, 300, 6, 360, 180);
  dump_coords("left edge",coords,180-6);
  points = SAGE_FastClippedLeftEdgeCalc(coords, 300, 6, 360, 180, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(300, 6, 360, 180);*/

  // Soft slope - left clipped       - OK le 27/03/2021
  /*SAGE_FastLeftEdgeCalculation(coords, 120, 6, -15, 80);
  dump_coords("left edge",coords,80-6);
  points = SAGE_FastClippedLeftEdgeCalc(coords, 120, 6, -15, 80, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(120, 6, -15, 80);*/
  // Soft slope - right clipped       - OK le 29/03/2021
  /*SAGE_FastLeftEdgeCalculation(coords, 350, 10, 50, 110);
  dump_coords("left edge",coords,110-10);
  points = SAGE_FastClippedLeftEdgeCalc(coords, 350, 10, 50, 110, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(350, 10, 50, 110);*/
  // Hard slope - left clipped         - OK le 27/03/2021
  /*SAGE_FastLeftEdgeCalculation(coords, 55, 6, -13, 180);
  dump_coords("left edge",coords,180-6);
  points = SAGE_FastClippedLeftEdgeCalc(coords, 55, 6, -13, 180, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(55, 6, -13, 180);*/
  // Hard slope - right clipped        - OK le 27/03/2021
  /*SAGE_FastLeftEdgeCalculation(coords, 332, 6, 280, 180);
  dump_coords("left edge",coords,180-6);
  points = SAGE_FastClippedLeftEdgeCalc(coords, 332, 6, 280, 180, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(332, 6, 280, 180);*/

  points = SAGE_FastClippedLeftEdgeCalc(coords, 200, -30, 20, 220, &clip);
  dump_coords("clip left edge",coords,points);
  SAGE_DumpLineCoords(200, -30, 20, 220);

}

void test_cliprightedge(void)
{
  SAGE_Clipping clip = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1};
  LONG points;

  // Soft slope + left clipped    - OK le 29/03/2021
  /*printf("** Soft slope line + left clip coords -10,6 ->  120,80 **\n");
  SAGE_FastRightEdgeCalculation(coords, -10, 6, 120, 80);
  dump_coords("right edge",coords,80-6);
  points = SAGE_FastClippedRightEdgeCalc(coords, -10, 6, 120, 80, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(-10, 6, 120, 80);*/
  // Soft slope + right clipped      - OK le 29/03/2021
  /*printf("** Soft slope line + right clip coords 50,10 ->  350,110 **\n");
  SAGE_FastRightEdgeCalculation(coords, 50, 10, 350, 110);
  dump_coords("right edge",coords,110-10);
  points = SAGE_FastClippedRightEdgeCalc(coords, 50, 10, 350, 110, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(50, 10, 350, 110);*/
  // Hard slope + left clipped     - OK le 29/03/2021
  /*SAGE_FastRightEdgeCalculation(coords, -4, 6, 50, 180);
  dump_coords("right edge",coords,180-6);
  points = SAGE_FastClippedRightEdgeCalc(coords, -4, 6, 50, 180, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(-4, 6, 50, 180);*/
  // Hard slope + right clipped      - OK le 29/03/2021
  /*SAGE_FastRightEdgeCalculation(coords, 300, 6, 360, 180);
  dump_coords("right edge",coords,180-6);
  points = SAGE_FastClippedRightEdgeCalc(coords, 300, 6, 360, 180, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(300, 6, 360, 180);*/

  // Soft slope - left clipped       - OK le 29/03/2021
  /*SAGE_FastRightEdgeCalculation(coords, 120, 6, -15, 80);
  dump_coords("right edge",coords,80-6);
  points = SAGE_FastClippedRightEdgeCalc(coords, 120, 6, -15, 80, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(120, 6, -15, 80);*/
  // Soft slope - right clipped       - OK le 29/03/2021
  /*SAGE_FastRightEdgeCalculation(coords, 350, 10, 50, 110);
  dump_coords("right edge",coords,110-10);
  points = SAGE_FastClippedRightEdgeCalc(coords, 350, 10, 50, 110, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(350, 10, 50, 110);*/
  // Hard slope - left clipped         - OK le 29/03/2021
  /*SAGE_FastRightEdgeCalculation(coords, 55, 6, -13, 180);
  dump_coords("right edge",coords,180-6);
  points = SAGE_FastClippedRightEdgeCalc(coords, 55, 6, -13, 180, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(55, 6, -13, 180);*/
  // Hard slope - right clipped        - OK le 29/03/2021
  /*SAGE_FastRightEdgeCalculation(coords, 332, 6, 280, 180);
  dump_coords("right edge",coords,180-6);
  points = SAGE_FastClippedRightEdgeCalc(coords, 332, 6, 280, 180, &clip);
  dump_coords("clip right edge",coords,points);
  SAGE_DumpLineCoords(332, 6, 280, 180);*/

}

void main(void)
{
  SAGE_Event * event = NULL;
  LONG color,x,y;
  BOOL finish = FALSE;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library VIDEO test (DRAW) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_VIDEO)) {
    printf("Opening screen\n");
    if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
      x = 0;
      y = 0;
      color = 0;
      test_coords();
      test_clipleftedge();
      test_cliprightedge();
      while (!finish) {
        while ((event = SAGE_GetEvent()) != NULL) {
          printf(
            "Event polled type %d, code %d, mouse %d,%d\n",
            event->type,
            event->code,
            event->mousex,
            event->mousey
          );
          if (event->type == SEVT_RAWKEY) {
            switch (event->code) {
              case SKEY_FR_ESC:
                printf("Exit loop\n");
                finish = TRUE;
                break;
            }
          }
        }
        SAGE_DrawPixel(x%SCREEN_WIDTH, y%SCREEN_HEIGHT, color%65536);
        x++;
        if (x%SCREEN_WIDTH == 0) y++;
        if (y > 50) y = 0;
        color += 16;
        if (SCREEN_DEPTH == 8) {
          test_lines8();
          test_triangle8();
        } else {
          test_lines16();
          test_triangle16();
        }
        if (!SAGE_RefreshScreen()) {
          printf("Error RefreshScreen !\n");
          SAGE_DisplayError();
          finish = TRUE;
        }
      }
      printf("Closing screen\n");
      SAGE_CloseScreen();
    } else {
      SAGE_DisplayError();
    }
  } else {
    SAGE_DisplayError();
  }
  SAGE_Exit();
  printf("End of test\n");
}
