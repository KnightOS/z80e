#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>

char *read_line(FILE *file);
char *read_line_s(const char *input, int *offset);

#endif
