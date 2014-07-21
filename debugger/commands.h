#ifndef DEBUGGER_COMMANDS_H
#define DEBUGGER_COMMANDS_H

#include "debugger.h"
#include "memory.h"

int command_hexdump(struct debugger_state *state, int argc, char **argv);
int command_disassemble(struct debugger_state *state, int argc, char **argv);
int command_print_registers(struct debugger_state *state, int argc, char **argv);
int command_print_expression(struct debugger_state *state, int argc, char **argv);
int command_stack(struct debugger_state *state, int argc, char **argv);
int command_print_mappings(struct debugger_state *state, int argc, char **argv);
int command_unhalt(struct debugger_state *state, int argc, char **argv);
int command_run(struct debugger_state *state, int argc, char **argv);
int command_step(struct debugger_state *state, int argc, char **argv);
int command_stop(struct debugger_state *state, int argc, char **argv);

void register_hexdump(const char *, int, ti_mmu_t *);
void register_disassemble(const char *, int, ti_mmu_t *);
void register_print_registers(const char *, int, z80cpu_t *);
void register_print_expression(const char *, int);
void register_stack(const char *, int, z80cpu_t *);
void register_print_mappings(const char *, int);
void register_unhalt(const char *, int, z80cpu_t *);
void register_step(const char *, int);
void register_run(const char *, int);
void register_stop(const char *, int);

uint16_t parse_expression(debugger_state_t *, const char *);
#endif
