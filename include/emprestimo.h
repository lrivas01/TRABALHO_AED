#ifndef EMPRESTIMO_H
#define EMPRESTIMO_H

#define MAX_DATA 10

/*
 * EMPRESTIMO - struct que armazena informações do nó de empréstimo
 *
 * @codigo_usuario - identificador único do usuário
 * @codigo_livro - identificador único do livro
 * @data_emprestimo - data que livro foi emprestado (no formato DD/MM/AAAA)
 * @data_devolucao - data que livro foi devolvido (no formato DD/MM/AAAA)
 * @proximo - inteiro que indica posicao do próximo nó de empréstimo
 *
 * A estrutura armazena informações para o empréstimo de um livro para um 
 * usuário. Todos os campos são obrigatórios, exceto 'data_devolucao'. Ele pode
 * assumir valor nulo, através da atribuição do caractere '\0' ao índice 0.
 */
typedef struct {
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	char data_emprestimo[MAX_DATA + 1];
	char data_devolucao[MAX_DATA + 1];
	int proximo;
} EMPRESTIMO;

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
);

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
);

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
);
#endif
