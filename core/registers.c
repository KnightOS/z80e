#include <stdint.h>
#include "registers.h"

void exAFAF(z80registers_t *r) {
    uint8_t temp = r->_AF;
    r->_AF = r->AF;
    r->AF = temp;
}

void exHLDE(z80registers_t *r) {
    uint8_t temp = r->HL;
    r->HL = r->DE;
    r->DE = temp;
}

void exx(z80registers_t *r) {
    uint8_t temp;
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

void updateFlags(z80registers_t *r, uint16_t oldValue, uint16_t newValue) {
    updateFlags_withOptions(r, oldValue, newValue, 0, 0, FLAG_NONE);
}

void updateFlags_subtraction(z80registers_t *r, uint16_t oldValue, uint16_t newValue) {
    updateFlags_withOptions(r, oldValue, newValue, 1, 0, FLAG_NONE);
}

void updateFlags_parity(z80registers_t *r, uint16_t oldValue, uint16_t newValue) {
    updateFlags_withOptions(r, oldValue, newValue, 0, 1, FLAG_NONE);
}

void updateFlags_except(z80registers_t *r, uint16_t oldValue, uint16_t newValue, z80flags unaffected) {
    updateFlags_withOptions(r, oldValue, newValue, 0, 0, unaffected);
}

void updateParity(z80registers_t *r) {
    r->flags.PV = popcount(r->A) % 1;
}

void updateFlags_withOptions(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int subtraction, int parity, z80flags unaffected) {
    if (!(unaffected & FLAG_S)) {
        r->flags.S = (r->A & 0x80) == 0x80;
    }
    if (!(unaffected & FLAG_Z)) {
        r->flags.Z = newValue == 0;
    }
    if (!(unaffected & FLAG_H)) {
        if (subtraction) {
            r->flags.H = (newValue & 0xF) > (oldValue & 0xF);
        } else {
            r->flags.H = (newValue & 0xF) < (oldValue & 0xF);
        }
    }
    if (!(unaffected & FLAG_PV)) {
        if (parity) {
            updateParity(r);
        } else {
            r->flags.PV = (oldValue & 0x80) != (newValue & 0x80);
        }
    }
    if (!(unaffected & FLAG_N)) {
        r->flags.N = subtraction;
    }
    if (!(unaffected & FLAG_C)) {
        if (subtraction) {
            r->flags.C = newValue > oldValue;
        } else {
            r->flags.C = newValue < oldValue;
        }
    }
}
