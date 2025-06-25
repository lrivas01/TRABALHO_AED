#include "../include/utils.h"
#include "../include/erros.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

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


unsigned int ler_unsigned_int_direto() {
    char buffer[32];
    char *endptr;
    unsigned long valor;

    printf("Digite um número: ");

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        printf("Erro na leitura da entrada!\n");
        return 0; // retorna 0 em caso de erro
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    if (buffer[0] == '-') {
        printf("Número negativo não é válido!\n");
        return 0;
    }

    errno = 0;
    valor = strtoul(buffer, &endptr, 10);

    if (endptr == buffer || *endptr != '\0' || errno == ERANGE) {
        printf("Entrada inválida! Digite apenas números.\n");
        return 0;
    }

    if (valor > UINT_MAX) {
        printf("Número muito grande!\n");
        return 0;
    }

    return (unsigned int)valor;
}

void construir_caminho_completo(char* caminho_base, const char* nome_arquivo) {
    normalizar_para_sep(caminho_base); // ← 1. Normaliza para o separador do sistema

    // 2. Adiciona a barra final se necessário
    if (!caminho_termina_com_barra(caminho_base)) {
    #ifdef _WIN32
        strcat(caminho_base, "\\");
    #else
        strcat(caminho_base, "/");
    #endif
    }

    // 3. Concatena o nome do arquivo
    strcat(caminho_base, nome_arquivo);
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

int obter_data_atual(char *buffer, size_t tamanho) {
    time_t t = time(NULL);
    struct tm *data_local = localtime(&t);

    if (data_local) {
        strftime(buffer, tamanho, "%d/%m/%Y", data_local);
    } else {
        buffer[0] = '\0'; // string vazia em caso de erro
        return ERRO_OBTER_DATA;
    }
    return SUCESSO;
}

