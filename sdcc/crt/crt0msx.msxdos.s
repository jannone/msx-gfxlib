	;--- crt0.s for MSX-DOS -  by Konami Man, 11/2004
        ;                          small changes by Andrea Rossetti, 3/2015
        ;
	;    Advanced version: allows "int main(char** argv, int argc)",
	;    the returned value will be passed to _TERM on DOS 2,
	;    argv is always 0x100 (the startup code memory is recycled).
        ;    Overhead: 112 bytes.
	;
        ;    Compile programs with --code-loc 0x170 --data-loc X
        ;    X=0  -> global vars will be placed immediately after code
        ;    X!=0 -> global vars will be placed at address X
        ;            (make sure that X>0x100+code size)

	.globl	_main
	.globl	_getchar
	.globl	_putchar

        .globl s__INITIALIZER
        .globl s__INITIALIZED
        .globl l__INITIALIZER

	.area _HEADER (ABS)

        .org    0x0100  ;MSX-DOS .COM programs start address

        ;--- Step 1: jump to init code
        jp init

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
        ;--- Step 2: Build the parameter pointers table on 0x100,
        ;    and terminate each parameter with 0.
        ;    MSX-DOS places the command line length at 0x80 (one byte),
        ;    and the command line itself at 0x81 (up to 127 characters).

        ;* Check if there are any parameters at all

        ld      a,(#0x80)
        or      a
        ld      c,#0
        jr      z,cont

        ;* Terminate command line with 0
        ;  (DOS 2 does this automatically but DOS 1 does not)

        ld      hl,#0x80
        ld      c,(hl)        
        ld      b,#0

        ld      hl,#0x81
        add     hl,bc
        ld      (hl),#0

        ;* Copy the command line processing code to address ram_parloop and
        ;  execute it from there, this way the memory of the original code
        ;  can be recycled for the parameter pointers table.
        ;  (The space from 0x100 up to "cont" can be used,
        ;   this is room for about 40 parameters.
        ;   No real world application will handle so many parameters.)

        ld      hl,#parloop
        ld      de,#ram_parloop
        ld      bc,#parloopend-#parloop
        ldir

        ;* Initialize registers and jump to the loop routine

        ld      hl,#0x81        ;Command line pointer
        ld      c,#0            ;Number of params found
        ld      ix,#0x100       ;Params table pointer

        ld      de,#cont        ;To continue execution at "cont"
        push    de              ;when the routine RETs
        jp      ram_parloop

        ;>>> Command line processing routine begin

        ;* Loop over the command line: skip spaces

parloop:
        ld      a,(hl)
        or      a       ;Command line end found?
        ret     z

        cp      #32
        jr      nz,parfnd
        inc     hl
        jr      parloop

        ;* Parameter found: add its address to params table...

parfnd: ld      (ix),l
        ld      1(ix),h
        inc     ix
        inc     ix
        inc     c
        
        ld      a,c     ;protection against too many parameters
        cp      #40
        ret     nc
        
        ;* ...and skip chars until finding a space or command line end
        
parloop2:       ld      a,(hl)
        or      a       ;Command line end found?
        ret     z
        
        cp      #32
        jr      nz,nospc        ;If space found, set it to 0
                                ;(string terminator)...
        ld      (hl),#0
        inc     hl
        jr      parloop         ;...and return to space skipping loop

nospc:  inc     hl
        jr      parloop2

parloopend:
        
        ;>>> Command line processing routine end
        
        ;* Command line processing done. Here, C=number of parameters.

cont:
        call    gsinit          ; we won't use code at ram_parloop anymore:
                                ; it's safe to overwrite it with globals now

        ld      hl,#0x100       ; argv
        ld      b,#0  
        push    hl              ; Pass parameter argv to "main"
        push    bc              ; Pass parameter argc to "main"

        ;--- Step 4: Call the "main" function

	call    _main

        ;--- Step 5: Program termination.
        ;    Termination code for DOS 2 was returned on L.
                
        ld      c,#0x62   ;DOS 2 function for program termination (_TERM)
        ld      b,l
        call    #0x0005      ;On DOS 2 this terminates; on DOS 1 this returns...
        ld      c,#0x0
        jp      0x0005      ;...and then this one terminates
                       ;(DOS 1 function for program termination).

	.area	_CODE

_putchar::
	push ix
	ld   ix, #0
	add  ix, sp

        push af
        push bc
        push de

	ld   c, #0x02
        
	ld   e, 4(ix)
        ld   a, e
	cp   a, #0x0a 
	jr   nz, 00001$ ; if not '\n' skip print of additional '\r'
	ld   e, #0x0d 
	call #0x0005    ; print '\r'
        
	ld   e, #0x0a
00001$:	call 0x0005     ; print '\n'

        pop de
	pop bc
	pop af
        
	pop  ix
	ret

_getchar::
	ld      c, #0x08
	call	#0x0005
        ret
        
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

	.area	_DATA

; a temporary routine is loaded at ram_parloop startup,
; after execution the routine can be overwritten by global data
ram_parloop: 

; this symbol helps to determine where to "cut" the COM file
; using "dd" or similar utilities
end_of_com_file:
