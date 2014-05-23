#include "keyboard.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "asic.h"
#include "cpu.h"

typedef struct {
    uint8_t group_mask;
    uint8_t groups[8];
} keyboard_state_t;

/* Key codes in z80e are group << 4 | bit. That is, 0x14 is bit 4 of group 1. */
void depress_key(z80iodevice_t keyboard, uint8_t keycode) {
    keyboard_state_t *state = (keyboard_state_t*)keyboard.device;
    uint8_t group = keycode >> 4;
    uint8_t mask = 1 << (keycode & 0xF);
    state->groups[group] |= mask;
}

void release_key(z80iodevice_t keyboard, uint8_t keycode) {
    keyboard_state_t *state = (keyboard_state_t*)keyboard.device;
    uint8_t group = keycode >> 4;
    uint8_t mask = 1 << (keycode & 0xF);
    state->groups[group] &= ~mask;
}

uint8_t read_keyboard(void *_state) {
    keyboard_state_t *state = (keyboard_state_t*)state;
    uint8_t mask = state->group_mask;
    printf("[%02X]", mask);
    uint8_t value = 0;
    int i;
    for (i = 0; i < 8; i++) {
        if (mask & 1) {
            value |= state->groups[i];
        }
        mask >>= 1;
    }
    return 0;
}

void write_keyboard(void *_state, uint8_t value) {
    keyboard_state_t *state = (keyboard_state_t*)state;
    state->group_mask = value;
}

z80iodevice_t init_keyboard() {
    keyboard_state_t *state = calloc(1, sizeof(keyboard_state_t));
    state->group_mask = 0xFF;
    z80iodevice_t device = { state, read_keyboard, write_keyboard };
    return device;
}
