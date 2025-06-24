#ifndef ARQUIVO_H
#define ARQUIVO_H
#include<stdio.h>

/*
 * CABECALHO - struct que armazena dados de controle da lista encadeada em arquivo
 *
 * @pos_cabeca - posição do primeiro nó da lista encadeada de registros ativos
 * @pos_topo   - próxima posição livre no final do arquivo (usada se não houver posições livres reutilizáveis)
 * @pos_livre  - posição do primeiro nó da lista de registros removidos (espaços livres reutilizáveis)
 */
typedef struct CABECALHO {
    int pos_cabeca;
    int pos_topo;
    int pos_livre;
} CABECALHO;

/*
 * le_cabecalho - funcao que le o cabecalho do arquivo com as informacoes da lista
 *
 * @arq - ponteiro para arquivo binário aberto em modo leitura/escrita
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto e ser um arquivo da lista.
 *
 * Pós-condições:
 *	- Retorna um ponteiro para a estrutura CABECALHO.
 *	- Em caso de erro (falha em fseek, fread ou malloc), retorna NULL.
 */
CABECALHO* le_cabecalho(FILE* arq);

/*
 * escreve_cabecalho - Sobrescreve o cabeçalho do arquivo binário com nova estrutura
 *
 * @arq - ponteiro para arquivo binário aberto em modo leitura/escrita
 * @cab - ponteiro para estrutura CABECALHO com os dados atualizados
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto em modo leitura/escrita
 *	- O ponteiro cab deve ser válido e previamente preenchido
 *
 * Pós-condições:
 *	- O cabeçalho do arquivo é atualizado com os dados fornecidos
 *	- Retorna 0  em caso de sucesso
 *	- Retorna valor negativo caso ocorra erro
 */
int escreve_cabecalho(FILE* arq, CABECALHO* cab);

/*
 * cria_lista_vazia - Inicializa um arquivo binário com uma lista encadeada vazia
 *
 * @arq - ponteiro para arquivo binário já aberto em modo leitura/escrita
 *
 * Pré-condições:
 *	- O arquivo deve estar aberto com permissão de leitura e escrita
 *	- O ponteiro arq deve ser válido (não nulo)
 *
 * Pós-condições:
 *	- Um cabeçalho é escrito no início do arquivo, marcando a lista como vazia:
 *	- Retorno 0 caso sucesso
 *	- Retorno negativo caso erro
 */
int cria_lista_vazia(FILE* arq);

/*
 * inicializar_base_de_dados - inicializa arquivos binários de usuários, livros e empréstimos
 *
 * @caminho_diretório - diretório que os arquivos binários serão armazenados
 *
 * Essa função recebe um diretório, podendo utilizar separador '/' em ambientes Linux
 * ou '\' e '/' em ambientes Windows. Ela procura no diretório pelos arquivos binários e, 
 * se não existirem ou existirem sem cabeçalhos válidos, eles são criados. Caso existam,
 * nada é feito.
 *
 * Pré-condições:
 *	- O diretório deve existir e ser válido.
 *	- Em ambientes linux, o diretório deve estar no formato "/caminho/para/diretorio" ou "/caminho/para/diretorio/".
 *	- Em ambientes windows, além da forma linux-like, o diretório pode estar no formato "\caminho\para\diretorio" ou
 *		"\caminho\para\diretorio\".
 *	- O diretório deve possuir permissões de leitura e escrita.
 * Pós-condições:
 *	- Os arquivos binários para listas encadeadas são criados e inicializados com cabeçalho, caso não existam.
 *	- Se os arquivos existirem e estarem inicializados, a função não faz nada.
 *	- Retorna SUCESSO (0) em caso de sucesso.
 *	- Retorna valores negativos em caso de erro:
 *		- ERRO_INICIALIZAR_ARQUIVO (-22): caso algum arquivo não consiga ser inicializado.
 */
int inicializar_base_de_dados(char* caminho_diretorio);

int processar_lote(
    const char* caminho_arquivo_lote,
    const char* caminho_arquivo_emprestimo,
    const char* caminho_arquivo_livro,
    const char* caminho_arquivo_usuario
);

#endif //ARQUIVO_H
