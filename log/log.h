#ifndef LOG_H
#define LOG_H

#ifdef CURSES
#include <curses.h>

extern WINDOW *log_window;
#endif

typedef enum {
	L_ERROR = 0,
	L_WARN = 1,
	L_INFO = 2,
	L_DEBUG = 3,
} loglevel_t;

extern int logging_level;
void log_message(loglevel_t, const char *, const char *, ...);

#endif
