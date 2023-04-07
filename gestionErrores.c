#include <stdio.h>
#include "definiciones.h"
#include "sistemaEntrada.h"

void imprimirErrorTamLexema() {
    printf("ERROR: El lexema supera el tamano maximo. El tamano maximo permitido es %d.\n", TAM_BUFFER);
}

void imprimirErrorEnteroMalFormado() {
    printf("ERROR: Hay un numero entero mal formado.\n");
}

void imprimirErrorFloatMalFormado() {
    printf("ERROR: Hay un numero flotante mal formado.\n");
}

void imprimirErrorHexaMalFormado() {
    printf("ERROR: Hay un numero hexadecimal mal formado.\n");
}

void imprimirErrorString() {
    printf("ERROR: Hay un string con caracteres no permitidos después de \\.\n");
}
