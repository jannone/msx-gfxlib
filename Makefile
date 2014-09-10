GFXLIB_C = gfx.c line.c 3d.c
GFXLIB_H = $(GFXLIB_C:.c=.h)
GFXLIB = $(GFXLIB_C) $(GFXLIB_H)

EXAMPLES_C = $(wildcard ex*.c)
EXAMPLES = $(EXAMPLES_C:.c=.com)

all: $(EXAMPLES)

%.com: %.c $(GFXLIB)
	htc -Ptext=0100H,data=08000H,bss -o$@ $< $(GFXLIB_C) -lf
	#htc -Ptext=08000H,data,bss -o$@ $< $(GFXLIB_C) -lf

clean:
	rm *.o *~ $(EXAMPLES)
