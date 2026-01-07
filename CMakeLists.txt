cmake_minimum_required(VERSION 3.10)

# Nome del progetto e versione
project(ProgettoAutostrada VERSION 1.0)

# Imposta lo standard C++ a 17 (o 20 se preferisci, 17 è sicuro e moderno)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Aggiunge la cartella 'include' al path di inclusione
# Questo permette di scrivere #include "Simulatore.h" invece di "../include/Simulatore.h"
include_directories(include)

# Attiva i warning del compilatore (Best Practice: "Errori comuni" e "Coding Style")
if(MSVC)
    add_compile_options(/W3 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# --- ESEGUIBILE 1: Simulatore ---
# Compila il main principale e la logica del simulatore
add_executable(simulatore 
    src/main.cpp 
    src/Simulatore.cpp
)

# --- ESEGUIBILE 2: Tutor ---
# Compila il main del tutor e la logica del tutor
add_executable(tutor 
    src/main_tutor.cpp 
    src/Tutor.cpp
)

# --- GESTIONE CARTELLA DATA (Opzionale ma consigliato) ---
# Copia la cartella 'data' nella directory di build per far funzionare i path relativi
# quando esegui il programma dalla cartella di build.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})