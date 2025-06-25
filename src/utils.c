#include "../include/utils.h"
#include "../include/erros.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

/*
 * caminho_termina_com_barra - verifica se o caminho termina com '/' ou '\\'
 *
 * @caminho - string contendo o caminho a ser verificado
 *
 * Pre-condicoes:
 *      - O ponteiro 'caminho' deve ser valido (nao nulo).
 *
 * Pos-condicoes:
 *      - Retorna 1 se o caminho termina com '/' (Linux) ou '\\' ou '/' (Windows).
 *      - Retorna 0 se nao termina com separador.
 *      - Retorna -1 se a string for nula ou vazia.
 */
int caminho_termina_com_barra(const char *caminho) {
        if(caminho == NULL || caminho[0] == '\0')
                return -1;

        int tamanho_caminho = strlen(caminho);
        char ultimo_caractere = caminho[tamanho_caminho - 1];

#ifdef _WIN32
        return (ultimo_caractere == '\\' || ultimo_caractere == '/');
#else
        return (ultimo_caractere == '/');
#endif
}

/*
 * normalizar_para_sep - ajusta os separadores de caminho para o formato do sistema operacional
 *
 * @caminho - string contendo o caminho a ser normalizado (modificado in-place)
 *
 * Pre-condicoes:
 *      - A string deve estar corretamente alocada e terminada com '\0'.
 *
 * Pos-condicoes:
 *      - Em sistemas Windows, todos os '/' sao convertidos para '\\'.
 *      - Em sistemas Unix/Linux, todos os '\\' sao convertidos para '/'.
 */
void normalizar_para_sep(char* caminho) {
#ifdef _WIN32
        for (int i = 0; caminho[i] != '\0'; i++) {
                if (caminho[i] == '/') {
                        caminho[i] = '\\';
                }
        }
#else
        for (int i = 0; caminho[i] != '\0'; i++) {
                if (caminho[i] == '\\') {
                        caminho[i] = '/';
                }
        }
#endif
}

/*
 * ler_unsigned_int_direto - le um valor inteiro sem sinal da entrada padrao de forma segura
 *
 * Nao recebe parametros.
 *
 * Pre-condicoes:
 *      - Deve haver entrada disponivel no stdin.
 *
 * Pos-condicoes:
 *      - Retorna o valor lido se for um numero valido.
 *      - Retorna 0 em caso de erro, valor negativo ou entrada invalida.
 */
unsigned int ler_unsigned_int_direto() {
        char buffer[32];
        char *endptr;
        unsigned long valor;

        if(!fgets(buffer, sizeof(buffer), stdin)) {
                return 0; // retorna 0 em caso de erro
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if(buffer[0] == '-') {
                return 0;
        }

        errno = 0;
        valor = strtoul(buffer, &endptr, 10);

        if(endptr == buffer || *endptr != '\0' || errno == ERANGE) {
                return 0;
        }

        if(valor > UINT_MAX) {
                return 0;
        }

        return (unsigned int)valor;
}

/*
 * construir_caminho_completo - monta um caminho completo juntando caminho base e nome do arquivo
 *
 * @caminho_base - string com o caminho base (sera modificada)
 * @nome_arquivo - nome do arquivo a ser concatenado ao caminho
 *
 * Pre-condicoes:
 *      - caminho_base deve ter espaco suficiente para conter o resultado.
 *      - nome_arquivo deve ser uma string valida.
 *
 * Pos-condicoes:
 *      - caminho_base sera normalizado e tera a barra final adicionada, se necessario.
 *      - nome_arquivo sera concatenado ao caminho_base.
 */
void construir_caminho_completo(char* caminho_base, const char* nome_arquivo) {
        normalizar_para_sep(caminho_base); // 1. Normaliza para o separador do sistema

        // 2. Adiciona a barra final se necessário
        if(!caminho_termina_com_barra(caminho_base)) {
#ifdef _WIN32
        strcat(caminho_base, "\\");
#else
        strcat(caminho_base, "/");
#endif
        }

        // 3. Concatena o nome do arquivo
        strcat(caminho_base, nome_arquivo);
}

/*
 * trim - remove espacos em branco do inicio e do fim da string
 *
 * @str - ponteiro para string a ser modificada (in-place)
 *
 * Pre-condicoes:
 *      - str deve ser uma string valida, com terminador '\0'.
 *
 * Pos-condicoes:
 *      - Todos os espacos iniciais e finais serao removidos.
 *      - O conteudo da string sera deslocado se necessario.
 */
void trim(char *str) {
        if(!str) return;

        // Remover espaços do início
        char *inicio = str;
        while(isspace((unsigned char)*inicio)) inicio++;

        // Se a string for só espaços
        if(*inicio == '\0') {
                str[0] = '\0';
                return;
        }

        // Remover espaços do final
        char *fim = inicio + strlen(inicio) - 1;
        while(fim > inicio && isspace((unsigned char)*fim)) fim--;

        // Novo final de string
        *(fim + 1) = '\0';

        // Copiar resultado para o início da string
        if (str != inicio)
                memmove(str, inicio, fim - inicio + 2);  // +2 pra incluir '\0'
}

/*
 * obter_data_atual - obtém a data atual formatada como string
 *
 * @buffer - buffer onde a data será escrita
 * @tamanho - tamanho máximo do buffer
 *
 * Pré-condições:
 *      - 'buffer' deve ser um ponteiro v�lido com espa�o suficiente.
 * Pós-condições:
 *      - Em caso de sucesso, grava a data atual no formato DD/MM/AAAA no buffer.
 *      - Retorna SUCESSO (0) se a operação for bem-sucedida.
 *      - Retorna ERRO_OBTER_DATA (-12) se não for possível obter a data.
 */
int obter_data_atual(char *buffer, size_t tamanho) {
        time_t t = time(NULL);
        struct tm *data_local = localtime(&t);

        if (data_local) {
                strftime(buffer, tamanho, "%d/%m/%Y", data_local);
        } else {
                buffer[0] = '\0'; // string vazia em caso de erro
                return ERRO_OBTER_DATA;
        }
        return SUCESSO;
}

/*
 * ler_inteiro_seguro - le um valor inteiro da entrada padrao com validacao de caracteres
 *
 * @resultado - ponteiro onde o valor inteiro sera armazenado
 *
 * Pre-condicoes:
 *      - resultado deve ser um ponteiro valido.
 *
 * Pos-condicoes:
 *      - Retorna 1 se a entrada for valida e um inteiro foi armazenado.
 *      - Retorna 0 em caso de erro de leitura ou se a entrada for invalida.
 */
int ler_inteiro_seguro(int *resultado) {
        char buffer[100];

        if (!fgets(buffer, sizeof(buffer), stdin)) {
                return 0;
        }

        // Remove o '\n' do final, se existir
        buffer[strcspn(buffer, "\n")] = 0;

        // Verifica se a string é composta só por dígitos (e opcional sinal)
        char *ptr = buffer;
        if (*ptr == '-' || *ptr == '+') ptr++;

        while (*ptr) {
                if (!isdigit(*ptr)) return 0;
                ptr++;
        }

        // Converte para inteiro
        return sscanf(buffer, "%d", resultado) == 1;
}

/*
 * ler_unsigned_int_com_zero - le um valor inteiro sem sinal da entrada padrao, aceitando 0 como valido
 *
 * @saida - ponteiro onde o valor lido sera armazenado
 *
 * Pre-condicoes:
 *      - saida deve ser um ponteiro valido.
 *
 * Pos-condicoes:
 *      - Retorna 1 se um numero valido foi lido e armazenado.
 *      - Retorna 0 se a entrada for invalida ou negativa.
 */
int ler_unsigned_int_com_zero(unsigned int *saida) {
        char buffer[32];
        char *endptr;
        unsigned long valor;

        if (!fgets(buffer, sizeof(buffer), stdin)) {
                return 0; // erro
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (buffer[0] == '-') {
                return 0;
        }

        errno = 0;
        valor = strtoul(buffer, &endptr, 10);

        if (endptr == buffer || *endptr != '\0' || errno == ERANGE) {
                return 0;
        }

        if (valor > UINT_MAX) {
                return 0;
        }

        *saida = (unsigned int)valor;
        return 1; // sucesso
}

/*
 * linha_em_branco - verifica se a string contem apenas espacos em branco
 *
 * @linha - ponteiro para a string a ser verificada
 *
 * Pre-condicoes:
 *      - linha deve ser uma string valida.
 *
 * Pos-condicoes:
 *      - Retorna 1 se a string estiver vazia ou conter apenas espacos/tabs.
 *      - Retorna 0 se houver qualquer caractere nao branco.
 */
int linha_em_branco(const char *linha) {
        while (*linha) {
                if (!isspace((unsigned char)*linha)) {
                        return 0;  // encontrou um caractere não-branco
                }
        linha++;
        }
        return 1;  // só havia espaços/brancos
}
