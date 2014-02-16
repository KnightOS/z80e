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

z80cpu_t* cpu_init(void) {
    z80cpu_t *cpu = calloc(1, sizeof(z80cpu_t));
    z80iodevice_t nullDevice = { NULL, NULL, NULL };
    int i;
    for (i = 0; i < 0x100; i++) {
        cpu->devices[i] = nullDevice;
    }
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

uint16_t cpu_read_word(z80cpu_t* cpu, uint16_t address) {
    return cpu->read_byte(cpu->memory, address) | (cpu->read_byte(cpu->memory, address + 1) << 8);
}

void cpu_write_word(z80cpu_t* cpu, uint16_t address, uint16_t value) {
    cpu->write_byte(cpu->memory, address, value & 0xFF);
    cpu->write_byte(cpu->memory, address + 1, value >> 8);
}

void push(z80cpu_t* cpu, uint16_t value) {
    cpu_write_word(cpu, cpu->registers.SP - 2, value);
    cpu->registers.SP -= 2;
}

uint16_t pop(z80cpu_t* cpu) {
    uint16_t a = 0;
    a |= cpu_read_byte(cpu, cpu->registers.SP++);
    a |= cpu_read_byte(cpu, cpu->registers.SP++) << 8;
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

uint8_t write_r(int i, uint8_t value, struct ExecutionContext *context) {
    switch (i) {
    case 0: return context->cpu->registers.B = value;
    case 1: return context->cpu->registers.C = value;
    case 2: return context->cpu->registers.D = value;
    case 3: return context->cpu->registers.E = value;
    case 4: return context->cpu->registers.H = value;
    case 5: return context->cpu->registers.L = value;
    case 6:
        context->cycles += 3;
        cpu_write_byte(context->cpu, context->cpu->registers.HL, value);
        return value;
    case 7: return context->cpu->registers.A = value;
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

uint16_t write_rp(int i, uint16_t value, struct ExecutionContext *context) {
    switch (i) {
    case 0: return context->cpu->registers.BC = value;
    case 1: return context->cpu->registers.DE = value;
    case 2: return context->cpu->registers.HL = value;
    case 3: return context->cpu->registers.SP = value;
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

uint16_t write_rp2(int i, uint16_t value, struct ExecutionContext *context) {
    switch (i) {
    case 0: return context->cpu->registers.BC = value;
    case 1: return context->cpu->registers.DE = value;
    case 2: return context->cpu->registers.HL = value;
    case 3: return context->cpu->registers.AF = value;
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

void daa(struct ExecutionContext *context) {
    z80registers_t *r = &context->cpu->registers;
    uint8_t msn = r->A >> 4;
    uint8_t lsn = r->A & 0xF;
    uint8_t c = r->flags.C;
    if (lsn > 9 || r->flags.H) {
        r->A += 0x06;
        r->flags.C = 1;
    }
    if (msn > 9 || c) {
        r->A += 0x60;
        r->flags.C = 1;
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
    a |= cpu_read_byte(context->cpu, context->cpu->registers.PC++);
    a |= cpu_read_byte(context->cpu, context->cpu->registers.PC++) << 8;
}

int8_t read_d(struct ExecutionContext *context) {
    return (int8_t)cpu_read_byte(context->cpu, context->cpu->registers.PC++);
}

int cpu_execute(z80cpu_t* cpu, int cycles) {
    struct ExecutionContext context;
    context.cpu = cpu;
    while (cycles > 0) {
        if (cpu->IFF2) {
            if (cpu->IFF_wait) {
                cpu->IFF_wait = 0;
            } else {
                // TODO: Check interrupt queue
            }
        }

        context.cycles = 0;
        context.opcode = cpu_read_byte(cpu, cpu->registers.PC++);
        context.n = read_n;
        context.nn = read_nn;
        context.d = read_d;
        int8_t d; uint8_t n; uint16_t nn;
        uint8_t old; uint16_t old16;
        uint8_t new; uint16_t new16;
        z80iodevice_t ioDevice;

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
                    context.cycles += 10;
                    write_rp(context.p, context.nn(&context), &context);
                    break;
                case 1: // ADD HL, rp[p]
                    context.cycles += 11;
                    old16 = cpu->registers.HL;
                    cpu->registers.HL += read_rp(context.p, &context);
                    updateFlags_except(&cpu->registers, old16, cpu->registers.HL, FLAG_Z | FLAG_S | FLAG_PV);
                    break;
                }
                break;
            case 2:
                switch (context.q) {
                case 0:
                    switch (context.p) {
                    case 0: // LD (BC), A
                        context.cycles += 7;
                        cpu_write_byte(cpu, cpu->registers.BC, cpu->registers.A);
                        break;
                    case 1: // LD (DE), A
                        context.cycles += 7;
                        cpu_write_byte(cpu, cpu->registers.DE, cpu->registers.A);
                        break;
                    case 2: // LD (nn), HL
                        context.cycles += 16;
                        cpu_write_word(cpu, context.nn(&context), cpu->registers.HL);
                        break;
                    case 3: // LD (nn), A
                        context.cycles += 13;
                        cpu_write_byte(cpu, context.nn(&context), cpu->registers.A);
                        break;
                    }
                    break;
                case 1:
                    switch (context.p) {
                    case 0: // LD A, (BC)
                        context.cycles += 7;
                        cpu->registers.A = cpu_read_byte(cpu, cpu->registers.BC);
                        break;
                    case 1: // LD A, (DE)
                        context.cycles += 7;
                        cpu->registers.A = cpu_read_byte(cpu, cpu->registers.DE);
                        break;
                    case 2: // LD HL, (nn)
                        context.cycles += 16;
                        cpu->registers.HL = cpu_read_word(cpu, context.nn(&context));
                        break;
                    case 3: // LD A, (nn)
                        context.cycles += 13;
                        old16 = context.nn(&context);
                        cpu->registers.A = cpu_read_byte(cpu, old16);
                        break;
                    }
                    break;
                }
                break;
            case 3:
                switch (context.q) {
                case 0: // INC rp[p]
                    context.cycles += 6;
                    write_rp(context.p, read_rp(context.p, &context) + 1, &context);
                    break;
                case 1: // DEC rp[p]
                    context.cycles += 6;
                    write_rp(context.p, read_rp(context.p, &context) - 1, &context);
                    break;
                }
                break;
            case 4: // INC r[y]
                context.cycles += 4;
                old = read_r(context.y, &context);
                new = write_r(context.y, old + 1, &context);
                updateFlags_except(&cpu->registers, old, new, FLAG_C);
                break;
            case 5: // DEC r[y]
                context.cycles += 4;
                old = read_r(context.y, &context);
                new = write_r(context.y, old - 1, &context);
                updateFlags_except(&cpu->registers, old, new, FLAG_C);
                break;
            case 6: // LD r[y], n
                context.cycles += 7;
                write_r(context.y, context.nn(&context), &context);
                break;
            case 7:
                switch (context.y) {
                case 0: // RLCA
                    context.cycles += 4;
                    old = (cpu->registers.A & 0x80) > 0;
                    cpu->registers.flags.C = old;
                    cpu->registers.A <<= 1;
                    cpu->registers.A |= old;
                    cpu->registers.flags.N = cpu->registers.flags.H = 0;
                    break;
                case 1: // RRCA
                    context.cycles += 4;
                    old = (cpu->registers.A & 1) > 0;
                    cpu->registers.flags.C = old;
                    cpu->registers.A >>= 1;
                    cpu->registers.A |= old << 7;
                    cpu->registers.flags.N = cpu->registers.flags.H = 0;
                    break;
                case 2: // RLA
                    context.cycles += 4;
                    old = cpu->registers.flags.C;
                    cpu->registers.flags.C = (cpu->registers.A & 0x80) > 0;
                    cpu->registers.A <<= 1;
                    cpu->registers.A |= old;
                    cpu->registers.flags.N = cpu->registers.flags.H = 0;
                    break;
                case 3: // RRA
                    context.cycles += 4;
                    old = cpu->registers.flags.C;
                    cpu->registers.flags.C = (cpu->registers.A & 1) > 0;
                    cpu->registers.A >>= 1;
                    cpu->registers.A |= old << 7;
                    cpu->registers.flags.N = cpu->registers.flags.H = 0;
                    break;
                case 4: // DAA
                    context.cycles += 4;
                    old = cpu->registers.A;
                    daa(&context);
                    updateFlags_withOptions(&cpu->registers, old, new, 0, 1, FLAG_N);
                    break;
                case 5: // CPL
                    context.cycles += 4;
                    cpu->registers.A = ~cpu->registers.A;
                    cpu->registers.flags.N = cpu->registers.flags.H = 1;
                    break;
                case 6: // SCF
                    context.cycles += 4;
                    cpu->registers.flags.C = 1;
                    cpu->registers.flags.N = cpu->registers.flags.H = 0;
                    break;
                case 7: // CCF
                    context.cycles += 4;
                    cpu->registers.flags.H = cpu->registers.flags.C;
                    cpu->registers.flags.C = ~cpu->registers.flags.C;
                    cpu->registers.flags.N = 0;
                    break;
                }
                break;
            }
            break;
        case 1:
            if (context.z == 6 && context.y == 6) { // HALT
            } else { // LD r[y], r[z]
                context.cycles += 4;
                write_r(context.y, read_r(context.z, &context), &context);
            }
            break;
        case 2: // ALU[y] r[z]
            execute_alu(context.y, read_r(context.z, &context), &context);
            break;
        case 3:
            switch (context.z) {
            case 0: // RET cc[y]
                context.cycles += 5;
                if (read_cc(context.y, &context)) {
                    cpu->registers.PC = pop(cpu);
                    context.cycles += 6;
                }
                break;
            case 1:
                switch (context.q) {
                case 0: // POP rp2[p]
                    context.cycles += 10;
                    write_rp2(context.p, pop(cpu), &context);
                    break;
                case 1:
                    switch (context.p) {
                    case 0: // RET
                        context.cycles += 10;
                        cpu->registers.PC = pop(cpu);
                        break;
                    case 1: // EXX
                        context.cycles += 4;
                        exx(&cpu->registers);
                        break;
                    case 2: // JP HL
                        context.cycles += 4;
                        cpu->registers.PC = cpu->registers.HL;
                        break;
                    case 3: // LD SP, HL
                        context.cycles += 6;
                        cpu->registers.SP = cpu->registers.HL;
                        break;
                    }
                    break;
                }
                break;
            case 2: // JP cc[y], nn
                context.cycles += 10;
                nn = context.nn(&context);
                if (read_cc(context.y, &context)) {
                    cpu->registers.PC = nn;
                }
                break;
            case 3:
                switch (context.y) {
                case 0: // JP nn
                    context.cycles += 10;
                    cpu->registers.PC = context.nn(&context);
                    break;
                case 1: // 0xCB prefixed opcodes
                    break;
                case 2: // OUT (n), A
                    context.cycles += 11;
                    ioDevice = cpu->devices[context.n(&context)];
                    if (ioDevice.write_out != NULL) {
                        ioDevice.write_out(ioDevice.device, cpu->registers.A);
                    }
                    break;
                case 3: // IN A, (n)
                    context.cycles += 11;
                    ioDevice = cpu->devices[context.n(&context)];
                    if (ioDevice.read_in != NULL) {
                        cpu->registers.A = ioDevice.read_in(ioDevice.device);
                    }
                    break;
                case 4: // EX (SP), HL
                    context.cycles += 19;
                    old16 = cpu_read_word(cpu, cpu->registers.SP);
                    cpu_write_word(cpu, cpu->registers.SP, cpu->registers.HL);
                    cpu->registers.HL = old16;
                    break;
                case 5: // EX DE, HL
                    context.cycles += 4;
                    exDEHL(&cpu->registers);
                    break;
                case 6: // DI
                    cpu->IFF1 = 0;
                    cpu->IFF2 = 0;
                    break;
                case 7: // EI
                    cpu->IFF1 = 1;
                    cpu->IFF2 = 1;
                    cpu->IFF_wait = 1;
                    break;
                }
                break;
            case 4: // CALL cc[y], nn
                context.cycles += 10;
                nn = context.nn(&context);
                if (read_cc(context.y, &context)) {
                    context.cycles += 7;
                    push(cpu, cpu->registers.PC);
                    cpu->registers.PC = nn;
                }
                break;
            case 5:
                switch (context.q) {
                    case 0: // PUSH r2p[p]
                        context.cycles += 11;
                        push(cpu, read_rp2(context.p, &context));
                        break;
                    case 1:
                        switch (context.p) {
                            case 0: // CALL nn
                                context.cycles += 17;
                                nn = context.nn(&context);
                                push(cpu, cpu->registers.PC);
                                cpu->registers.PC = nn;
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
                execute_alu(context.y, context.n(&context), &context);
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
