#include <stdio.h>
#include <stdint.h>
#include "disassemble.h"

struct context {
    uint8_t prefix;
    uint8_t second_prefix;
    struct disassemble_memory *memory;
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
    write_pointer write;
};

void parse_n(struct context *context) {
    context->write("0x%02X", context->memory->read_byte(context->memory, context->memory->current++));
}

void parse_d(struct context *context) {
    context->write("%d", (int8_t)context->memory->read_byte(context->memory, context->memory->current++));
}

void parse_nn(struct context *context) {
    uint8_t first = context->memory->read_byte(context->memory, context->memory->current++);
    uint8_t second = context->memory->read_byte(context->memory, context->memory->current++);
    uint16_t total = first | second << 8;
    context->write("0x%04x", total);
}

void parse_HorIHw(struct context *context) {
	switch (context->second_prefix) {
	case 0xDD: context->write("IXH"); break;
	case 0xFD: context->write("IYH"); break;
	default: context->write("H"); break;
	}
}

void parse_LorILw(struct context *context) {
	switch (context->second_prefix) {
	case 0xDD: context->write("IXL"); break;
	case 0xFD: context->write("IYL"); break;
	default: context->write("L"); break;
	}
}

void parse_HLorIr(struct context *context) {
	switch (context->second_prefix) {
	case 0xDD: context->write("IX"); break;
	case 0xFD: context->write("IY"); break;
	default: context->write("HL"); break;
	}
}

void parse_r(struct context *context, uint8_t part) {
	switch (part) {
	case 0: context->write("B"); break;
	case 1: context->write("C"); break;
	case 2: context->write("D"); break;
	case 3: context->write("E"); break;
	case 4: parse_HorIHw(context);
		break;
	case 5: parse_LorILw(context);
		break;
	case 6:
	        if (context->second_prefix == 0xDD) {
			uint8_t d = context->memory->read_byte(context->memory, context->memory->current++);
			context->write("(IX + 0x%X)", d);
		} else if (context->second_prefix == 0xFD) {
			uint8_t d = context->memory->read_byte(context->memory, context->memory->current++);
			context->write("(IY + 0x%X)", d);
		} else {
			context->write("(HL)");
		}
		break;
	case 7: context->write("A"); break;
	}
}

void parse_cc(int i, struct context *context) {
    switch (i) {
    case 0: context->write("NZ"); break;
    case 1: context->write("Z"); break;
    case 2: context->write("NC"); break;
    case 3: context->write("C"); break;
    case 4: context->write("PO"); break;
    case 5: context->write("PE"); break;
    case 6: context->write("P"); break;
    case 7: context->write("M"); break;
    }
}

void parse_rp(int i, struct context *context) {
    switch (i) {
    case 0: context->write("BC"); break;
    case 1: context->write("DE"); break;
    case 2: parse_HLorIr(context); break;
    case 3: context->write("SP"); break;
    }
}

void parse_rp2(int i, struct context *context) {
    switch (i) {
    case 0: context->write("BC"); break;
    case 1: context->write("DE"); break;
    case 2: parse_HLorIr(context); break;
    case 3: context->write("AF"); break;
    }
}

void parse_rot(int y, struct context *context) {
    switch (y) { 
    case 0: // RLC r[z]
        context->write("RLC ");
        break;
    case 1: // RRC r[z]
        context->write("RRC ");
        break;
    case 2: // RL r[z]
        context->write("RL ");
        break;
    case 3: // RR r[z]
        context->write("RR ");
        break;
    case 4: // SLA r[z]
        context->write("SLA ");
        break;
    case 5: // SRA r[z]
        context->write("SRA ");
        break;
    case 6: // SLL r[z]
        context->write("SLL ");
        break;
    case 7: // SRL r[z]
        context->write("SRL ");
        break;
    }
}

void parse_im(int y, struct context *context) {
    switch (y) {
        case 0: context->write("IM 0"); break;
        case 1: context->write("IM 0"); break;
        case 2: context->write("IM 1"); break;
        case 3: context->write("IM 2"); break;
        case 4: context->write("IM 0"); break;
        case 5: context->write("IM 0"); break;
        case 6: context->write("IM 1"); break;
        case 7: context->write("IM 2"); break;
    }
}

void parse_alu(int i, struct context *context) {
    switch (i) {
    case 0: // ADD A, v
        context->write("ADD A, ");
        break;
    case 1: // ADC A, v
        context->write("ADC A, ");
        break;
    case 2: // SUB v
        context->write("SUB ");
        break;
    case 3: // SBC v
        context->write("SBC ");
        break;
    case 4: // AND v
        context->write("AND ");
        break;
    case 5: // XOR v
        context->write("XOR ");
        break;
    case 6: // OR v
        context->write("OR ");
        break;
    case 7: // CP v
        context->write("CP ");
        break;
    }
}

void parse_bli(int y, int z, struct context *context) {
    switch (y) {
    case 4:
        switch (z) {
        case 0: // LDI
            context->write("LDI");
            break;
        case 1: // CPI
            context->write("CPI");
            break;
        case 2: // INI
            context->write("INI");
            break;
        case 3: // OUTI
            context->write("OUTI");
            break;
        }
        break;
    case 5:
        switch (z) {
        case 0: // LDD
            context->write("LDD");
            break;
        case 1: // CPD
            context->write("CPD");
            break;
        case 2: // IND
            context->write("IND");
            break;
        case 3: // OUTD
            context->write("OUTD");
            break;
        }
        break;
    case 6:
        switch (z) {
        case 0: // LDIR
            context->write("LDIR");
            break;
        case 1: // CPIR
            context->write("CPIR");
            break;
        case 2: // INIR
            context->write("INIR");
            break;
        case 3: // OTIR
            context->write("OTIR");
            break;
        }
        break;
    case 7:
        switch (z) {
        case 0: // LDDR
            context->write("LDDR");
            break;
        case 1: // CPDR
            context->write("CPDR");
            break;
        case 2: // INDR
            context->write("INDR");
            break;
        case 3: // OTDR
            context->write("OTDR");
            break;
        }
        break;
    }
}

void parse_instruction(struct disassemble_memory *d, write_pointer write_p) {
	struct context context;

        context.prefix = 0;

        context.second_prefix = 0;

        uint8_t data = d->read_byte(d, d->current);

	if (data == 0xDD || data == 0xFD) {
		context.second_prefix = d->read_byte(d, d->current++);
                data = d->read_byte(d, d->current);
        }
	if (data == 0xCB || data == 0xED) {
		context.prefix = d->read_byte(d, d->current++);
                data = d->read_byte(d, d->current);
        }

	if (context.second_prefix && context.prefix) {
		data = d->read_byte(d, d->current-- + 1);
	}

	context.opcode = data;
        d->current++;
        context.memory = d;
	write_pointer write = context.write = write_p;

        if (context.prefix == 0xCB || context.prefix == 0xED) {
            switch (context.prefix) {
            case 0xCB:
                switch (context.x) {
                case 0: // rot[y] r[z]
                    parse_rot(context.y, &context);
                    parse_r(&context, context.z);
                    break;
                case 1: // BIT y, r[z]
                    write("BIT 0x%X, ", context.y);
                    parse_r(&context, context.z);
                    break;
                case 2: // RES y, r[z]
                    write("RES 0x%X, ", context.y);
                    parse_r(&context, context.z);
                    break;
                case 3: // SET y, r[z]
                    write("SET 0x%X, ", context.y);
                    parse_r(&context, context.z);
                    break;
                }
                break;
            case 0xED:
                switch (context.x) {
                case 1:
                    switch (context.z) {
                    case 0:
                        if (context.y == 6) { // IN (C)
                            write("IN (C)");
                        } else { // IN r[y], (C)
                            write("IN ");
                            parse_r(&context, context.y);
                            write(", (C)");
                        }
                        break;
                    case 1:
                        if (context.y == 6) { // OUT (C), 0
                            write("OUT (C), 0");
                        } else { // OUT (C), r[y]
                            write("OUT (C), ");
                            parse_r(&context, context.y);
                        }
                        break;
                    case 2:
                        if (context.q == 0) { // SBC HL, rp[p]
                            write("SBC HL, ");
                            parse_rp(context.p, &context);
                        } else { // ADC HL, rp[p]
                            write("ADC HL, ");
                            parse_rp(context.p, &context);
                        }
                        break;
                    case 3:
                        if (context.q == 0) { // LD (nn), rp[p]
                            write("LD (");
                            parse_nn(&context);
                            write("), ");
                            parse_rp(context.p, &context);
                        } else { // LD rp[p], (nn)
                            write("LD ");
                            parse_rp(context.p, &context);
                            write(", (");
                            parse_nn(&context);
                            write(")");
                        }
                        break;
                    case 4: // NEG
                        write("NEG");
                        break;
                    case 5:
                        if (context.y == 1) { // RETI
                            // Note: Intentionally not implemented, not relevant for TI devices
                        } else { // RETN
                            // Note: Intentionally not implemented, not relevant for TI devices
                        }
                        break;
                    case 6: // IM im[y]
                        parse_im(context.y, &context);
                        break;
                    case 7:
                        switch (context.y) {
                        case 0: // LD I, A
                            write("LD I, A");
                            break;
                        case 1: // LD R, A
                            write("LD R, A");
                            break;
                        case 2: // LD A, I
                            write("LD A, I");
                            break;
                        case 3: // LD A, R
                            write("LD A, R");
                            break;
                        case 4: // RRD
                            write("RRD");
                            break;
                        case 5: // RLD
                            write("RLD");
                            break;
                        default: // NOP (invalid instruction)
                            write("*INVALID* 0x%X", context.opcode);
                            break;
                        }
                        break;
                    }
                    break;
                case 2:
                    if (context.y >= 4) { // bli[y,z]
                        parse_bli(context.y, context.z, &context);
                    } else { // NONI (invalid instruction)
                        write("*INVALID* 0x%X", context.opcode);
                    }
                    break;
                default: // NONI (invalid instruction)
                    write("*INVALID* 0x%X", context.opcode);
                    break;
                }
                break;
            }
        } else {
            switch (context.x) {
            case 0:
                switch (context.z) {
                case 0:
                    switch (context.y) {
                    case 0: // NOP
                        write("NOP");
                        break;
                    case 1: // EX AF, AF'
                        write("EX AF, AF'");
                        break;
                    case 2: // DJNZ d
                        write("DJNZ ");
                        parse_d(&context);
                        break;
                    case 3: // JR d
                        write("JR ");
                        parse_d(&context);
                        break;
                    case 4:
                    case 5:
                    case 6:
                    case 7: // JR cc[y-4], d
                        write("JR ");
                        parse_cc(context.y - 4, &context);
                        write(", ");
                        parse_d(&context);
                        break;
                    }
                    break;
                case 1:
                    switch (context.q) {
                    case 0: // LD rp[p], nn
                        write("LD ");
                        parse_rp(context.p, &context);
                        write(", ");
                        parse_nn(&context);
                        break;
                    case 1: // ADD HL, rp[p]
                        write("ADD HL, ");
                        parse_rp(context.p, &context);
                        break;
                    }
                    break;
                case 2:
                    switch (context.q) {
                    case 0:
                        switch (context.p) {
                        case 0: // LD (BC), A
                            write("LD (BC), A");
                            break;
                        case 1: // LD (DE), A
                            write("LD (DE), A");
                            break;
                        case 2: // LD (nn), HL
                            write("LD (");
                            parse_nn(&context);
                            write("), ");
                            parse_HLorIr(&context);
                            break;
                        case 3: // LD (nn), A
                            write("LD (");
                            parse_nn(&context);
                            write("), A");
                            break;
                        }
                        break;
                    case 1:
                        switch (context.p) {
                        case 0: // LD A, (BC)
                            write("LD A, (BC)");
                            break;
                        case 1: // LD A, (DE)
                            write("LD A, (DE)");
                            break;
                        case 2: // LD HL, (nn)
                            write("LD");
                            parse_HLorIr(&context);
                            write(", (");
                            parse_nn(&context);
                            write(")");
                            break;
                        case 3: // LD A, (nn)
                            write("LD A, (");
                            parse_nn(&context);
                            write(")");
                            break;
                        }
                        break;
                    }
                    break;
                case 3:
                    switch (context.q) {
                    case 0: // INC rp[p]
                        write("INC ");
                        parse_rp(context.p, &context);
                        break;
                    case 1: // DEC rp[p]
                        write("DEC ");
                        parse_rp(context.p, &context);
                        break;
                    }
                    break;
                case 4: // INC r[y]
                    write("INC ");
                    parse_r(&context, context.y);
                    break;
                case 5: // DEC r[y]
                    write("DEC ");
                    parse_r(&context, context.y);
                    break;
                case 6: // LD r[y], n
                    write("LD ");
                    parse_r(&context, context.y);
                    write(", ");
                    parse_n(&context);
                    break;
                case 7:
                    switch (context.y) {
                    case 0: // RLCA
                        write("RLCA");
                        break;
                    case 1: // RRCA
                        write("RRCA");
                        break;
                    case 2: // RLA
                        write("RLA");
                        break;
                    case 3: // RRA
                        write("RRA");
                        break;
                    case 4: // DAA
                        write("DAA");
                        break;
                    case 5: // CPL
                        write("CPL");
                        break;
                    case 6: // SCF
                        write("SCF");
                        break;
                    case 7: // CCF
                        write("CCF");
                        break;
                    }
                    break;
                }
                break;
            case 1:
                if (context.z == 6 && context.y == 6) { // HALT
                    write("HALT");
                } else { // LD r[y], r[z]
                    write("LD ");
                    parse_r(&context, context.y);
                    write(", ");
                    parse_r(&context, context.z);
                }
                break;
            case 2: // ALU[y] r[z]
                parse_alu(context.y, &context);
                parse_r(&context, context.z);
                break;
            case 3:
                switch (context.z) {
                case 0: // RET cc[y]
                    write("RET ");
                    parse_cc(context.y, &context);
                    break;
                case 1:
                    switch (context.q) {
                    case 0: // POP rp2[p]
                        write("POP ");
                        parse_rp2(context.p,&context);
                        break;
                    case 1:
                        switch (context.p) {
                        case 0: // RET
                            write("RET");
                            break;
                        case 1: // EXX
                            write("EXX");
                            break;
                        case 2: // JP HL
                            write("JP HL");
                            break;
                        case 3: // LD SP, HL
                            write("LD SP, HL");
                            break;
                        }
                        break;
                    }
                    break;
                case 2: // JP cc[y], nn
                    write("JP ");
                    parse_cc(context.y, &context);
                    write(", ");
                    parse_nn(&context);
                    break;
                case 3:
                    switch (context.y) {
                    case 0: // JP nn
                        write("JP ");
                        parse_nn(&context);
                        break;
                    case 1: // 0xCB prefixed opcodes
                        // Handled before!
                        break;
                    case 2: // OUT (n), A
                        write("OUT (");
                        parse_n(&context);
                        write("), A");
                        break;
                    case 3: // IN A, (n)
                        write("IN A, (");
                        parse_n(&context);
                        write(")");
                        break;
                    case 4: // EX (SP), HL
                        write("EX (SP), ");
                        parse_HLorIr(&context);
                        break;
                    case 5: // EX DE, HL
                        write("EX DE, HL");
                        break;
                    case 6: // DI
                        write("DI");
                        break;
                    case 7: // EI
                        write("EI");
                        break;
                    }
                    break;
                case 4: // CALL cc[y], nn
                    write("CALL ");
                    parse_cc(context.y, &context);
                    write(", ");
                    parse_nn(&context);
                    break;
                case 5:
                    switch (context.q) {
                    case 0: // PUSH r2p[p]
                        write("PUSH ");
                        parse_rp2(context.p, &context);
                        break;
                    case 1:
                        switch (context.p) {
                        case 0: // CALL nn
                            write("CALL ");
                            parse_nn(&context);
                            break;
                        case 1: // 0xDD prefixed opcodes
                            // Handled before
                            break;
                        case 2: // 0xED prefixed opcodes
                            // Handled before
                            break;
                        case 3: // 0xFD prefixed opcodes
                            // Handled before
                            break;
                        }
                        break; 
                    }
                    break;
                case 6: // alu[y] n
                    parse_alu(context.y, &context);
                    parse_n(&context);
                    break;
                case 7: // RST y*8
                    write("RST 0x%X", context.y * 8);
                    break;
                }
                break;
            }
        }

	if (context.prefix && context.second_prefix) {
		d->current++;
	}
}
