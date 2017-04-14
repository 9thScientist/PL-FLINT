#include "aux.h"

void print_include_glib(int* ig) {
    if (*ig)
        fprintf(yyout,"#include <glib.h>\n\n");

    *ig = 0;

}

// Declares a new variable
// VAR <TYPE> <NAME>
void flint_var(char* str) {
    sds cmd = sdsnew(str);
    sds *tokens;
    int c;

    sdstrim(cmd, "[% ]");
    tokens = sdssplitlen(cmd, sdslen(cmd), " ", 1, &c);

    reg_variable(b, tokens[2], tokens[1]);

    sdsfreesplitres(tokens, c);
    sdsfree(cmd);
}

// Calls a function
// CALL <FUNC> <ARGUMENTS...>
void flint_call(char* str) {
    sds cmd = sdsnew(str), arg_tmp = sdsempty(), cll_tmp;
    sds *tokens;
    int i, c;

    sdstrim(cmd, "[% ]");
    sdsrange(cmd, 5, -1);
    tokens = sdssplitlen(cmd, sdslen(cmd), " ", 1, &c);

    // prepare arguments of function call
    for (i = 1; i < c; i++)
        arg_tmp = sdscatprintf(arg_tmp, "%s, ", tokens[i]);
    sdstrim(arg_tmp, ", ");

    // prepare function call
    cll_tmp = sdscatprintf(sdsempty(), "%s(%s)", tokens[0], arg_tmp);

    // Add atribute and build line
    add_attribute(b, cll_tmp);
    build_strapp_line(b);

    sdsfreesplitres(tokens, c);
    sdsfree(cmd);
    sdsfree(arg_tmp);
    sdsfree(cll_tmp);
}

// MAP <FUNC> <LENGHT> <LIST>
void flint_map(char* str) {
    int i, c;
    sds *tokens;
    sds aux = sdsnew(str), ln_aux = sdsempty();

    build_strapp_line(b);

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
    inc_indent(b);
    build_strapp_line(b);
    dec_indent(b);

    // Frees aux
    push_line(b, "\tfree(aux);");

    // Ends for
    push_line(b, "}");

    sdsfreesplitres(tokens, c);
    sdsfree(aux);
}

// IF <C>
void flint_if(char* str) {
    sds cmd = sdsnew(str), aux;
    sdstrim(cmd, "[% ]");
    sdsrange(cmd, 2, -1);

    aux = sdscatprintf(sdsempty(), "if (%s) {", cmd);
    push_sds_line(b, aux);

    inc_indent(b);

    sdsfree(aux);
    sdsfree(cmd);
}

// ENDIF
void flint_endif() {
    dec_indent(b);
    push_line(b, "}");
}

// BREAK
void flint_break() {
   push_line(b, "return g_string_free(str, FALSE);");
}

// Anotates a inline variable and adds it to the current line
void flint_invar(char* name) {
    sds aux = sdsnew(name);
    sdstrim(aux, "[% ]");

    reg_variable(b, aux, "char*");
    add_attribute(b, aux);

    sdsfree(aux);
}


void new_function(char* name) {
    sds aux = sdsnew(name);
    sdstrim(aux, "={");

    begin_function(b, aux);
}

void end_template_line() {
    cat_line(b, "\\n");
    build_strapp_line(b);
}

void end_function() {
    cat_line(b, "\\n");

    build_header(b);
    print_buffer(b, yyout);

    reset(b);
}
