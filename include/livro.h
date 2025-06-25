#ifndef LIVRO_H
#define LIVRO_H

#include <stdio.h>

#define MAX_TITULO 150
#define MAX_AUTOR 200
#define MAX_EDITORA 50

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
    char titulo[MAX_TITULO+1];
    char autor[MAX_AUTOR+1];
    char editora[MAX_EDITORA+1];
    int edicao;
    int ano;
    int exemplares;
    int prox;
} LIVRO;

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
 *   - retorno SUCESSO (0) em caso de sucesso
 *   - retorna código de erro negativo em caso de falhas (ex: erro ao abrir, ler, ou escrever)
 */
int cadastrar_livro(const char *nome_arq, LIVRO livro);

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
 *   - Retorna SUCESSO (0) em caso de sucesso
 *   - Retorna código de erro negativo se não encontrado ou ocorrer erro de leitura
 */
int imprimir_livro(const char *nome_arq, int codigo);

/*
 * listar_todos_livros - Lista todos os livros cadastrados na lista encadeada do arquivo
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 *
 * Pré-condições:
 *   - O arquivo pode ser aberto para leitura
 *
 * Pós-condições:
 *   - Os dados de todos os livros (em ordem lógica) são impressos na tela
 *   - Retorna SUCESSO (0) em caso de sucesso
 *   - Retorna valor negativo em caso de erro
 */
int listar_todos_livros(const char *nome_arq);

/*
 * buscar_autor_livro - Lista todos os livros escritos por um autor específico
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 * @autor    - nome do autor a ser buscado
 *
 * Pré-condições:
 *   - O arquivo pode ser aberto para leitura
 *
 * Pós-condições:
 *   - Títulos dos livros do autor são impressos na tela
 *   - Retorna SUCESSO (0) em caso de sucesso
 *   - Retorna código negativo em caso de erro
 */
int buscar_autor_livro(const char *nome_arq, const char *autor);

/*
 * buscar_titulo_livro - Busca e imprime os dados de um livro com base no título
 *
 * @nome_arq - nome do arquivo binário contendo os livros
 * @titulo   - título do livro a ser buscado
 *
 * Pré-condições:
 *   - O arquivo deve estar aberto para leitura
 *
 * Pós-condições:
 *   - Dados do livro encontrado são exibidos na tela
 *   - Retorna SUCESSO (0) em caso de sucesso
 *   - Retorna código de erro negativo se não encontrado ou ocorrer erro de leitura
 */
int buscar_titulo_livro(const char *nome_arq, const char *titulo);

/*
* calcular_total_livros - retorna a quantia total de livros
* @nome_arq - nome do arquivo binário contendo os livros
*
* Pré-condições:
*   - O arquivo deve estar aberto para leitura
*
* Pós-condições:
*   - Quantia de livros disponiveis é exibida
*   - Retorna SUCESSO (0) em caso de sucesso
*   - Retorna código de erro negativo se não encontrado ou ocorrer erro de leitura
*
*/
int calcular_total_livros(const char *nome_arq);
#endif
