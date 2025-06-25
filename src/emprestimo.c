#include "../include/emprestimo.h"
#include "../include/livro.h"
#include "../include/usuario.h"
#include "../include/arquivo.h"
#include "../include/erros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * escreve_no_emprestimo - função interna que escreve nó do tipo empréstimo em um arquivo de lista encadeada
 *
 * @arquivo_emprestimo - ponteiro para arquivo binário aberto em modo leitura/escrita contendo os empréstimos
 * @no_emprestimo - ponteiro para a estrutura EMPRESTIMO que será escrita no arquivo
 * @posicao - índice (posição) do nó no arquivo onde os dados serão escritos
 *
 * Pré-condições:
 *      - O arquivo deve estar aberto e posicionado para escrita.
 *	- O arquivo deve conter um cabeçalho válido para a lista encadeada.
 *	- A posição deve ser válida (não ultrapassar o limite de registros do arquivo).
 *	- O ponteiro no_emprestimo deve apontar para uma estrutura USUARIO válida e inicializada.
 * Pós-condições:
 *      - Os dados do nó são gravados na posição correta do arquivo.
 *	- Retorna SUCESSO (0) em caso de sucesso.
 *	- Retorna ERRO_ARQUIVO_SEEK (-1) em caso de erro no fseek.
 *	- Retorna ERRO_ARQUIVO_WRITE (-2) em caso de erro no fwrite.
 */
static int escreve_no_emprestimo(FILE* arquivo_emprestimo, EMPRESTIMO* no_emprestimo, int posicao) {
        if(fseek(arquivo_emprestimo, sizeof(CABECALHO) + posicao * sizeof(EMPRESTIMO), SEEK_SET) != 0)
	        return ERRO_ARQUIVO_SEEK;
        if(fwrite(no_emprestimo, sizeof(EMPRESTIMO), 1, arquivo_emprestimo) == 0)
	        return ERRO_ARQUIVO_WRITE;

        return SUCESSO;
}

/*
 * le_no_emprestimo - função interna que lê um nó do tipo usuário em um arquivo de lista encadeada
 * 
 * @arquivo_emprestimo - ponteiro para arquivo binário aberto em modo leitura contendo os empréstimos
 * @posicao - posicao do nó que será lido
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto e posicionado para leitura.
 *	- O arquivo deve conter um cabeçalho válido para lista encadeada.
 *	- A posição deve ser válida (não ultrapassar o número de registros).
 *
 * Pós-condições:
 *	- Retorna um ponteiro para a estrutura EMPRESTIMO correspondente à posição lida.
 *	- Em caso de erro (falha em fseek, fread ou malloc), retorna NULL.
 */
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

static int verificar_emprestimo_existente(const char* caminho_arquivo_emprestimo, unsigned int codigo_usuario, unsigned int codigo_livro) {
        int retorno = SUCESSO;
        FILE* arquivo = fopen(caminho_arquivo_emprestimo, "rb");
        if(!arquivo) {
                return ERRO_ABRIR_ARQUIVO;
        }

        CABECALHO* cabecalho = le_cabecalho(arquivo);
        if(!cabecalho) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_arquivo;
        }

        int pos = cabecalho->pos_cabeca;
        EMPRESTIMO emprestimo;

        while (pos != -1) {
                if(fseek(arquivo, sizeof(CABECALHO) + pos * sizeof(EMPRESTIMO), SEEK_SET) != 0) {
                        retorno = ERRO_ARQUIVO_SEEK;
                        goto liberar_cabecalho;
                }

                if(fread(&emprestimo, sizeof(EMPRESTIMO), 1, arquivo) != 1) {
                        retorno = ERRO_ARQUIVO_READ;
                        goto liberar_cabecalho;
                }

                if(emprestimo.codigo_livro == codigo_livro && emprestimo.codigo_usuario == codigo_usuario) {
                        retorno = ERRO_CONFLITO_ID;
                        goto liberar_cabecalho; // conflito encontrado
                }

                pos = emprestimo.proximo;
        }
liberar_cabecalho:
        free(cabecalho);
liberar_arquivo:
        fclose(arquivo);

        return retorno;

}

/*
 * emprestar_livro - função que registra um novo empréstimo
 *
 * @caminho_arquivo_emprestimo - caminho completo para o arquivo binário de empréstimo
 * @caminho_arquivo_livro - caminho completo para o arquivo binário de livro
 * @caminho_arquivo_usuario - caminho completo para o arquivo binário de usuário
 * @codigo_usuario - identificador do usuário que pegou livro emprestado
 * @codigo_livro - identificador do livro que foi emprestado
 * @data_emprestimo - data que o livro foi emprestado
 *
 * Pré-condições:
 *	- Caminhos para os arquivos devem ser válidos.
 *	- Arquivos de empréstimo e livro podem ser aberto em modo leitura e escrita.
 *	- Arquivo de usuário pode ser aberto em modo leitura.
 *	- Códigos de usuario e livro devem ser válidos.
 *	- Data do empréstimo deve estar no formato adequado (DD/MM/AAAA).
 * Pós-condições:
 *	- Um novo registro de empréstimo é registrado, reutilizando posições livres se existirem.
 *	- O cabeçalho do arquivo é atualizado para refletir a nova cabeça da lista encadeada e possíveis posições livres.
 *	- A quantidade de exemplares do livro é decrementada em 1.
 *	- Retorna SUCESSO (0) em caso de sucesso na operação.
 *	- Retorna valores negativos em caso de erro:
 *		- ERRO_ABRIR_ARQUIVO (-10): não foi possível abrir algum arquivo informado.
 *		- ERRO_LER_CABECALHO (-11): não foi possível ler o cabeçalho de algum arquivo informado.
 *		- ERRO_ARQUIVO_SEEK (-1): erro no posicionamento em algum arquivo (fseek).
 *		- ERRO_ARQUIVO_READ (-3): erro na leitura de algum arquivo (fread).
 *		- ERRO_ENCONTRAR_USUARIO (-16): não foi possível encontrar o usuário informado.
 *		- ERRO_ENCONTRAR_LIVRO (-15): não foi possível encontrar o livro informado.
 *		- ERRO_LIVROS_ESGOTADOS (-17): não há unidades disponíveis para empréstimo.
 *		- ERRO_ESCREVER_EMPRESTIMO (-18): não foi possível registrar o empréstimo na lista encadeada.
 *		- ERRO_LER_EMPRESTIMO (-19): não foi possível ler nó de empréstimo na lista encadeada.
 */
int emprestar_livro(
        const char* caminho_arquivo_emprestimo, 
        const char* caminho_arquivo_livro, 
        const char* caminho_arquivo_usuario, 
        const unsigned int codigo_usuario, 
        const unsigned int codigo_livro,
        const char* data_emprestimo
) {
        if(verificar_emprestimo_existente(caminho_arquivo_emprestimo, codigo_usuario, codigo_livro) == ERRO_CONFLITO_ID)
                return ERRO_CONFLITO_ID;

        int retorno = SUCESSO;

        // abrir arquivos
        FILE* arquivo_emprestimo = fopen(caminho_arquivo_emprestimo, "r+b");
        if(!arquivo_emprestimo) {
                retorno = ERRO_ABRIR_ARQUIVO;
                return retorno;
        }

        FILE* arquivo_livro = fopen(caminho_arquivo_livro, "r+b");
        if(!arquivo_livro) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_emprestimo;
        }

        FILE* arquivo_usuario = fopen(caminho_arquivo_usuario, "rb");
        if(!arquivo_usuario) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_livro;
        }

        // procurar usuario e ver se existe
        CABECALHO* cabecalho_usuario = le_cabecalho(arquivo_usuario);
        if(cabecalho_usuario == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_arquivo_usuario;
        }

        int posicao_atual_usuario = cabecalho_usuario->pos_cabeca;
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

        int posicao_atual_livro = cabecalho_livro->pos_cabeca;
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

        EMPRESTIMO emprestimo;
        emprestimo.codigo_livro = codigo_livro;
        emprestimo.codigo_usuario = codigo_usuario;
        strncpy(emprestimo.data_emprestimo, data_emprestimo, MAX_DATA);
        emprestimo.data_devolucao[0] = '\0';
        emprestimo.proximo = cabecalho_emprestimo->pos_cabeca;

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

/*
 * devolver_livro - registra devolução de livro
 *
 * @caminho_arquivo_emprestimo - caminho completo para arquivo binário de empréstimos
 * @caminho_arquivo_livro - caminho completo para arquivo binário de livros
 * @caminho_arquivo_usuario - caminho completo para arquivo binário de usuários
 * @codigo_usuario - identificador do usuário que pegou livro emprestado
 * @codigo_livro - identificador do livro que foi emprestado
 * @data_devolucao - data que livro foi devolvido
 *
 * Pré-condições:
 *	- Caminhos para os arquivos devem ser válidos.
 *	- Arquivos de empréstimo e livro podem ser abertos em modo leitura e escrita.
 *	- Códigos de usuario e livro devem ser válidos.
 *	- Data da devolução deve estar no formato adequado (DD/MM/AAAA).
 * Pós-condições:
 *	- A data de devolução é registrada no nó de empréstimo
 *	- A quantidade de exemplares do livro é incrementada em 1.
 *	- Retorna SUCESSO (0) em caso de sucesso.
 *	- Retorna valores negativos em caso de erro:
 *		- ERRO_ABRIR_ARQUIVO (-10): não foi possível abrir algum arquivo informado.
 *		- ERRO_LER_CABECALHO (-11): não foi possível ler o cabeçalho de algum arquivo informado.
 *		- ERRO_ARQUIVO_SEEK (-1): erro no posicionamento de algum arquivo (fseek).
 *		- ERRO_ARQUIVO_READ (-3): erro na leitura de algum arquivo (fread).
 *		- ERRO_ENCONTRAR_EMPRESTIMO (-20): não foi possível encontrar o empréstimo associado.
 *		- ERRO_ENCONTRAR_LIVRO (-15): não foi possível encontrar o livro informado.
 */
int devolver_livro(
        const char* caminho_arquivo_emprestimo, 
        const char* caminho_arquivo_livro, 
        const unsigned int codigo_usuario, 
        const unsigned int codigo_livro,
        const char* data_devolucao
) {
        int retorno = SUCESSO;

        // abrir arquivos
        FILE* arquivo_emprestimo = fopen(caminho_arquivo_emprestimo, "r+b");
        if(!arquivo_emprestimo) {
                retorno = ERRO_ABRIR_ARQUIVO;
                return retorno;
        }

        FILE* arquivo_livro = fopen(caminho_arquivo_livro, "r+b");
        if(!arquivo_livro) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_emprestimo;
        }
        
        // abrir cabecalhos
        CABECALHO* cabecalho_emprestimo = le_cabecalho(arquivo_emprestimo);
        if(cabecalho_emprestimo == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_arquivo_livro;
        }

        CABECALHO* cabecalho_livro = le_cabecalho(arquivo_livro);
        if(cabecalho_livro == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_emprestimo;
        }

        // procurar emprestimo utilizando id do usuario e id do livro
        int posicao_atual_emprestimo = cabecalho_emprestimo->pos_cabeca;
        int posicao_atual_livro = cabecalho_livro->pos_cabeca;

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

                posicao_atual_livro = no_livro_atual.prox;
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
liberar_arquivo_livro:
        fclose(arquivo_livro);
liberar_arquivo_emprestimo:
        fclose(arquivo_emprestimo);

        return retorno;
}

/*
 * listar_livros_emprestados - exibe na tela informações sobre empréstimos
 *
 * @caminho_arquivo_emprestimo - caminho completo para o arquivo binário de empréstimos
 * @caminho_arquivo_livro - caminho completo para o arquivo binário de livros
 * @caminho_arquivo_usuario - caminho completo para o arquivo binário de usuários
 *
 * Pré-condições:
 *	- Arquivos informados devem ser válidos.
 *	- Arquivos informados podem ser abertos em modo leitura.
 * Pós-condições:
 *	- Será exibido na tela via printf para cada empréstimo (apenas os que não tiveram livros devolvidos):
 *		- Código do usuário.
 *		- Nome do usuário.
 *		- Código do livro.
 *		- Título do livro.
 *		- Data do empréstimo.
 *	- Caso não haja nenhum empréstimo, uma mensagem informando isso será exibida.
 */
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
                return retorno;
        }

        FILE* arquivo_livro = fopen(caminho_arquivo_livro, "rb");
        if(!arquivo_livro) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_emprestimo;
        }

        FILE* arquivo_usuario = fopen(caminho_arquivo_usuario, "rb");
        if(!arquivo_usuario) {
                retorno = ERRO_ABRIR_ARQUIVO;
                goto liberar_arquivo_livro;
        }

        // obter cabecalhos dos arquivos
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

        CABECALHO* cabecalho_usuario = le_cabecalho(arquivo_usuario);
        if(cabecalho_usuario == NULL) {
                retorno = ERRO_LER_CABECALHO;
                goto liberar_cabecalho_livro;
        }

        // percorrer nós de empréstimos, usuario e livros
        int posicao_atual_emprestimo, posicao_original_emprestimo;
        posicao_atual_emprestimo = posicao_original_emprestimo = cabecalho_emprestimo->pos_cabeca;

        int posicao_atual_livro, posicao_original_livro;
        posicao_atual_livro = posicao_original_livro = cabecalho_livro->pos_cabeca;

        int posicao_atual_usuario, posicao_original_usuario;
        posicao_atual_usuario = posicao_original_usuario = cabecalho_usuario->pos_cabeca;

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


