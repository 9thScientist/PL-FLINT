#include "aux.h"

void print_include_glib(int* ig) {
    if (*ig)
        fprintf(yyout,"#include <glib.h>\n");

    *ig = 0;
}

// Registers variable
void reg_variable(char* name, char* type) {
    g_hash_table_insert(varstype, g_strdup(name), g_strdup(type));
}

void variable(char* name, char* type) {
    sds aux = sdsnew(name);
    sdstrim(aux, "[% ]");

    g_hash_table_insert(varstype, g_strdup(aux), g_strdup(type));
    line = sdscat(line, "%s");
    att = sdscatprintf(att, ", %s", aux);

    sdsfree(aux);
}

// MAP <FUNC> <LENGHT> <LIST>
void map(char* str) {
    int i, c;
    sds *tokens;
    sds aux = sdsnew(str), att_aux = sdsempty(), for_aux = sdsempty();
    sdstrim(aux, "[% ]");

    tokens = sdssplitlen(aux, sdslen(aux), " ", 1, &c);

    reg_variable(tokens[2], "int");
    reg_variable(tokens[3], "char**");
    att_aux = sdscatprintf(att_aux, "%s(%s[i])", tokens[1], tokens[3]);

    for_aux = sdscatprintf(for_aux, "for(int i = 0; i < %s; i++) {", tokens[2]);
    bprint(for_aux);

    line = sdscat(line, "%s");
    att = sdscatprintf(att, ", %s", att_aux);

    end_line();
    bprint("}");

    sdsfreesplitres(tokens, c);
    sdsfree(aux);
    sdsfree(att_aux);
    sdsfree(for_aux);
}

void begin_function(char* name) {
    sds aux = sdsnew(name);
    sdstrim(aux, "={");

    sdsfree(func);
    func = sdsempty();
    func = sdscatprintf(func, "char* %s(", aux);

    line = sdsempty();
    att  = sdsempty();

    sdsfree(aux);
}

// Adds a parameter to the function header
void add_param_function(gpointer name, gpointer type, gpointer user_data) {
    func = sdscatprintf(func, " %s %s,", type, name);
}

// Prints a transformed template line
void print_line(gpointer line, gpointer user_data) {
    fprintf(yyout,"\t%s\n", line);
}

void end_function() {
    g_hash_table_foreach(varstype, add_param_function, NULL);

    //Removes the last ',' appended by the add_param_function
    sdstrim(func, " ,");

    func = sdscat(func, ") {\n\tGString *str = g_string_new(NULL);\n\n");
    fprintf(yyout,"%s", func);

    g_queue_foreach(lines, print_line, NULL);

    fprintf(yyout,"\n\treturn g_string_free(str, FALSE);\n}\n");

    g_queue_free_full(lines, g_free);
    g_hash_table_remove_all(varstype);
}

void end_line() {
    sds aux = sdsempty();

    aux = sdscatprintf(aux, "\"%s\"%s", line, att);
    aux = sdscatprintf(sdsempty(), "g_string_append_printf(str, %s);", aux);

    bprint(aux);

    sdsfree(aux);
    sdsfree(line);
    sdsfree(att);

    line = sdsempty();
    att  = sdsempty();
}

// Prints line to buffer
void bprint(char *line) {
    g_queue_push_tail(lines, g_strdup(line));
}
