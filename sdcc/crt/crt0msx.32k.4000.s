; crt0 for MSX ROM of 32KB, starting at 0x4000
; suggested options: --code-loc 0x4020 --data-loc 0xc000

.globl	_init
.globl	_main
.globl _putchar
.globl _getchar

.globl s__INITIALIZER
.globl s__INITIALIZED
.globl l__INITIALIZER
        
.area _HEADER (ABS)
; Reset vector
	.org 0x4000
	.db  0x41
	.db  0x42
	.dw  init
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000

	;; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
	.area	_INITIALIZER
	.area   _GSINIT
	.area   _GSFINAL

	.area	_DATA
	.area	_INITIALIZED
	.area	_BSEG
	.area   _BSS
	.area   _HEAP

	.area	_CODE

init:
	ld      sp,(0xfc4a) ; Stack at the top of memory.
	call    find_rom_page_2 ; map 2nd slot the same as slot used at 0x4000
	call    gsinit ; Initialise global variables
	call    _init ; init gfxlib
	call    _main 
	call    #0x0000; call CHKRAM

;------------------------------------------------
; find_rom_page_2
; original name     : LOCALIZAR_SEGUNDA_PAGINA
; Original author   : Eduardo Robsy Petrus
; Snippet taken from: http://karoshi.auic.es/index.php?topic=117.msg1465
;
; Rutina que localiza la segunda pagina de 16 KB
; de una ROM de 32 KB ubicada en 4000h
; -Basada en la rutina de Konami-
; Compatible con carga en RAM
; Compatible con expansores de slots
;------------------------------------------------
; Comprobacion de RAM/ROM

find_rom_page_2::
	ld hl, #0x4000
	ld b, (hl)
	xor a
	ld (hl), a
	ld a, (hl)
	or a
	jr nz,5$ ; jr nz,@@ROM
	; El programa esta en RAM - no buscar
	ld (hl),b
	ret
5$: ; ----------- @@ROM:
	di
	; Slot primario
	call #0x0138 ; call RSLREG
	rrca
	rrca
	and #0x03
	; Slot secundario
	ld c, a
	ld hl, #0xfcc1
	add a, l
	ld l, a
	ld a, (hl)
	and #0x80
	or c
	ld c, a
	inc l
	inc l
	inc l
	inc l
	ld a, (hl)
	; Definir el identificador de slot
	and #0x0c
	or c
	ld h, #0x80
	; Habilitar permanentemente
	call #0x0024 ; call ENASLT
	ei
	ret
;------------------------------------------------
        
;char.c:3: void putchar(char chr) {
;	---------------------------------
; Function putchar
; ---------------------------------
;_putchar_start::
_putchar:
	push ix
	ld   ix, #0
	add  ix, sp
;char.c:7: __endasm;
	ld   a, 4(ix)
	cp   a, #0x0a
	jr   nz, 2$
1$:	ld   a, #0x0d
	call 0x00A2 ; call CHPUT
	ld   a, #0x0a
2$:	call 0x00A2 ; call CHPUT
	pop  ix
	ret
;_putchar_end::

;char.c:10: char getchar(void) {
;	---------------------------------
; Function getchar
; ---------------------------------
;_getchar_start::
_getchar:
;char.c:15: __endasm;
	call 0x009F
	ld h,#0x00
	ld l,a
	ret
;_getchar_end::
;
; ------------------------------------------
; Special load routine that initiates static writable data

	.area   _GSINIT

gsinit::
        push af
        push bc
        push de
        push hl
        ld      bc, #l__INITIALIZER
        ld      a,b
        or      a,c
        jr      z, gsinit_next
        ld      de, #s__INITIALIZED
        ld      hl, #s__INITIALIZER
        ldir        
gsinit_next:
	.area   _GSFINAL
        pop hl
        pop de
        pop bc
        pop af
	ret

        
