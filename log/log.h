#ifndef LOG_H
#define LOG_H

#ifdef CURSES
#include <curses.h>

extern WINDOW *log_window;
#endif

typedef enum {
	L_DEBUG,
	L_INFO,
	L_WARN,
	L_ERROR,
} loglevel_t;

#define log_message(level, part, ...) \
	_log_message(level, part, __FILE__, __LINE__, __func__, __VA_ARGS__)

void _log_message(loglevel_t, const char *, const char *, int, const char *, const char *, ...);

#endif