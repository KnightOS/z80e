#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include <registers.h>

typedef struct {
    //z80hwdevice_t devices[0x100]; // TODO
    z80registers_t registers;
    void* memory;
    uint8_t (*read_byte)(void*, uint16_t);
    void (*write_byte)(void*, uint16_t, uint8_t);
} z80cpu_t;

z80cpu_t* cpu_init();
void cpu_free(z80cpu_t* cpu);
uint8_t cpu_read_byte(z80cpu_t* cpu, uint16_t address);
void cpu_write_byte(z80cpu_t* cpu, uint16_t address, uint8_t value);
uint8_t cpu_hw_in(z80cpu_t* cpu, uint8_t port);
void cpu_hw_out(z80cpu_t* cpu, uint8_t port, uint8_t value);
int cpu_execute(z80cpu_t* cpu, int cycles);

#endif
