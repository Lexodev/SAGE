/**
 * dragon_crown.h
 *
 * Simple animation demo for Vampire & high end amiga
 * SAS/C 6.X
 *
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.2 December 2020
 */

#ifndef _DRAGON_CROWN_H_
#define _DRAGON_CROWN_H_

#include <exec/types.h>
#include "/src/sage.h"

// Screen size
#define SCREENWIDTH           960L
#define SCREENHEIGHT          540L
#define SCREENDEPTH           16L
#define VIEWWIDTH             960L
#define VIEWHEIGHT            540L

// Background
#define BGLAYER               0
#define BGWIDTH               1920L
#define BGHEIGHT              540L
#define BGPOSX                0L
#define BGPOSY                0L

// Logo
#define LOGOLAYER             1
#define LOGOWIDTH             320L
#define LOGOHEIGHT            149L
#define LOGOPOSX              (SCREENWIDTH-LOGOWIDTH)/2
#define LOGOPOSY              10L

// Vampire
#define VAMPIRELAYER          2
#define VAMPIREWIDTH          360L
#define VAMPIREHEIGHT         360L
#define VAMPIREPOSX           BGPOSX + 4L
#define VAMPIREPOSY           4L

// Help
#define HELPLAYER             3
#define HELPWIDTH             400L
#define HELPHEIGHT            445L
#define HELPPOSX              SCREENWIDTH-HELPWIDTH-BGPOSX
#define HELPPOSY              0L

// Font
#define FONTBANK              0
#define FONTFRAME             10L
#define FONTWIDTH             32L
#define FONTHEIGHT            25L
#define FONTPOSX              (VIEWWIDTH-(FONTWIDTH*2)-4)
#define FONTPOSY              4L

// Sprites

typedef struct {
  STRPTR filename;
  UWORD bank;
  ULONG nbframe, width, height;
  LONG posx, posy;
  UWORD stepx, stepy, frame;
} SpriteInfo;

#define NBSPRITES             6
#define SPRFRAMERATE          4
#define SPRFIGHTER            1
#define SPRAMAZON             2
#define SPRWIZARD             3
#define SPRELF                4
#define SPRDWARF              5
#define SPRSORCERESS          6

// IDLE Sorceress (mouse cursor)
#define SPR0FRAME             31L
#define SPR0WIDTH             164L
#define SPR0HEIGHT            217L
#define SPR0POSX              20
#define SPR0POSY              20
#define SPR0STEPX             4L
#define SPR0STEPY             4L

// Fighter
#define SPR1FRAME             36L
#define SPR1WIDTH             176L
#define SPR1HEIGHT            259L
#define SPR1POSY              VIEWHEIGHT-SPR1HEIGHT-60
#define SPR1STEPX             1L

// Amazon
#define SPR2FRAME             12L
#define SPR2WIDTH             152L
#define SPR2HEIGHT            248L
#define SPR2POSY              VIEWHEIGHT-SPR2HEIGHT-55
#define SPR2STEPX             6L

// Wizard
#define SPR3FRAME             12L
#define SPR3WIDTH             144L
#define SPR3HEIGHT            216L
#define SPR3POSY              VIEWHEIGHT-SPR3HEIGHT-40
#define SPR3STEPX             4L

// Elf
#define SPR4FRAME             12L
#define SPR4WIDTH             164L
#define SPR4HEIGHT            240L
#define SPR4POSY              VIEWHEIGHT-SPR4HEIGHT-45
#define SPR4STEPX             5L

// Dwarf
#define SPR5FRAME             14L
#define SPR5WIDTH             212L
#define SPR5HEIGHT            187L
#define SPR5POSY              VIEWHEIGHT-SPR5HEIGHT-30
#define SPR5STEPX             2L

// Sorceress
#define SPR6FRAME             12L
#define SPR6WIDTH             184L
#define SPR6HEIGHT            214L
#define SPR6POSY              VIEWHEIGHT-SPR6HEIGHT-35
#define SPR6STEPX             3L

// Transparent color
#define TRANSPCOLOR           0xF81F

// Music
#define DRAGON_MUSIC          0

// Sound
#define SOUND_EVIL            1

#endif
