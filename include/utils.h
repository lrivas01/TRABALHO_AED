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

int caminho_termina_com_barra(const char* caminho);
void normalizar_para_sep(char* caminho);
void trim(char* str);
unsigned int ler_unsigned_int_direto();
void construir_caminho_completo(char* caminho_base, const char* nome_arquivo);
int obter_data_atual(char *buffer, size_t tamanho);
int ler_inteiro_seguro(int *resultado);
int ler_unsigned_int_com_zero(unsigned int *saida);
int linha_em_branco(const char* linha);
#endif // UTILS_H
