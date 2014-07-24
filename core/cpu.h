#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include <registers.h>

typedef struct z80cpu z80cpu_t;

#include "hooks.h"

typedef struct {
    void *device;
    uint8_t (*read_in)(void *);
    void (*write_out)(void *, uint8_t);
} z80iodevice_t;

struct z80cpu {
    z80iodevice_t devices[0x100];
    z80registers_t registers;
    struct {
        uint8_t IFF1 : 1;
        uint8_t IFF2 : 1;
        uint8_t int_mode : 2;
        // Internal use:
        uint8_t IFF_wait : 1;
        uint8_t halted : 1;
        uint8_t INT_pending : 1;
    };
    uint8_t bus;
    uint8_t prefix;
    void *memory;
    uint8_t (*read_byte)(void *, uint16_t);
    void (*write_byte)(void *, uint16_t, uint8_t);
    hook_info_t *hook;
};


uint8_t cpu_read_register_byte(z80cpu_t *, registers);
uint16_t cpu_read_register_word(z80cpu_t *, registers);

uint8_t cpu_write_register_byte(z80cpu_t *, registers, uint8_t);
uint16_t cpu_write_register_word(z80cpu_t *, registers, uint16_t);

z80cpu_t* cpu_init(void);
void cpu_free(z80cpu_t *cpu);
uint8_t cpu_read_byte(z80cpu_t *cpu, uint16_t address);
void cpu_write_byte(z80cpu_t *cpu, uint16_t address, uint8_t value);
uint16_t cpu_read_word(z80cpu_t *cpu, uint16_t address);
void cpu_write_word(z80cpu_t *cpu, uint16_t address, uint16_t value);
uint8_t cpu_hw_in(z80cpu_t *cpu, uint8_t port);
void cpu_hw_out(z80cpu_t *cpu, uint8_t port, uint8_t value);
int cpu_execute(z80cpu_t *cpu, int cycles);
void cpu_raise_interrupt(z80cpu_t *cpu);

#endif
