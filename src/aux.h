#include <glib.h>
#include <stdio.h>
#include "sds/sds.h"
#include "buffer.h"

extern BUFFER b;
extern FILE* yyout;

void print_include_glib(int* ig);
void variable(char* name);
void map(char* str);
void new_function(char* name);
void end_function();
void end_template_line();
