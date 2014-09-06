#ifndef DEBUGGER_TUI_H
#define DEBUGGER_TUI_H

#include <z80e/ti/asic.h>
#include <z80e/debugger/debugger.h>

typedef struct {
	debugger_t *debugger;
} tui_state_t;

void tui_init(tui_state_t *state);
void tui_tick(tui_state_t *state);

#endif
