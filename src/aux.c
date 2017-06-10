#include <ctype.h>
#include <string.h>
#include "aux.h"

sds* vars_condition(sds cnd, int *length);
sds clean(sds str, char c);

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

    sdstrim(cmd, "[% ]\n");
    tokens = sdssplitlen(cmd, sdslen(cmd), " ", 1, &c);

    reg_variable(b, tokens[2], tokens[1]);

    sdsfreesplitres(tokens, c);
    sdsfree(cmd);
}

// Calls a function
// CALL <FUNC> <ARGUMENTS...>
void flint_call(char* str) {
    sds cmd = sdsnew(str), arg_tmp = sdsempty();
    sds cll_tmp, nm_tmp;
    sds *tokens;
    int i, c;

    sdstrim(cmd, "[% ]\n");
    sdsrange(cmd, 5, -1);
    tokens = sdssplitlen(cmd, sdslen(cmd), " ", 1, &c);

    // prepare arguments of function call
    for (i = 1; i < c; i++) {
        arg_tmp = sdscatprintf(arg_tmp, "%s, ", tokens[i]);
        reg_variable(b, tokens[i], "char*");
    }
    sdstrim(arg_tmp, ", ");

    // prepare name of aux variable
    nm_tmp = sdscatprintf(sdsempty(), "%s_aux", tokens[0]);

    // prepare function call
    cll_tmp = sdscatprintf(sdsempty(), "char* %s = %s(%s);", nm_tmp, tokens[0], arg_tmp);
    push_line(b, cll_tmp);

    // Add atribute and build line
    add_attribute(b, nm_tmp);
    build_strapp_line(b);

    // Frees aux variable
    sdsfree(cll_tmp);
    cll_tmp = sdscatprintf(sdsempty(), "free(%s);", nm_tmp);
    push_line(b, cll_tmp);

    sdsfreesplitres(tokens, c);
    sdsfree(cmd);
    sdsfree(arg_tmp);
    sdsfree(cll_tmp);
    sdsfree(nm_tmp);
}

// MAP <FUNC> <LENGHT> <LIST>
void flint_map(char* str) {
    int i, c;
    sds *tokens;
    sds aux = sdsnew(str), ln_aux = sdsempty();

    build_strapp_line(b);

    sdstrim(aux, "[% ]\n");
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
    sds *vars;
    int i, c;
    sdstrim(cmd, "[% ]\n");
    sdsrange(cmd, 2, -1);

    // Anotates variable in condition
    vars = vars_condition(cmd, &c);
    for (i = 0; i < c; i++)
        reg_variable(b, vars[i], "int");

    cmd = clean(cmd, ':');
    // Writes if
    aux = sdscatprintf(sdsempty(), "if (%s) {", cmd);
    push_sds_line(b, aux);

    inc_indent(b);

    sdsfree(aux);
    sdsfree(cmd);
    sdsfreesplitres(vars, c);
}

void flint_elif(char* str) {
    sds cmd = sdsnew(str), aux;
    sds *vars;
    int i, c;
    sdstrim(cmd, "[% ]\n");
    sdsrange(cmd, 4, -1);

    // Anotates variable in condition
    vars = vars_condition(cmd, &c);
    for (i = 0; i < c; i++)
        reg_variable(b, vars[i], "int");

    cmd = clean(cmd, ':');
    // Writes else if
    dec_indent(b);
    aux = sdscatprintf(sdsempty(), "} else if (%s) {", cmd);
    push_sds_line(b, aux);

    inc_indent(b);

    sdsfree(aux);
    sdsfree(cmd);
    sdsfreesplitres(vars, c);
}

// ELSE
void flint_else() {
    dec_indent(b);
    push_line(b, "} else {");
    inc_indent(b);
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
    sdstrim(aux, "[% ]\n");

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

sds clean(sds str, char c) {
    int i, l = sdslen(str);

    for (i = 0; i < l; i++)
        str[i] = (str[i] == c) ? ' ' : str[i];

    return str;
}

// returns variables in condition
sds* vars_condition(sds cnd, int *length) {
    sds aux = sdsdup(cnd);
    sds *r;
    int ig, i, l = sdslen(aux);

    ig = 0;
    for(i = 0; i < l; i++) {
        ig = (strchr("'\":", aux[i])) ? !ig : ig;
        if (ig || isupper(aux[i]) || strchr("<>=!|&()'\",-+/;:", aux[i]) || isdigit(aux[i]))
            aux[i] = ' ';
    }

    r = sdssplitargs(aux, length);
    sdsfree(aux);

    return r;
}
