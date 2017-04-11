#include "aux.h"

void variable(char* name, char* type) {
    sds aux = sdsnew(name);
    sdstrim(aux, "[% ]");

    g_hash_table_insert(varstype, g_strdup(aux), g_strdup(type));
    line = sdscat(line, "%s");
    att = sdscatprintf(att, ", %s", aux);

    sdsfree(aux);
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
    printf("\t%s\n", line);
}

void end_function() {
    g_hash_table_foreach(varstype, add_param_function, NULL);

    //Removes the last ',' appended by the add_param_function
    sdstrim(func, " ,");

    func = sdscat(func, ") {\n\tGString *str = g_string_new(NULL);\n\n");
    printf("%s", func);

    g_queue_foreach(lines, print_line, NULL);

    printf("\n\treturn g_string_free(str, FALSE);\n}\n");
}

void end_line() {
    sds aux = sdsempty();

    aux = sdscatprintf(aux, "\"%s\"%s", line, att);
    aux = sdscatprintf(sdsempty(), "g_string_append_printf(str, %s);", aux);

    g_queue_push_tail(lines, g_strdup(aux));

    sdsfree(aux);
    sdsfree(line);
    sdsfree(att);

    line = sdsempty();
    att  = sdsempty();
}


