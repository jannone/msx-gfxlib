/*=========================================================================

GFX EXAMPLE CODE - #7
	"starship game, a recap of all things"

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

// note: this is my personal favorite demo :) - Rafael

#include "gfx.h"

// shapes and attributes
#ifdef __SDCC
extern const u_char grass1[8];
extern const u_char grass1_attr[8];
extern const u_char grass2[8];
extern const u_char grass2_attr[8];
extern const u_char water1[8];
extern const u_char water2[8];
extern const u_char water3[8];
 
extern const u_char spaceship[32];
extern const u_char fire[32];
#else

extern u_char grass1[8];
extern u_char grass1_attr[8];
extern u_char grass2[8];
extern u_char grass2_attr[8];
extern u_char water1[8];
extern u_char water2[8];
extern u_char water3[8];

extern u_char spaceship[32];
extern u_char fire[32];
#endif

// terrain definitions

#define MAP_WIDTH	32
#define MAP_HEIGHT	24
#define MAP_SIZE	MAP_WIDTH * MAP_HEIGHT


// helper

#define KEEP_RANGE(var, min, max) \
	if (var < min) var = min; else if (var > max) var = max


// terrain functions

void show_map(u_char *map, int line_start) {
	int y, addr;
	u_char *l;

	l = map + (line_start * MAP_WIDTH);
	addr = 6144;
	for (y = line_start; y < MAP_HEIGHT; y++) {
		vwrite(l, addr, MAP_WIDTH);
		addr += MAP_WIDTH;
		l += MAP_WIDTH;
	}
	l = map;
	for (y = 0; y < line_start; y++) {
		vwrite(l, addr, MAP_WIDTH);
		addr += MAP_WIDTH;
		l += MAP_WIDTH;
	}
}

void map_fill_line(u_char* line, int river_width, int river_offs) {
	int river_left;
	u_char *last;

	river_left = (MAP_WIDTH - river_width) / 2 + river_offs;
	last = line + MAP_WIDTH;

	while (river_left--)
		*line++ = (get_rnd() & 1) + '%';

	while (river_width--)
		*line++ = (get_rnd() % 3) + '"';

	while (line < last)
		*line++ = (get_rnd() & 1) + '%';
}

void randomize_river(int *width, int *offs) {
	*width += (get_rnd() & 2) - 1;
	*offs  += (get_rnd() & 2) - 1;
	KEEP_RANGE(*width, 8, 16);
	KEEP_RANGE(*offs, -4, 4);
}


// player data

typedef struct {
	int x;
	int y;	
	int vx, vy;
	bool trig;
} player_t;


// rocket data

typedef struct {
	int x;
	int y;
	u_char sp;
	bool active;
} rocket_t;


// maximum number of rockets at a time

#define MAX_ROCKET	3


// rocket functions

rocket_t* alloc_rocket(rocket_t *array) {
	int c;
	for (c = 0; c < MAX_ROCKET; c++) {
		if (!array->active) {
			array->active = true;
			array->sp = 10 + c;
			return array;
		}
		++array;
	}
	return NULL;
}

void free_rocket(rocket_t *r) {
	r->active = false;
	put_sprite_16(r->sp, 0, 192, 0, 0);
}

void initialize_rockets(rocket_t *array) {
	int c;
	for (c = 0; c < MAX_ROCKET; c++)
		free_rocket(array++);
}

void move_rockets(rocket_t *array) {
	int c;
	for (c = 0; c < MAX_ROCKET; c++) {
		if (array->active) {
			if ((array->y -= 7) < 0)
				free_rocket(array);
			else
				put_sprite_16(array->sp, array->x, array->y, 1, 6);
		} 
		++array;
	}
}


// player functions

void move_player(player_t *player) {
	u_char s;
	char ax, ay;
	
	s = st_dir[get_stick(0)];

	ay = (s & st_up) ? -1 : ((s & st_down) ? 1 : 0);
	ax = (s & st_left) ? -1 : ((s & st_right) ? 1 : 0);
	
	player->vx += ax;
	player->vy += ay;
	KEEP_RANGE(player->vx, -3, 3);
	KEEP_RANGE(player->vy, -3, 3);

	player->x += player->vx;
	player->y += player->vy;

	put_sprite_16(0, player->x, player->y, 0, 15);
	put_sprite_16(31, player->x + 6, player->y + 6, 0, 1);		
}


// main :)

main() {
	// allocating stuff

	int c, river_width, river_offs;
	player_t player;
	rocket_t rockets[MAX_ROCKET], *rckt;

	u_char *l;
	u_char map[MAP_WIDTH * MAP_HEIGHT];

	// set screen
	set_color(15, 1, 1);
	set_mangled_mode();

	// set sprites
	set_sprite_mode(sprite_large);

	set_sprite_16(0, spaceship);
	set_sprite_16(1, fire);

	// set char shapes and colors
	set_char('"', water1, NULL, 0x54, place_all);
	set_char('#', water2, NULL, 0x54, place_all);
	set_char('$', water3, NULL, 0x54, place_all);
	set_char('%', grass1, grass1_attr, 0, place_all);
	set_char('&', grass2, grass2_attr, 0, place_all);

	// initializing 
	initialize_rockets(rockets);

	river_width = 16;
	river_offs = 0;

	l = map + MAP_SIZE - MAP_WIDTH;
	for (c = 0; c < MAP_HEIGHT; c++) {
		randomize_river(&river_width, &river_offs);
		map_fill_line(l, river_width, river_offs);
		l -= MAP_WIDTH;
	}

	c = 0;

	player.x = 128;
	player.y = 96;
	player.vx = player.vy = player.trig = 0;

	// main loop
	for (;;) {
		move_player(&player);

		player.trig |= get_trigger(0);

		if ((c & 2) && player.trig && (rckt = alloc_rocket(rockets))) {
			rckt->x = player.x + 4;
			rckt->y = player.y - 8;
			player.trig = false;
		}
		move_rockets(rockets);

		if (--c < 0)
			c += MAP_HEIGHT;
		l = map + c * MAP_WIDTH;
		randomize_river(&river_width, &river_offs);
		map_fill_line(l, river_width, river_offs);
		show_map(map, c);
	}
}

// binary data (shapes, attributes, whatever...)

#ifdef __SDCC

#define BITS_TO_BYTE(a,b,c,d,e,f,g,h) ((a)<<7 | (b)<<6 | (c)<<5 | (d)<<4 | (e)<<3 | (f)<<2 | (g)<<1 | (h))

const u_char grass1[8]={
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,1,1,1,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(1,0,0,0,0,0,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,1,1,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,1,0,0),
  BITS_TO_BYTE(0,0,0,1,1,0,1,0)
};

const u_char grass1_attr[8]={
  0x32,
  0x32,
  0x32,
  0xB2,
  0x32,
  0x32,
  0xB2,
  0xA2
};

const u_char grass2[8]={
  BITS_TO_BYTE(0,1,0,1,1,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(1,1,1,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,1,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,1,0,0,0,0,0)
};

const u_char grass2_attr[8]={
  0x0A2,
  0x032,
  0x032,
  0x0B2,
  0x032,
  0x032,
  0x032,
  0x0B2
};

const u_char water1[8]={
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(1,1,1,1,1,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,1,1,1,1,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0)
};

const u_char water2[8]={
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(1,1,1,1,1,1,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,1,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0)
};

const u_char water3[8]={
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0)
};

const u_char spaceship[32]={
  BITS_TO_BYTE(0,0,0,0,0,0,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,1,1),
  BITS_TO_BYTE(0,0,0,0,0,0,1,1),
  BITS_TO_BYTE(0,0,0,0,0,1,1,1),
  BITS_TO_BYTE(0,0,0,0,0,1,1,1),
  BITS_TO_BYTE(1,1,0,0,0,1,1,1),
  BITS_TO_BYTE(1,1,0,0,0,1,1,1),
  BITS_TO_BYTE(1,1,1,0,0,1,1,1),
  
  BITS_TO_BYTE(1,1,1,0,1,1,1,0),
  BITS_TO_BYTE(1,1,1,0,1,1,1,0),
  BITS_TO_BYTE(1,1,1,1,1,1,1,0),
  BITS_TO_BYTE(1,1,1,1,1,1,1,1),
  BITS_TO_BYTE(1,1,1,0,0,1,1,1),
  BITS_TO_BYTE(1,1,0,0,0,0,1,1),
  BITS_TO_BYTE(1,1,1,1,0,0,0,0),
  BITS_TO_BYTE(1,0,0,1,0,0,0,0),

  BITS_TO_BYTE(1,1,0,0,0,0,0,0),
  BITS_TO_BYTE(1,1,0,0,0,0,0,0),
  BITS_TO_BYTE(1,1,0,0,0,0,0,0),
  BITS_TO_BYTE(1,1,1,0,0,0,0,0),
  BITS_TO_BYTE(1,1,1,0,0,0,0,0),
  BITS_TO_BYTE(1,1,1,0,0,0,1,1),
  BITS_TO_BYTE(1,1,1,0,0,0,1,1),
  BITS_TO_BYTE(1,1,1,0,0,1,1,1),
  
  BITS_TO_BYTE(0,1,1,1,0,1,1,1),
  BITS_TO_BYTE(0,1,1,1,0,1,1,1),
  BITS_TO_BYTE(0,1,1,1,1,1,1,1),
  BITS_TO_BYTE(1,1,1,1,1,1,1,1),
  BITS_TO_BYTE(1,1,1,0,0,1,1,1),
  BITS_TO_BYTE(1,1,0,0,0,0,1,1),
  BITS_TO_BYTE(0,0,0,0,1,1,1,1),
  BITS_TO_BYTE(0,0,0,0,1,0,0,1)
};

const u_char fire[32]={
  BITS_TO_BYTE(0,0,0,1,1,0,0,0),
  BITS_TO_BYTE(0,0,1,1,1,1,0,0),
  BITS_TO_BYTE(0,0,1,1,1,1,0,0),
  BITS_TO_BYTE(0,1,1,1,1,1,1,0),
  BITS_TO_BYTE(0,1,1,1,1,1,1,0),
  BITS_TO_BYTE(0,1,1,1,1,1,1,0),
  BITS_TO_BYTE(0,1,1,1,0,1,1,0),
  BITS_TO_BYTE(0,1,0,1,0,1,0,0),

  BITS_TO_BYTE(0,0,0,1,0,0,0,0),
  BITS_TO_BYTE(0,0,0,1,0,0,0,0),
  BITS_TO_BYTE(0,0,0,1,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),

  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0),
  BITS_TO_BYTE(0,0,0,0,0,0,0,0)
};

#else

#asm
psect data

global _grass1
_grass1:
	defb	00000000B
	defb	00111000B
	defb	00000000B
	defb	10000011B
	defb	00000000B
	defb	00110000B
	defb	00000100B
	defb	00011010B

global _grass1_attr
_grass1_attr:
	defb	032h ;00000000B
	defb	032h ;00111000B
	defb	032h ;00000000B
	defb	0B2h ;10000011B
	defb	032h ;00000000B
	defb	032h ;00110000B
	defb	0B2h ;00000100B
	defb	0A2h ;00011010B

global _grass2
_grass2:
	defb	01011000B
	defb	00000000B
	defb	00000000B
	defb	11100000B
	defb	00000000B
	defb	00000111B
	defb	00000000B
	defb	00100000B

global _grass2_attr
_grass2_attr:
	defb	0A2h 
	defb	032h 
	defb	032h 
	defb	0B2h 
	defb	032h 
	defb	032h 
	defb	032h 
	defb	0B2h 

global _water1
_water1:
	defb	00000000B
	defb	11111000B
	defb	00000000B
	defb	00000000B
	defb	00111111B
	defb	00000000B
	defb	00000000B
	defb	00000000B

global _water2
_water2:
	defb	00000000B
	defb	00000000B
	defb	00000000B
	defb	00000000B
	defb	11111111B
	defb	00000000B
	defb	00000111B
	defb	00000000B

global _water3
_water3:
	defb	0,0,0,0,0,0,0,0

global _spaceship
_spaceship:
	defb	00000011B
	defb	00000011B
	defb	00000011B
	defb	00000111B
	defb	00000111B
	defb	11000111B
	defb	11000111B
	defb	11100111B
	defb	11101110B
	defb	11101110B
	defb	11111110B
	defb	11111111B
	defb	11100111B
	defb	11000011B
	defb	11110000B
	defb	10010000B

	defb	11000000B
	defb	11000000B
	defb	11000000B
	defb	11100000B
	defb	11100000B
	defb	11100011B
	defb	11100011B
	defb	11100111B
	defb	01110111B
	defb	01110111B
	defb	01111111B
	defb	11111111B
	defb	11100111B
	defb	11000011B
	defb	00001111B
	defb	00001001B

global _fire
_fire:
	defb	00011000B
	defb	00111100B
	defb	00111100B
	defb	01111110B
	defb	01111110B
	defb	01111110B
	defb	01110110B
	defb	01010100B
	defb	00010000B
	defb	00010000B
	defb	00010000B
	defb	00000000B
	defb	00000000B
	defb	00000000B
	defb	00000000B
	defb	00000000B
	defb	0,0,0,0,0,0,0,0
	defb	0,0,0,0,0,0,0,0

#endasm

#endif
