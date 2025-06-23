#include "../include/arquivo.h"
#include "../include/erros.h"
#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_ARQUIVO_EMPRESTIMO "emprestimo.dat"
#define NOME_ARQUIVO_LIVRO      "livro.dat"
#define NOME_ARQUIVO_USUARIO    "usuario.dat"

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

int inicializar_arquivo(const char* caminho) {
        FILE* arquivo = fopen(caminho, "a+b");  // cria se não existir
        if (arquivo == NULL) 
                return ERRO_ABRIR_ARQUIVO;

        // voltar para o início do arquivo
        if(fseek(arquivo, 0, SEEK_SET) != 0) {
                fclose(arquivo);
                return ERRO_ARQUIVO_SEEK;
        }

        CABECALHO cabecalho;
        if (fread(&cabecalho, sizeof(CABECALHO), 1, arquivo) != 1) {
                // arquivo novo ou corrompido, criar novo cabeçalho
                int retorno = cria_lista_vazia(arquivo);
                if(retorno < 0) {
                        fclose(arquivo);
                        return ERRO_CRIAR_LISTA;
                }
        }

        fclose(arquivo);
        return SUCESSO;
}

int inicializar_base_de_dados(char *caminho_diretorio) {
        char caminho_base[TAM_MAX_CAMINHO];
        strncpy(caminho_base, caminho_diretorio, TAM_MAX_CAMINHO - 1);
        caminho_base[TAM_MAX_CAMINHO - 1] = '\0';

        // adicionar barra se necessário
        if(!caminho_termina_com_barra(caminho_base)) {
                strncat(caminho_base, SEPARADOR_DIRETORIO, TAM_MAX_CAMINHO - strlen(caminho_base) - 1);
        }

        normalizar_para_sep(caminho_base);

        char caminho_completo_emprestimo[TAM_MAX_CAMINHO];
        snprintf(caminho_completo_emprestimo, TAM_MAX_CAMINHO, "%s%s", caminho_base, NOME_ARQUIVO_EMPRESTIMO);

        char caminho_completo_livro[TAM_MAX_CAMINHO];
        snprintf(caminho_completo_livro, TAM_MAX_CAMINHO, "%s%s", caminho_base, NOME_ARQUIVO_LIVRO);

        char caminho_completo_usuario[TAM_MAX_CAMINHO];
        snprintf(caminho_completo_usuario, TAM_MAX_CAMINHO, "%s%s", caminho_base, NOME_ARQUIVO_USUARIO);

        if(
                (inicializar_arquivo(caminho_completo_emprestimo) != 0) ||
                (inicializar_arquivo(caminho_completo_livro) != 0) ||
                (inicializar_arquivo(caminho_completo_usuario) != 0)
        ) {
                return ERRO_INICIALIZAR_ARQUIVO;
        }

        return SUCESSO;
}
