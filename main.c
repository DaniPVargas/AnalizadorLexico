#include "analizadorLexico.h"
#include "analizadorSintactico.h"
#include "tablaSimbolos.h"
#include "sistemaEntrada.h"

int main() {

    //Inicializamos el buffer y la tabla de símbolos, e imprimimos el contenido inicial de la tabla.
    inicializarDobleBuffer();
    inicializarTabla();
    imprimirTabla();

    //Iniciamos el análisis léxico.
    iniciarAnalisis();

    //Imprimimos el contenido inicial de la tabla, y destruimos el buffer y la tabla.
    imprimirTabla();
    eliminarTabla();
    destruirDobleBuffer();
}

