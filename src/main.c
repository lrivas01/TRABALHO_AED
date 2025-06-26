#include "../include/arquivo.h"
#include "../include/emprestimo.h"
#include "../include/livro.h"
#include "../include/usuario.h"
#include "../include/utils.h"

#include <stdio.h>
#include <string.h>

void limpar_enter (char *str);
void exibir_menu ();

void opcao_cadastrar_livro (char* caminho_livros);
void opcao_imprimir_livro(char* caminho_livros);
void opcao_cadastrar_usuario (char* caminho_usuarios);
void opcao_buscar_por_titulo (char *caminho_livros);
void opcao_emprestar_livro(char* caminho_emprestimos, char* caminho_livros, char* caminho_usuarios);
void opcao_devolver_livro(char* caminho_emprestimos, char* caminho_livros);
void opcao_total_cadastrados(char *caminho_livros);
void opcao_carregar_lote(char* caminho_emprestimos, char* caminho_livros, char* caminho_usuarios);

int main () {
        char diretorio[TAM_MAX_CAMINHO];
        char caminho_livros[TAM_MAX_CAMINHO];
        char caminho_usuarios[TAM_MAX_CAMINHO];
        char caminho_emprestimos[TAM_MAX_CAMINHO];
        int sucesso = 0;

        printf("------ SISTEMA BIBLIOTECA ------\n");

        do {
                printf("Informe o diretorio dos arquivos ou pressione Enter para usar diretorio atual: ");
                fgets(diretorio, TAM_MAX_CAMINHO, stdin);
                diretorio[strcspn(diretorio, "\n")] = '\0';

                if(strlen(diretorio) == 0) {
                        strcpy(diretorio, ".");
                }

                strncpy(caminho_livros, diretorio, TAM_MAX_CAMINHO - 1);
                construir_caminho_completo(caminho_livros, "livro.dat");

                strncpy(caminho_usuarios, diretorio, TAM_MAX_CAMINHO - 1);
                construir_caminho_completo(caminho_usuarios, "usuario.dat");

                strncpy(caminho_emprestimos, diretorio, TAM_MAX_CAMINHO - 1);
                construir_caminho_completo(caminho_emprestimos, "emprestimo.dat");

                int r = inicializar_base_de_dados(diretorio);
                if (r < 0) {
                        printf("Nao foi possivel inicializar os arquivos no diretorio '%s'.\n", diretorio);
                        printf("Verifique se o caminho existe e se ha permissoes de escrita/leitura.\n\n");
                }
                else {
                        sucesso = 1;
                }
        } while (!sucesso);

        int opcao;
        do {
                exibir_menu();
                printf("Escolha uma opcao: \n");

                scanf("%d", &opcao);
                getchar();

                switch (opcao) {
                        case 1:
                                opcao_cadastrar_livro(caminho_livros);
                                break;
                        case 2:
                                opcao_imprimir_livro(caminho_livros);
                                break;
                        case 3:
                                listar_todos_livros(caminho_livros);
                                break;
                        case 4:
                                opcao_buscar_por_titulo(caminho_livros);
                                break;
                        case 5:
                                opcao_total_cadastrados(caminho_livros);
                                break;
                        case 6:
                                opcao_cadastrar_usuario(caminho_usuarios);
                                break;
                        case 7:
                                opcao_emprestar_livro(caminho_emprestimos,caminho_livros,caminho_usuarios);
                                break;
                        case 8:
                                opcao_devolver_livro(caminho_emprestimos,caminho_livros);
                                break;
                        case 9:
                                listar_livros_emprestados(caminho_emprestimos, caminho_livros, caminho_usuarios);
                                break;
                        case 10:
                                opcao_carregar_lote(caminho_emprestimos, caminho_livros, caminho_usuarios);
                                break;
                        case 0:
                                printf("Encerrando o programa.\n");
                                break;
                        default:
                                printf("Opcao invalida. Tente novamente.\n");
                }

        } while (opcao != 0);

        return SUCESSO;
}

/*
 * limpar_enter - remover caractere '\n' de string
 *
 * @str - string para remover '\n'
 *
 * Pré-condições:
 *      - str deve ser uma string terminada em '\0'.
 * Pós-condições:
 *      - Se houver um caractere '\n' na string, será substituído por '\0'.
 *      - A string será truncada na primeira ocorrência de '\n'.
 */
void limpar_enter(char *str) {
        str[strcspn(str, "\n")] = '\0';
}

/*
 * exibir_menu - exibe o menu principal do programa
 *
 * Pré-condições:
 *      Nenhuma.
 * Pós-condições:
 *      - Menu principal será exibido na saída padrão
 */
void exibir_menu() {
        printf("\n====MENU PRINCIPAL====\n");
        printf("1  - CADASTRAR LIVRO\n");
        printf("2  - IMPRIMIR DADOS DO LIVRO\n");
        printf("3  - LISTAR TODOS OS LIVROS\n");
        printf("4  - BUSCA POR TITULO\n");
        printf("5  - CALCULAR TOTAL\n");
        printf("6  - CADASTRAR USUARIO\n");
        printf("7  - EMPRESTAR LIVRO\n");
        printf("8  - DEVOLVER LIVRO\n");
        printf("9  - LISTAR LIVROS EMPRESTADOS\n");
        printf("10 - CARREGAR ARQUIVO\n");
        printf("0  - SAIR\n");
        printf("========================\n");
}

/*
 * opcao_cadastrar_livro - interage com usuário para cadastrar novo livro
 *
 * @caminho_livros - caminho completo para o arquivo binário que armazena livros.
 *
 * Pré-condições:
 *              - O caminho para o arquivo deve ser válido.
 *              - O arquivo pode ser aberto em modo leitura e escrita.
 * Pós-condições:
 *              - Um novo livro é adicionado ao arquivo, se todos os dados forem válidos
 */
void opcao_cadastrar_livro(char* caminho_livros) {
        LIVRO livro;

        printf("\nCodigo do livro: ");
        while ((livro.codigo = ler_unsigned_int_direto()) == 0) {
                printf("Codigo invalido (deve ser um numero maior que zero)\n");
                printf("Codigo do livro: ");
        }

        printf("\nTítulo: ");
        fgets(livro.titulo, MAX_TITULO, stdin);
        limpar_enter(livro.titulo);

        printf("\nAutor: ");
        fgets(livro.autor, MAX_AUTOR, stdin);
        limpar_enter(livro.autor);

        printf("\nEditora: ");
        fgets(livro.editora, MAX_EDITORA, stdin);
        limpar_enter(livro.editora);

        printf("\nEdicao: ");
        while ((livro.edicao = ler_unsigned_int_direto()) == 0) {
                printf("Edicao invalida (deve ser um numero maior que zero)\n");
                printf("Edicao: ");
        }

        printf("\nAno: ");
        while ((livro.ano = ler_unsigned_int_direto()) == 0) {
                printf("Ano invalido (deve ser um numero maior que zero)\n");
                printf("Ano: ");
        }

        unsigned int exemplares;
        printf("\nQuantidade de exemplares: ");
        while (!ler_unsigned_int_com_zero(&exemplares)) {
                printf("Digite um valor valido (nao negativo)\n");
                printf("Quantidade de exemplares: ");
        }
        livro.exemplares = exemplares;

        int resposta;
        if((resposta = cadastrar_livro(caminho_livros,livro)) != 0) {
                printf("\nErro ao cadastrar livro");

                if(resposta == ERRO_CONFLITO_ID)
                        printf(": Livro com codigo ja cadastrado\n");
                else
                        printf("\n");
        }
        else {
                printf("\nLivro \"%s\" cadastrado com sucesso!\n", livro.titulo);
        }
}

/*
 * opcao_imprimir_livro - interage com o usuário para imprimir informações sobre livro
 *
 * @caminho_livros - caminho completo para arquivo binário que armazena livros
 *
 * Pré-condições:
 *              - Caminho para arquivo deve ser válido e ser acessível em modo leitura e escrita.
 *              - Deve ser um arquivo inicializado (com cabeçalho).
 * Pós-condições:
 *              - Imprime informações sobre livro, se encontrado
 */
void opcao_imprimir_livro(char* caminho_livros) {
        unsigned int codigo;

        printf("Digite o codigo do livro: ");
        while ((codigo = ler_unsigned_int_direto()) == 0) {
                printf("Codigo invalido (deve ser um numero maior que zero)\n");
                printf("Digite o codigo do livro: ");
        }

        int retorno = imprimir_livro(caminho_livros, codigo);
        if (retorno != 0) {
                if (retorno == ERRO_ENCONTRAR_LIVRO) {
                        printf("\nLivro com codigo \"%d\" não encontrado.\n", codigo);
                }
                else {
                        printf("\nErro ao buscar livro\n");
                }
        }
}

/*
 * opcao_cadastrar_usuario - interage com o usuário para cadastrar usuário
 *
 * @caminho_usuarios - caminho completo para arquivo binário que armazena usuários
 *
 * Pré-condições:
 *              - Caminho para o arquivo deve ser válido e pode ser aberto em modo leitura e escrita.
 *              - Arquivo deve ser inicializado (conter cabeçalho).
 * Pós-condições:
 *              - Usuário é registrado se todas as informações forem válidas.
 */
void opcao_cadastrar_usuario (char* caminho_usuarios) {
        USUARIO usuario;

        printf("\nInsira o nome do usuario: ");
        fgets(usuario.nome, MAX_NOME, stdin);
        usuario.nome[strcspn(usuario.nome, "\n")] = '\0';

        printf("\nInsira o codigo do usuario: ");
        while ((usuario.codigo = ler_unsigned_int_direto()) == 0) {
	        printf("Codigo invalido (deve ser um numero maior que zero)\n");
	        printf("Insira o codigo do usuario: ");
        }

        int retorno;
        if((retorno = cadastrar_usuario(caminho_usuarios, usuario)) == 0) {
                printf("\nUsuario cadastrado com sucesso!\n");
        }
        else {
                printf("\nFalha ao cadastrar usuario\n");
                if(retorno == ERRO_CONFLITO_ID)
                        printf("Codigo de usuario informado ja esta registrado\n\n");
        }
}

/*
 * opcao_buscar_por_titulo - interage com o usuário para realizar busca de livro por título
 *
 * @caminho_livros - caminho completo para arquivo binário que armazena livros
 *
 * Pré-condições:
 *              - Arquivo deve ser válido e pode ser aberto em leitura e escrita.
 *              - Arquivo deve ser inicializado (conter cabeçalho).
 * Pós-condições:
 *              - Livro buscado é exibido se for encontrado pelo título.
 */
void opcao_buscar_por_titulo (char *caminho_livros) {
        char titulo[MAX_TITULO+1];

        printf("\nInsira o nome do livro: ");
        fgets(titulo, MAX_TITULO+1, stdin);
        titulo[strcspn(titulo, "\n")] = '\0';

        buscar_titulo_livro(caminho_livros, titulo);
}

/*
 * opcao_emprestar_livro - interage com o usuário para realizar empréstimo de livro
 *
 * @caminho_emprestimos - caminho completo para arquivo binário de empréstimos.
 * @caminho_livros - caminho completo para arquivo binário de livros.
 * @caminho_usuarios - caminho completo para arquivo binário de usuários.
 *
 * Pré-condições:
 *              - Arquivos devem ser válidos com permissões de leitura e escrita.
 *              - Arquivos devem estar inicializados (com cabeçalho).
 * Pós-condições:
 *              - Empréstimo é registrado se dados fornecidos forem corretos.
 *              - Quantidade de exemplares do livro emprestado é decrementada.
 */
void opcao_emprestar_livro (char* caminho_emprestimos, char* caminho_livros, char* caminho_usuarios) {
        char data[MAX_DATA+1];
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	
        printf("\nDigite o codigo do usuario: ");
	while ((codigo_usuario = ler_unsigned_int_direto()) == 0) {
	        printf("Codigo invalido (deve ser um numero maior que zero)\n");
	        printf("Digite o codigo do usuario: ");
        }

        printf("\nDigite o codigo do livro: ");
	while ((codigo_livro = ler_unsigned_int_direto()) == 0) {
	        printf("Codigo invalido (deve ser um numero maior que zero)\n");
	        printf("Digite o codigo do livro: ");
        }

        if(obter_data_atual(data, sizeof(data)) != 0) {
                printf("\nNao foi possivel obter a data atual. Emprestimo nao registrado.");
                return;
        }

        int res = 0;
        if((res = emprestar_livro(caminho_emprestimos, caminho_livros, caminho_usuarios, codigo_usuario, codigo_livro, data)) < 0)
                printf("\nErro ao realizar o emprestimo do livro\n");
        else
                printf("\nLivro cadastrado com sucesso\n");

        if(res == ERRO_CONFLITO_ID)
                printf("Livro ja emprestado a este usuario\n");
        if(res == ERRO_LIVROS_ESGOTADOS)
                printf("Exemplares do livro esgotados\n");
}

/*
 * opcao_devolver_livro - interage com o usuário para registrar devolução de livro
 *
 * @caminho_emprestimos - caminho completo para arquivo binário de empréstimos.
 * @caminho_livros - caminho completo para arquivo binário de livros.
 *
 * Pré-condições:
 *              - Arquivos devem ser válidos e podem ser acessados em leitura e escrita.
 *              - Arquivos devem estar inicializados (com cabeçalho).
 * Pós-condições:
 *              - Devolução de livro é registrada se todos os dados forem corretos.
 *              - Quantidade de exemplares do livro devolvido é incrementada.
 */
void opcao_devolver_livro (char* caminho_emprestimos, char*caminho_livros) {
	char data[MAX_DATA + 1];
	unsigned int codigo_usuario;
	unsigned int codigo_livro;

	printf("\nDigite o codigo do usuario: ");
	while ((codigo_usuario = ler_unsigned_int_direto()) == 0) {
	        printf("Codigo invalido (deve ser um numero maior que zero)\n");
	        printf("Digite o codigo do usuario: ");
        }

	printf("\nDigite o codigo do livro: ");
	while ((codigo_livro = ler_unsigned_int_direto()) == 0) {
	        printf("Codigo invalido (deve ser um numero maior que zero)\n");
	        printf("Digite o codigo do livro: ");
        }

        if(obter_data_atual(data, sizeof(data)) != 0) {
                printf("\nNao foi possivel obter a data atual. Devolucao nao registrada.");
                return;
        }
        data[MAX_DATA] = '\0';

        if(devolver_livro(caminho_emprestimos, caminho_livros, codigo_usuario, codigo_livro, data ) < 0 )
                printf("Erro ao realizar a devolucao do livro\n");
        else
                printf("Devolucao realizada com sucesso\n");
}

/*
 * opcao_total_cadastrados - exibe quantidade total de livros cadastrados
 *
 * @caminho_livros - caminho completo para arquivo binário de livros
 *
 * Pré-condições:
 *              - Arquivo deve ser válido e possuir permissões de leitura.
 *              - Arquivo deve estar inicializado (com cabeçalho).
 * Pós-condições:
 *              - Quantidade total de livros cadastrados é exibida.
 */
void opcao_total_cadastrados(char *caminho_livros) {
        int quantidade_total_livros = calcular_total_livros(caminho_livros);
        if(quantidade_total_livros!=0) {
                printf("Erro ao calcular total de livros\n");
        }
}

/*
 * opcao_carregar_lote - interage com usuário para carregar informações em lote
 *
 * @caminho_emprestimos - caminho completo para arquivo binário de empréstimos
 * @caminho_livros - caminho completo para arquivo binário de livros
 * @caminho_usuarios - caminho completo para arquivo de usuários
 *
 * Pré-condições:
 *              - Arquivos devem ser válidos e possuir permissões de leitura e escrita.
 *              - Arquivos devem estar inicializados (com cabeçalho).
 * Pós-condições:
 *              - Registro de livros, usuários e empréstimos é feito se estiverem no
 *              formato correto.
 */
void opcao_carregar_lote(char* caminho_emprestimos, char* caminho_livros, char* caminho_usuarios) {
        char diretorio[TAM_MAX_CAMINHO];
        printf("\nInforme o caminho para o arquivo contendo os registros\n");
        printf("Ha suporte para o formato \"./nome_exemplo.txt\" para indicar diretorio atual\n");
        fgets(diretorio, TAM_MAX_CAMINHO, stdin);
        diretorio[strcspn(diretorio, "\n")] = '\0';

        int retorno = processar_lote(diretorio, caminho_emprestimos, caminho_livros, caminho_usuarios);

        if(retorno == ERRO_ABRIR_ARQUIVO)
                printf("\nNao foi possivel abrir o arquivo\n");
        else
                printf("\nCarregamento concluido!\n");
}
