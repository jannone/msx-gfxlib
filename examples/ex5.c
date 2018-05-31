/*=========================================================================

GFX EXAMPLE CODE - #5
	"the evil eye, line drawing"

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
#include <stdio.h>
#include <math.h>
#include "gfx.h"
#include "line.h"

#ifdef __SDCC
#include "defs_sdcc.h"
#endif

typedef struct {
	int x;
	int y;
} point_t;

#define MAX_POINT	9

main() {
	u_char* buf;
	double	M_PI;
	double	a;
	int	c, i;
	surface_t surf;
	point_t	p[MAX_POINT];
#ifdef __SDCC
       // init();
#endif
    buf = (u_char*)malloc(MODE2_MAX);
    printf("calculating, wait...\n");

	M_PI = 8.0 * atan(1.0);

	// calculates points from circumference
	for (c = 0; c < MAX_POINT; c++) {
		a = (M_PI * (double)c) / (double)MAX_POINT;
		p[c].x = (int)(100.0 * cos(a) + 128.0);
		p[c].y = (int)(80.0 * sin(a) + 96.0);
	}

	// clear the off-screen surface
	 printf("clearing buffer...\n");
#ifdef __SDCC
	memset(buf, 0, MODE2_MAX);
#else
	memset(buf, MODE2_MAX, 0);
#endif
	printf("drawing...\n");
	surf.data.ram = buf;

	// draw the eye's lines into the surface (obs: we are NOT in graphic mode yet)
	for (c = 0; c < MAX_POINT; c++)
	 	for (i = c+1; i < MAX_POINT; i++)
	 		surface_line(&surf, p[c].x, p[c].y, p[i].x, p[i].y);

	// set screen to graphic mode
	set_color(15, 1, 1);
	set_mode(mode_2);

	fill(MODE2_ATTR, 0xF1, MODE2_MAX);

	// finally show the surface
	vwrite(surf.data.ram, 0, MODE2_MAX);

	// draw the eye's lines into the surface (obs: we are NOT in graphic mode yet)
	//for (c = 0; c < MAX_POINT; c++)
	// 	for (i = c+1; i < MAX_POINT; i++)
	// 		line_slow(p[c].x, p[c].y, p[i].x, p[i].y);

	while (!get_trigger(0)) {}

	set_mode(mode_0);
}
