#----------------------------------------------------------
# SAGE library makefile
# Fabrice Labrador <fabrice.labrador@gmail.com>
# V2.0 January 2021
#----------------------------------------------------------
PREFX := /opt/amiga

CC := $(PREFX)/bin/m68k-amigaos-gcc -w -c 
AS := $(PREFX)/bin/m68k-amigaos-as
AR := $(PREFX)/bin/m68k-amigaos-ar
LD := $(PREFX)/bin/m68k-amigaos-ld
RL := $(PREFX)/bin/m68k-amigaos-ranlib
VASM := $(PREFX)/bin/vasmm68k_mot

# Build options
OPT= -m68040 -noixemul -O2 -fbbb=- -I../libinclude -mhard-float -D_SAGE_DEBUG_MODE_=0 -DASM="" -DSAVEDS="" -o $@
LIB=
SAGELIB=libsage.a

# Objects
LIBOBJ=sage.o
COREOBJ=sage_logger.o sage_error.o sage_memory.o sage_timer.o sage_thread.o sage_vampire.o
VIDEOOBJ=sage_video.o sage_vblint.o sage_bitmap.o sage_blitter.o sage_ammxblit.o sage_event.o sage_screen.o sage_layer.o sage_draw.o sage_fastdraw.o sage_sprite.o sage_tile.o sage_tilemap.o sage_picture.o
INPUTOBJ=sage_input.o sage_keyboard.o sage_joyport.o
AUDIOOBJ=sage_audio.o sage_loadwave.o sage_load8svx.o sage_sound.o sage_loadtracker.o sage_loadaiff.o sage_music.o
INTOBJ=sage_interrupt.o sage_itserver.o
D3EOBJ=sage_3d.o sage_math.o
NETOBJ=sage_network.o
EXTOBJ=ext/PT-AHIPlay.o

ALLOBJ=$(VIDEOOBJ) $(INPUTOBJ) $(AUDIOOBJ) $(INTOBJ) $(D3EOBJ) $(NETOBJ) $(LIBOBJ) $(COREOBJ) $(EXTOBJ)

# Build sage library
#dist: cleanlib internal module external
#  @echo "** Build complete **"


all: $(SAGELIB)

$(SAGELIB): $(ALLOBJ)
	$(AR) cru $(SAGELIB) $(ALLOBJ)
	$(RL) $(SAGELIB)

#external: ext/PT-AHIPlay.o
#  oml $(SAGELIB) Add ext/PT-AHIPlay.o

# Build lib module
buildlib: $(LIBOBJ)

sage.o: sage.c sage.h
	$(CC) sage.c $(OPT)

# Build core module
buildcore: $(COREOBJ)

sage_logger.o: sage_logger.c sage_logger.h
	$(CC) sage_logger.c $(OPT)

sage_error.o: sage_error.c sage_error.h
	$(CC) sage_error.c $(OPT)

sage_memory.o: sage_memory.c sage_memory.h
	$(CC) sage_memory.c $(OPT)

sage_timer.o: sage_timer.c sage_timer.h
	$(CC) sage_timer.c $(OPT)

sage_thread.o: sage_thread.c sage_thread.h
	$(CC) sage_thread.c $(OPT)

sage_vampire.o: sage_vampire.c sage_vampire.h
	$(CC) sage_vampire.c $(OPT)

# Build external lib
PT-AHIPlay.o: ext/PT-AHIPlay.s ext/PT-AHIPlay.i
	$(VASM) -Fhunk -I/opt/amiga/m68k-amigaos/ndk-include -o ext/PT-AHIPlay.o ext/PT-AHIPlay.s

# Build video module
buildvideo: $(VIDEOOBJ)

sage_video.o: sage_video.c sage_video.h
	$(CC) sage_video.c $(OPT)

sage_bitmap.o: sage_bitmap.c sage_bitmap.h
	$(CC) sage_bitmap.c $(OPT)

sage_blitter.o: sage_blitter.asm sage_blitter.h
	$(VASM) -m68040 -Fhunk -o sage_blitter.o sage_blitter.asm

sage_ammxblit.o: sage_ammxblit.asm sage_blitter.h
	$(VASM) -m68080 -Fhunk -o sage_ammxblit.o sage_ammxblit.asm

sage_event.o: sage_event.c sage_event.h sage_frkey.h sage_enkey.h sage_dekey.h
	$(CC) sage_event.c $(OPT)

sage_screen.o: sage_screen.c sage_screen.h
	$(CC) sage_screen.c $(OPT)

sage_vblint.o: sage_vblint.asm sage_vblint.h
	$(VASM) -m68040 -Fhunk -o sage_vblint.o sage_vblint.asm

sage_layer.o: sage_layer.c sage_layer.h
	$(CC) sage_layer.c $(OPT)

sage_sprite.o: sage_sprite.c sage_sprite.h
	$(CC) sage_sprite.c $(OPT)

sage_tile.o: sage_tile.c sage_tile.h
	$(CC) sage_tile.c $(OPT)

sage_tilemap.o: sage_tilemap.c sage_tilemap.h
	$(CC) sage_tilemap.c $(OPT)

sage_draw.o: sage_draw.c sage_draw.h
	$(CC) sage_draw.c $(OPT)

sage_fastdraw.o: sage_fastdraw.asm sage_draw.h
	$(VASM) -m68040 -Fhunk -o sage_fastdraw.o sage_fastdraw.asm

sage_picture.o: sage_picture.c sage_picture.h
	$(CC) sage_picture.c $(OPT)

# Build input module
buildinput: $(INPUTOBJ)

sage_input.o: sage_input.c sage_input.h
	$(CC) sage_input.c $(OPT)

sage_keyboard.o: sage_keyboard.c sage_keyboard.h sage_frkey.h sage_enkey.h sage_dekey.h
	$(CC) sage_keyboard.c $(OPT)

sage_joyport.o: sage_joyport.c sage_joyport.h
	$(CC) sage_joyport.c $(OPT)

# Build audio module
buildaudio: $(AUDIOOBJ)

sage_audio.o: sage_audio.c sage_audio.h
	$(CC) sage_audio.c $(OPT)

sage_loadwave.o: sage_loadwave.c sage_loadwave.h
	$(CC) sage_loadwave.c $(OPT)

sage_load8svx.o: sage_load8svx.c sage_load8svx.h
	$(CC) sage_load8svx.c $(OPT)

sage_sound.o: sage_sound.c sage_sound.h
	$(CC) sage_sound.c $(OPT)

sage_loadtracker.o: sage_loadtracker.c sage_loadtracker.h
	$(CC) sage_loadtracker.c $(OPT)

sage_loadaiff.o: sage_loadaiff.c sage_loadaiff.h
	$(CC) sage_loadaiff.c $(OPT)

sage_music.o: sage_music.c sage_music.h
	$(CC) sage_music.c $(OPT)

# Build interruption module
buildinter: $(INTOBJ)

sage_interrupt.o: sage_interrupt.c sage_interrupt.h
	$(CC) sage_interrupt.c $(OPT)

sage_itserver.o: sage_itserver.asm
	$(VASM) -m68040 -Fhunk -o sage_itserver.o sage_itserver.asm

# Build 3D module
build3de: $(D3EOBJ)

sage_3d.o: sage_3d.c sage_3d.h
	$(CC) sage_3d.c $(OPT)

sage_math.o: sage_math.c sage_math.h
	$(CC) sage_math.c $(OPT)

# Build network module
buildnet: $(NETOBJ)

sage_network.o: sage_network.c sage_network.h
	$(CC) sage_network.c $(OPT)

# Clean files
clean: cleanlib cleanobj
	@echo "Clean complete"

cleanlib:
	rm $(SAGELIB)

cleanobj:
	rm *.o
