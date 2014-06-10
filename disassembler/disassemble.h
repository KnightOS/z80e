#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H

#include <stdint.h>

struct disassemble_memory {
    uint8_t (*read_byte)(struct disassemble_memory *, uint16_t);
    uint16_t current;
};


void parse_instruction(struct disassemble_memory *);

#endif
