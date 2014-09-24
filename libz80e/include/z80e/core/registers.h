#ifndef REGISTERS_H
#define REGISTERS_H
#include <stdint.h>

typedef struct {
	union {
		uint16_t AF;
		struct {
			union {
				uint8_t F;
				struct {
					uint8_t C  : 1;
					uint8_t N  : 1;
					uint8_t PV : 1;
					uint8_t _3 : 1;
					uint8_t H  : 1;
					uint8_t _5 : 1;
					uint8_t Z  : 1;
					uint8_t S  : 1;
				} flags;
			};
			uint8_t A;
		};
	};
	union {
		uint16_t BC;
		struct {
			uint8_t C;
			uint8_t B;
		};
	};
	union {
		uint16_t DE;
		struct {
			uint8_t E;
			uint8_t D;
		};
	};
	union {
		uint16_t HL;
		struct {
			uint8_t L;
			uint8_t H;
		};
	};
	uint16_t _AF, _BC, _DE, _HL;
	uint16_t PC, SP;
	union {
		uint16_t IX;
		struct {
			uint8_t IXL;
			uint8_t IXH;
		};
	};
	union {
		uint16_t IY;
		struct {
			uint8_t IYL;
			uint8_t IYH;
		};
	};
	uint8_t I, R;
} z80registers_t;

typedef enum {
	FLAG_S =  1 << 7,
	FLAG_Z =  1 << 6,
	FLAG_5 =  1 << 5,
	FLAG_H =  1 << 4,
	FLAG_3 =  1 << 3,
	FLAG_PV = 1 << 2,
	FLAG_N  = 1 << 1,
	FLAG_C  = 1 << 0,
	FLAG_NONE = 0
} z80flags;

typedef enum {
	A = (1 << 0), F = (1 << 1), AF = (1 << 2), _AF = (1 << 3),
	B = (1 << 4), C = (1 << 5), BC = (1 << 6), _BC = (1 << 7),
	D = (1 << 8), E = (1 << 9), DE = (1 << 10), _DE = (1 << 11),
	H = (1 << 12), L = (1 << 13), HL = (1 << 14), _HL = (1 << 15),
	PC = (1 << 16), SP = (1 << 17), I = (1 << 18), R = (1 << 19),
	IXH = (1 << 20), IXL = (1 << 21), IX = (1 << 22),
	IYH = (1 << 23), IYL = (1 << 24), IY = (1 << 25),
} registers;

void exAFAF(z80registers_t *r);
void exDEHL(z80registers_t *r);
void exx(z80registers_t *r);
void updateParity(z80registers_t *r, uint32_t newValue);
void updateFlags(z80registers_t *r, uint32_t oldValue, uint32_t newValue, int word);
void updateFlags_subtraction(z80registers_t *r, uint32_t oldValue, uint32_t newValue, int word);
void updateFlags_parity(z80registers_t *r, uint32_t oldValue, uint32_t newValue, int word);
void updateFlags_except(z80registers_t *r, uint32_t oldValue, uint32_t newValue, int word, z80flags unaffected);
void updateFlags_withOptions(z80registers_t *r, uint32_t oldValue, uint32_t newValue, int subtraction, int parity, int word, z80flags unaffected);

void print_state(z80registers_t *);

#endif
