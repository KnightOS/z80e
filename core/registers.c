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
