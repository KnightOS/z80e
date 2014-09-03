#include <z80e/debugger/debugger.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*openti_js_print_string)(int, const char *);
typedef void (*openti_js_window_closed)(int);
typedef int (*openti_js_new_state)(int, debugger_state_t *, const char *);

typedef struct {
	int js_reference;
	openti_js_print_string print_string;
	openti_js_new_state new_state;
	openti_js_window_closed window_closed;
} openti_interface_state;

char openti_print_buffer[256];

int openti_state_vprint(debugger_state_t *state, const char *format, va_list list) {
	openti_interface_state *istate = state->interface_state;
	int count = vsnprintf(openti_print_buffer, 256, format, list);
	istate->print_string(istate->js_reference, openti_print_buffer);
	return count;
}

int openti_state_print(debugger_state_t *state, const char *format, ...) {
	va_list list;
	va_start(list, format);
	return openti_state_vprint(state, format, list);
}

void openti_close_window(debugger_state_t *state) {
	openti_interface_state *istate = state->interface_state;
	istate->window_closed(istate->js_reference);
	free(state->interface_state);
	free(state);
}

debugger_state_t *openti_create_new_state(debugger_state_t *old_state, const char *title) {
	openti_interface_state *old_istate = old_state->interface_state;

	debugger_state_t *state = calloc(sizeof(debugger_state_t), 1);
	state->print = openti_state_print;
	state->vprint = openti_state_vprint;
	state->debugger = old_state->debugger;
	state->asic = state->debugger->asic;
	state->log = state->asic->log;
	state->create_new_state = openti_create_new_state;
	state->close_window = openti_close_window;

	openti_interface_state *istate = state->interface_state = calloc(sizeof(openti_interface_state), 1);
	istate->js_reference = old_istate->new_state(old_istate->js_reference, state, title);
	return state;
}

debugger_state_t *openti_new_state(debugger_t *debugger, int ref) {
	debugger_state_t *state = calloc(sizeof(debugger_state_t), 1);
	state->print = openti_state_print;
	state->vprint = openti_state_vprint;
	state->debugger = debugger;
	state->asic = debugger->asic;
	state->log = state->asic->log;
	state->create_new_state = openti_create_new_state;
	state->close_window = openti_close_window;

	openti_interface_state *istate = state->interface_state = calloc(sizeof(openti_interface_state), 1);
	istate->js_reference = ref;

	state->interface_state = istate;
	return state;
}
