#include <stdio.h>
#include <stdint.h>
#include "registers.h"

void exAFAF(z80registers_t *r) {
    uint16_t temp = r->_AF;
    r->_AF = r->AF;
    r->AF = temp;
}

void exDEHL(z80registers_t *r) {
    uint16_t temp = r->HL;
    r->HL = r->DE;
    r->DE = temp;
}

void exx(z80registers_t *r) {
    uint16_t temp;
    temp = r->_HL; r->_HL = r->HL; r->HL = temp;
    temp = r->_DE; r->_DE = r->DE; r->DE = temp;
    temp = r->_BC; r->_BC = r->BC; r->BC = temp;
}

const uint64_t m1  = 0x5555555555555555;
const uint64_t m2  = 0x3333333333333333;
const uint64_t m4  = 0x0f0f0f0f0f0f0f0f;
const uint64_t h01 = 0x0101010101010101;
int popcount(uint64_t x) {
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return (x * h01) >> 56;
}

void updateFlags(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int carryBit) {
    updateFlags_withOptions(r, oldValue, newValue, 0, 0, carryBit, FLAG_NONE);
}

void updateFlags_subtraction(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int carryBit) {
    updateFlags_withOptions(r, oldValue, newValue, 1, 0, carryBit, FLAG_NONE);
}

void updateFlags_parity(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int carryBit) {
    updateFlags_withOptions(r, oldValue, newValue, 0, 1, carryBit, FLAG_NONE);
}

void updateFlags_except(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int carryBit, z80flags unaffected) {
    updateFlags_withOptions(r, oldValue, newValue, 0, 0, carryBit, unaffected);
}

void updateParity(z80registers_t *r, uint16_t value) {
    r->flags.PV = popcount(value) % 2 == 0;
}

void updateFlags_withOptions(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int subtraction, int parity, int carryBit, z80flags unaffected) {
    if (!(unaffected & FLAG_S)) {
        if (carryBit) {
            r->flags.S = (newValue & 0x8000) != 0;
        } else {
            r->flags.S = (newValue & 0x80) != 0;
        }
    }
    if (!(unaffected & FLAG_Z)) {
        r->flags.Z = newValue == 0;
    }
    if (!(unaffected & FLAG_H)) {
        if (carryBit) {
            if (subtraction) {
                r->flags.H = (newValue & 0xFFF) > (oldValue & 0xFFF);
            } else {
                r->flags.H = (newValue & 0xFFF) < (oldValue & 0xFFF);
            }
        } else {
            if (subtraction) {
                r->flags.H = (newValue & 0xF) > (oldValue & 0xF);
            } else {
                r->flags.H = (newValue & 0xF) < (oldValue & 0xF);
            }
        }
    }
    if (!(unaffected & FLAG_PV)) {
        if (parity) {
            updateParity(r, newValue);
        } else {
            if (carryBit) {
	        r->flags.PV = (newValue & 0x8000) != (oldValue & 0x8000);
            } else {
	        r->flags.PV = (newValue & 0x80) != (oldValue & 0x80);
            }
        }
    }
    if (!(unaffected & FLAG_N)) {
        r->flags.N = subtraction ? 1 : 0;
    }
    if (!(unaffected & FLAG_C)) {
        if (subtraction) {
            r->flags.C = newValue > oldValue;
        } else {
            r->flags.C = newValue < oldValue;
        }
    }

    if (carryBit) {
        newValue >>= 8;
    }

    if (!(unaffected & FLAG_3)) {
        r->flags._3 = newValue & 0x04 ? 1 : 0;
    }
    if (!(unaffected & FLAG_5)) {
        r->flags._5 = newValue & 0x10 ? 1 : 0;
    }
}

void print_state(z80registers_t *r) {
    printf("   AF: 0x%04X   BC: 0x%04X   DE: 0x%04X  HL: 0x%04X\n", r->AF, r->BC, r->DE, r->HL);
    printf("  'AF: 0x%04X  'BC: 0x%04X  'DE: 0x%04X 'HL: 0x%04X\n", r->_AF, r->_BC, r->_DE, r->_HL);
    printf("   PC: 0x%04X   SP: 0x%04X   IX: 0x%04X  IY: 0x%04X\n", r->PC, r->SP, r->IX, r->IY);
    printf("Flags: ");
    if (r->flags.S) printf("S ");
    if (r->flags.Z) printf("Z ");
    if (r->flags.H) printf("H ");
    if (r->flags._3) printf("5 ");
    if (r->flags.PV) printf("P/V ");
    if (r->flags._5) printf("3 ");
    if (r->flags.N) printf("N ");
    if (r->flags.C) printf("C ");
    if (r->F == 0) printf("None set");
    printf("\n");
}
