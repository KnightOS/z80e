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
} z80state_t;

typedef struct {
    z80state_t* state;
} z80cpu_t;

typedef uint8_t (*mem_read_function)(z80cpu_t*, uint16_t); // cpu, address
typedef void (*mem_write_function)(z80cpu_t*, uint16_t, uint8_t); // cpu, address, value

typedef struct {
    mem_read_function read_memory;
    mem_write_function write_memory;
} z80mmu_t;

z80cpu_t* z80cpu_init();
void z80cpu_free(z80cpu_t* cpu);

#endif
