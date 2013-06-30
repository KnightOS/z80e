#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "mmu.h"

typedef struct {
    uint8_t A, F, H, L, B, C, D, E;
    uint16_t IX, IY;
} z80registers_t;

typedef struct {
    z80registers_t primary_registers;
    z80registers_t shadow_registers;
    uint8_t I, R;
} z80state_t;

typedef struct {
    z80state_t state;
    mmu_ref memory;
} z80cpu_t;

z80cpu_t* z80cpu_init();
void z80cpu_free(z80cpu_t* cpu);
uint8_t z80cpu_read_byte(z80cpu_t* cpu, uint16_t address);
void z80cpu_write_byte(z80cpu_t* cpu, uint16_t address, uint8_t value);

#endif
