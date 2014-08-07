#include "display.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

#ifdef CURSES
#include <curses.h>
#endif

#ifdef CURSES
#define lcd_print(...) wprintw(lcd->lcd_display, __VA_ARGS__)
#else
#define lcd_print(...) printf( __VA_ARGS__)
#endif

struct ti_bw_lcd {
	uint8_t up: 1; // set=up, unset=down
	uint8_t counter: 1; // set=Y, unset=X
	uint8_t word_length: 1; // set=8, unset=6
	uint8_t display_on: 1; // set=on, unset=off
	uint8_t op_amp1: 2; // 0-3
	uint8_t op_amp2: 2; // 0-3

	int X; // which is up-down
	int Y; // which is left-right
	int Z; // which is which y is rendered at top
	uint8_t contrast; // 0-63
	uint8_t *ram; // [X * 64 + Y]

#ifdef CURSES
	WINDOW *lcd_display;
#endif
};

#ifdef CURSES
void bw_lcd_set_window(void *device, WINDOW *win) {
	ti_bw_lcd_t *lcd = device;
	lcd->lcd_display = win;
	lcd_print("Enabled LCD window here");
	wrefresh(win);
}
#endif

void setup_lcd_display(asic_t *asic) {
	ti_bw_lcd_t *lcd = malloc(sizeof(ti_bw_lcd_t));

	bw_lcd_reset(lcd);

	lcd->ram = malloc((64 * 128));
	int cY;
	int cX;
	for (cX = 0; cX < 64; cX++) {
		for (cY = 0; cY < 120; cY++) {
			bw_lcd_write_screen(lcd, cY, cX, 0);
		}
	}

	asic->cpu->devices[0x10].device = lcd;
	asic->cpu->devices[0x10].read_in = bw_lcd_status_read;
	asic->cpu->devices[0x10].write_out = bw_lcd_status_write;

	asic->cpu->devices[0x11].device = lcd;
	asic->cpu->devices[0x11].read_in = bw_lcd_data_read;
	asic->cpu->devices[0x11].write_out = bw_lcd_data_write;
}
void unicode_to_utf8(char *b, uint32_t c) {
	if (c<0x80) *b++=c;
	else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
	else if (c-0xd800u<0x800) return;
	else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
	else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
}

void bw_lcd_dump(ti_bw_lcd_t *lcd) {
	int cY;
	int cX;

	#ifdef CURSES
	wmove(lcd->lcd_display, 0, 0);
	#endif

#define LCD_BRAILLE
#ifndef LCD_BRAILLE
	for (cX = 0; cX < 64; cX++) {
		for (cY = 0; cY < 120; cY++) {
			lcd_print("%c", bw_lcd_read_screen(lcd, cY, cX) ? 'O' : ' ');
		}
		lcd_print("\n");
	}
#else
	for (cX = 0; cX < 64; cX += 4) {
		for (cY = 0; cY < 120; cY += 2) {
			int a = bw_lcd_read_screen(lcd, cY + 0, cX + 0);
			int b = bw_lcd_read_screen(lcd, cY + 0, cX + 1);
			int c = bw_lcd_read_screen(lcd, cY + 0, cX + 2);
			int d = bw_lcd_read_screen(lcd, cY + 1, cX + 0);
			int e = bw_lcd_read_screen(lcd, cY + 1, cX + 1);
			int f = bw_lcd_read_screen(lcd, cY + 1, cX + 2);
			int g = bw_lcd_read_screen(lcd, cY + 0, cX + 3);
			int h = bw_lcd_read_screen(lcd, cY + 1, cX + 3);
			uint32_t byte_value = 0x2800;
			byte_value += (
				(a << 0) |
				(b << 1) |
				(c << 2) |
				(d << 3) |
				(e << 4) |
				(f << 5) |
				(g << 6) |
				(h << 7));
			char buff[5] = {0};
			unicode_to_utf8(buff, byte_value);
			lcd_print("%s", buff);
		}
#ifndef CURSES
		lcd_print("\n");
#endif
	}
#endif
	bw_lcd_state_dump(lcd);
#ifdef CURSES
	wrefresh(lcd->lcd_display);
#endif
}

void bw_lcd_state_dump(ti_bw_lcd_t *lcd) {
	lcd_print("C: 0x%02X X: 0x%02X Y: 0x%02X Z: 0x%02X\n", lcd->contrast, lcd->X, lcd->Y, lcd->Z);
	lcd_print("   %c%c%c%c  O1: 0x%01X 02: 0x%01X\n", lcd->up ? 'V' : '^', lcd->counter ? '-' : '|',
		lcd->word_length ? '8' : '6', lcd->display_on ? 'O' : ' ', lcd->op_amp1, lcd->op_amp2);
}

uint8_t bw_lcd_read_screen(ti_bw_lcd_t *lcd, int Y, int X) {
	int location = X * 120 + Y;
	return !!(lcd->ram[location >> 3] & (1 << (location % 8)));
}

void bw_lcd_write_screen(ti_bw_lcd_t *lcd, int Y, int X, char val) {
	val = !!val;
	int location = X * 120 + Y;
	lcd->ram[location >> 3] &= ~(1 << (location % 8));
	lcd->ram[location >> 3] |= (val << (location % 8));
}

void bw_lcd_reset(ti_bw_lcd_t *lcd) {
	lcd->display_on = 0;
	lcd->word_length = 1;
	lcd->up = 1;
	lcd->counter = 0;
	lcd->Y = 0;
	lcd->Z = 0;
	lcd->X = 0;
	lcd->contrast = 0;
	lcd->op_amp1 = 0;
	lcd->op_amp2 = 0;
}

uint8_t bw_lcd_status_read(void *device) {
	ti_bw_lcd_t *lcd = device;

	uint8_t retval = 0;
	retval |= (0) << 7;
	retval |= (lcd->word_length) << 6;
	retval |= (lcd->display_on) << 5;
	retval |= (0) << 4;
	retval |= (lcd->counter) << 1;
	retval |= (lcd->up) << 0;

	return retval;
}

void bw_lcd_status_write(void *device, uint8_t val) {
	ti_bw_lcd_t *lcd = device;

	log_message(L_DEBUG, "lcd", "Wrote 0x%02X (0b%d%d%d%d%d%d%d%d) to status", val,
		!!(val & (1 << 7)),
		!!(val & (1 << 6)),
		!!(val & (1 << 5)),
		!!(val & (1 << 4)),
		!!(val & (1 << 3)),
		!!(val & (1 << 2)),
		!!(val & (1 << 1)),
		!!(val & (1 << 0))
	);
	if (val & 0xC0) { // 0b11XXXXXX
		lcd->contrast = val & 0x3F;
		log_message(L_DEBUG, "lcd", "\tSet contrast to 0x%02X", lcd->contrast);
	} else if (val & 0x80) { // 0b10XXXXXX
		lcd->X = val & 0x3F;
		log_message(L_DEBUG, "lcd", "\tSet X (vertical!) to 0x%02X", lcd->X);
	} else if (val & 0x40) { // 0b01XXXXXX
		lcd->Z = val & 0x3F;
		log_message(L_DEBUG, "lcd", "\tSet Z (vertical scroll) to 0x%02X", lcd->Z);
	} else if (val & 0x20) { // 0b001XXXXX
		lcd->Y = val & 0x1F;
		log_message(L_DEBUG, "lcd", "\tSet Y (horizontal!) to 0x%02X", lcd->Y);
	} else if ((val & 0x18) == 0x18) { // 0b00011***
		// test mode - not emulating yet
	} else if (val & 0x10) { // 0b00010*XX
		lcd->op_amp1 = val & 0x03;
		log_message(L_DEBUG, "lcd", "\tSet Op-Amp 1 to 0x%02X", lcd->op_amp1);
	} else if (val & 0x08) { // 0b00001*XX
		lcd->op_amp2 = val & 0x03;
		log_message(L_DEBUG, "lcd", "\tSet Op-Amp 2 to 0x%02X", lcd->op_amp2);
	} else if (val & 0x04) { // 0b000001XX
		lcd->counter = !!(val & 0x02);
		lcd->up = !!(val & 0x01);
		log_message(L_DEBUG, "lcd", "\tSet counter to %s and Up/Down to %s",
			lcd->counter ? "Y" : "X", lcd->up ? "Up" : "Down");
	} else if (val & 0x02) { // 0b0000001X
		lcd->display_on = !!(val & 0x01);
		log_message(L_DEBUG, "lcd", "\tDisplay turned %s", lcd->display_on ? "ON" : "OFF");
	} else { // 0b0000000X
		lcd->word_length = !!(val & 0x01);
		log_message(L_DEBUG, "lcd", "\tWord Length set to %d", lcd->word_length ? 8 : 6);
	}
	bw_lcd_dump(lcd);
}

void bw_lcd_advance_int_pointer(ti_bw_lcd_t *lcd, int *Y, int *X) {
	if (lcd->counter) { // Y
		(*Y)++;
		*Y = *Y % 120;
	} else { // X
		(*X)++;
		*X = *X % 64;
	}
}

void bw_lcd_advance_pointer(ti_bw_lcd_t *lcd) {
	int maxX = lcd->word_length ? 15 : 20;
	if (lcd->counter) { // Y
		if (!lcd->up) {
			lcd->Y--;
			if (lcd->Y < 0) {
				lcd->Y = maxX - 1;
			}
		} else {
			lcd->Y++;
			lcd->Y = lcd->Y % maxX;
		}
	} else { // X
		if (!lcd->up) {
			lcd->X--;
			if (lcd->X < 0) {
				lcd->X = 63;
			}
		} else {
			lcd->X++;
			lcd->X = lcd->X % 64;
		}
	}
}

uint8_t bw_lcd_data_read(void *device) {
	ti_bw_lcd_t *lcd = device;

	int cY = lcd->Y * (lcd->word_length ? 8 : 6);
	int cX = lcd->X;

	int max = lcd->word_length ? 8 : 6;
	int i = 0;
	uint8_t retval = 0;
	for (i = 0; i < max; i++) {
		retval |= bw_lcd_read_screen(lcd, cY, cX);
		retval <<= 1;
		cY++;
	}

	bw_lcd_advance_pointer(lcd);
	return retval;
}

void bw_lcd_data_write(void *device, uint8_t val) {
	ti_bw_lcd_t *lcd = device;

	int cY = lcd->Y * (lcd->word_length ? 8 : 6);
	int cX = lcd->X;

	int max = lcd->word_length ? 8 : 6;
	int i = 0;
	cY += max - 1;
	for (i = 0; i < max; i++) {
		bw_lcd_write_screen(lcd, cY, cX, val & (1 << i));
		cY--;
	}

	log_message(L_DEBUG, "lcd", "Wrote %02X (0b%d%d%d%d%d%d%d%d) to %d (Y), %d (X)",
		val,
		!!(val & (1 << 7)), !!(val & (1 << 6)),
		!!(val & (1 << 5)), !!(val & (1 << 4)),
		!!(val & (1 << 3)), !!(val & (1 << 2)),
		!!(val & (1 << 1)), !!(val & (1 << 0)),
		lcd->Y, lcd->X);

	bw_lcd_advance_pointer(lcd);
	bw_lcd_dump(lcd);
}
