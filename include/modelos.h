#ifndef _MODELOS_H
#define _MODELOS_H

/**
 * LIVRO - estrutura de dados de um livro
 * @codigo: número único que identifica o livro
 * @titulo: título do livro, com máximo de 150 caracteres
 * @autor: autor do livro, com máximo de 200 caracteres
 * @editora: editora do livro, com máximo de 50 caracteres
 * @edicao: edição do livro
 * @ano: ano de publicação do livro
 * @exemplares: quantidade de exemplares disponíveis do livro
 *
 * Esta estrutura define as características obrigatórias para
 * cada unidade de livro disponível.
 */
typedef struct {
	unsigned int codigo;
	char titulo[151];
	char autor[201];
	char editora[51];
	unsigned int edicao;
	unsigned int ano;
	unsigned int exemplares;
}LIVRO;

/**
 * USUARIO - estrutura de dados de um usuário
 * @codigo: número único que identifica o usuário
 * @nome: nome do usuário, com máximo de 50 caracteres
 *
 * Esta estrutura define as características obrigatórias para
 * cada usuário existente.
 */
typedef struct {
	unsigned int codigo;
	char nome[51];
}USUARIO;

/**
 * EMPRESTIMO - estrutura de dados para controle de empréstimos de livros
 * @codigo_usuario: código do usuário que efetuou empréstimo
 * @codigo_livro: código do livro emprestado
 * @data_emprestimo: data que foi feito o empréstimo, no formato DD/MM/AAAA
 * @data_devolucao: data que livro foi devolvido, no formato DD/MM/AAAA
 *
 * Esta estrutura relaciona os usuários e livros, registrando informações
 * sobre empréstimos de livros e suas devoluções.
 */
typedef struct {
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	char data_emprestimo[11];
	char data_devolucao[11];
}EMPRESTIMO;

#endif // _MODELOS_H