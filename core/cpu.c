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
    uint8_t (*n)(struct ExecutionContext*);
    uint16_t (*nn)(struct ExecutionContext*);
    int8_t (*d)(struct ExecutionContext*);
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

void push(z80cpu_t* cpu, uint16_t value) {
    cpu_write_byte(cpu, --cpu->registers.SP, value & 0xFF);
    cpu_write_byte(cpu, --cpu->registers.SP, value >> 8);
}

uint16_t pop(z80cpu_t* cpu) {
    uint16_t a;
    a |= cpu_read_byte(cpu, cpu->registers.SP++) << 8;
    a |= cpu_read_byte(cpu, cpu->registers.SP++);
    return a;
}

uint8_t read_r(int i, struct ExecutionContext *context) {
    switch (i) {
    case 0: return context->cpu->registers.B;
    case 1: return context->cpu->registers.C;
    case 2: return context->cpu->registers.D;
    case 3: return context->cpu->registers.E;
    case 4: return context->cpu->registers.H;
    case 5: return context->cpu->registers.L;
    case 6:
        context->cycles += 3;
        return cpu_read_byte(context->cpu, context->cpu->registers.HL);
    case 7: return context->cpu->registers.A;
    }
}

void write_r(int i, uint8_t value, struct ExecutionContext *context) {
    switch (i) {
    case 0: context->cpu->registers.B = value; break;
    case 1: context->cpu->registers.C = value; break;
    case 2: context->cpu->registers.D = value; break;
    case 3: context->cpu->registers.E = value; break;
    case 4: context->cpu->registers.H = value; break;
    case 5: context->cpu->registers.L = value; break;
    case 6:
        context->cycles += 3;
        cpu_write_byte(context->cpu, context->cpu->registers.HL, value);
        break;
    case 7: context->cpu->registers.A = value; break;
    }
}

uint16_t read_rp(int i, struct ExecutionContext *context) {
    switch (i) {
    case 0: return context->cpu->registers.BC;
    case 1: return context->cpu->registers.DE;
    case 2: return context->cpu->registers.HL;
    case 3: return context->cpu->registers.SP;
    }
}

void write_rp(int i, uint16_t value, struct ExecutionContext *context) {
    switch (i) {
    case 0: context->cpu->registers.BC = value; break;
    case 1: context->cpu->registers.DE = value; break;
    case 2: context->cpu->registers.HL = value; break;
    case 3: context->cpu->registers.SP = value; break;
    }
}

uint16_t read_rp2(int i, struct ExecutionContext *context) {
    switch (i) {
    case 0: return context->cpu->registers.BC;
    case 1: return context->cpu->registers.DE;
    case 2: return context->cpu->registers.HL;
    case 3: return context->cpu->registers.AF;
    }
}

void write_rp2(int i, uint16_t value, struct ExecutionContext *context) {
    switch (i) {
    case 0: context->cpu->registers.BC = value; break;
    case 1: context->cpu->registers.DE = value; break;
    case 2: context->cpu->registers.HL = value; break;
    case 3: context->cpu->registers.AF = value; break;
    }
}

uint8_t read_cc(int i, struct ExecutionContext *context) {
    z80registers_t *r = &context->cpu->registers;
    switch (i) {
    case 0: return !r->flags.Z;
    case 1: return  r->flags.Z;
    case 2: return !r->flags.C;
    case 3: return  r->flags.C;
    case 4: return !r->flags.PV;
    case 5: return  r->flags.PV;
    case 6: return !r->flags.N;
    case 7: return  r->flags.N;
    }
}

void execute_alu(int i, uint8_t v, struct ExecutionContext *context) {
    uint8_t old;
    context->cycles += 4;
    z80registers_t *r = &context->cpu->registers;
    switch (i) {
    case 0: // ADD A, v
        old = r->A;
        r->A += v;
        updateFlags(r, old, r->A);
        break;
    case 1: // ADC A, v
        old = r->A;
        r->A += v + r->flags.C;
        updateFlags(r, old, r->A);
        break;
    case 2: // SUB v
        old = r->A;
        r->A -= v;
        updateFlags_subtraction(r, old, r->A);
        break;
    case 3: // SBC v
        old = r->A;
        r->A -= v + r->flags.C;
        updateFlags_subtraction(r, old, r->A);
        break;
    case 4: // AND v
        old = r->A;
        r->A &= v;
        updateFlags_parity(r, old, r->A);
        r->flags.C = r->flags.N = 0;
        r->flags.H = 1;
        break;
    case 5: // XOR v
        old = r->A;
        r->A ^= v;
        updateFlags_parity(r, old, r->A);
        r->flags.C = r->flags.N = r->flags.H = 0;
        break;
    case 6: // OR v
        old = r->A;
        r->A |= v;
        updateFlags_parity(r, old, r->A);
        r->flags.C = r->flags.N = r->flags.H = 0;
        break;
    case 7: // CP v
        old = r->A - v;
        updateFlags_subtraction(r, r->A, old);
        break;
    }
}

uint8_t read_n(struct ExecutionContext *context) {
    return cpu_read_byte(context->cpu, context->cpu->registers.PC++);
}

uint16_t read_nn(struct ExecutionContext *context) {
    uint16_t a;
    a |= cpu_read_byte(context->cpu, context->cpu->registers.PC++) << 8;
    a |= cpu_read_byte(context->cpu, context->cpu->registers.PC++);
}

int8_t read_d(struct ExecutionContext *context) {
    return (int8_t)cpu_read_byte(context->cpu, context->cpu->registers.PC++);
}

int cpu_execute(z80cpu_t* cpu, int cycles) {
    struct ExecutionContext context;
    context.cpu = cpu;
    while (cycles > 0) {
        context.cycles = 0;
        context.opcode = cpu_read_byte(cpu, cpu->registers.PC++);
        context.n = read_n;
        context.nn = read_nn;
        context.d = read_d;
        int8_t d; uint8_t n; uint16_t nn;

        switch (context.x) {
        case 0:
            switch (context.z) {
            case 0:
                switch (context.y) {
                case 0: // NOP
                    context.cycles += 4;
                    break;
                case 1: // EX AF, AF'
                    context.cycles += 4;
                    exAFAF(&cpu->registers);
                    break;
                case 2: // DJNZ d
                    context.cycles += 8;
                    d = context.d(&context);
                    cpu->registers.B--;
                    if (cpu->registers.B != 0) {
                        context.cycles += 5;
                        cpu->registers.PC += d;
                    }
                    break;
                case 3: // JR d
                    context.cycles += 12;
                    d = context.d(&context);
                    cpu->registers.PC += d;
                    break;
                case 4:
                case 5:
                case 6:
                case 7: // JR cc[y-4], d
                    context.cycles += 7;
                    d = context.d(&context);
                    if (read_cc(context.y - 4, &context)) {
                        context.cycles += 5;
                        cpu->registers.PC += d;
                    }
                    break;
                }
                break;
            case 1:
                switch (context.q) {
                case 0: // LD rp[p], nn
                    break;
                case 1: // ADD HL, rp[p]
                    break;
                }
                break;
            case 2:
                switch (context.q) {
                case 0:
                    switch (context.p) {
                    case 0: // LD (BC), A
                        break;
                    case 1: // LD (DE), A
                        break;
                    case 2: // LD (nn), HL
                        break;
                    case 3: // LD (nn), A
                        break;
                    }
                    break;
                case 1:
                    switch (context.p) {
                        case 0: // LD A, (BC)
                            break;
                        case 1: // LD A, (DE)
                            break;
                        case 2: // LD HL, (nn)
                            break;
                        case 3: // LD A, (nn)
                            break;
                    }
                    break;
                }
                break;
            case 3:
                switch (context.q) {
                case 0: // INC rp[p]
                    break;
                case 1: // DEC rp[p]
                    break;
                }
                break;
            case 4: // INC r[y]
                break;
            case 5: // DEC r[y]
                break;
            case 6: // LD r[y], n
                break;
            case 7:
                switch (context.y) {
                case 0: // RLCA
                    break;
                case 1: // RRCA
                    break;
                case 2: // RLA
                    break;
                case 3: // RRA
                    break;
                case 4: // DAA
                    break;
                case 5: // CPL
                    break;
                case 6: // SCF
                    break;
                case 7: // CCF
                    break;
                }
                break;
            }
            break;
        case 1:
            if (context.z == 6 && context.y == 6) { // HALT
            } else { // LD r[y], r[z]
            }
            break;
        case 2: // ALU[y] r[z]
            execute_alu(context.y, read_r(context.z, &context), &context);
            break;
        case 3:
            switch (context.z) {
            case 0: // RET cc[y]
                break;
            case 1:
                switch (context.q) {
                case 0: // POP rp2[p]
                    break;
                case 1:
                    switch (context.p) {
                    case 0: // RET
                        break;
                    case 1: // EXX
                        break;
                    case 2: // JP HL
                        break;
                    case 3: // LD SP, HL
                        break;
                    }
                    break;
                }
                break;
            case 2: // JP cc[y], nn
                break;
            case 3:
                switch (context.y) {
                case 0: // JP nn
                    break;
                case 1: // 0xCB prefixed opcodes
                    break;
                case 2: // OUT (n), A
                    break;
                case 3: // IN A, (n)
                    break;
                case 4: // EX (SP), HL
                    break;
                case 5: // EX DE, HL
                    break;
                case 6: // DI
                    break;
                case 7: // EI
                    break;
                }
                break;
            case 4: // CALL cc[y], nn
                break;
            case 5:
                switch (context.q) {
                    case 0: // PUSH r2p[p]
                        break;
                    case 1:
                        switch (context.p) {
                            case 0: // CALL nn
                                break;
                            case 1: // 0xDD prefixed opcodes
                                break;
                            case 2: // 0xED prefixed opcodes
                                break;
                            case 3: // 0xFD prefixed opcodes
                                break;
                        }
                        break; 
                }
                break;
            case 6: // alu[y] n
                break;
            case 7: // RST y*8
                context.cycles += 11;
                push(context.cpu, context.cpu->registers.PC + 2);
                context.cpu->registers.PC = context.y * 8;
                break;
            }
            break;
        }

        cycles -= context.cycles;
        if (context.cycles == 0) {
            cycles--; // Temporary: prevents infinite loops with unimplemented opcodes
        }
    }
    return cycles;
}
