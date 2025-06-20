#include "livro.h"
#include"arquivo.h"
#include"erros.h"

#include <stdlib.h>
#include<string.h>
#include <stdio.h>


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
    if (!livro)
        return NULL;
    if(fseek(arq, sizeof(CABECALHO)+pos*sizeof(LIVRO), SEEK_SET)!=0) {
        free(livro);
        return NULL;
    }
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
        return ERRO_ARQUIVO_SEEK;
    if (fwrite(livro, sizeof(LIVRO), 1, arq) != 1) {
        return ERRO_ARQUIVO_WRITE;
    }
    return SUCESSO;
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
    if (!arq) return ERRO_ABRIR_ARQUIVO;

    CABECALHO *cab = le_cabecalho(arq);
    if (!cab) {
        fclose(arq);
        return ERRO_LER_CABECALHO;
    }

    int nova_pos;
    if (cab->pos_livre == -1) {
        // Sem espaço livre: insere no final
        nova_pos = cab->pos_topo;
        if(fseek(arq, 0, SEEK_END)!=0) {
            free(cab);
            return ERRO_ARQUIVO_SEEK;
    } else {
        // Reaproveita espaço
        nova_pos = cab->pos_livre;
        LIVRO *livro_removido = le_no_livro(arq, nova_pos);
        if (livro_removido==NULL) {
            free(cab);
            fclose(arq);
            return ERRO_ARQUIVO_READ;
        }
        cab->pos_livre = livro_removido->prox;
        free(livro_removido);
    }

    // Inserção no início da lista encadeada
    novo.prox = cab->pos_cabeca;
    if (escreve_no_livro(arq, &novo, nova_pos) != 0) {
        free(cab);
        fclose(arq);
        return ERRO_ARQUIVO_WRITE;
    }

    cab->pos_cabeca = nova_pos;

    if (nova_pos == cab->pos_topo)
        cab->pos_topo++;

    if (escreve_cabecalho(arq, cab) != 0) {
        free(cab);
        fclose(arq);
        return ERRO_ESCREVER_CAB;
    }

    free(cab);
    fclose(arq);
    return SUCESSO;
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
    FILE *arq = fopen(nome_arq, "rb");
    if (!arq) {
        return ERRO_ABRIR_ARQUIVO	;
    }

    CABECALHO cab;
    if (fread(&cab, sizeof(CABECALHO), 1, arq) != 1) {
        fclose(arq);
        return ERRO_LER_CABECALHO	;
    }

    int pos = cab.pos_cabeca;
    LIVRO livro;
    while (pos != -1) {
        if (fseek(arq, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET) != 0) {
            fclose(arq);
            return ERRO_POSICAO_INVALIDA;
        }

        if (fread(&livro, sizeof(LIVRO), 1, arq) != 1) {
            fclose(arq);
            return ERRO_LER_LIVRO;
        }

        if (livro.codigo == codigo) {
            printf("Código: %d\nTítulo: %s\nAutor: %s\nEditora: %s\nEdicao: %d\nAno: %d\nExemplares: %d\n\n",
                   livro.codigo, livro.titulo, livro.autor, livro.editora,
                   livro.edicao, livro.ano, livro.exemplares);
            fclose(arq);
            return SUCESSO;
        }

        pos = livro.prox;
    }
    //livro nao encontrado
    fclose(arq);
    return ERRO_ENCONTRAR_LIVRO;
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
int listar_todos(const char *nome_arq) {
    FILE *arquivo = fopen(nome_arq, "rb");
    if (!arquivo) {
        return ERRO_ABRIR_ARQUIVO;
    }

    CABECALHO cab;
    if (fread(&cab, sizeof(CABECALHO), 1, arquivo) != 1) {
        fclose(arquivo);
        return ERRO_LER_CABECALHO;
    }

    int pos = cab.pos_cabeca;
    LIVRO livro;
    if (pos == -1) {
        printf("Nenhum livro cadastrado.\n");
    }

    while (pos != -1) {
        if (fseek(arquivo, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET) != 0) {
            fclose(arquivo);
            return ERRO_ARQUIVO_SEEK;
        }

        if (fread(&livro, sizeof(LIVRO), 1, arquivo) != 1) {
            fclose(arquivo);
            return ERRO_ARQUIVO_READ;
        }

        printf("Código: %d | Título: %s | Autor: %s | Ano: %d | Exemplares: %d\n",
               livro.codigo, livro.titulo, livro.autor, livro.ano, livro.exemplares);
        pos = livro.prox;
    }

    fclose(arq);
    return SUCESSO;
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
int buscar_autor(const char *nome_arq, const char *autor) {
        FILE *arq = fopen(nome_arq, "rb");
        if (!arq) {
            return ERRO_ABRIR_ARQUIVO;
        }

        CABECALHO cab;
        if (fread(&cab, sizeof(CABECALHO), 1, arq) != 1) {
            fclose(arq);
            return ERRO_LER_CABECALHO;
        }

        int pos = cab.pos_cabeca;
        LIVRO livro;
        int encontrado = 0;

        while (pos != -1) {
            if (fseek(arq, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET) != 0) {
                fclose(arq);
                return ERRO_ARQUIVO_SEEK;
            }

            if (fread(&livro, sizeof(LIVRO), 1, arq) != 1) {
                fclose(arq);
                return ERRO_ARQUIVO_READ;
            }

            if (strcmp(livro.autor, autor) == 0) {
                printf("Título: %s | Código: %d\n", livro.titulo, livro.codigo);
                encontrado = 1;
            }

            pos = livro.prox;
        }
        fclose(arq);
        return SUCESSO;
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
int buscar_titulo(const char *nome_arq, const char *titulo) {
    FILE *arq = fopen(nome_arq, "rb");
        if (!arq) {
            return ERRO_ABRIR_ARQUIVO;
        }


        CABECALHO cab;
        if (fread(&cab, sizeof(CABECALHO), 1, arq) != 1) {
            fclose(arq);
            return ERRO_LER_CABECALHO;
        }

    int pos = cab.pos_cabeca;
    LIVRO livro;

    while (pos != -1) {
        if (fseek(arq, sizeof(CABECALHO) + pos * sizeof(LIVRO), SEEK_SET) != 0) {
            fclose(arq);
            return ERRO_ARQUIVO_SEEK;
        }
        if (fread(&livro, sizeof(LIVRO), 1, arq) != 1) {
            fclose(arq);
            return ERRO_ARQUIVO_READ;
        }

        if (strcmp(livro.titulo, titulo) == 0) {
            printf("Código: %d\nTítulo: %s\nAutor: %s\nEditora: %s\nEdicao: %d\nAno: %d\nExemplares: %d\n\n",
                   livro.codigo, livro.titulo, livro.autor, livro.editora,
                   livro.edicao, livro.ano, livro.exemplares);
            fclose(arq);
            return SUCESSO;
        }

        pos = livro.prox;
    }

    printf("Livro com título \"%s\" não encontrado.\n", titulo);
    fclose(arq);
}
