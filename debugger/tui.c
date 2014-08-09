#include "tui.h"
#include "debugger.h"
#include "disassemble.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifndef EMSCRIPTEN
#include <readline/readline.h>
#include <readline/history.h>



int print_tui(struct debugger_state *a, const char *b, ...) {
	va_list list;
	va_start(list, b);
#ifdef CURSES
	return vwprintw(((tui_state_t *)a->interface_state)->debugger_window, b, list);
#else
	return vprintf(b, list);
#endif
}

int vprint_tui(struct debugger_state *a, const char *b, va_list list) {
#ifdef CURSES
	return vwprintw(((tui_state_t *)a->interface_state)->debugger_window, b, list);
#else
	return vprintf(b, list);
#endif
}
#endif

#ifndef EMSCRIPTEN
void tui_close_window(struct debugger_state *state) {
	free(state);
}

debugger_state_t *tui_new_state(struct debugger_state *state, const char *title) {
	debugger_state_t *stat = calloc(sizeof(debugger_state_t), 1);
	stat->print = print_tui;
	stat->vprint = vprint_tui;
	stat->state = 0;
	stat->interface_state = state->interface_state;
	stat->asic = state->asic;
	stat->debugger = state->debugger;
	stat->create_new_state = tui_new_state;
	stat->close_window = tui_close_window;
	return stat;
}

tui_state_t *current_state;

#ifdef CURSES
#define dprint(...) wprintw(state->debugger_window, __VA_ARGS__)
#else
#define dprint(...) printf(__VA_ARGS__)
#endif
void tui_init(tui_state_t *notused) {
}

struct tui_disasm {
	ti_mmu_t *mmu;
	char *string_pointer;
};

uint8_t tui_disassemble_read(struct disassemble_memory *state, uint16_t mem) {
	struct tui_disasm *disasm = state->extra_data;
	return ti_read_byte(disasm->mmu, mem);
}

int tui_disassemble_write(struct disassemble_memory *state, const char *format, ...) {
	struct tui_disasm *disasm = state->extra_data;
	va_list list;
	va_start(list, format);
	int count = vsprintf(disasm->string_pointer, format, list);
	disasm->string_pointer += count;
	return count;
}

void tui_tick(tui_state_t *state) {
	current_state = state;
	asic_t *asic = state->debugger->asic;
	struct tui_disasm disasm_custom = { asic->mmu, 0 };
	struct disassemble_memory disasm = { tui_disassemble_read, 0, &disasm_custom };
	while (1) {
                char prompt_buffer[80];
		char *current_pointer = prompt_buffer;
		ti_mmu_bank_state_t *st = &asic->mmu->banks[asic->cpu->registers.PC / 0x4000];
		current_pointer += sprintf(prompt_buffer, "z80e [%c:%02X:0x%04X ", st->flash ? 'F' : 'R', st->page, asic->cpu->registers.PC);

		disasm_custom.string_pointer = current_pointer;
		disasm.current = asic->cpu->registers.PC;
		parse_instruction(&disasm, tui_disassemble_write);
		current_pointer = disasm_custom.string_pointer;

		sprintf(current_pointer, "] %s> ", asic->cpu->halted ? "HALT " : "");
		#ifdef CURSES
			char _result[256];
			wprintw(state->debugger_window, "%s", prompt_buffer);
			echo();
			wgetnstr(state->debugger_window, _result, 256);
			noecho();
			char *result = _result;
		#else
			char *result = readline(prompt_buffer);
		#endif
		if (result) {
			int from_history = 0;

			if (*result == 0) {
				HIST_ENTRY *hist = history_get(where_history());
				if (hist == 0) {
					#ifndef CURSES
						free(result);
					#endif
					continue;
				}
				result = (char *)hist->line;
				from_history = 1;
			}
			if (strcmp(result, "exit") == 0) {
				break;
			}

			add_history(result);
			debugger_command_t *command = 0;

			int argc = 0;
			char **cmdline = debugger_parse_commandline(result, &argc);

			if (strcmp(cmdline[0], "set") == 0) {
				if (argc != 2) {
					dprint("Invalid use of 'set'!\n");
				} else {
					if (strcmp(cmdline[1], "echo") == 0) {
						state->debugger->flags.echo = 1;
					} else if (strcmp(cmdline[1], "echo_reg") == 0) {
						state->debugger->flags.echo_reg = 1;
					} else {
						dprint("Unknown variable '%s'!\n", cmdline[1]);
					}
				}
			} else if (strcmp(cmdline[0], "unset") == 0) {
				if (argc != 2) {
					dprint("Invalid use of 'unset'!\n");
				} else {
					if (strcmp(cmdline[1], "echo") == 0) {
						state->debugger->flags.echo = 0;
					} else if (strcmp(cmdline[1], "echo_reg") == 0) {
						state->debugger->flags.echo_reg = 0;
					} else {
						dprint("Unknown variable '%s'!\n", cmdline[1]);
					}
				}
			} else {
				int status = find_best_command(state->debugger, cmdline[0], &command);
				if (status == -1) {
					dprint("Error: Ambiguous command %s\n", result);
				} else if (status == 0) {
					dprint("Error: Unknown command %s\n", result);
				} else {
					debugger_state_t dstate = { print_tui, vprint_tui, command->state, state, asic, state->debugger, tui_new_state, tui_close_window };
					debugger_state_t *used_state = tui_new_state(&dstate, cmdline[0]);
					used_state->state = command->state;
					int output = command->function(used_state, argc, cmdline);
					tui_close_window(used_state);
					if (output != 0) {
						dprint("The command returned %d\n", output);
					}
				}
			}

			char **tmp = cmdline;
			while (*tmp != 0) {
				free(*tmp);
				tmp++;
			}

			free(cmdline);
			if (!from_history) {
				#ifndef CURSES
					free(result);
				#endif
			}
		} else if (result == NULL) {
			break;
		}
	}
}
#endif
