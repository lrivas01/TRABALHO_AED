#include "livro.h"
//#include"utils.h
#include"arquivo.h"


#include <stdlib.h>
#include<string.h>
#include <stdio.h>
#include <stddef.h>


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
LIVRO* le_no_livro(FILE *arq, int pos) {
    LIVRO *livro = malloc(sizeof(LIVRO));
    if (!livro) return NULL;
    fseek(arq, sizeof(CABECALHO)+pos*sizeof(LIVRO), SEEK_SET);
    if (fread(livro, sizeof(LIVRO), 1, arq) != 1) {
        free(livro);
        return NULL;
    }
    return livro;
}

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
int escreve_no_livro(FILE* arq,LIVRO* livro,int pos){
    if (fseek(arq, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET) != 0)
        return -1;
    if (fwrite(livro, sizeof(LIVRO), 1, arq) != 1) {
        return -2;
    }


}

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
int cadastrar_livro(const char *nome_arquivo, LIVRO novo) {
    FILE *arq = fopen(nome_arquivo, "rb+");
    if (!arq) return -10;

    CABECALHO *cab = le_cabecalho(arq);
    if (!cab) {
        fclose(arq);
        return -11;
    }

    int nova_pos;
    if (cab->pos_livre == -1) {
        // Sem espaço livre: insere no final
        nova_pos = cab->pos_topo;
        fseek(arq, 0, SEEK_END);
    } else {
        // Reaproveita espaço
        nova_pos = cab->pos_livre;
        LIVRO *livro_removido = le_no_livro(arq, nova_pos);
        if (livro_removido==NULL) {
            free(cab);
            fclose(arq);
            return -2;
        }
        cab->pos_livre = livro_removido->prox;
        free(livro_removido);
    }

    // Inserção no início da lista encadeada
    novo.prox = cab->pos_cabeca;
    escreve_no_livro(arq, &novo, nova_pos);

    cab->pos_cabeca = nova_pos;

    if (nova_pos == cab->pos_topo)
        cab->pos_topo++;

    escreve_cabecalho(arq, cab);

    free(cab);
    fclose(arq);
    return 0;
}

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
 *
 * Retorno:
 *   - 0 em caso de sucesso
 *   - Código de erro negativo em caso de falha
 */
int remover_livro(const char *nome_arquivo, int codigo) {
    FILE *arquivo = fopen(nome_arquivo, "rb+");
    if (!arquivo)return -10;
    CABECALHO cabecalho;
    if (fread(&cabecalho, sizeof(CABECALHO), 1, arquivo) != 1) {
        fclose(arquivo);
        return -11;
    }

    int pos_atual = cabecalho.pos_cabeca;
    int pos_anterior = -1;
    LIVRO livro;

    while (pos_atual != -1) {
        fseek(arquivo, sizeof(CABECALHO) + pos_atual * sizeof(LIVRO), SEEK_SET);
        if (fread(&livro, sizeof(LIVRO), 1, arquivo) != 1) {
            fclose(arquivo);
            return -3;
        }

        if (livro.codigo == codigo) {
            if (pos_anterior == -1) {
                // Primeiro elemento é o que será removido
                cabecalho.pos_cabeca = livro.prox;
            } else {
                // Atualiza o anterior para pular o atual
                fseek(arquivo, sizeof(CABECALHO) + pos_anterior * sizeof(LIVRO) + offsetof(LIVRO, prox), SEEK_SET);
                fwrite(&livro.prox, sizeof(int), 1, arquivo);
            }

            // Encadeia o elemento removido na lista de livres
            livro.prox = cabecalho.pos_livre;
            fseek(arquivo, sizeof(CABECALHO) + pos_atual * sizeof(LIVRO), SEEK_SET);
            fwrite(&livro, sizeof(LIVRO), 1, arquivo);

            cabecalho.pos_livre = pos_atual;

            fseek(arquivo, 0, SEEK_SET);
            fwrite(&cabecalho, sizeof(CABECALHO), 1, arquivo);

            fclose(arquivo);
            printf("Livro com código %d removido com sucesso.\n", codigo);
            return 0;
        }

        pos_anterior = pos_atual;
        pos_atual = livro.prox;
    }

    fclose(arquivo);
    return -1;
}

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
int imprimir_livro(const char *nome_arq, int codigo) {
    FILE *arquivo = fopen(nome_arq, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return -10;
    }

    CABECALHO cab;
    if (fread(&cab, sizeof(CABECALHO), 1, arquivo) != 1) {
        fclose(arquivo);
        return -11;
    }

    int pos = cab.pos_cabeca;
    LIVRO livro;
    while (pos != -1) {
        fseek(arquivo, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET);
        if (fread(&livro, sizeof(LIVRO), 1, arquivo) != 1) {
            break;
        };

        if (livro.codigo == codigo) {
            printf("Código: %d\nTítulo: %s\nAutor: %s\nEditora: %s\nEdicao: %d\nAno: %d\nExemplares: %d\n\n",
                   livro.codigo, livro.titulo, livro.autor, livro.editora,
                   livro.edicao, livro.ano, livro.exemplares);
            fclose(arquivo);
            return 0;
        }

        pos = livro.prox;
    }

    printf("Livro com código %d não encontrado.\n", codigo);
    fclose(arquivo);
    return -1;
}

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
void listar_Todos(const char *nome_arq) {
    FILE *arquivo = fopen(nome_arq, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    CABECALHO cab;
    fread(&cab, sizeof(CABECALHO), 1, arquivo);

    int pos = cab.pos_cabeca;
    LIVRO livro;
    if (pos == -1) {
        printf("Nenhum livro cadastrado.\n");
    }

    while (pos != -1) {
        fseek(arquivo, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET);
        fread(&livro, sizeof(LIVRO), 1, arquivo);

        printf("Código: %d | Título: %s | Autor: %s | Ano: %d | Exemplares: %d\n",
               livro.codigo, livro.titulo, livro.autor, livro.ano, livro.exemplares);

        pos = livro.prox;
    }

    fclose(arquivo);
}

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
void buscar_Autor(const char *nome_arq, const char *autor) {
    FILE *arquivo = fopen(nome_arq, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    CABECALHO cab;
    fread(&cab, sizeof(CABECALHO), 1, arquivo);

    int pos = cab.pos_cabeca;
    LIVRO livro;
    int encontrado = 0;

    while (pos != -1) {
        fseek(arquivo, pos, SEEK_SET);
        fread(&livro, sizeof(LIVRO), 1, arquivo);

        if (strcmp(livro.autor, autor) == 0) {
            printf("Título: %s | Código: %d\n", livro.titulo, livro.codigo);
            encontrado = 1;
        }

        pos = livro.prox;
    }

    if (!encontrado) {
        printf("Nenhum livro do autor \"%s\" encontrado.\n", autor);
    }

    fclose(arquivo);
}

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
void buscar_Titulo(const char *nome_arq, const char *titulo) {
    FILE *arquivo = fopen(nome_arq, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    CABECALHO cab;
    fread(&cab, sizeof(CABECALHO), 1, arquivo);

    int pos = cab.pos_cabeca;
    LIVRO livro;

    while (pos != -1) {
        fseek(arquivo, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET);

        fread(&livro, sizeof(LIVRO), 1, arquivo);

        if (strcmp(livro.titulo, titulo) == 0) {
            printf("Código: %d\nTítulo: %s\nAutor: %s\nEditora: %s\nEdicao: %d\nAno: %d\nExemplares: %d\n\n",
                   livro.codigo, livro.titulo, livro.autor, livro.editora,
                   livro.edicao, livro.ano, livro.exemplares);
            fclose(arquivo);
            return;
        }

        pos = livro.prox;
    }

    printf("Livro com título \"%s\" não encontrado.\n", titulo);
    fclose(arquivo);
}
