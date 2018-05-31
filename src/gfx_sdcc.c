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

void relocate_callbios_from_rom_to_ram() __naked {
__asm

callbios_ram_1      .equ #callbios_rom_1 - #callbios_rom_start + #callbios
callbios_rom_length .equ #callbios_rom_end - #callbios_rom_start

  push af
  push bc
  push de
  push hl
  ld hl, #callbios_rom_start
  ld de, #callbios
  ld bc, #callbios_rom_length
  ldir
  pop hl
  pop de
  pop bc
  pop af
  ret
  
callbios_rom_start:
  ld      (callbios_ram_1), ix
  rst     #0x30
callbios_rom_0:
  .db    0
callbios_rom_1:
  .dw    0
  ret
callbios_rom_end:
  
.area   _DATA
callbios:
  .ds #callbios_rom_length
.area   _CODE           
__endasm;
}

const u_char st_dir[]={
  0x00, // 0
  0x01, // 1
  0x03, // 2
  0x02, // 3
  0x06, // 4
  0x04, // 5
  0x0C, // 6
  0x08, // 7
  0x09  // 8
};

void init() {
  // TODO: delimitare con #ifdef ROM ?
  relocate_callbios_from_rom_to_ram();
}

#define _init_sprites()

#pragma disable_warning 85
void set_vdp(u_char reg, u_char value) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_wrtvdp .equ #0x0047

	push bc

	ld c, 4(ix)
	ld b, 5(ix)

	ld ix, #f_wrtvdp
	call callbios

	pop bc

	pop ix           ;epilogue
	ret

__endasm;
}

void set_mode(u_int mode) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	push bc
	ld c, 4(ix)
	ld b, 5(ix)
	push bc
	pop ix
	call callbios
	pop bc

	pop ix           ;epilogue
	ret
                            
__endasm;
	_init_sprites();
}

void set_color(u_char front, u_char back, u_char border) {
	*(u_char*)0xf3e9 = front;
	*(u_char*)0xf3ea = back;
	*(u_char*)0xf3eb = border;
__asm
	f_chgclr .equ #0x0062

	push ix
	ld ix, #f_chgclr
	call callbios
	pop ix	
__endasm;
}

void cls()
{
__asm
        push ix

        f_cls .equ #0x00C3

        push af
	push bc
	push de  
	push hl

	ld ix, #f_cls
	call callbios

	pop hl
	pop de
	pop bc
        pop af

	pop ix           ;epilogue
	ret
                            
__endasm;
}

#pragma disable_warning 85
void fill(u_int addr, u_char value, u_int count) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

        f_filvrm .equ #0x0056

        push af
	push bc
	push hl

	ld l, 4(ix)
	ld h, 5(ix)
	ld c, 7(ix)
	ld b, 8(ix)
	ld a, 6(ix)

	ld ix, #f_filvrm
	call callbios

	pop bc
	pop hl
	pop af

	pop ix           ;epilogue
	ret
                            
__endasm;
}

void vpoke(u_int addr, u_char value) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_wrtvrm .equ #0x004D

	;push hl
	;push ix

	;ld l, (ix+6)
	;ld h, (ix+7)
	;ld a, (ix+8)
	
	;ld ix, f_wrtvrm
	;call callbios

	;pop ix
	;pop hl

	p_vdp_data .equ #0x98
	p_vdp_cmd  .equ #0x99

	; enter vdp address pointer

	ld a, 4(ix)
	;di
	out (p_vdp_cmd), a
	ld a, 5(ix)
	and #0x3f
	or  #0x40
	;ei
	out (p_vdp_cmd), a

	; enter data

	ld a, 6(ix)
	out (p_vdp_data), a

	pop ix           ;epilogue
	ret
		
__endasm;
}

u_char vpeek(u_int addr) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_rdvrm .equ 0x004A

	;push hl
	;push ix
	;ld l, 6(ix)
	;ld h, 7(ix)
	;ld ix, f_rdvrm
	;call callbios
	;pop ix
	;pop hl
	;ld l, a

	; enter vdp address pointer

	ld a, 4(ix)
	;di
	out (p_vdp_cmd), a
	ld a, 5(ix)
	and #0x3f
	;ei
	out (p_vdp_cmd), a

	; read data

	in a, (p_vdp_data)
	ld l, a

	pop ix           ;epilogue
	ret

__endasm;
}

void vmerge(u_int addr, u_char value) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	;f_wrtvrm .equ 0x004D
	;f_rdvrm .equ 0x0004A

	;push hl
	;push ix

	;ld l, 6(ix)
	;ld h, 7(ix)
	;ld ix, f_rdvrm
	;call callbios

	;pop ix
	;push ix

	;or 8(ix)
	;ld ix, f_wrtvrm
	;call callbios

	;pop ix
	;pop hl

	; enter vdp address pointer

	ld l, 4(ix)
	ld h, 5(ix)
	ld c, #p_vdp_cmd
	ld b, c

	;di
	out (c), l
	ld a, h
	and #0x3f
	;ei
	out (c), a

	; read data

	ld c, #p_vdp_data
	in h, (c)
	ld c, b

	; enter same address

	;di
	out (c), l
	or  #0x40
	;ei
	out (c), a

	ld a, 6(ix)
	out (p_vdp_data), a

	pop ix           ;epilogue
	ret

__endasm;
}

void vwrite(void *source, u_int dest, u_int count) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_ldirvm .equ 0x0005C

	push hl
	push bc
	push de
	push ix

	ld l, 4(ix)
	ld h, 5(ix)
	ld e, 6(ix)
	ld d, 7(ix)
	ld c, 8(ix)
	ld b, 9(ix)

	ld ix, #f_ldirvm
	call callbios

	pop ix
	pop de
	pop bc
	pop hl

	pop ix           ;epilogue
	ret

__endasm;
}

void vread(u_int source, void* dest, u_int count) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_ldirmv .equ 0x0056

	push hl
	push bc
	push de
	push ix

	ld l, 4(ix)
	ld h, 5(ix)
	ld e, 6(ix)
	ld d, 7(ix)
	ld c, 8(ix)
	ld b, 9(ix)

	ld ix, #f_ldirmv
	call callbios

	pop ix
	pop de
	pop bc
	pop hl

	pop ix           ;epilogue
	ret

__endasm;
}

void chput(char value)
{
value;
__asm
	f_chput .equ 0x00A2

  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   A,4(IX)   
  call f_chput
  
  pop  IX
__endasm;
}

void locate(u_char x, u_char y) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_posit .equ 0x00C6
	
	push hl
	push ix

	ld h, 4(ix)
	ld l, 5(ix) ; --- TODO: controllare se va 6(ix) o 5(ix)
	
	ld ix, #f_posit
	call callbios

	pop ix
	pop hl

	pop ix           ;epilogue
	ret

__endasm;
}

u_char get_stick(u_char id) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_gtstck .equ 0x00D5

	push hl
	push de
	push bc
	push ix

	ld a, 4(ix)
	ld ix, #f_gtstck
	call callbios

	pop ix
	pop bc
	pop de
	pop hl
	
	ld l, a

	pop ix           ;epilogue
	ret

__endasm;
}

bool get_trigger(u_char id) __naked {
__asm

	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp

	f_gttrig .equ 0x00D8

	push ix
	ld a, 4(ix)

	ld ix, #f_gttrig
	call callbios
	pop ix
	ld l, a

	pop ix           ;epilogue
	ret

__endasm;
}


void psg_init() {
__asm
	f_gicini .equ 0x0090

	push ix
	ld ix, #f_gicini
	call callbios
	pop ix
__endasm;
}

void psg_set(u_char reg, u_char value) __naked {
/*
	WRTPSG (0093H)		*1
	Function:  writes data in the PSG register
	Input:     A for PSG register number, E for data
	Output:    none
	Registers: none

	RDPSG (0096H)		*1
	Function:  reads the PSG register value
	Input:     A for PSG register number
	Output:    A for the value which was read
	Registers: none
*/
__asm
	push	ix  ; prologue
	ld	ix, #0x0000
	add	ix, sp

	f_wrtpsg .equ 0x0093
	f_rdpsg  .equ 0x0096

  push af
	push de
	ld a, 4(ix)          
  cp #0x07
  jr nz, 00001$ ; if not reg 7 then perform the usual wrtpsg call ...
          
	push ix
	ld ix, #f_rdpsg ; ... else mask bits 6 and 7 of register 7
	call callbios
	pop ix

  and #0xc0
	ld d, a ; save bits 6 and 7 in reg d
	ld a, 5(ix)
	and #0x3f
	or d
	ld e, a ; e <- { 4(ix) masked by d }
	ld a, #0x07
	jr 00002$                     
                        
00001$:	ld e, 5(ix)
00002$:	push ix
	ld ix, #f_wrtpsg
	call callbios
	pop ix

	pop de
	pop af

	pop ix           ;epilogue
	ret

__endasm;
}

u_char psg_get(u_char reg) __naked {
__asm
	push	ix  	; prologue
	ld	ix,#0
	add	ix,sp
  
	f_rdpsg .equ 0x0096

	ld a, 4(ix)
	push ix
	ld ix, #f_rdpsg
	call callbios
	pop ix

        ld h, #0x00
        ld l, a

	pop ix           ;epilogue
	ret
__endasm;
}

#pragma disable_warning 93
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
