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
        const char* caminho_arquivo_emprestimo, 
        const char* caminho_arquivo_livro, 
        const char* caminho_arquivo_usuario, 
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

int devolver_livro(
        const char* caminho_arquivo_emprestimo, 
        const char* caminho_arquivo_livro, 
        const char* caminho_arquivo_usuario, 
        const unsigned int codigo_usuario, 
        const unsigned int codigo_livro,
        const char* data_devolucao
) {
        int retorno = SUCESSO;

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

        // abrir cabecalhos
        CABECALHO* cabecalho_emprestimo = le_cabecalho(arquivo_emprestimo);
        if(cabecalho_emprestimo == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_arquivo_usuario;
        }

        CABECALHO* cabecalho_livro = le_cabecalho(arquivo_livro);
        if(cabecalho_livro == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_emprestimo;
        }

        // procurar emprestimo utilizando id do usuario e id do livro
        int posicao_atual_emprestimo = cabecalho_emprestimo->pos_topo;
        int posicao_atual_livro = cabecalho_livro->pos_topo;

        EMPRESTIMO no_emprestimo_atual;
        LIVRO no_livro_atual;

        while(posicao_atual_emprestimo != -1) {
                if(fseek(arquivo_emprestimo, sizeof(CABECALHO) + posicao_atual_emprestimo * sizeof(EMPRESTIMO), SEEK_SET) != 0) {
                        retorno = ERRO_ARQUIVO_SEEK;
                        goto liberar_cabecalho_livro;
                }

                if(fread(&no_emprestimo_atual, sizeof(EMPRESTIMO), 1, arquivo_emprestimo) != 1) {
                        retorno = ERRO_ARQUIVO_READ;
                        goto liberar_cabecalho_livro;
                }

                if(no_emprestimo_atual.codigo_livro == codigo_livro && no_emprestimo_atual.codigo_usuario == codigo_usuario)
                        break;

                posicao_atual_emprestimo = no_emprestimo_atual.proximo;
        }

        if(posicao_atual_emprestimo == -1) {
                retorno = ERRO_ENCONTRAR_EMPRESTIMO;
                goto liberar_cabecalho_livro;
        }

        // procurar livro
        while(posicao_atual_livro != -1) {
                if(fseek(arquivo_livro, sizeof(CABECALHO) + posicao_atual_livro * sizeof(LIVRO), SEEK_SET) != 0) {
                        retorno = ERRO_ARQUIVO_SEEK;
                        goto liberar_cabecalho_livro;
                }

                if(fread(&no_livro_atual, sizeof(LIVRO), 1, arquivo_livro) != 1) {
                        retorno = ERRO_ARQUIVO_READ;
                        goto liberar_cabecalho_livro;
                }

                if(no_livro_atual.codigo == codigo_livro)
                        break;
        }
        if(posicao_atual_livro == -1) {
                retorno = ERRO_ENCONTRAR_LIVRO;
                goto liberar_cabecalho_livro;
        }
        // registrar devolução
        strncpy(no_emprestimo_atual.data_devolucao, data_devolucao, MAX_DATA);

        // incrementar quantidade do livro
        no_livro_atual.exemplares++;

        // registrar no arquivo binário
        if(fseek(arquivo_emprestimo, sizeof(CABECALHO) + posicao_atual_emprestimo * sizeof(EMPRESTIMO), SEEK_SET) != 0) {
                retorno = ERRO_ARQUIVO_SEEK;
                goto liberar_cabecalho_livro;
        }
        if(fwrite(&no_emprestimo_atual, sizeof(EMPRESTIMO), 1, arquivo_emprestimo) != 1) {
                retorno = ERRO_ARQUIVO_WRITE;
                goto liberar_cabecalho_livro;
        }

        if(fseek(arquivo_livro, sizeof(CABECALHO) + posicao_atual_livro * sizeof(LIVRO), SEEK_SET) != 0) {
                retorno = ERRO_ARQUIVO_SEEK;
                goto liberar_cabecalho_livro;
        }
        if(fwrite(&no_livro_atual, sizeof(LIVRO), 1, arquivo_livro) != 1) {
                retorno = ERRO_ARQUIVO_WRITE;
                goto liberar_cabecalho_livro;
        }

liberar_cabecalho_livro:
        free(cabecalho_livro);
liberar_cabecalho_emprestimo:
        free(cabecalho_emprestimo);
liberar_arquivo_usuario:
        fclose(arquivo_usuario);
liberar_arquivo_livro:
        fclose(arquivo_livro);
liberar_arquivo_emprestimo:
        fclose(arquivo_emprestimo);

        return retorno;
}

int listar_livros_emprestados(
        const char* caminho_arquivo_emprestimo, 
        const char* caminho_arquivo_livro, 
        const char* caminho_arquivo_usuario
) {
        int retorno = SUCESSO;
        // exibir código de usuário, nome de usuário, código de livro, título de livro, data de emprestimo (somente os nn devolvidos)

        // abrir arquivos
        FILE* arquivo_emprestimo = fopen(caminho_arquivo_emprestimo, "rb");
        if(!arquivo_emprestimo) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_emprestimo;
        }

        FILE* arquivo_livro = fopen(caminho_arquivo_livro, "rb");
        if(!arquivo_livro) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_livro;
        }

        FILE* arquivo_usuario = fopen(caminho_arquivo_usuario, "rb");
        if(!arquivo_usuario) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_usuario;
        }

        // obter cabecalhos dos arquivos
        CABECALHO* cabecalho_emprestimo = le_cabecalho(arquivo_emprestimo);
        if(cabecalho_emprestimo == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_arquivo_usuario;
        }

        CABECALHO* cabecalho_livro = le_cabecalho(arquivo_livro);
        if(arquivo_livro == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_emprestimo;
        }

        CABECALHO* cabecalho_usuario = le_cabecalho(arquivo_usuario);
        if(cabecalho_usuario == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_livro;
        }

        // percorrer nós de empréstimos, usuario e livros
        int posicao_atual_emprestimo, posicao_original_emprestimo;
        posicao_atual_emprestimo = posicao_original_emprestimo = cabecalho_emprestimo->pos_topo;

        int posicao_atual_livro, posicao_original_livro;
        posicao_atual_livro = posicao_original_livro = cabecalho_livro->pos_topo;

        int posicao_atual_usuario, posicao_original_usuario;
        posicao_atual_usuario = posicao_original_usuario = cabecalho_usuario->pos_topo;

        EMPRESTIMO no_emprestimo_atual;
        LIVRO no_livro_atual;
        USUARIO no_usuario_atual;

        printf("Emprestimos efetuados (nao devolvidos):\n\n");
        int existe_emprestimo = 0;
        while(posicao_atual_emprestimo != -1) {
                if(fseek(arquivo_emprestimo, sizeof(CABECALHO) + posicao_atual_emprestimo * sizeof(EMPRESTIMO), SEEK_SET) != 0) {
                        retorno = ERRO_ARQUIVO_SEEK;
                        goto liberar_cabecalho_usuario;
                }

                if(fread(&no_emprestimo_atual, sizeof(EMPRESTIMO), 1, arquivo_emprestimo) != 1) {
                        retorno = ERRO_ARQUIVO_READ;
                        goto liberar_cabecalho_usuario;
                }

                // verificar se emprestimo foi devolvido (não é necessário exibir)
                if(no_emprestimo_atual.data_devolucao[0] != '\0') {
                        posicao_atual_emprestimo = no_emprestimo_atual.proximo;
                        continue;
                }
                else {
                        existe_emprestimo = 1;
                }

                // do contrário, necessário exibir. procurar informações sobre o usuário e o livro
                while(posicao_atual_livro != -1) {
                        if(fseek(arquivo_livro, sizeof(CABECALHO) + posicao_atual_livro * sizeof(LIVRO), SEEK_SET) != 0) {
                                retorno = ERRO_ARQUIVO_SEEK;
                                goto liberar_cabecalho_usuario;
                        }

                        if(fread(&no_livro_atual, sizeof(LIVRO), 1, arquivo_livro) != 1) {
                                retorno = ERRO_ARQUIVO_READ;
                                goto liberar_cabecalho_usuario;
                        }

                        if(no_livro_atual.codigo == no_emprestimo_atual.codigo_livro)
                                break;

                        posicao_atual_livro = no_livro_atual.prox;
                }

                while(posicao_atual_usuario != -1) {
                        if(fseek(arquivo_usuario, sizeof(CABECALHO) + posicao_atual_usuario * sizeof(USUARIO), SEEK_SET) != 0) {
                                retorno = ERRO_ARQUIVO_SEEK;
                                goto liberar_cabecalho_usuario;
                        }

                        if(fread(&no_usuario_atual, sizeof(USUARIO), 1, arquivo_usuario) != 1) {
                                retorno = ERRO_ARQUIVO_READ;
                                goto liberar_cabecalho_usuario;
                        }

                        if(no_usuario_atual.codigo == no_emprestimo_atual.codigo_usuario)
                                break;

                        posicao_atual_usuario = no_usuario_atual.proximo;
                }

                // livro e usuario relativos ao empréstimo encontrados. agora imprimir:
                printf("Codigo de usuario: %d\n", no_usuario_atual.codigo);
                printf("Nome do usuario: %s\n", no_usuario_atual.nome);
                printf("Codigo de livro: %d\n", no_livro_atual.codigo);
                printf("Titulo do livro: %s\n", no_livro_atual.titulo);
                printf("Data de empréstimo: %s\n\n", no_emprestimo_atual.data_emprestimo);

                // ir para o próximo nó do empréstimo
                posicao_atual_emprestimo = no_emprestimo_atual.proximo;

                // resetar posições dos livros e usuarios
                posicao_atual_livro = posicao_original_livro;
                posicao_atual_usuario = posicao_original_usuario;
        }

        if(!existe_emprestimo)
                printf("Nenhum emprestimo encontrado.\n");

liberar_cabecalho_usuario:
        free(cabecalho_usuario);
liberar_cabecalho_livro:
        free(cabecalho_livro);
liberar_cabecalho_emprestimo:
        free(cabecalho_emprestimo);
liberar_arquivo_usuario:
        fclose(arquivo_usuario);
liberar_arquivo_livro:
        fclose(arquivo_livro);
liberar_arquivo_emprestimo:
        fclose(arquivo_emprestimo);

        return retorno;
}
