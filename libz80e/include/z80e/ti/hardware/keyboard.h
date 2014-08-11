#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <z80e/core/cpu.h>

z80iodevice_t init_keyboard();
void free_keyboard(z80iodevice_t keyboard);
void release_key(z80iodevice_t keyboard, uint8_t keycode);
void depress_key(z80iodevice_t keyboard, uint8_t keycode);

#endif
