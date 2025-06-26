# Sistema de Gerenciamento de Biblioteca

## Autores:
- Lucas Rivas das Neves
- Erick winck

## Funcionalidades Implementadas

### 1. Cadastrar Livro
Permite registrar um novo livro no sistema com os campos: código, título, autor, editora, edição, ano e número de exemplares.

### 2. Imprimir Dados do Livro
Busca um livro pelo código e exibe todas as suas informações cadastradas.

### 3. Listar Todos os Livros
Mostra uma lista com código, título, autor e número de exemplares disponíveis para todos os livros.

### 4. Busca por Título
Permite procurar um livro pelo título completo, exibindo todas as informações do livro.

### 5. Calcular Total de Livros
Exibe número total de livros cadastrados no sistema (não quantificando número de exemplares).

### 6. Cadastrar Usuário
Adiciona um novo usuário à base de dados, pedindo código e nome.

### 7. Emprestar Livro
Registra um empréstimo de um livro a um usuário, usando a data atual. Diminui o número de exemplares disponíveis. Exibe mensagem se não houver exemplares.

### 8. Devolver Livro
Registra a devolução de um livro, atualizando a data e aumentando o número de exemplares disponíveis.

### 9. Listar Livros Emprestados
Exibe os livros atualmente emprestados (sem data de devolução), com código e nome do usuário, título e data do empréstimo.

### 10. Carregar Arquivo
Lê um arquivo `.txt` com dados de livros, usuários e empréstimos, e adiciona ao sistema.

L;\<codigo>;\<titulo>;\<autor>;\<editora>;\<edicao>;\<ano>;\<exemplares>

U;\<codigo>;\<nome>

E;\<codigo_usuario>;\<codigo_livro>;\<data_emprestimo>;\<data_devolucao>

Com L,U,E significando respectivamente:

L: livro

U: usuário

E: empréstimo

## Observações Técnicas

- Todas as informações são salvas em arquivos binários com listas encadeadas.
- As funções seguem um padrão de documentação com pré-condições, pós-condições e descrição.
- Campos são tratados para ignorar espaços extras antes e depois dos valores.
