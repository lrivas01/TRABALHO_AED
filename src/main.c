#include"../include/livro.h"
#include"../include/arquivo.h"
#include"../include/usuario.h"
#include"../include/utils.h"
#define MAX_CAMINHO 32768 //tamanho maximo + 1 que um caminho pode ter no windows, no linux o limite é menor

#include<stdio.h>
#include<string.h>

void limpar_enter (char *str);
void limpar_buffer ();
void exibir_menu ();
void opcao_cadastrar_livro (char* caminho_livros);
void opcao_imprimir_livro(char* caminho_livros);
void opcao_cadastrar_usuario (char* caminho_usuarios);
void opcao_buscar_por_titulo (char *caminho_livros);
void opcao_emprestar_livro(caminho_emprestimos,caminho_livros,caminho_usuarios);
void opcao_devolver_livro();

int main () {
    char diretorio[MAX_CAMINHO];
    char caminho_livros[MAX_CAMINHO];
    char caminho_usuarios[MAX_CAMINHO];
    char caminho_emprestimos[MAX_CAMINHO];
    int sucesso = 0;

    printf("------ SISTEMA BIBLIOTECA ------\n");

    do {
        printf("Informe o diretório dos arquivos ou pressione Enter para usar './dados/': ");
        fgets(diretorio, MAX_CAMINHO, stdin);
        diretorio[strcspn(diretorio, "\n")] = '\0';

        if (strlen(diretorio) == 0) {
            strcpy(diretorio, ".");
        }

        snprintf(caminho_livros, MAX_CAMINHO, "%s/livro.dat", diretorio);
        snprintf(caminho_usuarios, MAX_CAMINHO, "%s/usuario.dat", diretorio);
        snprintf(caminho_emprestimos, MAX_CAMINHO, "%s/emprestimo.dat", diretorio);

        int r = inicializar_base_de_dados(diretorio);
        printf("---------codigo de r: %d--------\n", r);
        if (r < 0) {
            printf("Não foi possível inicializar os arquivos no diretório '%s'.\n", diretorio);
            printf("Verifique se o caminho existe e se há permissões de escrita/leitura.\n\n");
        } else {
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
                    if(calcular_total_livros(caminho_livros)!=0)//total de nós, nao e exemplares
                    	printf("Erro ao calcular total de livros\n");
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
                     listar_emprestados(caminho_emprestimos,caminho_livros,caminho_usuarios);
                    break;
                case 10:
                    // carregar arquivo(...);
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





void limpar_enter (char *str) {
    str[strcspn(str, "\n")] = '\0';
}

void limpar_buffer () {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_menu () {
   	printf("\n====MENU PRINCIPAL====\n");
    printf("1 - CADASTRAR LIVROS\n");
    printf("2 - IMPRIMIR DADOS DO LIVRO\n");
    printf("3 - LISTAR TODOS OS LIVROS\n");
    printf("4 - BUSCA POR TITULO\n");
    printf("5 - CALCULAR TOTAL\n");
    printf("6 - CADASTRAR USUARIO\n");
    printf("7 - EMPRESTAR LIVRO\n");
    printf("8 - DEVOLVER LIVRO\n");
    printf("9 - LISTAR LIVROS EMPRESTADOS\n");
    printf("10 - CARREGAR ARQUIVO\n");
    printf("0 - SAIR\n");
    printf("========================\n");
}



void opcao_cadastrar_livro (char* caminho_livros) {
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
    if((resposta = cadastrar_livro(caminho_livros,livro)) == 0)
   	printf("Livro cadastrado com sucesso!\n");
    else
        printf("Erro ao cadastrar livro: erro %d\n", resposta);
}


void opcao_imprimir_livro(char* caminho_livros) {
    unsigned int codigo;
    unsigned int resultado;

    printf("Digite o código do livro: ");

    resultado = ler_unsigned_int_direto();

    if (resultado < 0) {
        printf("Erro na leitura da entrada!\n");
        return;
    }


    int retorno = imprimir_livro(caminho_livros, codigo);

    if (retorno != 0) {
        if (retorno == ERRO_ENCONTRAR_LIVRO) {
            printf("Livro com código %d não encontrado.\n", codigo);
        } else {
            printf("Erro ao buscar livro: código de erro %d\n", retorno);
        }
    }
}



void opcao_cadastrar_usuario (char* caminho_usuarios) {
    unsigned int codigo;
    char nome[MAX_NOME+1];
    printf("Insira o nome do usuario: ");
    fgets(nome, MAX_NOME+1, stdin);
    nome[strcspn(nome, "\n")] = '\0';
    printf("\nInsira o codigo do usuario: ");
    if (scanf("%u", &codigo) != 1) {
        printf("Entrada inválida para o código!\n");
        limpar_buffer();
        return;
    }
    limpar_buffer(); // limpa o \n deixado pelo scanf
    if(cadastrar_usuario(caminho_usuarios,codigo,nome)==0)
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


void opcao_emprestar_livro (caminho_emprestimos,caminho_livros,caminho_usuarios) {
	char buffer[MAX_DATA+1];
	unsigned int codigo_usuario;
	unsigned int codigo_livro;
	printf("Digite o codigo do usuario: ");
	codigo_usuario = ler_unsigned_int_direto();
	printf("\nDigite o codigo do livro: ");
	codigo_livro =  ler_unsigned_int_direto();
	printf("\nDigite a data do emprestimo: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        printf("erro na leitura de data"); // erro na leitura
    }

    buffer[strcspn(buffer, "\n")] = '\0';
	if(emprestar_livro(
        caminho_emprestimos,
        caminho_livros,
        caminho_usuarios,
        codigo_usuarios,
        codigo_livros,
        buffer
	) < 0)printf("Erro ao realizar o emprestimo do livro\n");

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
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        printf("erro na leitura de data"); // erro na leitura
    }

    buffer[strcspn(buffer, "\n")] = '\0';

if(devolver_livro(
        caminho_emprestimos,
        caminho_livros,
        codigo_usuario,
        codigo_livro,
        buffer
) < 0 )printf("erro ao realizar a devolução do livro\n");

}