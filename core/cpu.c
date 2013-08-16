#include "cpu.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

z80cpu_t* z80cpu_init() {
    z80cpu_t* cpu = malloc(sizeof(z80cpu_t));
    cpu->state = malloc(sizeof(z80state_t));
    memset(cpu->state, 0, sizeof(z80state_t));
    int i;
    for (i = 0; i < sizeof(cpu->devices) / sizeof(z80hwdevice_t); i++) {
        cpu->devices[i].device = NULL;
    }
    return cpu;
}

void z80cpu_free(z80cpu_t* cpu) {
    free(cpu->state);
    free(cpu);
}

uint8_t z80cpu_read_byte(z80cpu_t* cpu, uint16_t address) {
    return cpu->read_byte(cpu->memory, address);
}

void z80cpu_write_byte(z80cpu_t* cpu, uint16_t address, uint8_t value) {
    cpu->write_byte(cpu->memory, address, value);
}

uint8_t z80cpu_hw_in(z80cpu_t* cpu, uint8_t port) {
    z80hwdevice_t device = cpu->devices[port];
    if (device.device == NULL) // TODO: Properly fallback to shadow port
        return 0;
    return device.read_in(device.device);
}

void z80cpu_hw_out(z80cpu_t* cpu, uint8_t port, uint8_t value) {
    z80hwdevice_t device = cpu->devices[port];
    if (device.device == NULL)
        return;
    return device.write_out(device.device, value);
}

int16_t z80cpu_execute(z80cpu_t* cpu, int16_t cycles) {
    while (cycles > 0) {
        uint8_t instruction = z80cpu_read_byte(cpu, cpu->state->PC++);
        switch (instruction) {
            case 0x00: // nop
                cycles -= 4;
                break;
            default:
                // TODO: What is the correct way to emulate an unrecognized instruction?
                cycles--;
                break;
        }
    }
}
