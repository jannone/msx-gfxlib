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

/*! \file line.h
    \brief line drawing functions
*/
// LINE.H : line drawing functions (header)

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

#ifndef LINE_H
#define LINE_H

#include "gfx.h"

/// get a dithered pattern of intensity \a I on the screen line \a Y
#define DITHER(I, Y)	(dithpat[I][Y & 1])

extern u_char dithpat[5][2];

/// draw a line on a surface
void surface_line(surface_t*, int x1, int y1, int x2, int y2);

/// draw a line on video
void line(int x1, int y1, int x2, int y2);

/// draw a line on video (slow)
void line_slow(int x1, int y1, int x2, int y2);

/// calculate a triangle side
void calculate_side(int x1, int y1, int x2, int y2, int low[], int high[]);

/// draw horizontal line on video. \a value can be a bit pattern. note: x1 <= x2
void hline(int x1, int y1, int x2, u_char value);

/// draw horizontal line on surface. \a value can be a bit pattern. note: x1 <= x2
void surface_hline(surface_t *s, int x1, int y1, int x2, u_char value);

#endif
