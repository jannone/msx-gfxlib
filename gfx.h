/*=========================================================================

GFX - a small graphics library 

Copyright (C) 2004  Rafael de Oliveira Jannone

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Contact the author:
	by e-mail : rafael AT jannone DOT org
	homepage  : http://jannone.org/gfxlib
	ICQ UIN   : 10115284

See the License at http://www.gnu.org/copyleft/lesser.txt

=========================================================================*/

/*! \file gfx.h
    \brief main library functions
*/
// GFX.H : main library functions (header)

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

#ifndef GFX_H
#define GFX_H

#include "defs.h"

// --- library functions / enums

/// @name Video
/// VRAM and VDP related functions
//@{

/// video modes
enum video_mode {
	mode_0 = 0x6C,
	mode_1 = 0x6F,
	mode_2 = 0x72,
	mode_3 = 0x75
};

/// mangled screen sections (3 maps)
enum screen_map {
	place_1 = 1,
	place_2 = 2,
	place_3 = 4,
	place_all = 255
};

/// screen 2 section bytecount
#define MODE2_MAX	(256 * 24)

/// screen 2 attributes section start address
#define MODE2_ATTR	(8192)

/// screen 2 width in pixels
#define MODE2_WIDTH	256

/// screen 2 height in pixels
#define MODE2_HEIGHT	192

// VDP and VRAM functions

/// set screen mode
extern void set_mode(u_int mode);

/// set screen to mangled mode (screen 1 + 2)
extern void set_mangled_mode();

/// set screen color
extern void set_color(u_char front, u_char back, u_char border);

/// fill vram from \a addr, with \a value, for \a count bytes
extern void fill(u_int addr, u_char value, u_int count);

/// set \a value at a given vram address \a addr
extern void vpoke(u_int addr, u_char value);

/// get value from a given vram address \a addr
extern u_char vpeek(u_int addr);

/// set \a value at a given vram address \a addr, merging bits (OR) with the existing value
extern void vmerge(u_int addr, u_char value);

/// transfer \a count bytes from ram to vram
extern void vwrite(void* source, u_int dest, u_int count);

/// transfer \a count bytes from vram to ram
extern void vread(u_int source, void* dest, u_int count);

/// set a vdp register with a \a value
extern void set_vdp(u_char reg, u_char value);

/// get a vdp value from register \a reg
extern u_char get_vdp(u_char reg);

/// move the screen cursor to a given position
extern void locate(u_char x, u_char y);


// primitives (not many yet :))

/// vertical fill on vram starting at adress \a addr, of a given \a value, for \a count lines
extern void fill_v(u_int addr, u_char value, u_char count);

/// set point at the given position on vram
extern void pset(int x, int y);


// mangled mode chars

/// set char \a c shape, from \a form, at the given screen map \a place
void set_char_form(char c, void* form, u_char place);

/// set char \a c attributes, from \a attr, at the given screen map \a place
void set_char_attr(char c, void *attr, u_char place);

/// set char \a c with \a color, at the given screen map \a place
void set_char_color(char c, u_char color, u_char place);

// set char \a c shape, attributes and color, all in one
void set_char(char c, void* form, void *attr, u_char color, u_char place);

//@}


/// @name Sprites
//@{

/// sprite data
typedef struct {
        u_char y;	///< Y position
        u_char x;	///< X position
        u_char handle;	///< internal vdp handle
        u_char color;	///< sprite color
} sprite_t;

/// sprite modes
enum sprite_mode {
	sprite_default = 0,
	sprite_scaled = 1,
	sprite_large = 2
};

/// set the sprite \a mode
extern void set_sprite_mode(u_char mode);

/*

// this is not compiling... I suggest some #define's instead

extern void *set_sprite(u_char, void*);
extern void *put_sprite(u_char, int, int, u_char, u_char);
*/

/// set the sprite \a handle, with the shape from \a data (small size)
extern void set_sprite_8(u_char handle, void* data);

/// set the sprite \a handle, with the shape from \a data (big size)
extern void set_sprite_16(u_char handle, void* data);

/// put the sprite with \a id and shape from \a handle, into the given position with \a color (small size)
extern void put_sprite_8(u_char id, int x, int y, u_char handle, u_char color);

/// put the sprite with \a id and shape from \a handle, into the given position with \a color (big size)
extern void put_sprite_16(u_char id, int x, int y, u_char handle, u_char color);

//@}


// surface
// FIXME: this is not usable right now

/// represents a drawing surface
typedef struct {
        int width;
        int height;
	int type;
	union {
        u_char* ram;	///< ram adress, for off-screen surfaces
		u_int vram;
	} data;
} surface_t;

enum surface_type {
	surface_ram,
	surface_vram	
};

typedef struct {
	int x, y;
	int width, height;
} rect_t; 

extern void blit(surface_t *source, surface_t *dest, rect_t *from, rect_t *to);
extern void blit_ram_vram(u_char* source, u_int dest, u_char w, u_char h, int sjmp, int djmp);
extern void blit_fill_vram(u_int dest, u_char value, u_char w, u_char h, int djmp);


/// @name Controllers
//@{

enum stick_direction {
	st_up = 1,
	st_right = 2,
	st_down = 4,
	st_left = 8
};

extern u_char st_dir[];

/// get state of joystick number \a id
extern u_char get_stick(u_char id);

/// get state of joystick button (trigger) number \a id, true = pressed
extern bool get_trigger(u_char id);

//@}


/// @name Random
/// Fast and dirty pseudo-random number generator
//@{

/// seed the pseudo-random number generator
extern void seed_rnd(int seed);

/// get the next number from the pseudo-random number generator
extern u_char get_rnd();

//@}

/// @name PSG
/// PSG (sound generator) manipulation functions
//@{

/// alias for setting psg registers (for the BASIC fans)
#define sound(reg, value)	psg_set(reg, value)

/// convert a given frequency into a suitable period for PSG
#define psgT(hz)		((int)(111760.0 / (hz)))

enum {	
	chanNone = 0,	///< no channel
	chan0 = 1,	///< the first audio channel
	chan1 = 2,	///< the second audio channel
	chan2 = 4,	///< the third audio channel
	chanAll = 7	///< all audio channels
};

/// volume envelopes, where U = up, D = down, H = high
enum {
	envD = 0,	///< envelope, falling into silence
	envU = 4,	///< envelope, raising to highest volume, then silenced
	envDD = 8,	///< envelope, falling into silence multiple times
	envDUD = 10,	///< envelope, first falling, and then triangle shaped
	envDH = 11,	///< envelope, falling into silence, then abrupt high volume
	envUU = 12,	///< envelope, raising until top multiple times
	envUH = 13,	///< envelope, raising until top and then mantaining high volume
	envUDUD = 14	///< envelope, triangle shaped
};

/// set a psg register with a \a value
void psg_set(u_char reg, u_char value);

/// get value from psg register
u_char psg_get(u_char reg);

/// initialize psg
void psg_init();

/// set a given tone for the channel (0-2)
void psg_tone(u_char channel, int period);

/// set the global noise period
void psg_noise(u_char period);

/// set channel's volume
void psg_volume(u_char channel, u_char volume);

/// set the volume envelope of number \a waveform, with the given period, on a group of channels (ORed bits)
void psg_envelope(u_char waveform, int period, u_char channels);

/// set noise or tone generation on a group of channels (ORed bits)
void psg_channels(u_char tone_channels, u_char noise_channels);

/// get the group of channels currently generating tone (ORed bits)
u_char psg_tone_channels();

/// get the group of channels currently generating noise (ORed bits)
u_char psg_noise_channels();

/// initialize a lookup table for tones (in period units)
void psg_init_tone_table(int tones[128]);

//@}

#endif

