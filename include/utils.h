#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#ifdef _WIN32
	#include <windows.h>
	#define SEPARADOR_DIRETORIO "\\"
	#define TAM_MAX_CAMINHO MAX_PATH
#else
	#include <limits.h>
	#define SEPARADOR_DIRETORIO "/"
	#define TAM_MAX_CAMINHO PATH_MAX
#endif // _WIN32


/*
 * caminho_termina_com_barra - verifica se o caminho termina com '/' ou '\\'
 *
 * @caminho - string contendo o caminho a ser verificado
 *
 * Pre-condicoes:
 *	- O ponteiro 'caminho' deve ser valido (nao nulo).
 *
 * Pos-condicoes:
 *	- Retorna 1 se o caminho termina com '/' (Linux) ou '\\' ou '/' (Windows).
 *	- Retorna 0 se nao termina com separador.
 *	- Retorna -1 se a string for nula ou vazia.
 */
int caminho_termina_com_barra(const char* caminho);

/*
 * normalizar_para_sep - ajusta os separadores de caminho para o formato do sistema operacional
 *
 * @caminho - string contendo o caminho a ser normalizado (modificado in-place)
 *
 * Pre-condicoes:
 *	- A string deve estar corretamente alocada e terminada com '\0'.
 *
 * Pos-condicoes:
 *	- Em sistemas Windows, todos os '/' sao convertidos para '\\'.
 *	- Em sistemas Unix/Linux, todos os '\\' sao convertidos para '/'.
 */
void normalizar_para_sep(char* caminho);

/*
 * trim - remove espacos em branco do inicio e do fim da string
 *
 * @str - ponteiro para string a ser modificada (in-place)
 *
 * Pre-condicoes:
 *	- str deve ser uma string valida, com terminador '\0'.
 *
 * Pos-condicoes:
 *	- Todos os espacos iniciais e finais serao removidos.
 *	- O conteudo da string sera deslocado se necessario.
 */
void trim(char* str);

/*
 * ler_unsigned_int_direto - le um valor inteiro sem sinal da entrada padrao de forma segura
 *
 * Nao recebe parametros.
 *
 * Pre-condicoes:
 *	- Deve haver entrada disponivel no stdin.
 *
 * Pos-condicoes:
 *	- Retorna o valor lido se for um numero valido.
 *	- Retorna 0 em caso de erro, valor negativo ou entrada invalida.
 */
unsigned int ler_unsigned_int_direto();

/*
 * construir_caminho_completo - monta um caminho completo juntando caminho base e nome do arquivo
 *
 * @caminho_base - string com o caminho base (sera modificada)
 * @nome_arquivo - nome do arquivo a ser concatenado ao caminho
 *
 * Pre-condicoes:
 *	- caminho_base deve ter espaco suficiente para conter o resultado.
 *	- nome_arquivo deve ser uma string valida.
 *
 * Pos-condicoes:
 *	- caminho_base sera normalizado e tera a barra final adicionada, se necessario.
 *	- nome_arquivo sera concatenado ao caminho_base.
 */
void construir_caminho_completo(char* caminho_base, const char* nome_arquivo);

/*
 * obter_data_atual - obtém a data atual formatada como string
 *
 * @buffer - buffer onde a data será escrita
 * @tamanho - tamanho máximo do buffer
 *
 * Pré-condições:
 *	- 'buffer' deve ser um ponteiro v�lido com espa�o suficiente.
 * Pós-condições:
 *	- Em caso de sucesso, grava a data atual no formato DD/MM/AAAA no buffer.
 *	- Retorna SUCESSO (0) se a operação for bem-sucedida.
 *	- Retorna ERRO_OBTER_DATA (-12) se não for possível obter a data.
 */
int obter_data_atual(char *buffer, size_t tamanho);

/*
 * ler_inteiro_seguro - le um valor inteiro da entrada padrao com validacao de caracteres
 *
 * @resultado - ponteiro onde o valor inteiro sera armazenado
 *
 * Pre-condicoes:
 *	- resultado deve ser um ponteiro valido.
 *
 * Pos-condicoes:
 *	- Retorna 1 se a entrada for valida e um inteiro foi armazenado.
 *	- Retorna 0 em caso de erro de leitura ou se a entrada for invalida.
 */
int ler_inteiro_seguro(int *resultado);

/*
 * ler_unsigned_int_com_zero - le um valor inteiro sem sinal da entrada padrao, aceitando 0 como valido
 *
 * @saida - ponteiro onde o valor lido sera armazenado
 *
 * Pre-condicoes:
 *	- saida deve ser um ponteiro valido.
 *
 * Pos-condicoes:
 *	- Retorna 1 se um numero valido foi lido e armazenado.
 *	- Retorna 0 se a entrada for invalida ou negativa.
 */
int ler_unsigned_int_com_zero(unsigned int *saida);

/*
 * linha_em_branco - verifica se a string contem apenas espacos em branco
 *
 * @linha - ponteiro para a string a ser verificada
 *
 * Pre-condicoes:
 *	- linha deve ser uma string valida.
 *
 * Pos-condicoes:
 *	- Retorna 1 se a string estiver vazia ou conter apenas espacos/tabs.
 *	- Retorna 0 se houver qualquer caractere nao branco.
 */
int linha_em_branco(const char* linha);
#endif // UTILS_H
