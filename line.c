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

// LINE.C : line drawing functions

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

#include "line.h"

#define LINE_T_MEMBERS \
	int dinc1, dinc2;	\
	char xinc1, xinc2;	\
	char yinc1, yinc2;	\
	int numpixels;		\
	int d

typedef struct {
	LINE_T_MEMBERS;
} line_t;

/*
	ok, what follows is quite an evil hack. lemme explain.

	we are using a kludge here, using local variables
	as if they were inside a struct.
	normally, you would do something like (line_t*)&dinc1 to
	accomplish that, however the HT compiler actually maps
	the variables backwards, meaning that we should point
	to the last local variable as if it were the first member
	of the struct.

	that's why we need REVERSE_LINE_T_MEMBERS and all this
	crap
*/

#define LINE_T_FIRST	dinc1

#define REVERSE_LINE_T_MEMBERS \
	int d;			\
	int numpixels;		\
	char yinc2, yinc1;	\
	char xinc2, xinc1;	\
	int dinc2, dinc1


// this macro is probably deprecated by now
/*
#define COPY_FROM_LINE_T(name)	\
	dinc1 = name.dinc1;	\
	dinc2 = name.dinc2;	\
	xinc1 = name.xinc1;	\
	xinc2 = name.xinc2;	\
	yinc1 = name.yinc1;	\
	yinc2 = name.yinc2;	\
	numpixels = name.numpixels;	\
	d = name.d
*/

void compute_line(int x1, int y1, int x2, int y2, line_t *r) {
	int deltax, deltay;

	deltax = (x2 - x1); if (deltax<0) deltax=-deltax;
	deltay = (y2 - y1); if (deltay<0) deltay=-deltay;

	if (deltax >= deltay) {
		r->numpixels = deltax + 1;
		r->d = (2 * deltay) - deltax;
		r->dinc1 = deltay << 1;
		r->dinc2 = (deltay - deltax) << 1;
		
		r->xinc1 = 1;
		r->xinc2 = 1;
		
		r->yinc1 = 0;
		r->yinc2 = 1;
	} else {
		r->numpixels = deltay + 1;
		r->d = (2 * deltax) - deltay;
		r->dinc1 = deltax << 1;
		r->dinc2 = (deltax - deltay) << 1;
		
		r->xinc1 = 0;
		r->xinc2 = 1;
		
		r->yinc1 = 1;
		r->yinc2 = 1;
	}

	if (x1 > x2) {
		r->xinc1 = -(r->xinc1);
		r->xinc2 = -(r->xinc2);
	}
	if (y1 > y2) {
		r->yinc1 = -(r->yinc1);
		r->yinc2 = -(r->yinc2);
	}
}

void line(int x1, int y1, int x2, int y2)
{
	REVERSE_LINE_T_MEMBERS;

	int i;
	u_int addr, last;
	u_char bit, value;

	compute_line(x1, y1, x2, y2, (line_t*)&LINE_T_FIRST);

	addr = map_pixel(x1, y1);
	bit = map_subpixel(x1);
	value = vpeek(addr);
	last = addr;

	for (i=0; i < numpixels; i++) {
		value |= bit;

		if (d<0) {
			d += dinc1;
			if (xinc1) {
				if (xinc1 > 0) {
					bit >>= 1;
					if (!bit) {
						addr += 8;
						bit = 128;
					}
				} else {
					bit <<= 1;
					if (!bit) {
						addr -=8;
						bit = 1;
					}
				}
			}
			if (yinc1) {
				if (yinc1 > 0) {
					addr += 1;
					if (!(addr & 7))
						addr += 256 - 8;
				} else {
					addr -= 1;
					if ((addr & 7) == 7)
						addr -= 256 - 8;
				}
			}
		}
		else
		{
			d += dinc2;
			if (xinc2) {
				if (xinc2 > 0) {
					bit >>= 1;
					if (!bit) {
						addr += 8;
						bit = 128;
					}
				} else {
					bit <<= 1;
					if (!bit) {
						addr -=8;
						bit = 1;
					}
				}
			}
			if (yinc2) {
				if (yinc2 > 0) {
					addr += 1;
					if (!(addr & 7))
						addr += 256 - 8;
				} else {
					addr -= 1;
					if ((addr & 7) == 7)
						addr -= 256 - 8;
				}
			}
		}
		if (last != addr) {
			vpoke(last, value);
			value = vpeek(addr);
			last = addr;
		}
	}
	vpoke(addr, value);
}

void line_slow(int x1, int y1, int x2, int y2)
{
	REVERSE_LINE_T_MEMBERS;

	int i, x, y;
	u_int addr, last;
	u_char value;

	compute_line(x1, y1, x2, y2, (line_t*)&LINE_T_FIRST);

	x = x1;
	y = y1;
	addr = map_pixel(x, y);
	last = addr;
	value = vpeek(addr);

	for (i=0; i < numpixels; i++) {
		value |= map_subpixel(x);
		if (d<0) {
			d += dinc1;
			x += xinc1;
			y += yinc1;
		}
		else
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
		addr = map_pixel(x, y);
		if (last != addr) {
			vpoke(last, value);
			value = vpeek(addr);
			last = addr;
		}
	}
	vpoke(addr, value);
}

void surface_line(surface_t* s, int x1, int y1, int x2, int y2) {
	REVERSE_LINE_T_MEMBERS;

	int i;
	u_int addr;
	u_char* data, bit;

	compute_line(x1, y1, x2, y2, (line_t*)&LINE_T_FIRST);

	data = s->data.ram;
	addr = map_pixel(x1, y1);
	bit = map_subpixel(x1);

	i = numpixels;
	while (i--) {
		data[addr] |= bit;

		if (d<0) {
			d += dinc1;
			if (xinc1) {
				if (xinc1 > 0) {
					bit >>= 1;
					if (!bit) {
						addr += 8;
						bit = 128;
					}
				} else {
					bit <<= 1;
					if (!bit) {
						addr -=8;
						bit = 1;
					}
				}
			}
			if (yinc1) {
				if (yinc1 > 0) {
					addr += 1;
					if (!(addr & 7))
						addr += 256 - 8;
				} else {
					addr -= 1;
					if ((addr & 7) == 7)
						addr -= 256 - 8;
				}
			}
		}
		else
		{
			d += dinc2;
			if (xinc2) {
				if (xinc2 > 0) {
					bit >>= 1;
					if (!bit) {
						addr += 8;
						bit = 128;
					}
				} else {
					bit <<= 1;
					if (!bit) {
						addr -=8;
						bit = 1;
					}
				}
			}
			if (yinc2) {
				if (yinc2 > 0) {
					addr += 1;
					if (!(addr & 7))
						addr += 256 - 8;
				} else {
					addr -= 1;
					if ((addr & 7) == 7)
						addr -= 256 - 8;
				}
			}
		}
	}
}

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void calculate_side(int x1, int y1, int x2, int y2, int low[], int high[]) {
	int step;
	int lines;
	int ly, hy, ux;
	int *pl, *ph;

	if (y2 < y1) {
		int t;
		t = x2; x2 = x1; x1 = t;
		t = y2; y2 = y1; y1 = t;		
	}
	lines = y2 - y1;

	pl = low + y1;
	ph = high + y1;

	if (!lines) {
		ly = *pl; hy = *ph;
		if (x1 < x2) {
			low[y1] = MIN(ly, x1); high[y1] = MAX(hy, x2);
		} else {
			low[y1] = MIN(ly, x2); high[y1] = MAX(hy, x1);
		}
		return;
	}

	step = i2f(x2 - x1);
	step /= lines;
	x1 = i2f(x1);

	while (lines--) {
		ly = *pl; hy = *ph;
		ux = f2i(x1);
		*pl++ = MIN(ly, ux); *ph++ = MAX(hy, ux);
		x1 += step;
	}
}

void surface_hline(surface_t* s, int x1, int y1, int x2, u_char value) {
	int bi, bf;
	u_char di, df, c;
	u_char *data;

	if (x1 > x2)
		return;

	data = s->data.ram + map_pixel(x1, y1);

	bi = x1 >> 3;
	bf = x2 >> 3;
	di = x1 & 7;
	if (bi == bf) {
		c = ~(u_char)(255 >> (u_char)(x2 - x1 + 1)) >> di;
		*data = (c & value) | (*data & ~c);
		return;
	}
	df = x2 & 7;
	if (di > 0) {
		c = 255 >> di;
		*data = (c & value) | (*data & ~c);
		data += 8;
		++bi;
	}
	if (df < 7) {
		--bf;
	}
	while (bi <= bf) {
		*data = value;
		data += 8;
		bi++;
	}
	if (df < 7) {
		c = ~(255 >> df);
		*data = (c & value) | (*data & ~c);
	}
}

void hline(int x1, int y1, int x2, u_char value) {
	int bi, bf;
	u_char di, df, c;
	int data;

	if (x1 > x2)
		return;

	data = map_pixel(x1, y1);

	bi = x1 >> 3;
	bf = x2 >> 3;
	di = x1 & 7;
	if (bi == bf) {
		c = ~(u_char)(255 >> (u_char)(x2 - x1 + 1)) >> di;
		vpoke(data, (c & value) | (vpeek(data) & ~c));
		return;
	}
	df = x2 & 7;
	if (di > 0) {
		c = 255 >> di;
		vpoke(data, (c & value) | (vpeek(data) & ~c));
		data += 8;
		++bi;
	}
	if (df < 7) {
		--bf;
	}
	while (bi <= bf) {
		vpoke(data, value);
		data += 8;
		bi++;
	}
	if (df < 7) {
		c = ~(255 >> df);
		vpoke(data, (c & value) | (vpeek(data) & ~c));
	}
}

#asm
psect data

global _dithpat
_dithpat:
	defb	00000000B
	defb	00000000B

	defb	00000000B
	defb	10101010B

	defb	01010101B
	defb	10101010B

	defb	11111111B
	defb	10101010B

	defb	11111111B
	defb	11111111B
#endasm
