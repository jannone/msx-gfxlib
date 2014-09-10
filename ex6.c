/*=========================================================================

GFX EXAMPLE CODE - #6
	"mangled screen"

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

#include <stdio.h>
#include "gfx.h"

#asm
psect data

global _diamond
_diamond:
	defb	00010000B
	defb	00111000B
	defb	01011100B
	defb	11111110B
	defb	01110100B
	defb	00111000B
	defb	00010000B
	defb	00000000B

global _diamond_attr
_diamond_attr:
	defb	070H
	defb	070H
	defb	070H
	defb	0F0H
	defb	050H
	defb	050H
	defb	050H
	defb	000H

#endasm

extern u_char diamond[8];
extern u_char diamond_attr[8];

main() {
	int d;
	int x, y;

	set_color(15, 1, 1);
	set_mangled_mode();

	set_sprite_mode(sprite_default);

	// set char '&' to a diamond shape WITH colors (attributes)
	set_char('&', diamond, diamond_attr, 0, place_all);

	// also set sprite 0 with the diamond shape
	set_sprite_8(0, diamond);

	// print the '&' in the middle of the screen
	vpoke(6144 + 32 * 12 + 16, '&');

	while (!get_trigger(0)) {
		// show the sprite into whatever position the joystick is pointing
		d = get_stick(0);
		switch (d) {
			case 0: x = 0; y = 0; break;
			case 1: x = 0; y = -16; break;
			case 2: x = 16; y = -16; break;
			case 3: x = 16; y = 0; break;
			case 4: x = 16; y = 16; break;
			case 5: x = 0; y = 16; break;
			case 6: x = -16; y = 16; break;
			case 7: x = -16; y = 0; break;
			case 8: x = -16; y = -16; break;
		}
		put_sprite_8(0, 128 + x, 96 + y, 0, 15);
	}

	set_mode(mode_0);
}

