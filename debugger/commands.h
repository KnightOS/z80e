#ifndef DEBUGGER_COMMANDS_H
#define DEBUGGER_COMMANDS_H

#include "debugger.h"
#include "memory.h"

int command_hexdump(struct debugger_state *state, int argc, char **argv);
int command_disassemble(struct debugger_state *state, int argc, char **argv);
int command_print_registers(struct debugger_state *state, int argc, char **argv);
int command_print_expression(struct debugger_state *state, int argc, char **argv);
int command_stack(struct debugger_state *state, int argc, char **argv);

void register_hexdump(const char *, ti_mmu_t *);
void register_disassemble(const char *, ti_mmu_t *);
void register_print_registers(const char *name, z80cpu_t *cpu);
void register_print_expression(const char *name);
void register_stack(const char *name);

uint16_t parse_expression(debugger_state_t *, const char *);
#endif
