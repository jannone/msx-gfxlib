/*=========================================================================

GFX EXAMPLE CODE - #12
	"PSG sound"

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

main() {
	int tone[128];

	// prepare for the lamest example ever :)
	// at least is very simple and self-explained

	printf("initializing tone table\n");

	psg_init_tone_table(tone);

	printf("initializing psg\n");

	psg_init();

	printf("sound output\n");

	// psgT transforms a frequency (in Hertz) to a suitable period value for PSG

	psg_channels(chanAll, chanNone); // set all channels to tone generation

	// the following frequencies are based on the 440 Hz (central A tone) tunning
	/*
	psg_tone(0, psgT(130.8)); // produce a C tone on the first channel
	psg_tone(1, psgT(164.8)); // produce a E tone on the second channel
	psg_tone(2, psgT(195.9)); // produce a G tone on the third channel
	*/

	// this is the equivalent chord using the precalculated tone table
	psg_tone(0, tone[48]);
	psg_tone(1, tone[52]);
	psg_tone(2, tone[55]);

	// both methods should give the SAME result
	printf("tone table values: %d %d %d\n", tone[48], tone[52], tone[55]);
	printf("psgT calculated  : %d %d %d\n", psgT(130.8), psgT(164.8), psgT(195.9));

	psg_envelope(envUH, psgT(10), chanAll); // set a raising volume envelope on all channels

	printf("press spacebar to stop\n");

	while (!get_trigger(0)) {}

	psg_envelope(envD, psgT(10), chanAll); // set a fading volume envelope on all channels
}
