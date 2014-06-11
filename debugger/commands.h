#ifndef DEBUGGER_COMMANDS_H
#define DEBUGGER_COMMANDS_H

#include "memory.h"

void register_hexdump(const char *, ti_mmu_t *);
void register_disassemble(const char *, ti_mmu_t *);
void register_print_registers(const char *name, z80cpu_t *cpu);

#endif
