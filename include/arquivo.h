#ifndef ARQUIVO_H
#define ARQUIVO_H
#include<stdio.h>

/*
 * CABECALHO - struct que armazena dados de controle da lista encadeada em arquivo
 *
 * @pos_cabeca - posição do primeiro nó da lista encadeada de registros ativos
 * @pos_topo   - próxima posição livre no final do arquivo (usada se não houver posições livres reutilizáveis)
 * @pos_livre  - posição do primeiro nó da lista de registros removidos (espaços livres reutilizáveis)
 */
typedef struct CABECALHO {
    int pos_cabeca;
    int pos_topo;
    int pos_livre;
} CABECALHO;

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
CABECALHO* le_cabecalho(FILE* arq);

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
int escreve_cabecalho(FILE* arq, CABECALHO* cab);

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
int cria_lista_vazia(FILE* arq);

int inicializar_base_de_dados(char* caminho_diretorio);

#endif //ARQUIVO_H
