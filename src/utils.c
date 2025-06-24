#include "../include/utils.h"
#include <string.h>
#include <ctype.h>
int caminho_termina_com_barra(const char *caminho) {
        if(caminho == NULL || caminho[0] == '\0')
                return -1;

        int tamanho_caminho = strlen(caminho);
        char ultimo_caractere = caminho[tamanho_caminho - 1];

#ifdef _WIN32
        return (ultimo_caractere == '\\' || ultimo_caractere == '/');
#else
        return (ultimo_caractere == '/');
#endif
}

void normalizar_para_sep(char* caminho) {
#ifdef _WIN32
    for (int i = 0; caminho[i] != '\0'; i++) {
        if (caminho[i] == '/') {
            caminho[i] = '\\';
        }
    }
#else
    for (int i = 0; caminho[i] != '\0'; i++) {
        if (caminho[i] == '\\') {
            caminho[i] = '/';
        }
    }
#endif
}


void trim(char *str) {
    if (!str) return;

    // Remover espaços do início
    char *inicio = str;
    while (isspace((unsigned char)*inicio)) inicio++;

    // Se a string for só espaços
    if (*inicio == '\0') {
        str[0] = '\0';
        return;
    }

    // Remover espaços do final
    char *fim = inicio + strlen(inicio) - 1;
    while (fim > inicio && isspace((unsigned char)*fim)) fim--;

    // Novo final de string
    *(fim + 1) = '\0';

    // Copiar resultado para o início da string
    if (str != inicio)
        memmove(str, inicio, fim - inicio + 2);  // +2 pra incluir '\0'
}

