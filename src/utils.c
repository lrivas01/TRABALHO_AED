#include "../include/utils.h"
#include <string.h>

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

