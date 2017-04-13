#include "aux.h"

void print_include_glib(int* ig) {
    if (*ig)
        fprintf(yyout,"#include <glib.h>\n");

    *ig = 0;

}
// Anotates a inline variable and adds it to the current line
void variable(char* name) {
    sds aux = sdsnew(name);
    sdstrim(aux, "[% ]");

    reg_variable(b, aux, "char*");
    add_attribute(b, aux);

    sdsfree(aux);
}

// MAP <FUNC> <LENGHT> <LIST>
void map(char* str) {
    int i, c;
    sds *tokens;
    sds aux = sdsnew(str), ln_aux = sdsempty();

    build_strapp_line(b, 0);

    sdstrim(aux, "[% ]");
    tokens = sdssplitlen(aux, sdslen(aux), " ", 1, &c);

    // registers found variables
    reg_variable(b, tokens[2], "int");
    reg_variable(b, tokens[3], "char**");

    // Adds for to buffer
    ln_aux = sdscatprintf(ln_aux, "for(int i = 0; i < %s; i++) {", tokens[2]);
    push_sds_line(b, ln_aux);
    sdsfree(ln_aux);

    // Declare aux string
    ln_aux = sdscatprintf(sdsempty(), "\tchar* aux = %s(%s[i]);", tokens[1], tokens[3]);
    push_sds_line(b, ln_aux);
    sdsfree(ln_aux);

    // Appends aux to return string
    add_attribute(b, "aux");
    build_strapp_line(b, 1);

    // Frees aux
    push_line(b, "\tfree(aux);");

    // Ends for
    push_line(b, "}");

    sdsfreesplitres(tokens, c);
    sdsfree(aux);
}

void new_function(char* name) {
    sds aux = sdsnew(name);
    sdstrim(aux, "={");

    begin_function(b, aux);
}

void end_template_line() {
    cat_line(b, "\\n");
    build_strapp_line(b, 0);
}

void end_function() {
    end_template_line();

    build_header(b);
    print_buffer(b, yyout);

    reset(b);
}
