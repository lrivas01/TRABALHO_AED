#ifndef EMPRESTIMO_H
#define EMPRESTIMO_H

#define MAX_DATA 10

typedef struct {
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	char data_emprestimo[MAX_DATA + 1];
	char data_devolucao[MAX_DATA + 1];
	int proximo;
} EMPRESTIMO;

int emprestar_livro(
	const char* caminho_arquivo_emprestimo,
	const char* caminho_arquivo_livro, 
	const char* caminho_arquivo_usuario, 
	const unsigned int codigo_usuario, 
	const unsigned int codigo_livro,
	const char* data_emprestimo
);
int devolver_livro(
	const char* caminho_arquivo_emprestimo,
	const char* caminho_arquivo_livro, 
	const char* caminho_arquivo_usuario, 
	unsigned int codigo_usuario, 
	unsigned int codigo_livro
);
int listar_livros_emprestados(
	const char* caminho_arquivo_emprestimo,
	const char* caminho_arquivo_livro,
	const char* caminho_arquivo_usuario
);

#endif
