#include "gfx.h"

static int rnd = 0;

void seed_rnd(int seed)
{
	rnd = seed;
}

u_char get_rnd()
{
	rnd = ((rnd + 1) & 0x0FFF);
	return *(u_char *)rnd;
}

u_char get_vdp(u_char reg)
{
	return *(u_char *)(0xF3DF + reg);
}

void set_mangled_mode()
{
	set_mode(mode_1);
	set_mode(0x7E);
	vwrite((void *)0x1BBF, 0x0800, 0x800);
	vwrite((void *)0x1BBF, 0x1000, 0x800);
	fill(MODE2_ATTR, 0xF0, 0x17FF);
	fill(0xFF8, 0xFF, 8);
	fill(0x17F8, 0xFF, 8);

	// _init_sprites();
}

void pset(int x, int y)
{
	vmerge(map_pixel(x, y), map_subpixel(x));
}

void set_char_form(char c, void *form, u_char place)
{
	u_int addr = c;
	addr <<= 3;
	if (place & place_1)
	{
		vwrite(form, addr, 8);
	}
	if (place & place_2)
	{
		vwrite(form, (256 * 8) + addr, 8);
	}
	if (place & place_3)
	{
		vwrite(form, (256 * 8 * 2) + addr, 8);
	}
}

void set_char_attr(char c, void *attr, u_char place)
{
	u_int addr = c;
	addr <<= 3;
	addr += MODE2_ATTR;

	if (place & place_1)
	{
		vwrite(attr, addr, 8);
	}
	if (place & place_2)
	{
		vwrite(attr, (256 * 8) + addr, 8);
	}
	if (place & place_3)
	{
		vwrite(attr, (256 * 8 * 2) + addr, 8);
	}
}

void set_char_color(char c, u_char color, u_char place)
{
	u_int addr = c;
	addr <<= 3;
	addr += MODE2_ATTR;

	if (place & place_1)
	{
		fill(addr, color, 8);
	}
	if (place & place_2)
	{
		fill((256 * 8) + addr, color, 8);
	}
	if (place & place_3)
	{
		fill((256 * 8 * 2) + addr, color, 8);
	}
}

void set_char(char c, void *form, void *attr, u_char color, u_char place)
{
	set_char_form(c, form, place);
	if (attr)
	{
		set_char_attr(c, attr, place);
	}
	else
	{
		set_char_color(c, color, place);
	}
}

void fill_v(u_int addr, u_char value, u_char count)
{
	u_char diff;

	diff = addr & 7;
	if (diff)
	{
		diff = 8 - diff;
		if (diff > count)
		{
			diff = count;
		}
		fill(addr, value, diff);
		addr = (addr & ~(7)) + 256;
		count -= diff;
	}

	diff = count >> 3;
	while (diff--)
	{
		fill(addr, value, 8);
		addr += 256;
		count -= 8;
	}

	if (count > 0)
	{
		fill(addr, value, count);
	}
}

void set_sprite_mode(u_char mode)
{
	u_char m = get_vdp(1);
	set_vdp(1, (m & 0xFC) | mode);

	// _init_sprites();
}

void set_sprite_8(u_char handle, void *data)
{
	vwrite(data, 14336 + (handle << 3), 8);
}

void set_sprite_16(u_char handle, void *data)
{
	vwrite(data, 14336 + (handle << 5), 32);
}

void put_sprite_8(u_char id, int x, int y, u_char handle, u_char color)
{
	sprite_t sp;
	if (x < 0)
	{
		x += 32;
		color |= 128;
	}
	sp.y = y - 1;
	sp.x = x;
	sp.handle = handle;
	sp.color = color;
	vwrite(&sp, 6912 + (id << 2), 4);
}

void put_sprite_16(u_char id, int x, int y, u_char handle, u_char color)
{
	sprite_t sp;
	if (x < 0)
	{
		x += 32;
		color |= 128;
	}
	sp.y = y - 1;
	sp.x = x;
	sp.handle = (handle << 2);
	sp.color = color;
	vwrite(&sp, 6912 + (id << 2), 4);
}

void blit_ram_vram(u_char *source, u_int dest, u_char w, u_char h, int sjmp, int djmp)
{
	while (h--)
	{
		vwrite(source, dest, w);
		source += sjmp;
		dest += djmp;
	}
}

void blit_fill_vram(u_int dest, u_char value, u_char w, u_char h, int djmp)
{
	while (h--)
	{
		fill(dest, value, w);
		dest += djmp;
	}
}

/* unfinished
void blit_ram_vram(surface_t *source, surface_t *dest, rect_t *from, rect_t *to) {

	int s_jmp, d_jmp, h;
	u_char* s_addr;
	u_int   d_addr;

	s_jmp = source->width - from->width;
	d_jmp = dest->width - from->width;
	h = from->height;

	s_addr = 0; // ?
	d_addr = 0; // ?
}

void blit(surface_t *source, surface_t *dest, rect_t *from, rect_t *to) {
	// one can always dream :)	
}

*/

void psg_tone(u_char channel, int period)
{
	channel <<= 1;
	psg_set(channel, period & 255);
	psg_set(channel + 1, period >> 8);
}

void psg_noise(u_char period)
{
	psg_set(6, period & 31);
}

void psg_volume(u_char channel, u_char volume)
{
	psg_set(channel + 8, volume & 15);
}

void psg_envelope(u_char waveform, int period, u_char channels)
{
	psg_set(13, waveform);
	psg_set(11, period & 255);
	psg_set(12, period >> 8);
	if (channels & 1)
	{
		psg_set(8, 16);
	}
	if (channels & 2)
	{
		psg_set(9, 16);
	}
	if (channels & 4)
	{
		psg_set(10, 16);
	}
	// FIXME: perhaps we should mute all others?
}

void psg_channels(u_char tone_channels, u_char noise_channels)
{
	psg_set(7, (tone_channels << 3) | noise_channels);
}

u_char psg_noise_channels()
{
	return psg_get(7) & 7;
}

u_char psg_tone_channels()
{
	return (psg_get(7) >> 3) & 7;
}
