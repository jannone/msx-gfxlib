; crt0 for MSX ROM of 16KB, starting at 0x4000
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

; Ordering of segments for the linker.
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
  	call    gsinit ; Initialise global variables
	call    _init ; init gfxlib
	call    _main 
	call    #0x0000; call CHKRAM
        
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

        
