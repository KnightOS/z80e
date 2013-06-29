#ifndef CPU_H
#define CPU_H
#include <stdint.h>

typedef struct {
    uint8_t A, F, H, L, B, C, D, E;
    uint16_t IX, IY;
} z80registers_t;

typedef struct {
    z80registers_t* primary_registers;
    z80registers_t* shadow_registers;
    uint8_t I, R;
} z80cpu_t;

z80cpu_t* z80cpu_init();

#endif
