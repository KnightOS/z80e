#ifndef DEBUGGER_TUI_H
#define DEBUGGER_TUI_H

#include "asic.h"

#ifdef CURSES
#include <curses.h>
#endif

typedef struct {
	asic_t *asic;
#ifdef CURSES
	WINDOW *debugger_window;
#endif
} tui_state_t;

char **tui_parse_commandline(const char *, int *);

#ifndef EMSCRIPTEN
void tui_init(tui_state_t *state);
void tui_tick(tui_state_t *state);
#endif

#endif
