#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sds/sds.h"
#include "buffer.h"

struct buffer {
    GQueue *var_name, *var_type;
    GQueue* lines;
    sds hdr, line, att;
    int indent;
};

void add_param_function(gpointer name, gpointer type, gpointer b);
int contains(GQueue* q, char* s);
void print_line(gpointer line, gpointer o);
void get_attribute_type(BUFFER b, char *name, char *type);

BUFFER b_init() {
    BUFFER b = malloc(sizeof(struct buffer));

    b->var_name = g_queue_new();
    b->var_type = g_queue_new();
    b->lines = g_queue_new();
    b->line = sdsempty();
    b->att = sdsempty();
    b->indent = 0;

    return b;
}

int get_indent(BUFFER b) {
    return b->indent;
}

void inc_indent(BUFFER b) {
    b->indent++;
}

void dec_indent(BUFFER b) {
    b->indent--;
}

// Registers variable
void reg_variable(BUFFER b, char* name, char* type) {
    if (contains(b->var_name, name) == -1) {
        g_queue_push_tail(b->var_name, g_strdup(name));
        g_queue_push_tail(b->var_type, g_strdup(type));
    }
}

// Concats string to line
void cat_line(BUFFER b, char* str) {
    b->line = sdscat(b->line, str);
}

// Adds attributes to line
void add_attribute(BUFFER b, char* str) {
    char type[2]; // could be either "%s", "%d" or "%f"
    get_attribute_type(b, str, type);
    b->line = sdscat(b->line, type);
    b->att = sdscatprintf(b->att, ", %s", str);
}

// Start building function with given name
void begin_function(BUFFER b, char* name) {
    b->hdr = sdscatprintf(sdsempty(), "char* %s(", name);
    push_line(b, "GString *str = g_string_new(NULL);\n");
}

// Builds function header
void build_header(BUFFER b) {
    char *type, *name;

    while (!g_queue_is_empty(b->var_name)) {
        type = g_queue_pop_head(b->var_type);
        name = g_queue_pop_head(b->var_name);
        b->hdr = sdscatprintf(b->hdr, " %s %s,", type, name);
        free(type);
        free(name);
    }

    //Removes the last ',' appended by the add_param_function
    sdstrim(b->hdr, " ,");
    b->hdr = sdscat(b->hdr, ") {");
}

// Builds a string_append line
void build_strapp_line(BUFFER b) {
    sds aux;


    aux = sdscatprintf(sdsempty(), "g_string_append_printf(str, \"%s\"%s);", b->line, b->att);
    push_sds_line(b, aux);
    sdsfree(aux);
    sdsfree(b->line);
    sdsfree(b->att);

    b->line = sdsempty();
    b->att = sdsempty();
}

// Pushes new line to buffer
void push_line(BUFFER b, const char* str) {
    sds aux = sdsempty();

    for (int i = 0; i < b->indent; i++)
        aux = sdscat(aux, "\t");

    g_queue_push_tail(b->lines, sdscat(aux, str));
}

void push_sds_line(BUFFER b, sds str) {
//    g_queue_push_tail(b->lines, sdsdup(str));
    push_line(b, str);
}

// Prints a transformed template line (aux)
void print_line(gpointer line, gpointer o) {
       fprintf(o, "\t%s\n", line);
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
    g_queue_clear(b->var_name);
    g_queue_clear(b->var_type);

    sdsfree(b->hdr);
    sdsfree(b->line);
    sdsfree(b->att);

    b->hdr = sdsempty();
    b->line = sdsempty();
    b->att = sdsempty();

    b->indent = 0;
}

int contains(GQueue* q, char* s) {
    int i, r = -1, l;
    char *t;

    l = g_queue_get_length(q);

    for (i = 0; r == -1 && i < l; i++) {
        t = g_queue_peek_nth(q, i);
        r = (!strcmp(t, s)) ? i : -1;
    }

    return r;
}

/* returns attribute type.
 * returns %s if var type is char* (default)
 * returns %d if var type is int
 * returns %f if var type is float
 */
void get_attribute_type(BUFFER b, char *name, char *type) {
    int p;
    char *t;

    p = contains(b->var_name, name);
    t = g_queue_peek_nth(b->var_type, p);

    if (!strcmp(t, "int"))
        strcpy(type, "%d");
    else if (!strcmp(t, "float"))
        strcpy(type, "%f");
    else
        strcpy(type, "%s");
}
