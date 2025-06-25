#include "../include/arquivo.h"
#include "../include/erros.h"
#include "../include/utils.h"
#include "../include/emprestimo.h"
#include "../include/livro.h"
#include "../include/usuario.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_ARQUIVO_EMPRESTIMO "emprestimo.dat"
#define NOME_ARQUIVO_LIVRO      "livro.dat"
#define NOME_ARQUIVO_USUARIO    "usuario.dat"
// Em caso de alteração da constante MAX_BUFFER_TEMP, alterar tamanho do sscanf na função 'processar_lote' para MAX_BUFFER_TEMP - 1
#define MAX_BUFFER_TEMP         256


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
int cria_lista_vazia(FILE *arq) {
        CABECALHO cab;
        cab.pos_cabeca = -1;
        cab.pos_topo = 0;
        cab.pos_livre = -1;

        if (fseek(arq, 0, SEEK_SET) != 0) {
                return ERRO_ARQUIVO_SEEK;
        }

        if (fwrite(&cab, sizeof(CABECALHO), 1, arq) != 1) {
                return ERRO_ARQUIVO_WRITE;
        }

        return SUCESSO;
}

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
CABECALHO* le_cabecalho(FILE *arq) {
        CABECALHO *cab = malloc(sizeof(CABECALHO));
        if (!cab) return NULL;
        if (fseek(arq, 0, SEEK_SET) != 0) {
                free(cab);
                return NULL;
        }

        if (fread(cab, sizeof(CABECALHO), 1, arq) != 1) {
                free(cab);
                return NULL;
        }
        return cab;
}

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
int escreve_cabecalho(FILE* arq,CABECALHO* cab) {
        if (fseek(arq, 0, SEEK_SET) != 0)
                return ERRO_ARQUIVO_SEEK;

        if (fwrite(cab, sizeof(CABECALHO), 1, arq) != 1)
                return ERRO_ARQUIVO_WRITE;

        return SUCESSO;
}

/*
 * inicializar_arquivo - função interna que inicializa um arquivo binário com cabeçalho
 *
 * @caminho - caminho completo para o arquivo binário a ser inicializado
 *
 * Pré-condições:
 *      - Caminho para o arquivo deve ser válido.
 *      - Deve ser possível abrir ou criar o arquivo em modo leitura/escrita.
 * Pós-condições:
 *      - Caso o arquivo não exista, um arquivo binário é criado com a estrutura de cabeçalho.
 *      - Caso o arquivo exista, verifica-se se possui um cabeçalho válido. Se não tiver, cria um.
 *      - Caso o arquivo exista e tenha um cabeçalho válido, nada é feito.
 *      - Retorna SUCESSO (0) em caso de sucesso.
 *      - Retorna valores negativos em caso de erro:
 *              - ERRO_ABRIR_ARQUIVO (-10): o arquivo não pôde ser aberto (falta de permissões ou diretório inválido).
 *              - ERRO_ARQUIVO_SEEK (-1): não foi possível fazer posicionamento no arquivo (erro no fseek).
 *              - ERRO_CRIAR_LISTA (-22): não foi possível criar um cabeçalho novo no arquivo.
 */
static int inicializar_arquivo(const char* caminho) {
        FILE* arquivo = fopen(caminho, "a+b");  // cria se não existir
        if (arquivo == NULL) 
                return ERRO_ABRIR_ARQUIVO;

        // voltar para o início do arquivo
        if(fseek(arquivo, 0, SEEK_SET) != 0) {
                fclose(arquivo);
                return ERRO_ARQUIVO_SEEK;
        }

        CABECALHO cabecalho;
        if (fread(&cabecalho, sizeof(CABECALHO), 1, arquivo) != 1) {
                // arquivo novo ou corrompido, criar novo cabeçalho
                int retorno = cria_lista_vazia(arquivo);
                if(retorno < 0) {
                        fclose(arquivo);
                        return ERRO_CRIAR_LISTA;
                }
        }

        fclose(arquivo);
        return SUCESSO;
}

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
int inicializar_base_de_dados(char *caminho_diretorio) {
        char caminho_base[TAM_MAX_CAMINHO];
        strncpy(caminho_base, caminho_diretorio, TAM_MAX_CAMINHO - 1);
        caminho_base[TAM_MAX_CAMINHO - 1] = '\0';

        // adicionar barra se necessário
        if(!caminho_termina_com_barra(caminho_base)) {
                strncat(caminho_base, SEPARADOR_DIRETORIO, TAM_MAX_CAMINHO - strlen(caminho_base) - 1);
        }

        normalizar_para_sep(caminho_base);

        char caminho_completo_emprestimo[TAM_MAX_CAMINHO];
        snprintf(caminho_completo_emprestimo, TAM_MAX_CAMINHO, "%s%s", caminho_base, NOME_ARQUIVO_EMPRESTIMO);

        char caminho_completo_livro[TAM_MAX_CAMINHO];
        snprintf(caminho_completo_livro, TAM_MAX_CAMINHO, "%s%s", caminho_base, NOME_ARQUIVO_LIVRO);

        char caminho_completo_usuario[TAM_MAX_CAMINHO];
        snprintf(caminho_completo_usuario, TAM_MAX_CAMINHO, "%s%s", caminho_base, NOME_ARQUIVO_USUARIO);

        if(
                (inicializar_arquivo(caminho_completo_emprestimo) != 0) ||
                (inicializar_arquivo(caminho_completo_livro) != 0) ||
                (inicializar_arquivo(caminho_completo_usuario) != 0)
        ) {
                return ERRO_INICIALIZAR_ARQUIVO;
        }

        return SUCESSO;
}

int processar_lote(
        const char *caminho_arquivo_lote,
        const char* caminho_arquivo_emprestimo,
        const char* caminho_arquivo_livro,
        const char* caminho_arquivo_usuario
) {
        FILE* arquivo = fopen(caminho_arquivo_lote, "r");
        if (!arquivo) {
                return ERRO_ABRIR_ARQUIVO;
        }

        char linha[512];
        int numero_linha = 1;

        while (fgets(linha, sizeof(linha), arquivo)) {
                linha[strcspn(linha, "\n")] = '\0';
                trim(linha);
                if (linha[0] == 'L') {
                        LIVRO livro;
                        char titulo_temp[MAX_BUFFER_TEMP], autor_temp[MAX_BUFFER_TEMP], editora_temp[MAX_BUFFER_TEMP];

                        int lidos = sscanf(linha + 2, "%u;%255[^;];%255[^;];%255[^;];%u;%u;%u",
                                &livro.codigo,
                                titulo_temp,
                                autor_temp,
                                editora_temp,
                                &livro.edicao,
                                &livro.ano,
                                &livro.exemplares
                        );
                        trim(titulo_temp);
                        trim(autor_temp);
                        trim(editora_temp);

                        strncpy(livro.titulo, titulo_temp, MAX_TITULO);
                        livro.titulo[MAX_TITULO] = '\0';

                        strncpy(livro.autor, autor_temp, MAX_AUTOR);
                        livro.autor[MAX_AUTOR] = '\0';

                        strncpy(livro.editora, editora_temp, MAX_EDITORA);
                        livro.editora[MAX_EDITORA] = '\0';

                        // avaliação em curto-circuito
                        if (lidos != 7 || cadastrar_livro(caminho_arquivo_livro, livro) != SUCESSO) {
                                printf("Erro ao processar livro na linha %d: %s\n", numero_linha, linha);
                        }

                } else if (linha[0] == 'U') {
                        USUARIO usuario;
                        char nome_temp[MAX_BUFFER_TEMP];

                        int lidos = sscanf(linha + 2, "%u;%255[^\n]", &usuario.codigo, nome_temp);

                        trim(nome_temp);
                        strncpy(usuario.nome, nome_temp, MAX_NOME);
                        usuario.nome[MAX_NOME] = '\0';

                        if (lidos != 2 || cadastrar_usuario(caminho_arquivo_usuario, usuario) != SUCESSO) {
                                printf("Erro ao processar usuário na linha %d: %s\n", numero_linha, linha);
                        }

                } else if (linha[0] == 'E') {
                        int cod_usuario, cod_livro;
                        char data_emp[MAX_DATA + 1], data_dev[MAX_DATA + 1] = "";
                        char data_emp_temp[MAX_BUFFER_TEMP], data_dev_temp[MAX_BUFFER_TEMP] = "";

                        int lidos = sscanf(linha + 2, "%u;%u;%255[^;];%255[^\n]", &cod_usuario, &cod_livro, data_emp_temp, data_dev_temp);

                        trim(data_emp_temp);
                        trim(data_dev_temp);

                        strncpy(data_emp, data_emp_temp, MAX_DATA);
                        data_emp[MAX_DATA] = '\0';
                        strncpy(data_dev, data_dev_temp, MAX_DATA);
                        data_dev[MAX_DATA] = '\0';
                        if (lidos < 3) {
                                printf("Erro ao processar empréstimo na linha %d: %s\n", numero_linha, linha);
                        }
                        else {
                                if (
                                        emprestar_livro(
                                                caminho_arquivo_emprestimo,
                                                caminho_arquivo_livro,
                                                caminho_arquivo_usuario,
                                                cod_usuario,
                                                cod_livro,
                                                data_emp
                                        ) != SUCESSO
                                ) {
                                        printf("Erro ao emprestar livro na linha %d: %s\n", numero_linha, linha);
                                }
                                // Se foi fornecida a data de devolução
                                if (lidos == 4 && strlen(data_dev) > 0) {
                                        if (
                                                devolver_livro(
                                                        caminho_arquivo_emprestimo,
                                                        caminho_arquivo_livro,
                                                        cod_usuario,
                                                        cod_livro,
                                                        data_dev
                                                ) != SUCESSO
                                        ) {
                                                printf("Erro ao devolver livro na linha %d: %s\n", numero_linha, linha);
                                        }
                                }
                        }

                }
                else {
                        printf("Linha %d com tipo desconhecido: %s\n", numero_linha, linha);
                }

                numero_linha++;
        }

        fclose(arquivo);
        return SUCESSO;
}
