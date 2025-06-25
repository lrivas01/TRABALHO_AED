#include "../include/usuario.h"
#include "../include/arquivo.h"
#include "../include/erros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * le_no_usuario - função interna que lê um nó do tipo usuário em um arquivo de lista encadeada
 * 
 * @arquivo - ponteiro para arquivo binário aberto em modo leitura/escrita contendo os usuários
 * @posicao - posicao do nó que será lido
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto e posicionado para leitura.
 *	- O arquivo deve conter um cabeçalho válido para lista encadeada.
 *	- A posição deve ser válida (não ultrapassar o número de registros).
 *
 * Pós-condições:
 *	- Retorna um ponteiro para a estrutura USUARIO correspondente à posição lida.
 *	- Em caso de erro (falha em fseek, fread ou malloc), retorna NULL.
 */
static USUARIO* le_no_usuario(FILE* arquivo, int posicao) {
	USUARIO* no_usuario = malloc(sizeof(USUARIO));
	if(no_usuario == NULL)
		return NULL;

	if(
		fseek(arquivo, sizeof(CABECALHO) + posicao * sizeof(USUARIO), SEEK_SET) != 0 ||
		fread(no_usuario, sizeof(USUARIO), 1, arquivo) != 1
	) {
		free(no_usuario);
		return NULL;
	}
	return no_usuario;
}

/*
 * escreve_no_usuario - função interna que escreve um nó do tipo usuário em um arquivo de lista encadeada
 *
 * @arquivo - ponteiro para arquivo binário aberto em modo leitura/escrita contendo os usuários
 * @no_usuario - ponteiro para a estrutura USUARIO que será escrita no arquivo
 * @posicao - índice (posição) do nó no arquivo onde os dados serão escritos
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto e posicionado para escrita.
 *	- O arquivo deve conter um cabeçalho válido para a lista encadeada.
 *	- A posição deve ser válida (não ultrapassar o limite de registros do arquivo).
 *	- O ponteiro no_usuario deve apontar para uma estrutura USUARIO válida e inicializada.
 *
 * Pós-condições:
 *	- Os dados do nó são gravados na posição correta do arquivo.
 *	- Retorna SUCESSO (0) em caso de sucesso.
 *	- Retorna ERRO_ARQUIVO_SEEK (-1) em caso de erro no fseek.
 *	- Retorna ERRO_ARQUIVO_WRITE (-2) em caso de erro no fwrite.
 */
static int escreve_no_usuario(FILE* arquivo, USUARIO* no_usuario, int posicao) {
	if(fseek(arquivo,sizeof(CABECALHO) + posicao * sizeof(USUARIO), SEEK_SET) != 0)
		return ERRO_ARQUIVO_SEEK;
	if(fwrite(no_usuario, sizeof(USUARIO), 1, arquivo) == 0)
		return ERRO_ARQUIVO_WRITE;

	return SUCESSO;
}

/*
 * verificar_id_usuario - verifica se código do usuário já foi registrado
 *
 * @caminho_arquivo_usuario - caminho completo para o arquivo binário que armazena usuários
 *
 * Pré-condições:
 *	- Caminho para o arquivo deve ser válido e pode ser aberto para leitura.
 *	- Arquivo deve ser inicializado (conter cabeçalho).
 * Pós-condições:
 *	- Retorna SUCESSO se não for encontrado conflito.
 *	- Retorna valores negativos em caso de erro:
 *		- ERRO_CONFLITO_ID: foi identificado conflito.
 *		- ERRO_ABRIR_ARQUIVO: não foi possível abrir o arquivo.
 *		- ERRO_LER_CABECALHO: não foi possível ler o cabeçalho do arquivo.
 *		- ERRO_ARQUIVO_SEEK: erro no posicionamento do arquivo (fseek).
 *		- ERRO_ARQUIVO_READ: erro na leitura do arquivo (fread).
 */
static int verificar_id_usuario(const char* caminho_arquivo_usuario, unsigned int codigo_usuario) {
	int retorno = SUCESSO;
	FILE* arquivo = fopen(caminho_arquivo_usuario, "rb");
	if (!arquivo) {
		return ERRO_ABRIR_ARQUIVO;
	}

	CABECALHO* cabecalho = le_cabecalho(arquivo);
	if(!cabecalho) {
		retorno = ERRO_LER_CABECALHO;
		goto liberar_arquivo;
	}

	int pos = cabecalho->pos_cabeca;
	USUARIO usuario;

	while (pos != -1) {
		if(fseek(arquivo, sizeof(CABECALHO) + pos * sizeof(USUARIO), SEEK_SET) != 0) {
			retorno = ERRO_ARQUIVO_SEEK;
			goto liberar_cabecalho;
		}

		if(fread(&usuario, sizeof(USUARIO), 1, arquivo) != 1) {
			retorno = ERRO_ARQUIVO_READ;
			goto liberar_cabecalho;
		}

		if(usuario.codigo == codigo_usuario) {
			retorno = ERRO_CONFLITO_ID;
			goto liberar_cabecalho; // conflito encontrado
		}

		pos = usuario.proximo;
	}
liberar_cabecalho:
	free(cabecalho);
liberar_arquivo:
	fclose(arquivo);

	return retorno;
}

/*
 * cadastrar_usuario - cadastra um novo usuário no arquivo de usuários em lista encadeada
 *
 * @nome_arquivo - caminho para o arquivo binário onde os usuários são armazenados
 * @usuario - estrutura USUARIO contendo informações do usuário que será registrado
 *
 * Pré-condições:
 *	- O arquivo especificado por nome_arquivo deve existir e ser acessível em modo leitura e escrita.
 *	- O arquivo deve conter um cabeçalho válido para gerenciar a lista encadeada de usuários.
 *
 * Pós-condições:
 *	- Um novo registro de usuário é inserido no arquivo, reutilizando posições livres se existirem.
 *	- O cabeçalho do arquivo é atualizado para refletir a nova cabeça da lista encadeada e possíveis posições livres.
 *	- Retorna SUCESSO (0) em caso de sucesso.
 *	- Retorna valores negativos em caso de erro:
 *		- ERRO_CONFLITO_ID: O código do usuário já foi utilizado
 *		- ERRO_ABRIR_ARQUIVO (-10): falha ao abrir o arquivo
 *		- ERRO_LER_CABECALHO (-11): falha ao ler o cabeçalho do arquivo
 *		- ERRO_LER_USUARIO (-13): falha ao ler o nó do usuário no arquivo
 *		- ERRO_ESCREVER_USUARIO (-14): escrever o nó de usuário no arquivo
 *		- ERRO_ESCREVER_CABECALHO (-12): falha ao escrever o cabeçalho atualizado no arquivo
 */
int cadastrar_usuario(const char *nome_arquivo, USUARIO usuario) {
	if(verificar_id_usuario(nome_arquivo, usuario.codigo) == ERRO_CONFLITO_ID)
		return ERRO_CONFLITO_ID;

	int retorno = SUCESSO;
	USUARIO* auxiliar = NULL;

	FILE* arquivo = fopen(nome_arquivo, "r+b");
	if(arquivo == NULL){
		retorno = ERRO_ABRIR_ARQUIVO;
		goto liberar_arquivo;
	}
	
	CABECALHO* cabecalho = le_cabecalho(arquivo);
	if(cabecalho == NULL){
		retorno = ERRO_LER_CABECALHO;
		goto liberar_cabecalho;
	}
	
	usuario.proximo = cabecalho->pos_cabeca;

	if(cabecalho->pos_livre == -1) {
		if(escreve_no_usuario(arquivo, &usuario, cabecalho->pos_topo) != 0) {
			retorno = ERRO_ESCREVER_USUARIO;
			goto liberar_cabecalho;
		}
		cabecalho->pos_cabeca = cabecalho->pos_topo;
		cabecalho->pos_topo++;
	}
	else {
		auxiliar = le_no_usuario(arquivo, cabecalho->pos_livre);
		if(auxiliar == NULL){
			retorno = ERRO_LER_USUARIO;
			goto liberar_cabecalho;
		}
		if(escreve_no_usuario(arquivo, &usuario, cabecalho->pos_livre) != 0) {
			retorno = ERRO_ESCREVER_USUARIO;
			goto liberar_auxiliar;
		}
		cabecalho->pos_cabeca = cabecalho->pos_livre;
		cabecalho->pos_livre = auxiliar->proximo;
		free(auxiliar);
	}

	if(escreve_cabecalho(arquivo, cabecalho) != 0) {
		retorno = ERRO_ESCREVER_CABECALHO;
		goto liberar_cabecalho;
	}

liberar_auxiliar:
	if(auxiliar != NULL) free(auxiliar);
liberar_cabecalho:
	free(cabecalho);
liberar_arquivo:
	fclose(arquivo);

	return retorno;
}
