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

// GFX.C : main library functions

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

#include "gfx.h"


// do a interslot call to BIOS

#asm

psect text

;callbios: ld (callbios0), ix
;	defb 0CDh
;callbios0:
;	defw 0000h
;	ret

callbios:
        ld      (callbios1), ix
        rst     030h
callbios0:
        defb    0
callbios1:
        defw    0
        ret

psect data
global _st_dir
_st_dir:
	defb 0000B ; 0
	defb 0001B ; 1
	defb 0011B ; 2
	defb 0010B ; 3
	defb 0110B ; 4
	defb 0100B ; 5
	defb 1100B ; 6
	defb 1000B ; 7
	defb 1001B ; 8

#endasm

void init() {
#asm
	;in a, (0A8h)
	;and 0FCh
	;out (0A8h), a
#endasm
}

/*

// this is not currently compiling... must investigate

void *set_sprite(u_char, void*);
void *put_sprite(u_char, int, int, u_char, u_char);

void _init_sprites() {
	u_char m = get_vdp(1);
	if (m & sprite_large) {
		set_sprite = set_sprite_16;
		put_sprite = put_sprite_16;
	} else {
		set_sprite = set_sprite_8;
		put_sprite = put_sprite_8;
	}
}
*/
#define _init_sprites()

void set_vdp(u_char reg, u_char value) {
#asm
	f_wrtvdp equ 047h

	push bc
	push ix

	ld c, (ix+6)
	ld b, (ix+8)

	ld ix, f_wrtvdp
	call callbios
	
	pop ix
	pop bc
#endasm
}

void set_mode(u_int mode) {
#asm
	push bc
	push ix
	ld c, (ix+6)
	ld b, (ix+7)
	push bc
	pop ix
	call callbios
	pop ix
	pop bc
#endasm
	_init_sprites();
}

void set_color(u_char front, u_char back, u_char border) {
	*(u_char*)0xf3e9 = front;
	*(u_char*)0xf3ea = back;
	*(u_char*)0xf3eb = border;
#asm
	f_chgclr equ 062h

	push ix
	ld ix, f_chgclr
	call callbios
	pop ix	
#endasm
}

void cls()
{
#asm
	f_cls equ 0C3h

	push hl
	push de
	push bc
	push ix

	ld ix, f_cls
	call callbios
	pop ix
	pop bc
	pop de
	pop hl
#endasm
}

void fill(u_int addr, u_char value, u_int count) {
#asm
	f_filvrm equ 056h

	push hl
	push bc
	push ix

	ld l, (ix+6)
	ld h, (ix+7)
	ld c, (ix+10)
	ld b, (ix+11)
	ld a, (ix+8)

	ld ix, f_filvrm
	call callbios
	pop ix
	pop bc
	pop hl
#endasm
}

void vpoke(u_int addr, u_char value) {
#asm
	f_wrtvrm .equ 0x004D

	;push hl
	;push ix

	;ld l, (ix+6)
	;ld h, (ix+7)
	;ld a, (ix+8)
	
	;ld ix, f_wrtvrm
	;call callbios

	;pop ix
	;pop hl

	p_vdp_data .equ 0x098
	p_vdp_cmd  .equ 0x099

	; enter vdp address pointer

	ld a, (ix+6)
	;di
	out (p_vdp_cmd), a
	ld a, (ix+7)
	and 00111111B
	or  01000000B
	;ei
	out (p_vdp_cmd), a

	; enter data

	ld a, (ix+8)
	out (p_vdp_data), a
		
#endasm
}

u_char vpeek(u_int addr) {
#asm
	f_rdvrm equ 04Ah

	;push hl
	;push ix
	;ld l, (ix+6)
	;ld h, (ix+7)
	;ld ix, f_rdvrm
	;call callbios
	;pop ix
	;pop hl
	;ld l, a

	; enter vdp address pointer

	ld a, (ix+6)
	;di
	out (p_vdp_cmd), a
	ld a, (ix+7)
	and 00111111B
	;ei
	out (p_vdp_cmd), a

	; read data

	in a, (p_vdp_data)
	ld l, a
#endasm      
}

void vmerge(u_int addr, u_char value) {
#asm
	;f_wrtvrm equ 04Dh
	;f_rdvrm equ 04Ah

	;push hl
	;push ix

	;ld l, (ix+6)
	;ld h, (ix+7)
	;ld ix, f_rdvrm
	;call callbios

	;pop ix
	;push ix

	;or (ix+8)
	;ld ix, f_wrtvrm
	;call callbios

	;pop ix
	;pop hl

	; enter vdp address pointer

	ld l, (ix+6)
	ld h, (ix+7)
	ld c, p_vdp_cmd
	ld b, c

	;di
	out (c), l
	ld a, h
	and 00111111B
	;ei
	out (c), a

	; read data

	ld c, p_vdp_data
	in h, (c)
	ld c, b

	; enter same address

	;di
	out (c), l
	or  01000000B
	;ei
	out (c), a

	ld a, (ix+8)
	out (p_vdp_data), a
#endasm    
}

void vwrite(void *source, u_int dest, u_int count) {
#asm
	f_ldirvm equ 05Ch

	push hl
	push bc
	push de
	push ix

	ld l, (ix+6)
	ld h, (ix+7)
	ld e, (ix+8)
	ld d, (ix+9)
	ld c, (ix+10)
	ld b, (ix+11)

	ld ix, f_ldirvm
	call callbios

	pop ix
	pop de
	pop bc
	pop hl
#endasm
}

void vread(u_int source, void* dest, u_int count) {
#asm
	f_ldirmv equ 056h

	push hl
	push bc
	push de
	push ix

	ld l, (ix+6)
	ld h, (ix+7)
	ld e, (ix+8)
	ld d, (ix+9)
	ld c, (ix+10)
	ld b, (ix+11)

	ld ix, f_ldirmv
	call callbios

	pop ix
	pop de
	pop bc
	pop hl
#endasm
}

void locate(u_char x, u_char y) {
#asm
	f_posit equ 0C6h
	
	push hl
	push ix

	ld h, 6(ix)
	ld l, 8(ix)
	
	ld ix, f_posit
	call callbios

	pop ix
	pop hl
#endasm          
}

u_char get_stick(u_char id) {
#asm
	f_gtstck equ 0D5h

	push hl
	push de
	push bc
	push ix

	ld a, (ix+6)
	ld ix, f_gtstck
	call callbios

	pop ix
	pop bc
	pop de
	pop hl
	
	ld l, a
#endasm
}

bool get_trigger(u_char id) {
#asm
	f_gttrig equ 0D8h

	push ix
	ld a, (ix+6)

	ld ix, f_gttrig
	call callbios
	pop ix
	ld l, a	
#endasm
}


void psg_init() {
#asm
	f_gicini equ 090h

	push ix
	ld ix, f_gicini
	call callbios
	pop ix
#endasm       
}

void psg_set(u_char reg, u_char value) {
#asm
	f_wrtpsg equ 093h
	f_rdpsg  equ 096h

	push de
	push ix
	ld a, (ix + 8)
	ld e, a
	ld a, (ix + 6)
	ld ix, f_wrtpsg
	call callbios
	pop ix
	pop de
#endasm          
}

u_char psg_get(u_char reg) {
#asm
	f_rdpsg equ 096h

	push ix
	ld a, (ix + 6)
	ld ix, f_rdpsg
	call callbios
	pop ix
#endasm          
}

void psg_init_tone_table(int tones[128]) {
	// this is not the most precise method of calculation
	// but it is fast!
	// also, the accumulated error is within a safe margin
	int c;
	double n = 8.1757989156;
	double s = 1.0594630943;
	tones[0] = psgT(n);
	for (c = 1; c < 128; c++) {
		n *= s;
		tones[c] = psgT(n);
	}
}

// TODO: reg 7 masking
