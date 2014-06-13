#ifndef DEBUGGER_COMMANDS_H
#define DEBUGGER_COMMANDS_H

#include "debugger.h"
#include "memory.h"

void register_hexdump(const char *, ti_mmu_t *);
void register_disassemble(const char *, ti_mmu_t *);
void register_print_registers(const char *name, z80cpu_t *cpu);
void register_print_expression(const char *name);

uint16_t parse_expression(debugger_state_t *, const char *);
#endif
