cmake_minimum_required(VERSION 3.31)
project(TRABALHO_AED C)

set(CMAKE_C_STANDARD 11)

include_directories(TrabalhoBiblioteca/include)

add_executable(TRABALHO_AED
        TrabalhoBiblioteca/include/livro.h
        TrabalhoBiblioteca/src/livro.c
        TrabalhoBiblioteca/src/main.c
        TrabalhoBiblioteca/include/arquivo.h
        TrabalhoBiblioteca/src/arquivo.c)
