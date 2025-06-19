cmake_minimum_required(VERSION 3.31)
project(TRABALHO_AED C)

set(CMAKE_C_STANDARD 11)

# Inclui os diretórios de headers
include_directories(include)
include_directories(src)

# Adiciona o executável com os arquivos fonte
add_executable(TRABALHO_AED
        src/livro.c
        src/arquivo.c
)
