#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "registers.h"
#include "cpu.h"

struct ExecutionContext {
    uint8_t cycles;
    z80cpu_t *cpu;
    union {
        uint8_t opcode;
        struct {
            uint8_t z : 3;
            uint8_t y : 3;
            uint8_t x : 2;
        };
        struct {
            uint8_t   : 3;
            uint8_t q : 1;
            uint8_t p : 2;
        };
    };
};

z80cpu_t* cpu_init() {
    z80cpu_t* cpu = calloc(1, sizeof(z80cpu_t));
    return cpu;
}

void cpu_free(z80cpu_t* cpu) {
    free(cpu);
}

uint8_t cpu_read_byte(z80cpu_t* cpu, uint16_t address) {
    return cpu->read_byte(cpu->memory, address);
}

void cpu_write_byte(z80cpu_t* cpu, uint16_t address, uint8_t value) {
    cpu->write_byte(cpu->memory, address, value);
}

uint8_t read_r(int r, struct ExecutionContext context) {
    switch (r) {
    case 0: return context.cpu->registers.B;
    case 1: return context.cpu->registers.C;
    case 2: return context.cpu->registers.D;
    case 3: return context.cpu->registers.E;
    case 4: return context.cpu->registers.H;
    case 5: return context.cpu->registers.L;
    case 6:
        context.cycles += 3;
        return cpu_read_byte(context.cpu, context.cpu->registers.HL);
    case 7: return context.cpu->registers.A;
    }
}

void write_r(int r, uint8_t value, struct ExecutionContext context) {
    switch (r) {
    case 0: context.cpu->registers.B = value; break;
    case 1: context.cpu->registers.C = value; break;
    case 2: context.cpu->registers.D = value; break;
    case 3: context.cpu->registers.E = value; break;
    case 4: context.cpu->registers.H = value; break;
    case 5: context.cpu->registers.L = value; break;
    case 6:
        context.cycles += 3;
        cpu_write_byte(context.cpu, context.cpu->registers.HL, value);
        break;
    case 7: context.cpu->registers.A = value; break;
    }
}

uint16_t read_rp(int r, struct ExecutionContext context) {
    switch (r) {
    case 0: return context.cpu->registers.BC;
    case 1: return context.cpu->registers.DE;
    case 2: return context.cpu->registers.HL;
    case 3: return context.cpu->registers.SP;
    }
}

void write_rp(int r, uint16_t value, struct ExecutionContext context) {
    switch (r) {
    case 0: context.cpu->registers.BC = value; break;
    case 1: context.cpu->registers.DE = value; break;
    case 2: context.cpu->registers.HL = value; break;
    case 3: context.cpu->registers.SP = value; break;
    }
}

uint16_t read_rp2(int r, struct ExecutionContext context) {
    switch (r) {
    case 0: return context.cpu->registers.BC;
    case 1: return context.cpu->registers.DE;
    case 2: return context.cpu->registers.HL;
    case 3: return context.cpu->registers.AF;
    }
}

void write_rp2(int r, uint16_t value, struct ExecutionContext context) {
    switch (r) {
    case 0: context.cpu->registers.BC = value; break;
    case 1: context.cpu->registers.DE = value; break;
    case 2: context.cpu->registers.HL = value; break;
    case 3: context.cpu->registers.AF = value; break;
    }
}

int16_t cpu_execute(z80cpu_t* cpu, int16_t cycles) {
    struct ExecutionContext context;
    context.cpu = cpu;
    while (cycles > 0) {
        context.cycles = 0;
        context.opcode = cpu_read_byte(cpu, cpu->registers.PC++);

        printf("opcode: 0x%02X\n", context.opcode);
        printf("x, y, z: %d, %d, %d\n", context.x, context.y, context.z);
        printf("p, q: %d, %d\n", context.p, context.q);

        cycles -= context.cycles;
        if (context.cycles == 0) {
            cycles--; // Temporary: prevents infinite loops with unimplemented opcodes
        }
    }
}
