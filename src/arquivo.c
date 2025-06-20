#include "arquivo.h"
#include "erros.h"

#include <stdlib.h>

int cria_lista_vazia(FILE *arq) {
    CABECALHO cab;
    cab.pos_cabeca = -1;
    cab.pos_topo = 0;
    cab.pos_livre = -1;

    if (fseek(arq, 0, SEEK_SET) != 0) {
        return ERRO_ARQUIVO_SEEK;
    }

    if (fwrite(&cab, sizeof(CABECALHO), 1, arq) != 1) {
        return ERRO_ARQUIVO_WRITE;
    }

    return SUCESSO;
}

CABECALHO* le_cabecalho(FILE *arq) {
    CABECALHO *cab = malloc(sizeof(CABECALHO));
    if (!cab) return NULL;
    if (fseek(arq, 0, SEEK_SET) != 0) {
        free(cab);
        return NULL;
    }

    if (fread(cab, sizeof(CABECALHO), 1, arq) != 1) {
        free(cab);
        return NULL;
    }
    return cab;
}

int escreve_cabecalho(FILE* arq,CABECALHO* cab) {
    if (fseek(arq, 0, SEEK_SET) != 0)
        return ERRO_ARQUIVO_SEEK;

    if (fwrite(cab, sizeof(CABECALHO), 1, arq) != 1)
        return ERRO_ARQUIVO_WRITE;

    return SUCESSO;
}
