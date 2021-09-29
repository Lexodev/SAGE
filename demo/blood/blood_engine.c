/**
 * blood_engine.c
 * 
 * Blood project
 * Raycast engine
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 February 2021
 */

#include <math.h>

#include "/src/sage.h"

#include "blood.h"
#include "blood_control.h"
#include "blood_engine.h"

/** @var Player position and view angle */
LONG playerx, playery, playerangle;
UWORD run = 0;

/** Precalcs */
LONG sinus[ANGLE_360];
LONG cosinus[ANGLE_360];
LONG tangente[ANGLE_360];
LONG invtang[ANGLE_360];
LONG scale[WIDTH_BLOC*DEEP+1];
LONG correct[WIDTH_SCREEN];
LONG profondeur[HEIGHT_SCREEN/2];

/** @var Ray intersections */
typedef struct
{
  LONG intersect_x, intersect_y;
  LONG distance, colonne;
} intersection;

intersection * inter_tab[DEEP * 2];

/** @var Raycast functions */
VOID (*ray_tab[ANGLE_360]) (LONG, LONG, LONG);

VOID direct_bas_gauche(LONG, LONG, LONG);
VOID direct_bas_droit(LONG, LONG, LONG);
VOID direct_haut_gauche(LONG, LONG, LONG);
VOID direct_haut_droit(LONG, LONG, LONG);
VOID direct_gauche(LONG, LONG, LONG);
VOID direct_droit(LONG, LONG, LONG);
VOID direct_bas(LONG, LONG, LONG);
VOID direct_haut(LONG, LONG, LONG);

LONG move_x[ANGLE_360];
LONG move_y[ANGLE_360];

LONG run_x[ANGLE_360];
LONG run_y[ANGLE_360];

LONG limit_x[ANGLE_360];
LONG limit_y[ANGLE_360];

BYTE shading[WIDTH_BLOC * DEEP];

extern UBYTE * texture_adr[];
extern UBYTE * chunky_buffer;
extern UBYTE keyboard_state[];

/** Dungeon map */
#define MAP_WIDTH             24
#define MAP_HEIGHT            24
#define MAP_STARTX            12*WIDTH_BLOC+(WIDTH_BLOC/2)
#define MAP_STARTY            5*WIDTH_BLOC+(WIDTH_BLOC/2)
#define MAP_STARTA            ANGLE_270

BYTE plan[] = {
  36,36,36,36,36,36,36,36,36,35,35,35,35,35,43,28,28,28,28,28,28,28,28,28,
  36, 1, 1, 1, 1, 1, 1, 1,36, 9,35, 9, 9,35, 8,53, 3, 3, 3, 3, 3, 3, 3,28,
  36, 1,37,36,36,36,37, 1,36, 9,59, 9, 9,35, 8,28, 3,21, 3,21, 3,21, 3,28,
  36, 1,36, 1, 1, 1,36, 1,36, 9,35, 9, 9,35, 8,28, 3, 3, 3, 3, 3, 3, 3,28,
  36, 1,36, 1,37, 1,36, 1,36, 9,35, 9, 9,35, 8,28, 3,21, 3,21, 3,21, 3,28,
  36, 1,36, 1,36,36,37, 1,36, 9,35, 9, 9,35, 8,28, 3, 3, 3, 3, 3, 3, 3,28,
  36, 1, 1, 1, 1, 1, 1, 1,36, 9,35,35,35,35, 8,28, 3,21, 3,21, 3,21, 3,28,
  36, 1, 1,36,36,51,36,36,37, 9,35, 8, 8, 8, 8,28, 3, 3, 3, 3, 3, 3, 3,28,
  36,36,36,36,37, 1, 1, 1, 1, 8, 8, 8, 8, 8, 8,28,28,28,28,28,28,28,53,28,
  35,35,35,35,35,24,24,24,24,55,39,39,39,39,39,39,39,39,39,26, 4, 4, 4,26,
  35, 7, 7, 7,35,24, 5, 5, 5, 5, 5,32,32,32,32,32,39, 6,39,26, 4, 4, 4,26,
  35, 7, 7, 7,35,24, 5, 5, 5, 5, 5, 5, 5, 5, 5,32,39, 6, 6,54, 4, 4, 4,26,
  35, 7, 7, 7,35,24, 5, 5, 5, 5,32,32,32,32, 5,32,29,39,39,26, 4, 4, 4,26,
  35,57,35,35,35,30,29, 2, 2, 2,29, 1, 1, 1, 1, 1, 1,29,20,26, 4, 4, 4,26,
  38, 6, 6,38,30, 1,30,29, 2, 2,29, 1,29,29, 1, 1, 1, 1,20,26, 4, 4, 4,26,
  38,42, 6, 6,22, 1, 1,30,29, 1, 1, 1, 1, 1, 1,29,29, 1,20,26,26,26,26,26,
  38,42, 6,38,44,22, 1, 1,30,30,30,30,29, 1,29,20,20, 4,20,27,27,27,27,27,
  38, 6, 6, 3, 4,44,30, 1, 1, 1, 1, 1,30,29,20, 4, 4, 4, 4,50,00,00,00,27,
  38, 6,38,44, 4,45,44,32,32, 1, 1, 1, 1,30,20, 4, 4,20,20,48,33,00,00,27,
  38, 6,38,45, 4, 4, 4, 5,32,32,32,30, 1,30,20,20,20,20,48,00,50,00,00,27,
  38, 6,38,41,44,45, 4, 5, 5, 5, 5,34, 1,23,23,23,47,47,20,48,33,00,00,27,
  41, 3, 3, 3,41,41,44, 5, 5, 4, 5, 8, 8, 8, 8,58, 2, 2,47,47,27,00,00,27,
  41, 3, 3, 3, 3, 3,41,30, 5, 5, 5,34,43,43,43,43,46, 2, 2, 2,52,00,00,27,
  41,41,41,41,41,41,41,30,30,30,30,30,43,43,43,43,46,46,46,46,46,27,27,27
};

VOID precalcul(VOID)
{
  DOUBLE angle, step;
  LONG i, j, k, l;
   
  SAGE_AppliLog(" Precalc tables");
  step = (DOUBLE)WIDTH_VIEW/(DOUBLE)WIDTH_SCREEN;
  angle = 0.0;
  for (i = ANGLE_0;i < ANGLE_360;i++) {
    sinus[i] = sin(RAD(angle)) * MULTIPLY;
    cosinus[i] = cos(RAD(angle)) * MULTIPLY;
    move_x[i] = (cosinus[i] * STEP_AVANCE) >> DECAL;
    move_y[i] = (sinus[i] * STEP_AVANCE) >> DECAL;
    run_x[i] = (cosinus[i] * STEP_AVANCE * 2) >> DECAL;
    run_y[i] = (sinus[i] * STEP_AVANCE * 2) >> DECAL;
    limit_x[i] = (cosinus[i] * (DEEP*WIDTH_BLOC)) >> DECAL;
    limit_y[i] = (sinus[i] * (DEEP*WIDTH_BLOC)) >> DECAL;
    if (i != ANGLE_90 && i != ANGLE_270) {
      tangente[i] = tan(RAD(angle)) * MULTIPLY;
      if (i != ANGLE_0 && i != ANGLE_180) {
        invtang[i] = (1.0 / tan(RAD(angle))) * MULTIPLY;
      } else {
        invtang[i] = 0;
      }
    } else {
      tangente[i] = 0;
      invtang[i] = 0;
    }
    angle += step;
  }
  step = (DOUBLE) 90.0 / (DOUBLE) (WIDTH_BLOC*DEEP);
  angle = 90.0;
  for (i = 1;i < (WIDTH_BLOC*DEEP);i++) {
    angle -= step;
    scale[i] = (WIDTH_BLOC) * tan(RAD(angle));
    if (scale[i] < MIN_HEIGHT) {
      scale[i] = MIN_HEIGHT;
    }
  }
  scale[0] = scale[1];
  scale[WIDTH_BLOC*DEEP] = scale[(WIDTH_BLOC*DEEP)-1];
  step = (DOUBLE)WIDTH_VIEW/(DOUBLE)WIDTH_SCREEN;
  angle = 0.0 - (step * (WIDTH_SCREEN/2));
  for (i = 0;i < WIDTH_SCREEN;i++) {
    correct[i] = (1.0 / cos(RAD(angle))) * MULTIPLY;
    angle += step;
  }
  j = 0;
  for (i = 0;i < (HEIGHT_SCREEN/2);i++) {
    while (scale[j] > (HEIGHT_SCREEN-(i*2))) {
      j++;
    }
    profondeur[i] = j;
  }
  i = 0;
  j = 0;
  for (k = 0;k < (COLOR_NBR/SHADE_NBR);k++) {
    for (l = 0;l < ((WIDTH_BLOC*DEEP)/(COLOR_NBR/SHADE_NBR));l++) {
      shading[i++] = j;
    }
    j += SHADE_NBR;
  }
}

VOID init_tab(VOID)
{
  LONG i, err;

  SAGE_AppliLog(" Init jump table");
  for (i = ANGLE_0;i < ANGLE_360;i++)  {
    if (i > ANGLE_0 && i < ANGLE_90) {
      ray_tab[i] = direct_bas_droit;
    } else if (i > ANGLE_90 && i < ANGLE_180) {
      ray_tab[i] = direct_bas_gauche;
    } else if (i > ANGLE_180 && i < ANGLE_270) {
      ray_tab[i] = direct_haut_gauche;
    } else if (i > ANGLE_270 && i < ANGLE_360) {
      ray_tab[i] = direct_haut_droit;
    } else if (i == ANGLE_0) {
      ray_tab[i] = direct_droit;
    } else if (i == ANGLE_90) {
      ray_tab[i] = direct_bas;
    } else if (i == ANGLE_180) {
      ray_tab[i] = direct_gauche;
    } else if (i == ANGLE_270) {
      ray_tab[i] = direct_haut;
    }
  }
  for (i = 0;i < (DEEP*2);i++) {
    inter_tab[i] = AllocMem(sizeof(intersection), MEMF_PUBLIC);
  }
}

/**
 * Raycast engine
 */
VOID direct_bas_droit(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG xray_x, yray_y, diff;
  LONG dist_x = 0, dist_y = 0;
  LONG fin_x = 0, fin_y = 0, count = 1;
  intersection * inter;

  while (!fin_x || !fin_y) {
    if (dist_x < (DEEP*WIDTH_BLOC)) {
      xray_y += WIDTH_BLOC;
      diff = xray_y - playery;
      xray_x = playerx + ((diff * invtang[angle])>>DECAL);
      dist_x = (diff<<DECAL) / sinus[angle];
      if (xray_x < (MAP_WIDTH*WIDTH_BLOC) && xray_y < (MAP_WIDTH*WIDTH_BLOC) && dist_x < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = xray_x;
        inter->intersect_y = xray_y;
        inter->colonne = xray_x % WIDTH_BLOC;
        inter->distance = dist_x;
      }  else {
        dist_x = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_x = 1;
    }
    if (dist_y < (DEEP*WIDTH_BLOC)) {
      yray_x += WIDTH_BLOC;
      diff = yray_x - playerx;
      yray_y = playery + ((diff * tangente[angle]) >> DECAL);
      dist_y = (diff<<DECAL) / cosinus[angle];
      if (yray_x < (MAP_WIDTH*WIDTH_BLOC) && yray_y < (MAP_WIDTH*WIDTH_BLOC) && dist_y < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = yray_x;
        inter->intersect_y = yray_y;
        inter->colonne = yray_y % WIDTH_BLOC;
        inter->distance = dist_y;
      } else {
        dist_y = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_y = 1;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx+limit_x[angle];
  inter->intersect_y = playery+limit_y[angle];
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_bas_gauche(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG xray_x, yray_y, diff;
  LONG dist_x = 0, dist_y = 0;
  LONG fin_x = 0, fin_y = 0, count = 1;
  intersection * inter;
   
  yray_x--;
  while (!fin_x || !fin_y) {
    if (dist_x < (DEEP*WIDTH_BLOC)) {
      xray_y += WIDTH_BLOC;
      diff = xray_y - playery;
      xray_x = playerx + ((diff * invtang[angle])>>DECAL);
      dist_x = (diff<<DECAL) / sinus[angle];
      if (xray_x >= 0 && xray_y < (MAP_WIDTH*WIDTH_BLOC) && dist_x < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = xray_x;
        inter->intersect_y = xray_y;
        inter->colonne = xray_x % WIDTH_BLOC;
        inter->distance = dist_x;
      } else {
        dist_x = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_x = 1;
    }
    if (dist_y < (DEEP*WIDTH_BLOC)) {
      diff = yray_x - playerx;
      yray_y = playery + ((diff * tangente[angle]) >> DECAL);
      dist_y = (diff<<DECAL) / cosinus[angle];
      if (yray_x >= 0 && yray_y < (MAP_WIDTH*WIDTH_BLOC) && dist_y < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = yray_x;
        inter->intersect_y = yray_y;
        inter->colonne = yray_y % WIDTH_BLOC;
        inter->distance = dist_y;
        yray_x -= WIDTH_BLOC;
      } else {
        dist_y = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_y = 1;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx+limit_x[angle];
  inter->intersect_y = playery+limit_y[angle];
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_haut_gauche(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG xray_x, yray_y, diff;
  LONG dist_x = 0, dist_y = 0;
  LONG fin_x = 0, fin_y = 0, count = 1;
  intersection * inter;
   
  xray_y--;
  yray_x--;
  while (!fin_x || !fin_y) {
    if (dist_x < (DEEP*WIDTH_BLOC)) {
      diff = xray_y - playery;
      xray_x = playerx + ((diff * invtang[angle])>>DECAL);
      dist_x = (diff<<DECAL) / sinus[angle];
      if (xray_x >= 0 && xray_y >= 0 && dist_x < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = xray_x;
        inter->intersect_y = xray_y;
        inter->colonne = xray_x % WIDTH_BLOC;
        inter->distance = dist_x;
        xray_y -= WIDTH_BLOC;
      } else {
        dist_x = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_x = 1;
    }
    if (dist_y < (DEEP*WIDTH_BLOC)) {
      diff = yray_x - playerx;
      yray_y = playery + ((diff * tangente[angle]) >> DECAL);
      dist_y = (diff<<DECAL) / cosinus[angle];
      if (yray_x >= 0 && yray_y >= 0 && dist_y < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = yray_x;
        inter->intersect_y = yray_y;
        inter->colonne = yray_y % WIDTH_BLOC;
        inter->distance = dist_y;
        yray_x -= WIDTH_BLOC;
      } else {
        dist_y = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_y = 1;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx+limit_x[angle];
  inter->intersect_y = playery+limit_y[angle];
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_haut_droit(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG xray_x, yray_y, diff;
  LONG dist_x = 0, dist_y = 0;
  LONG fin_x = 0, fin_y = 0, count = 1;
  intersection * inter;
   
  xray_y--;
  while (!fin_x || !fin_y) {
    if (dist_x < (DEEP*WIDTH_BLOC)) {
      diff = xray_y - playery;
      xray_x = playerx + ((diff * invtang[angle])>>DECAL);
      dist_x = (diff<<DECAL) / sinus[angle];
      if (xray_y >= 0 && xray_x < (MAP_WIDTH*WIDTH_BLOC) && dist_x < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = xray_x;
        inter->intersect_y = xray_y;
        inter->colonne = xray_x % WIDTH_BLOC;
        inter->distance = dist_x;
        xray_y -= WIDTH_BLOC;
      } else {
        dist_x = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_x = 1;
    }
    if (dist_y < (DEEP*WIDTH_BLOC)) {
      yray_x += WIDTH_BLOC;
      diff = yray_x - playerx;
      yray_y = playery + ((diff * tangente[angle]) >> DECAL);
      dist_y = (diff<<DECAL) / cosinus[angle];
      if (yray_y >= 0 && yray_x < (MAP_WIDTH*WIDTH_BLOC) && dist_y < (DEEP*WIDTH_BLOC)) {
        inter = inter_tab[count++];
        inter->intersect_x = yray_x;
        inter->intersect_y = yray_y;
        inter->colonne = yray_y % WIDTH_BLOC;
        inter->distance = dist_y;
      } else {
        dist_y = DEEP*WIDTH_BLOC;
      }
    } else {
      fin_y = 1;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx+limit_x[angle];
  inter->intersect_y = playery+limit_y[angle];
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_droit(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG yray_y;
  LONG dist = 0;
  LONG count = 1;
  intersection * inter;
   
  while (dist < (DEEP*WIDTH_BLOC)) {
    yray_x += WIDTH_BLOC;
    dist = yray_x-playerx;
    if (yray_x < (MAP_WIDTH*WIDTH_BLOC) && dist < (DEEP*WIDTH_BLOC)) {
      inter = inter_tab[count++];
      inter->intersect_x = yray_x;
      inter->intersect_y = playery;
      inter->colonne = playery % WIDTH_BLOC;
      inter->distance = dist;
    } else {
      dist = DEEP*WIDTH_BLOC;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx+(DEEP*WIDTH_BLOC);
  inter->intersect_y = playery;
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_bas(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG xray_x;
  LONG dist = 0;
  LONG count = 1;
  intersection * inter;
   
  while (dist < (DEEP*WIDTH_BLOC)) {
    xray_y += WIDTH_BLOC;
    dist = xray_y-playery;
    if (xray_y < (MAP_WIDTH*WIDTH_BLOC) && dist < (DEEP*WIDTH_BLOC)) {
      inter = inter_tab[count++];
      inter->intersect_x = playerx;
      inter->intersect_y = xray_y;
      inter->colonne = playerx % WIDTH_BLOC;
      inter->distance = dist;
    } else {
      dist = DEEP*WIDTH_BLOC;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx;
  inter->intersect_y = playery+(DEEP*WIDTH_BLOC);
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_gauche(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG yray_y;
  LONG dist = 0;
  LONG count = 1;
  intersection * inter;
   
  yray_x--;
  while (dist < (DEEP*WIDTH_BLOC)) {
    dist = playerx-yray_x;
    if (yray_x >= 0 && dist < (DEEP*WIDTH_BLOC)) {
      inter = inter_tab[count++];
      inter->intersect_x = yray_x;
      inter->intersect_y = playery;
      inter->colonne = playery % WIDTH_BLOC;
      inter->distance = dist;
      yray_x -= WIDTH_BLOC;
    } else {
      dist = DEEP*WIDTH_BLOC;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx+(DEEP*WIDTH_BLOC);
  inter->intersect_y = playery;
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID direct_haut(LONG angle, LONG xray_y, LONG yray_x)
{
  LONG xray_x;
  LONG dist = 0;
  LONG count = 1;
  intersection * inter;
   
  xray_y--;
  while (dist < (DEEP*WIDTH_BLOC)) {
    dist = playery-xray_y;
    if (xray_y >= 0 && dist < (DEEP*WIDTH_BLOC)) {
      inter = inter_tab[count++];
      inter->intersect_x = playerx;
      inter->intersect_y = xray_y;
      inter->colonne = playerx % WIDTH_BLOC;
      inter->distance = dist;
      xray_y -= WIDTH_BLOC;
    } else {
      dist = DEEP*WIDTH_BLOC;
    }
  }
  inter = inter_tab[count];
  inter->intersect_x = playerx;
  inter->intersect_y = playery+(DEEP*WIDTH_BLOC);
  inter->distance = DEEP*WIDTH_BLOC;
}

VOID tri_intersect(WORD lim_inf, WORD lim_sup)
{
  LONG key;
  WORD cp_inf, cp_sup;
  intersection * temp;

  if (lim_inf < lim_sup) {
    key = inter_tab[lim_inf]->distance;
    cp_inf = lim_inf+1;
    cp_sup = lim_sup;
    while (cp_inf <= cp_sup) {
      while (inter_tab[cp_inf]->distance <= key && cp_inf <= lim_sup) {
        cp_inf++;
      }
      while (inter_tab[cp_sup]->distance > key && cp_sup > lim_inf) {
        cp_sup--;
      }
      if (cp_inf <= cp_sup) {
        temp = inter_tab[cp_inf];
        inter_tab[cp_inf] = inter_tab[cp_sup];
        inter_tab[cp_sup] = temp;
        cp_inf++;
        cp_sup--;
      }
    }
    temp = inter_tab[lim_inf];
    inter_tab[lim_inf] = inter_tab[cp_sup];
    inter_tab[cp_sup] = temp;
    tri_intersect(lim_inf,cp_sup-1);
    tri_intersect(cp_sup+1,lim_sup);
  }  
}

VOID draw_wall(BYTE bloc, int colonne, int startline, int ray, int dist)
{
  LONG height, zoom, step;
  UBYTE * src, * dst;
  
  height = CHUNKY_HEIGHT-(startline<<1);
  if (height < CHUNKY_HEIGHT) {
    src = texture_adr[bloc] + (colonne * WIDTH_BLOC);
    dst = chunky_buffer + (startline*CHUNKY_WIDTH) + ray;
    step = ( WIDTH_BLOC << DECAL ) / height;
    zoom = 0;
  } else {
    src = texture_adr[bloc] + (colonne * WIDTH_BLOC);
    dst = chunky_buffer + ray;
    step = ( WIDTH_BLOC << DECAL ) / height;
    zoom = (( height - CHUNKY_HEIGHT ) >> 1 ) * step;
    height = CHUNKY_HEIGHT;
  }
  while (height--) {
    *dst = *(src + (zoom >> DECAL)) + shading[dist];
    zoom += step;
    dst += CHUNKY_WIDTH;
  }
}

VOID draw_floor(BYTE bloc, int startline, int endline, int ray, int angle)
{
  UBYTE * dst1, * dst2, * src1, * src2;
  LONG rx, ry, rz;
   
  if (endline >= 0) {
    if (startline < 0) {
      startline = 0;
    }
    src1 = texture_adr[bloc+CEIL_OFFSET];
    dst1 = chunky_buffer + (startline*CHUNKY_WIDTH) + ray;
    src2 = texture_adr[bloc];
    dst2 = chunky_buffer + ((CHUNKY_HEIGHT-startline-1)*CHUNKY_WIDTH) + ray;
    while (startline < endline) {
      rz = (profondeur[startline]*correct[ray])>>DECAL;
      rx = (playerx+((cosinus[angle]*rz)>>DECAL))&(WIDTH_BLOC-1);
      ry = (playery+((sinus[angle]*rz)>>DECAL))&(WIDTH_BLOC-1);
      *dst1 = *(src1 + (ry*WIDTH_BLOC) + rx) + shading[profondeur[startline]];
      dst1 += CHUNKY_WIDTH;
      *dst2 = *(src2 + (ry*WIDTH_BLOC) + rx) + shading[profondeur[startline]];
      dst2 -= CHUNKY_WIDTH;
      startline++;
    }
  }
}

VOID draw_world(VOID)
{
  LONG ray, angle;
  LONG startx, starty;
  LONG count, wall;
  LONG sliney, eliney;
  BYTE bloc;
  intersection * inter;
   
  angle = playerangle - ANGLE_30;
  startx = playery & (-WIDTH_BLOC);
  starty = playerx & (-WIDTH_BLOC);
  for (ray = 0;ray < CHUNKY_WIDTH;ray++) {
    if (angle < ANGLE_0) {
      angle += ANGLE_360;
    } else if (angle >= ANGLE_360) {
      angle -= ANGLE_360;
    }
    (*ray_tab[angle])(angle, startx, starty);
    tri_intersect(0, (DEEP*2)-1);
    count = 0;
    wall = 0;
    inter = inter_tab[count];
    while (inter->distance < DEEP*WIDTH_BLOC && !wall) {
      sliney = (CHUNKY_HEIGHT-((scale[inter->distance]*correct[ray])>>DECAL))>>1;
      bloc = plan[((inter->intersect_y >> WALL_DECAL)*MAP_WIDTH)+(inter->intersect_x >> WALL_DECAL)];
      if (bloc >= WALL_ID) {
        draw_wall(bloc,inter->colonne, sliney, ray, inter->distance);
        wall = 1;
      } else {
        count++;
        inter = inter_tab[count];
        eliney = (CHUNKY_HEIGHT-((scale[inter->distance]*correct[ray])>>DECAL))>>1;
        draw_floor(bloc, sliney, eliney, ray, angle);
      }
    }
    angle++;
  }
}

VOID  move_forward(VOID)
{
  LONG tempx, tempy;
  LONG tempx1, tempy1, angle1;
  LONG tempx2, tempy2, angle2;
  BYTE bloc, bloc1, bloc2;
   
  angle1 = playerangle - ANGLE_30;
  if (angle1 < ANGLE_0) {
    angle1 += ANGLE_360;
  }
  angle2 = playerangle + ANGLE_30;
  if (angle2 >= ANGLE_360) {
    angle2 -= ANGLE_360;
  }
  if (run) {
    tempx1 = playerx + run_x[angle1];
    tempy1 = playery + run_y[angle1];
    tempx2 = playerx + run_x[angle2];
    tempy2 = playery + run_y[angle2];
    tempx = playerx + run_x[playerangle];
    tempy = playery + run_y[playerangle];
  } else {
    tempx1 = playerx + move_x[angle1];
    tempy1 = playery + move_y[angle1];
    tempx2 = playerx + move_x[angle2];
    tempy2 = playery + move_y[angle2];
    tempx = playerx + move_x[playerangle];
    tempy = playery + move_y[playerangle];
  }
  bloc = plan[((tempy >> WALL_DECAL)*MAP_WIDTH)+(tempx >> WALL_DECAL)];
  bloc1 = plan[((tempy1 >> WALL_DECAL)*MAP_WIDTH)+(tempx1 >> WALL_DECAL)];
  bloc2 = plan[((tempy2 >> WALL_DECAL)*MAP_WIDTH)+(tempx2 >> WALL_DECAL)];
  if (bloc < WALL_ID && bloc1 < WALL_ID && bloc2 < WALL_ID) {
    playerx = tempx;
    playery = tempy;
  }
}

VOID  move_backward(VOID)
{
  LONG tempx, tempy;
  LONG tempx1, tempy1, angle1;
  LONG tempx2, tempy2, angle2;
  BYTE bloc, bloc1, bloc2;

  angle1 = playerangle - ANGLE_30;
  if (angle1 < ANGLE_0) {
    angle1 += ANGLE_360;
  }
  angle2 = playerangle + ANGLE_30;
  if (angle2 >= ANGLE_360) {
    angle2 -= ANGLE_360;
  }
  if (run) {
    tempx1 = playerx - run_x[angle1];
    tempy1 = playery - run_y[angle1];
    tempx2 = playerx - run_x[angle2];
    tempy2 = playery - run_y[angle2];
    tempx = playerx - run_x[playerangle];
    tempy = playery - run_y[playerangle];
  } else {
    tempx1 = playerx - move_x[angle1];
    tempy1 = playery - move_y[angle1];
    tempx2 = playerx - move_x[angle2];
    tempy2 = playery - move_y[angle2];
    tempx = playerx - move_x[playerangle];
    tempy = playery - move_y[playerangle];
  }
  bloc = plan[((tempy >> WALL_DECAL)*MAP_WIDTH)+(tempx >> WALL_DECAL)];
  bloc1 = plan[((tempy1 >> WALL_DECAL)*MAP_WIDTH)+(tempx1 >> WALL_DECAL)];
  bloc2 = plan[((tempy2 >> WALL_DECAL)*MAP_WIDTH)+(tempx2 >> WALL_DECAL)];
  if (bloc < WALL_ID && bloc1 < WALL_ID && bloc2 < WALL_ID) {
    playerx = tempx;
    playery = tempy;
  }
}

VOID turn_left(VOID)
{
  if (run) {
    playerangle -= (STEP_ROT*2);
  } else {
    playerangle -= STEP_ROT;
  }
  if (playerangle < 0 ) {
    playerangle += ANGLE_360;
  }
}

VOID turn_right(VOID)
{
  if (run) {
    playerangle += (STEP_ROT*2);
  } else {
    playerangle += STEP_ROT;
  }
  if (playerangle >= ANGLE_360) {
    playerangle -= ANGLE_360;
  }
}

VOID open_door(VOID)
{
  LONG tempx, tempy;
  BYTE bloc;
   
  tempx = playerx + ((cosinus[playerangle] * WIDTH_BLOC) >> DECAL);
  tempy = playery + ((sinus[playerangle] * WIDTH_BLOC) >> DECAL);
  bloc = plan[(tempy/WIDTH_BLOC*MAP_WIDTH)+tempx/WIDTH_BLOC];
  if (bloc >= DOOR_ID) {
    plan[(tempy/WIDTH_BLOC*MAP_WIDTH)+tempx/WIDTH_BLOC] = bloc-DOOR_ID;
  }
}

VOID check_kbd(VOID)
{
  if (keyboard_state[KEY_UP]) {
    move_forward();
  } else if (keyboard_state[KEY_DOWN]) {
    move_backward();
  }
  if (keyboard_state[KEY_LEFT]) {
    turn_left();
  } else if (keyboard_state[KEY_RIGHT]) {
    turn_right();
  }
  if (keyboard_state[KEY_SPACE]) {
    open_door();
  }
}

VOID Engine(VOID)
{
  check_kbd();
  draw_world();
}

BOOL InitEngine(VOID)
{
  SAGE_AppliLog("Init engine");
  precalcul();
  init_tab();
  playerx = MAP_STARTX;
  playery = MAP_STARTY;
  playerangle = MAP_STARTA;
  return TRUE;
}
