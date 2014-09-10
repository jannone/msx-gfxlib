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

/*! \file defs.h
    \brief basic definitions and utilities
*/
// DEFS.H : shared defines

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

#ifndef DEFS_H
#define DEFS_H

// trivial stuff

#define u_int	unsigned int
#define u_char	unsigned char
#define bool	char
#define true	1
#define false	0
#ifndef NULL
	#define NULL 0
#endif

// screen mapping

/// maps a block in the screen 2 model
#define map_block(x,y)	((((y) & ~(7)) << 5) + ((x) & ~(7)))

/// maps a pixel coordinate to a vram address
#define map_pixel(x,y)	(map_block(x,y) + ((y) & 7))
/*
	here is how it works:

	// map the row start (by row I mean a block of 8 lines)
	addr = (y & ~(7)) << 5;		// this is the same as (y / 8) * 256

	// then we aim for the column (column = block of 8 pixels)
	addr += (x & ~(7));	// this is the same as (x / 8) * 8

	// finally, aim for the remaining "sub-row" inside the row block
	addr += (y & 7);
*/

/// maps the subpixel (bit) inside the vram byte
#define map_subpixel(x)	(128 >> ((x) & 7))

#define LONG(v)	((long)(v))

// fixed point arithmetic

/// integer to fixed-point
#define i2f(v)	((v) << 6)

/// fixed-point to integer
#define f2i(v)	((v) >> 6)

/// fixed-point multiplication
#define mulfx(x,y)	((LONG(y) * LONG(x)) >> 6)

/// fixed-point division
#define divfx(x,y)	((LONG(x) << 6) / LONG(y))

/// fixed-point square
#define sqrfx(x)	((LONG(x) * LONG(x)) >> 6)

/// fixed-point square root
#define sqrtfx(x)	(LONG(sqrt(x)) << 3)
// formula: sqrt(N * 64) = sqrt(N) * sqrt(64) -> must compensate with 64/sqrt(64) = 8

/// weighted average (w=0.0 -> x, w=0.5->average, w=1.0 ->y)
#define wgavgfx(x, y, w)	(mulfx(i2f(1) - w, x) + mulfx(w, y))

// malloc helpers

/// allocates space for 1 element of type \a x
#define new(x)		((x*)malloc(sizeof(x)))

/// allocates space for \a c elements of type \a x
#define newa(x, c)	((x*)malloc(sizeof(x) * c))

#endif
