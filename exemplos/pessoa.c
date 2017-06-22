
#include <stdio.h>
#include <stdlib.h>
#define SIZE 100

#include <glib.h>

char* Morada( char* rua, char* codigo_postal, char* cidade) {
	GString *str = g_string_new(NULL);

	g_string_append_printf(str, "\n");
	g_string_append_printf(str, "\t%s\n", rua);
	g_string_append_printf(str, "\t%s\n", codigo_postal);
	g_string_append_printf(str, "\t%s\n", cidade);

	return g_string_free(str, FALSE);
}


char* Pessoa( char* nome, int idade, char* rua, char* codigo_postal, char* cidade) {
	GString *str = g_string_new(NULL);

	g_string_append_printf(str, "Nome: %s\n", nome);
	g_string_append_printf(str, "Idade: %d\n", idade);
	char* Morada_aux = Morada(rua, codigo_postal, cidade);
	g_string_append_printf(str, "Morada: %s", Morada_aux);
	free(Morada_aux);

	return g_string_free(str, FALSE);
}



int main() {

    char nome[SIZE] = "Teófilo de Braga";
    char rua[SIZE] = "Rua da República Nº 1";
    char codigo[SIZE] = "1700-320";
    char cidade[SIZE] = "Lisboa";
    int idade = 174;
    char *postal;

    postal = Pessoa(nome, idade, rua, codigo, cidade);

    printf("%s\n", postal);
    free(postal);

    return 0;
}
