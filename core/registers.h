#ifndef REGISTERS_H
#define REGISTERS_H
#include <stdint.h>

typedef struct {
    union {
        uint16_t AF;
        struct {
            uint8_t A;
            union {
                uint8_t F;
                struct {
                    uint8_t S  : 1;
                    uint8_t Z  : 1;
                    uint8_t    : 1;
                    uint8_t H  : 1;
                    uint8_t    : 1;
                    uint8_t PV : 1;
                    uint8_t N  : 1;
                    uint8_t C  : 1;
                } flags;
            };
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
    uint16_t _AF, _BC, _DE, _HL;
    uint16_t PC, SP;
    union {
        uint16_t IX;
        struct {
            uint8_t IXH;
            uint8_t IXL;
        };
    };
    union {
        uint16_t IY;
        struct {
            uint8_t IYH;
            uint8_t IYL;
        };
    };
    uint8_t I, R;
} z80registers_t;

typedef enum {
    FLAG_S =  1 << 7,
    FLAG_Z =  1 << 6,
    //          << 5,
    FLAG_H =  1 << 4,
    //          << 3
    FLAG_PV = 1 << 2,
    FLAG_N  = 1 << 1,
    FLAG_C  = 1 << 0,
    FLAG_NONE = 0
} z80flags;

typedef enum {
        A, F, AF, _AF,
        B, C, BC, _BC,
        D, E, DE, _DE,
        H, L, HL, _HL,
        PC, SP, I, R,
        IXH, IXL, IX,
        IYH, IYL, IY,
} registers;

void exAFAF(z80registers_t *r);
void exDEHL(z80registers_t *r);
void exx(z80registers_t *r);
void updateParity(z80registers_t *r);
void updateFlags(z80registers_t *r, uint16_t oldValue, uint16_t newValue);
void updateFlags_subtraction(z80registers_t *r, uint16_t oldValue, uint16_t newValue);
void updateFlags_parity(z80registers_t *r, uint16_t oldValue, uint16_t newValue);
void updateFlags_except(z80registers_t *r, uint16_t oldValue, uint16_t newValue, z80flags unaffected);
void updateFlags_withOptions(z80registers_t *r, uint16_t oldValue, uint16_t newValue, int subtraction, int parity, z80flags unaffected);

#endif
