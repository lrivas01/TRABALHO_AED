#include"../include/livro.h"
#include"../include/arquivo.h"
#include"../include/usuario.h"
#define MAX_CAMINHO 32768 //tamanho maximo + 1 que um caminho pode ter no windows, no linux o limite é menor

#include<stdio.h>
#include<string.h>


int main () {
    int inicializar=0;
    int caminho_padrao;
    char caminho[MAX_CAMINHO];
    printf("------SISTEMA BIBLIOTECA-----\n");
    printf("Insira o caminho desejado para carregar as informações\n");
    printf("Deixe em branco para caminho padrão\n");
    do {
        fgets(caminho,MAX_CAMINHO,stdin);
        char *entrada = strtok(caminho, "\n");

       // Verifica se o usuário apenas pressionou Enter
       if (entrada == NULL || entrada[0] == '\0') {
           caminho_padrao = 1;
       } else {
           caminho_padrao = 0;
       }

        //usar strcmp para ver se e igual a String PADRAO se nao for, tentar inicializar no caminho pedido
         //inicializar vai ter algum valor diferente de zero caso nao consiga iniciar os arquivos no caminho
        if(!caminho_padrao)
            inicializar= inicializar_arquivo(entrada);
        if(!caminho_padrao&&!inicializar)
            printf("Caminho incorreto, insira novamente ou deixe em branco\n");
    }while (!caminho_padrao&&!inicializar);
    if (caminho_padrao){
        //usar strcmp na funcao para inicializar os tres na pasta local
        inicializar_arquivo("PADRAO");
    }

    int escolha;
    do {
        printf("-----Escolha uma opção-----\n");
        printf("\n1 - CADASTRAR LIVROS\n");
        printf("2 - EXEMPLARES\n");
        printf("3 - IMPRIMIR DADOS DO LIVRO\n");
        printf("4 - LISTAR TODOS OS LIVROS\n");
        printf("5 - BUSCA POR TITULO\n");
        printf("6 - CALCULAR TOTAL\n");
        printf("7 - CADASTRAR USUARIO\n");
        printf("8 - EMPRESTAR LIVRO\n");
        printf("9 - DEVOLVER LIVRO\n");
        printf("10 - LISTAR LIVROS EMPRESTADOS\n");
        printf("11 - CARREGAR ARQUIVO\n");
        printf("0 - SAIR\n");
        scanf("%d%*c",&escolha);
        //adicionar switch case com cada coisa




} while (escolha!=0);

    return SUCESSO;
}
