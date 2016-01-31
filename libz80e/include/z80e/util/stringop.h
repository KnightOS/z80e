#ifndef STRINGOP_H
#define STRINGOP_H
#include "list.h"

char *strip_whitespace(char *str, int *trimmed_start);
char *strip_comments(char *str);
list_t *split_string(const char *str, const char *delims);
void free_flat_list(list_t *list);
char *code_strchr(const char *string, char delimiter);
char *code_strstr(const char *haystack, const char *needle);
int unescape_string(char *string);

#ifdef _WIN32
int strcasecmp(const char* s1, const char* s2);
#endif

#endif
