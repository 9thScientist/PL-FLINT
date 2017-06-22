
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 100

#include <glib.h>

char* Str( char* pal) {
	GString *str = g_string_new(NULL);

	g_string_append_printf(str, "\n");
	g_string_append_printf(str, "%s\n", pal);

	return g_string_free(str, FALSE);
}


char* Cartao( char* nome, int gn, char** gs) {
	GString *str = g_string_new(NULL);

	g_string_append_printf(str, "\n");
	g_string_append_printf(str, "O teu nome é %s.\n", nome);
	g_string_append_printf(str, "Tu gostas de:\n");
	g_string_append_printf(str, "");
	for(int i = 0; i < gn; i++) {
		char* aux = Str(gs[i]);
		g_string_append_printf(str, "%s", aux);
		free(aux);
	}
	g_string_append_printf(str, "\n");

	return g_string_free(str, FALSE);
}


char* Pessoa( int idade, int gn, char** gs, char* nome) {
	GString *str = g_string_new(NULL);

	g_string_append_printf(str, "\n");
	if ( idade < 18) {
		g_string_append_printf(str, "Não tens idade para aceder a estes conteúdos!\n");
	} else if ( idade == 18) {
		g_string_append_printf(str, "Tu tens 18 anos! :D\n");
	} else {
		char* Cartao_aux = Cartao(nome, gn, gs);
		g_string_append_printf(str, "%s", Cartao_aux);
		free(Cartao_aux);
	}

	return g_string_free(str, FALSE);
}



int main() {
    int i;
    char *gostos[5];

    gostos[0] = strdup("Chocolate");
    gostos[1] = strdup("Bolachas");
    gostos[2] = strdup("Pink Floyd");
    gostos[3] = strdup("Fernando Pessoa");
    gostos[4] = strdup("Pudim Flan");

    scanf("%d", &i);

    char *pessoa = Pessoa(i, 5, gostos, "José Saramago");

    printf("%s\n", pessoa);
    free(pessoa);

    return 0;
}
