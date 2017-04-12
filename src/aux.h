#include <glib.h>
#include <stdio.h>
#include "sds/sds.h"

extern sds func; // Dynamic String for function header
extern GHashTable* varstype; // Hashtable that matches variable -> type
extern GQueue* lines; //Lines in the template
extern sds line, att; //Dynamic Strings for current line
extern FILE* yyout;

void print_include_glib(int* ig); 
void map(char* str);
void variable(char* name, char* type);
void begin_function(char* name);
void add_param_function(gpointer name, gpointer type, gpointer user_data);
void print_line(gpointer line, gpointer user_data);
void end_function();
void end_line();
void bprint(char *line);
