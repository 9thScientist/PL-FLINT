#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include "sds/sds.h"
#include "buffer.h"

struct buffer {
    GHashTable* varstype;
    GQueue* lines;
    sds hdr, line, att;
};

void add_param_function(gpointer name, gpointer type, gpointer b);
void print_line(gpointer line, gpointer o);

BUFFER b_init() {
    BUFFER b = malloc(sizeof(struct buffer));

    b->varstype = g_hash_table_new_full(NULL, g_str_equal, g_free, g_free);
    b->lines = g_queue_new();
    b->line = sdsempty();
    b->att = sdsempty();

    return b;
}

// Registers variable
void reg_variable(BUFFER b, char* name, char* type) {
    if (!g_hash_table_contains(b->varstype, name))
        g_hash_table_insert(b->varstype, g_strdup(name), g_strdup(type));
}

// Concats string to line
void cat_line(BUFFER b, char* str) {
    b->line = sdscat(b->line, str);
}

// Adds attributes to line
void add_attribute(BUFFER b, char* str) {
    b->line = sdscat(b->line, "%s");
    b->att = sdscatprintf(b->att, ", %s", str);
}

// Start building function with given name
void begin_function(BUFFER b, char* name) {
    b->hdr = sdscatprintf(sdsempty(), "char* %s(", name);
    push_line(b, "GString *str = g_string_new(NULL);\n");
}

void add_param_function_aux(BUFFER b, char* type, char* name) {
    b->hdr = sdscatprintf(b->hdr, " %s %s,", type, name);
}

// Adds a parameter to the function header (aux)
void add_param_function(gpointer name, gpointer type, gpointer b) {
    add_param_function_aux(b, type, name);
}

// Builds function header
void build_header(BUFFER b) {
    g_hash_table_foreach(b->varstype, add_param_function, b);

    //Removes the last ',' appended by the add_param_function
    sdstrim(b->hdr, " ,");
    b->hdr = sdscat(b->hdr, ") {");
}

// Builds a string_append line
void build_strapp_line(BUFFER b, int indent) {
    sds aux, ind = sdsempty();


    for(int i = 0; i < indent; i++)
        ind = sdscat(ind, "\t");

    aux = sdscatprintf(sdsempty(), "%sg_string_append_printf(str, \"%s\"%s);", ind, b->line, b->att);
    push_sds_line(b, aux);
    sdsfree(ind);
    sdsfree(aux);
    sdsfree(b->line);
    sdsfree(b->att);

    b->line = sdsempty();
    b->att = sdsempty();
}

// Pushes new line to buffer
void push_line(BUFFER b, const char* str) {
    g_queue_push_tail(b->lines, sdsnew(str));
}

void push_sds_line(BUFFER b, sds str) {
    g_queue_push_tail(b->lines, sdsdup(str));
}

// Prints a transformed template line (aux)
void print_line(gpointer line, gpointer o) {
    fprintf(o,"\t%s\n", line);
}

// Prints function buffer to out
void print_buffer(BUFFER b, FILE* out) {
    fprintf(out, "%s\n", b->hdr);

    g_queue_foreach(b->lines, print_line, out);

    fprintf(out,"\n\treturn g_string_free(str, FALSE);\n}\n");
}

// Resets buffer
void reset(BUFFER b) {
//    g_queue_free_full(b->lines, (GDestroyNotify) sdsfree);
    g_queue_clear(b->lines);
    g_hash_table_remove_all(b->varstype);

    sdsfree(b->hdr);
    sdsfree(b->line);
    sdsfree(b->att);

    b->hdr = sdsempty();
    b->line = sdsempty();
    b->att = sdsempty();
}
