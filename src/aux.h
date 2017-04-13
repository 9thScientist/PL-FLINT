#include <glib.h>
#include <stdio.h>
#include "sds/sds.h"
#include "buffer.h"

extern BUFFER b;
extern FILE* yyout;

void print_include_glib(int* ig);
void flint_var(char* name);
void flint_map(char* str);
void flint_if(char* str);
void flint_invar(char* name);
void flint_endif();
void new_function(char* name);
void end_function();
void end_template_line();
