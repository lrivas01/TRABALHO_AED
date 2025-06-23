#include "../include/emprestimo.h"
#include "../include/livro.h"
#include "../include/usuario.h"
#include "../include/arquivo.h"
#include "../include/erros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int escreve_no_emprestimo(FILE* arquivo_emprestimo, EMPRESTIMO* no_emprestimo, int posicao) {
        if(fseek(arquivo_emprestimo, sizeof(CABECALHO) + posicao * sizeof(EMPRESTIMO), SEEK_SET) != 0)
	        return ERRO_ARQUIVO_SEEK;
        if(fwrite(no_emprestimo, sizeof(EMPRESTIMO), 1, arquivo_emprestimo) == 0)
	        return ERRO_ARQUIVO_WRITE;

        return SUCESSO;
}

static EMPRESTIMO* le_no_emprestimo(FILE* arquivo_emprestimo, int posicao) {
        EMPRESTIMO* no_emprestimo = malloc(sizeof(EMPRESTIMO));
        if(no_emprestimo == NULL)
                return NULL;

        if(
    	        fseek(arquivo_emprestimo, sizeof(CABECALHO) + posicao * sizeof(EMPRESTIMO), SEEK_SET) != 0 ||
                fread(no_emprestimo, sizeof(EMPRESTIMO), 1, arquivo_emprestimo) != 1
        ) {
    	        free(no_emprestimo);
    	        return NULL;
        }

        return no_emprestimo;
}

int emprestar_livro(
        const char *caminho_arquivo_emprestimo, 
        const char *caminho_arquivo_livro, 
        const char *caminho_arquivo_usuario, 
        const unsigned int codigo_usuario, 
        const unsigned int codigo_livro,
        const char* data_emprestimo
) {
        int retorno = SUCESSO;
        EMPRESTIMO emprestimo;
        emprestimo.codigo_livro = codigo_livro;
        emprestimo.codigo_usuario = codigo_usuario;
        strncpy(emprestimo.data_emprestimo, data_emprestimo, MAX_DATA);
        emprestimo.data_devolucao[0] = '\0';

        // abrir arquivos
        FILE* arquivo_emprestimo = fopen(caminho_arquivo_emprestimo, "r+b");
        if(!arquivo_emprestimo) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_emprestimo;
        }

        FILE* arquivo_livro = fopen(caminho_arquivo_livro, "r+b");
        if(!arquivo_livro) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_livro;
        }

        FILE* arquivo_usuario = fopen(caminho_arquivo_usuario, "rb");
        if(!arquivo_usuario) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_usuario;
        }

        // procurar usuario e ver se existe
        CABECALHO* cabecalho_usuario = le_cabecalho(arquivo_usuario);
        if(cabecalho_usuario == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_arquivo_usuario;
        }

        int posicao_atual_usuario = cabecalho_usuario->pos_topo;
        USUARIO usuario;
        int usuario_existe = 0;
        while (posicao_atual_usuario != -1) {
                if(fseek(arquivo_usuario, sizeof(CABECALHO) + posicao_atual_usuario * sizeof(USUARIO), SEEK_SET) != 0) {
                        retorno = ERRO_ARQUIVO_SEEK;
                        goto liberar_cabecalho_usuario;
                }

                if(fread(&usuario, sizeof(USUARIO), 1, arquivo_usuario) != 1) {
                        retorno = ERRO_ARQUIVO_READ;
                        goto liberar_cabecalho_usuario;
                }

                if(usuario.codigo == codigo_usuario) {
                        usuario_existe = 1;
                        break;
                }

                posicao_atual_usuario = usuario.proximo;
        }

        if(!usuario_existe) {
                retorno = ERRO_ENCONTRAR_USUARIO;
                goto liberar_cabecalho_usuario;
        }

        // procurar livro e ver se existe
        CABECALHO* cabecalho_livro = le_cabecalho(arquivo_livro);
        if(cabecalho_livro == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_usuario;
        }

        int posicao_atual_livro = cabecalho_livro->pos_topo;
        LIVRO livro;
        int livro_existe = 0;
        while(posicao_atual_livro != -1) {
                if(fseek(arquivo_livro, sizeof(CABECALHO) + posicao_atual_livro * sizeof(LIVRO), SEEK_SET) != 0) {
                        retorno = ERRO_ARQUIVO_SEEK;
                        goto liberar_cabecalho_livro;
                }

                if(fread(&livro, sizeof(LIVRO), 1, arquivo_livro) != 1) {
                        retorno = ERRO_ARQUIVO_READ;
                        goto liberar_cabecalho_livro;
                }

                if(livro.codigo == codigo_livro) {
                        livro_existe = 1;
                        break;
                }

                posicao_atual_livro = livro.prox;
        }
        if(!livro_existe) {
                retorno = ERRO_ENCONTRAR_LIVRO;
                goto liberar_cabecalho_livro;
        }

        // verificar se há unidades de livro disponíveis
        if(livro.exemplares < 1) {
                retorno = ERRO_LIVROS_ESGOTADOS;
                goto liberar_cabecalho_livro;
        }

        // registrar emprestimo
        EMPRESTIMO* auxiliar = NULL;

        CABECALHO* cabecalho_emprestimo = le_cabecalho(arquivo_emprestimo);
        if(cabecalho_emprestimo == NULL){
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_livro;
        }

        if(cabecalho_emprestimo->pos_livre == -1) {
                if(escreve_no_emprestimo(arquivo_emprestimo, &emprestimo, cabecalho_emprestimo->pos_topo) != 0) {
                        retorno = ERRO_ESCREVER_EMPRESTIMO;
                        goto liberar_cabecalho_emprestimo;
                }
                cabecalho_emprestimo->pos_cabeca = cabecalho_emprestimo->pos_topo;
                cabecalho_emprestimo->pos_topo++;
        }
        else {
	        auxiliar = le_no_emprestimo(arquivo_emprestimo, cabecalho_emprestimo->pos_livre);
	        if(auxiliar == NULL){
	                retorno = ERRO_LER_EMPRESTIMO;
	                goto liberar_cabecalho_emprestimo;
	        }
	        if(escreve_no_emprestimo(arquivo_emprestimo, &emprestimo, cabecalho_emprestimo->pos_livre) != 0) {
	                retorno = ERRO_ESCREVER_EMPRESTIMO;
	                goto liberar_auxiliar;
	        }
	        cabecalho_emprestimo->pos_cabeca = cabecalho_emprestimo->pos_livre;
	        cabecalho_emprestimo->pos_livre = auxiliar->proximo;
	        free(auxiliar);
        }
        if(escreve_cabecalho(arquivo_emprestimo, cabecalho_emprestimo) != 0) {
                retorno = ERRO_ESCREVER_CABECALHO;
                goto liberar_cabecalho_emprestimo;
        }

        // decrementar quantidade do livro
        livro.exemplares--;
        if(fseek(arquivo_livro, sizeof(CABECALHO) + posicao_atual_livro * sizeof(LIVRO), SEEK_SET) != 0) {
                retorno = ERRO_ARQUIVO_SEEK;
                goto liberar_auxiliar;
        }

        if(fwrite(&livro, sizeof(LIVRO), 1, arquivo_livro) != 1) {
                retorno = ERRO_ARQUIVO_WRITE;
                goto liberar_auxiliar;
        }

        // liberar recursos alocados
liberar_auxiliar:
        if(auxiliar != NULL) free(auxiliar);
liberar_cabecalho_emprestimo:
        free(cabecalho_emprestimo);
liberar_cabecalho_livro:
        free(cabecalho_livro);
liberar_cabecalho_usuario:
        free(cabecalho_usuario);
liberar_arquivo_usuario:
        fclose(arquivo_usuario);
liberar_arquivo_livro:
        fclose(arquivo_livro);
liberar_arquivo_emprestimo:
        fclose(arquivo_emprestimo);

        return retorno;
}
