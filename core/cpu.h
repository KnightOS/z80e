#ifndef CPU_H
#define CPU_H
#include <stdint.h>

typedef struct {
    uint8_t A, F, H, L, B, C, D, E;
    uint16_t IX, IY;
} z80registers_t;

typedef struct {
    z80registers_t primary_registers;
    z80registers_t shadow_registers;
    uint8_t I, R;
    uint16_t PC, SP;
} z80state_t;

typedef struct {
    void* device;
    uint8_t (*read_in)(void*);
    void (*write_out)(void*, uint8_t);
} z80hwdevice_t;

typedef struct {
    z80state_t* state;
    z80hwdevice_t devices[0x100];
    void* memory;
    uint8_t (*read_byte)(void*, uint16_t);
    void (*write_byte)(void*, uint16_t, uint8_t);
} z80cpu_t;

z80cpu_t* z80cpu_init();
void z80cpu_free(z80cpu_t* cpu);
uint8_t z80cpu_read_byte(z80cpu_t* cpu, uint16_t address);
void z80cpu_write_byte(z80cpu_t* cpu, uint16_t address, uint8_t value);
uint8_t z80cpu_hw_in(z80cpu_t* cpu, uint8_t port);
void z80cpu_hw_out(z80cpu_t* cpu, uint8_t port, uint8_t value);

#endif
