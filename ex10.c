/*=========================================================================

GFX EXAMPLE CODE - #10
	"shaded triangle"

Copyright (C) 2004  Rafael de Oliveira Jannone

This example's source code is Public Domain.

WARNING: The author makes no guarantees and holds no responsibility for 
any damage, injury or loss that may result from the use of this source 
code. USE IT AT YOUR OWN RISK.

Contact the author:
	by e-mail : rafael AT jannone DOT org
	homepage  : http://jannone.org/gfxlib
	ICQ UIN   : 10115284

=========================================================================*/

#include <stdlib.h>
#include "line.h"

int low[MODE2_HEIGHT];
int high[MODE2_HEIGHT];
u_char *buf;

main() {
	int c, l;
	u_char i;
	surface_t surf;

	buf = (u_char*)malloc(MODE2_MAX);
	surf.data.ram = buf;

	set_color(15, 1, 1);
	set_mode(mode_2);
	fill(MODE2_ATTR, 0x31, MODE2_MAX);

	c = 0;

	// paint polygon
	for (;;) {	
		memset(buf, MODE2_MAX, 0);	// yeah... crap
		memset(low, MODE2_HEIGHT << 1, 64);	// yeah... crap
		memset(high, MODE2_HEIGHT << 1, 0);	// yeah... crap

		// calculate polygon
		calculate_side(128 + c, c + 20, 40, 180 - c, low, high);
		calculate_side(128 + c, c + 20, 255 - c, 96, low, high);
		calculate_side(255 - c, 96, 40, 180 - c, low, high);
		c = (c + 4) & 63;
		i = (c + 15) >> 4;

		for (l = 20; l < 180; l++) {
			surface_hline(&surf, low[l], l, high[l], DITHER(i, l));
		}
		vwrite(surf.data.ram, 0, MODE2_MAX);
		if (get_trigger(0))
			break;
	}

	set_mode(mode_0);
}
