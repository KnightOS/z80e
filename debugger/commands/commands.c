#include "memory.h"
#include "cpu.h"
#include "debugger.h"

int command_hexdump(struct debugger_state *state, int argc, char **argv);
int command_disassemble(struct debugger_state *state, int argc, char **argv);
int command_print_registers(struct debugger_state *state, int argc, char **argv);

void register_hexdump(const char *name, ti_mmu_t *mmu) {
	register_command(name, command_hexdump, mmu);
}

void register_disassemble(const char *name, ti_mmu_t *mmu) {
	register_command(name, command_disassemble, mmu);
}

void register_print_registers(const char *name, z80cpu_t *cpu) {
	register_command(name, command_print_registers, cpu);
}
