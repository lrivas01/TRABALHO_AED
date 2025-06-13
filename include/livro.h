#ifndef LIVRO_H
#define LIVRO_H

#include <stdio.h>
#define MAX_TITULO 151
#define MAX_AUTOR 201
#define MAX_EDITORA 51

/*
 * LIVRO - struct que armazena informações do livro
 *
 * @codigo - identificador único do livro
 * @titulo - nome do livro
 * @autor - nome do autor
 * @editora - nome da editora
 * @edicao - numero da edicao
 * @ano - ano de lancamento
 * @exemplares -  quantidaded e exemplares
 * @prox - identificador para o proximo livro na lista encadeada
 */
typedef struct {
    int codigo;
    char titulo[MAX_TITULO];
    char autor[MAX_AUTOR];
    char editora[MAX_EDITORA];
    int edicao;
    int ano;
    int exemplares;
    int prox;
} LIVRO;




/*
 * le_no_livro - le um no em uma determinada posicao do arquivo
 *
 * @arq - ponteiro para arquivo binário aberto em modo leitura/escrita
 * @pos - posição lógica do livro na lista (índice relativo ao início dos registros)
 *
* Pré-condições:
 *   - O arquivo deve estar aberto corretamente para leitura ou leitura/escrita
 *   - A posição deve ser válida (não negativa e dentro dos limites do arquivo)
 *
 * Pós-condições:
 *   - Um ponteiro para a estrutura LIVRO preenchida com os dados da posição indicada é retornado
 *   - Em caso de erro (malloc, fseek ou fread), retorna NULL
 */
LIVRO* le_no_livro(FILE* arq, int pos);



/*
 * escreve_no_livro - Escreve um nó do tipo LIVRO em uma posição lógica do arquivo binário
 *
 * @arq   - ponteiro para arquivo binário aberto em modo leitura/escrita
 * @livro - ponteiro para estrutura LIVRO contendo os dados a serem gravados
 * @pos   - posição lógica (índice) onde os dados devem ser gravados
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto em modo leitura/escrita
 *   - A posição deve ser válida
 *   - O ponteiro livro deve apontar para uma estrutura válida e inicializada
 *
 * Pós-condições:
 *   - Os dados da estrutura LIVRO são gravados na posição especificada no arquivo
 *   - Retorna 0 em caso de sucesso
 *   - Retorna código de erro (< 0) em caso de falha (por exemplo: erro de fseek ou fwrite)
 */

int escreve_no_livro(FILE* arq,LIVRO* livro,int pos);


/*
 * cadastrar_livro - Insere um novo livro na lista encadeada mantida em arquivo binário
 *
 * @nome_arq - nome do arquivo binário contendo a lista
 * @livro    - estrutura LIVRO preenchida com os dados do novo livro
 *
 * Pré-condições:
 *   - O arquivo deve existir e estar corretamente inicializado com um cabeçalho válido
 *   - O arquivo deve estar aberto para leitura e escrita
 *
 * Pós-condições:
 *   - O livro é adicionado no início da lista
 *   - O campo pos_cabeca do cabeçalho é atualizado
 *   - Se houver espaço livre disponível, ele é reutilizado
 *   - Se não houver, o livro é inserido na posição final (pos_topo é incrementado)
 *   - retorno 0 em caso de sucesso
 *   - retorna Código de erro negativo em caso de falhas (ex: erro ao abrir, ler, ou escrever)
 */
int cadastrar_livro(const char *nome_arq, LIVRO livro);

/*
 * remover_livro - Remove um livro da lista lógica e o encadeia na lista de posições livres
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 * @codigo   - código do livro a ser removido
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto para leitura e escrita
 *   - O cabeçalho deve estar corretamente configurado
 *
 * Pós-condições:
 *   - O livro é removido da lista lógica (encadeamento é ajustado)
 *   - A posição do livro é adicionada à lista de espaços livres
 *   - Retorna 0 em caso de sucesso
 *   - retorna Código de erro negativo em caso de falha
 */
int remover_livro(const char *nome_arq, int codigo);

/*
 * imprimir_livro - Imprime os dados de um livro com base no código fornecido
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 * @codigo   - código do livro a ser impresso
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto para leitura
 *
 * Pós-condições:
 *   - Os dados do livro com o código fornecido são impressos na tela, se encontrado
 *   - Retorna 0 em caso de sucesso
 *   - Retorna Código de erro negativo se não encontrado ou ocorrer erro de leitura
 */
int imprimir_livro(const char *nome_arq, int codigo);

/*
 * listar_Todos - Lista todos os livros cadastrados na lista encadeada do arquivo
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto para leitura
 *
 * Pós-condições:
 *   - Os dados de todos os livros (em ordem lógica) são impressos na tela
 */
void listar_Todos(const char *nome_arq);

/*
* buscar_Autor - Lista todos os livros escritos por um autor específico
*
* @nome_arq - nome do arquivo binário contendo os livros
* @autor    - nome do autor a ser buscado
*
* Pré-condições:
*   - O arquivo deve estar aberto para leitura
*
* Pós-condições:
*   - Títulos dos livros do autor são impressos na tela
*
*
 */
void buscar_Autor(const char *nome_arq, const char *autor);

/*
 * buscar_Titulo - Busca e imprime os dados de um livro com base no título
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 * @titulo   - título do livro a ser buscado
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto para leitura
 *
 * Pós-condições:
 *   - Dados do livro encontrado são exibidos na tela
 *
 */
void buscar_Titulo(const char *nome_arq, const char *titulo);

#endif