#include "log.h"

#include <limits.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef CURSES
WINDOW *log_window = 0;
#endif

int logging_level = INT_MAX;

const char *loglevel_to_string(loglevel_t level) {
	switch (level) {
	case L_DEBUG:
		return "DEBUG";
	case L_INFO:
		return "INFO";
	case L_WARN:
		return "WARN";
	case L_ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

void log_message(loglevel_t level, const char *part, const char *format, ...) {
	if (level > logging_level) {
		return;
	}

	va_list format_list;
	va_start(format_list, format);
	#ifdef CURSES
		if (log_window == 0) {
			log_window = stdscr;
		}

		vwprintw(log_window, format, format_list);
		wprintw(log_window, "\n");
		wrefresh(log_window);
	#else
		vprintf(format, format_list);
		printf("\n");
	#endif
}
