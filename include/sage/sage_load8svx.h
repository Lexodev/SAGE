/**
 * sage_load8svx.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * 8SVX sound loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 25/02/2025)
 */

#ifndef _SAGE_LOAD8SVX_H_
#define _SAGE_LOAD8SVX_H_

#include <exec/exec.h>
#include <dos/dos.h>

#include <sage/sage_sound.h>

#define SSND_8SVXTAG          0x38535658
#define SSND_8SVXOFFSET       8
#define SSND_VHDRTAG          0x56484452
#define SSND_BODYTAG          0x424F4459

/** 8SVX file header */
typedef struct
{
    ULONG oneShotHiSamples;     // high octave 1-shot samples
    ULONG repeatHiSamples;      // high octave repeat samples
    ULONG samplesPerHiCycle;    // high octave samples per cycle
    UWORD samplesPerSec;        // sampling rate
    UBYTE ctOctave;             // number of octaves
    UBYTE sCompression;         // Compression mode
    LONG vol;                   // Playback volume
} SAGE_8SVXHeader;

/** Load a 8SVX file */
SAGE_Sound *SAGE_Load8SVXSound(BPTR);

#endif
