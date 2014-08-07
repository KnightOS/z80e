#ifndef DEBUGGER_COMMANDS_H
#define DEBUGGER_COMMANDS_H

#include "debugger.h"
#include "memory.h"

int command_hexdump(debugger_state_t *state, int argc, char **argv);
int command_disassemble(debugger_state_t *state, int argc, char **argv);
int command_print_registers(debugger_state_t *state, int argc, char **argv);
int command_print_expression(debugger_state_t *state, int argc, char **argv);
int command_stack(debugger_state_t *state, int argc, char **argv);
int command_print_mappings(debugger_state_t *state, int argc, char **argv);
int command_unhalt(debugger_state_t *state, int argc, char **argv);
int command_run(debugger_state_t *state, int argc, char **argv);
int command_step(debugger_state_t *state, int argc, char **argv);
int command_stop(debugger_state_t *state, int argc, char **argv);
int command_on(debugger_state_t *state, int argc, char **argv);
int command_in(debugger_state_t *state, int argc, char **argv);
int command_out(debugger_state_t *state, int argc, char **argv);
int command_break(debugger_state_t *state, int argc, char **argv);

void register_hexdump(debugger_t *, const char *, int, ti_mmu_t *);
void register_disassemble(debugger_t *, const char *, int, ti_mmu_t *);
void register_print_registers(debugger_t *, const char *, int, z80cpu_t *);
void register_print_expression(debugger_t *, const char *, int);
void register_stack(debugger_t *, const char *, int, z80cpu_t *);
void register_print_mappings(debugger_t *, const char *, int);
void register_unhalt(debugger_t *, const char *, int, z80cpu_t *);
void register_step(debugger_t *, const char *, int);
void register_run(debugger_t *, const char *, int);
void register_stop(debugger_t *, const char *, int);
void register_on(debugger_t *, const char *, int);
void register_in(debugger_t *, const char *, int);
void register_out(debugger_t *, const char *, int);
void register_break(debugger_t *, const char *, int);

uint16_t parse_expression(debugger_state_t *, const char *);
#endif
