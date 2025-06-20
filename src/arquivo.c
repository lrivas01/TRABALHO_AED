#include"arquivo.h"
#include"erros.h"
#include<stdlib.h>

/*
 * cria_lista_vazia - Inicializa um arquivo binário com uma lista encadeada vazia
 *
 * @arq - ponteiro para arquivo binário já aberto em modo leitura/escrita
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto com permissão de leitura e escrita
 *   - O ponteiro arq deve ser válido (não nulo)
 *
 * Pós-condições:
 *   - Um cabeçalho é escrito no início do arquivo, marcando a lista como vazia:
 *   - Retorno 0 caso sucesso
 *   - Retorno negativo caso erro
 */
int cria_lista_vazia(FILE *arq) {
    CABECALHO cab;
    cab->pos_cabeca = -1;
    cab->pos_topo = 0;
    cab->pos_livre = -1;
    if (fseek(arq, 0, SEEK_SET) != 0) {
        return ERRO_ARQUIVO_SEEK;
    }

    if (fwrite(&cab, sizeof(CABECALHO), 1, arq) != 1) {
        return ERRO_ARQUIVO_WRITE;
    }
    return SUCESSO;
}

/*
 * le_cabecalho - funcao que le o cabecalho do arquivo com as informacoes da lista
 *
    * @arq - ponteiro para arquivo binário aberto em modo leitura/escrita
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto e ser um arquivo da lista.
 *
 * Pós-condições:
 *	- Retorna um ponteiro para a estrutura CABECALHO.
 *	- Em caso de erro (falha em fseek, fread ou malloc), retorna NULL.
 */
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

/*
 * escreve_cabecalho - Sobrescreve o cabeçalho do arquivo binário com nova estrutura
 *
 * @arq - ponteiro para arquivo binário aberto em modo leitura/escrita
 * @cab - ponteiro para estrutura CABECALHO com os dados atualizados
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto em modo leitura/escrita
 *   - O ponteiro cab deve ser válido e previamente preenchido
 *
 * Pós-condições:
 *   - O cabeçalho do arquivo é atualizado com os dados fornecidos
 *   - Retorna 0  em caso de sucesso
 *   - Retorna valor negativo caso ocorra erro
 */
int escreve_cabecalho(FILE* arq,CABECALHO* cab) {
    if (fseek(arq, 0, SEEK_SET) != 0)
        return ERRO_ARQUIVO_SEEK;

    if (fwrite(cab, sizeof(CABECALHO), 1, arq) != 1)
        return ERRO_ARQUIVO_WRITE;

    return SUCESSO;
}







