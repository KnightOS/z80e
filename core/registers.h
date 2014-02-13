#ifndef REGISTERS_H
#define REGISTERS_H
#include <stdint.h>

typedef struct {
    union {
        uint16_t AF;
        struct {
            uint8_t A;
            uint8_t F;
        };
    };
    union {
        uint16_t BC;
        struct {
            uint8_t B;
            uint8_t C;
        };
    };
    union {
        uint16_t DE;
        struct {
            uint8_t D;
            uint8_t E;
        };
    };
    union {
        uint16_t HL;
        struct {
            uint8_t H;
            uint8_t L;
        };
    };
    uint8_t _AF, _BC, _DE, _HL;
    uint16_t PC, SP, IX, IY;
} z80registers_t;

void exAFAF(z80registers_t *r);
void exHLDE(z80registers_t *r);
void exx(z80registers_t *r);

#endif
