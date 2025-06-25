#ifndef _USUARIO_H
#define _USUARIO_H

#include <stdio.h>
#include "erros.h"

#define MAX_NOME 50

/*
 * usuario - struct que armazena informações do usuário
 *
 * @codigo - identificador único do usuário
 * @nome - nome do usuário
 * @proximo - identificador para o próximo usuário na lista encadeada
 */
typedef struct usuario {
	unsigned int codigo;
	char nome[MAX_NOME + 1];
	int proximo;
} USUARIO;

/**
 * cadastrar_usuario - cadastra um novo usuário no arquivo de usuários em lista encadeada
 *
 * @nome_arquivo - caminho para o arquivo binário onde os usuários são armazenados
 * @codigo - identificador único do novo usuário
 * @nome - nome do usuário a ser cadastrado (string terminada em '\0')
 *
 * Pré-condições:
 *	- O arquivo especificado por nome_arquivo deve existir e ser acessível em modo leitura e escrita.
 *	- O arquivo deve conter um cabeçalho válido para gerenciar a lista encadeada de usuários.
 *	- O nome do usuário deve ser uma string válida com no máximo MAX_NOME caracteres.
 *
 * Pós-condições:
 *	- Um novo registro de usuário é inserido no arquivo, reutilizando posições livres se existirem.
 *	- O cabeçalho do arquivo é atualizado para refletir a nova cabeça da lista encadeada e possíveis posições livres.
 *	- Retorna SUCESSO (0) em caso de sucesso.
 *	- Retorna valores negativos em caso de erro:
 *		- ERRO_ABRIR_ARQUIVO (-10): falha ao abrir o arquivo
 *		- ERRO_LER_CABECALHO (-11): falha ao ler o cabeçalho do arquivo
 *		- ERRO_LER_USUARIO (-13): falha ao ler o nó do usuário no arquivo
 *		- ERRO_ESCREVER_USUARIO (-14): escrever o nó de usuário no arquivo
 *		- ERRO_ESCREVER_CABECALHO (-12): falha ao escrever o cabeçalho atualizado no arquivo
 */
int cadastrar_usuario(const char *nome_arquivo, USUARIO usuario);

#endif // _USUARIO_H
