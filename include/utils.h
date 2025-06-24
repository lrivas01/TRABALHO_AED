#ifndef UTILS_H
#define UTILS_H

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
#endif // UTILS_H
