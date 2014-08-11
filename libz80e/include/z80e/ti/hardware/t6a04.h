#ifndef TI_DISPLAY_H
#define TI_DISPLAY_H

#include <z80e/ti/asic.h>

void setup_lcd_display(asic_t *);

typedef struct ti_bw_lcd ti_bw_lcd_t;

#ifdef CURSES
#include <curses.h>
void bw_lcd_set_window(void *device, WINDOW *win);
#endif

void bw_lcd_state_dump(ti_bw_lcd_t *lcd);
void bw_lcd_dump(ti_bw_lcd_t *lcd);

uint8_t bw_lcd_read_screen(ti_bw_lcd_t *, int, int);
void bw_lcd_write_screen(ti_bw_lcd_t *, int, int, char);

void bw_lcd_reset(ti_bw_lcd_t *);

uint8_t bw_lcd_status_read(void *);
void bw_lcd_status_write(void *, uint8_t);

uint8_t bw_lcd_data_read(void *);
void bw_lcd_data_write(void *, uint8_t);

void bw_lcd_advance_int_pointer(ti_bw_lcd_t *, int *, int *);
void bw_lcd_advance_pointer(ti_bw_lcd_t *);

#endif
