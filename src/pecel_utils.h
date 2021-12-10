#ifndef HEADER_PECEL_UTILS_H
#define HEADER_PECEL_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "type.h"

int extract_line_size(FILE* f);
void remove_linefeed(char* line);
void remove_carriage_ret(char* line);
int extract_line_val(char* line, char* delim, char** out);
char* to_lower(char* s);
char* to_upper(char* s);
#endif