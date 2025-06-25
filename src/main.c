#include "../include/arquivo.h"
#include "../include/emprestimo.h"
#include "../include/livro.h"
#include "../include/usuario.h"
#include "../include/utils.h"

#include <stdio.h>
#include <string.h>

void limpar_enter (char *str);
void limpar_buffer ();
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
                printf("Informe o diretório dos arquivos ou pressione Enter para usar diretório atual: ");
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
                printf("---------codigo de r: %d--------\n", r);
                if (r < 0) {
                        printf("Não foi possível inicializar os arquivos no diretório '%s'.\n", diretorio);
                        printf("Verifique se o caminho existe e se há permissões de escrita/leitura.\n\n");
                }
                else {
                        sucesso = 1;
                }
        } while (!sucesso);

        int opcao;
        do {
                exibir_menu();
                printf("Escolha uma opção: ");

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
                                processar_lote("./lote.txt", caminho_emprestimos, caminho_livros, caminho_usuarios);
                                break;
                        case 0:
                                printf("Encerrando o programa.\n");
                                break;
                        default:
                                printf("Opção inválida. Tente novamente.\n");
                }

        } while (opcao != 0);

        return SUCESSO;
}

void limpar_enter(char *str) {
        str[strcspn(str, "\n")] = '\0';
}

void limpar_buffer() {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_menu() {
        printf("\n====MENU PRINCIPAL====\n");
        printf("1  - CADASTRAR LIVROS\n");
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

void opcao_cadastrar_livro(char* caminho_livros) {
        LIVRO livro;

        printf("Código do livro: ");
        scanf("%d", &livro.codigo);
        getchar();

        printf("Título: ");
        fgets(livro.titulo, MAX_TITULO, stdin);
        limpar_enter(livro.titulo);

        printf("Autor: ");
        fgets(livro.autor, MAX_AUTOR, stdin);
        limpar_enter(livro.autor);

        printf("Editora: ");
        fgets(livro.editora, MAX_EDITORA, stdin);
        limpar_enter(livro.editora);

        printf("Edição: ");
        scanf("%d", &livro.edicao);

        printf("Ano: ");
        scanf("%d", &livro.ano);

        printf("Quantidade de exemplares: ");
        scanf("%d", &livro.exemplares);

        int resposta;
        if((resposta = cadastrar_livro(caminho_livros,livro)) != 0) {
                printf("Erro ao cadastrar livro");

                if(resposta == ERRO_CONFLITO_ID)
                        printf(": Livro com codigo ja cadastrado\n");
                else
                        printf("\n");
        }
        else {
                printf("Livro cadastrado com sucesso!\n");
        }
        getchar();
        getchar();
        printf("\033[2J\033[H");
}

void opcao_imprimir_livro(char* caminho_livros) {
        unsigned int codigo;

        printf("Digite o código do livro: ");

        codigo = ler_unsigned_int_direto();

        int retorno = imprimir_livro(caminho_livros, codigo);

        if (retorno != 0) {
                if (retorno == ERRO_ENCONTRAR_LIVRO) {
                        printf("Livro com código %d não encontrado.\n", codigo);
                }
                else {
                        printf("Erro ao buscar livro: código de erro %d\n", retorno);
                }
        }
}

void opcao_cadastrar_usuario (char* caminho_usuarios) {
        USUARIO usuario;
        printf("Insira o nome do usuario: ");
        fgets(usuario.nome, MAX_NOME, stdin);
        usuario.nome[strcspn(usuario.nome, "\n")] = '\0';
        printf("\nInsira o codigo do usuario: ");
        if(scanf("%u", &usuario.codigo) != 1) {
                printf("Entrada inválida para o código!\n");
                limpar_buffer();
                return;
        }
        limpar_buffer(); // limpa o \n deixado pelo scanf
        if(cadastrar_usuario(caminho_usuarios, usuario) == 0)
                printf("Usuario cadastrado com sucesso!\n");
        else
                printf("Falha ao cadastrar usuario\n");
}


void opcao_buscar_por_titulo (char *caminho_livros) {
        char nome[MAX_TITULO+1];
        printf("Insira o nome do livro: ");
        fgets(nome, MAX_TITULO+1, stdin);
        nome[strcspn(nome, "\n")] = '\0';
        buscar_titulo_livro(caminho_livros,nome);
}


void opcao_emprestar_livro (char* caminho_emprestimos, char* caminho_livros, char* caminho_usuarios) {
        char data[MAX_DATA+1];
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	
        printf("Digite o codigo do usuario: ");
	codigo_usuario = ler_unsigned_int_direto();
	
        printf("\nDigite o codigo do livro: ");
	codigo_livro =  ler_unsigned_int_direto();
	
        if(obter_data_atual(data, sizeof(data)) != 0) {
                printf("\nNao foi possivel obter a data atual. Emprestimo nao registrado.");
                return;
        }
        int res = 0;
	if((res = emprestar_livro(caminho_emprestimos, caminho_livros, caminho_usuarios, codigo_usuario, codigo_livro, data)) < 0)
                printf("Erro ao realizar o emprestimo do livro\n");

        if(res == ERRO_CONFLITO_ID)
                printf("Livro ja empresta a este usuario\n");
        if(res == ERRO_LIVROS_ESGOTADOS)
                printf("Exemplares do livro esgotados\n");

}

void opcao_devolver_livro (char* caminho_emprestimos,char*caminho_livros) {
	char buffer[MAX_DATA+1];
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	printf("Digite o codigo do usuario: ");
	codigo_usuario = ler_unsigned_int_direto();
	printf("\nDigite o codigo do livro: ");
	codigo_livro =  ler_unsigned_int_direto();
	printf("\nDigite a data da devolução: ");
        if(!fgets(buffer, sizeof(buffer), stdin)) {
                printf("erro na leitura de data"); // erro na leitura
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if(devolver_livro(caminho_emprestimos, caminho_livros, codigo_usuario, codigo_livro, buffer ) < 0 )
                printf("erro ao realizar a devolução do livro\n");

}

void opcao_total_cadastrados (char *caminho_livros) {
        int x=calcular_total_livros(caminho_livros);
        if(x<0) {
                printf("Erro ao calcular total de livros\n");
        }
        else {
                printf("Quantia total de Livros cadastrados: %d\n",x);
        }
}

void opcao_carregar_lote(char* caminho_emprestimos, char* caminho_livros, char* caminho_usuarios) {
        return;
}
