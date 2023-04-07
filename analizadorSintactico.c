#include <stdio.h>
#include <stdlib.h>
#include "analizadorLexico.h"
#include "analizadorSintactico.h"
#include "sistemaEntrada.h"


void imprimirComponenteLexico(contenedor c);

/**
 * Función que inicia el análisis léxico, pidiendo componentes léxicos al analizador
 * léxico hasta que este devuelva el componente asociado a EOF.
 */
void iniciarAnalisis() {
    contenedor c;
    //Hacer funcion de sistemaEntrada

    do { //Pedimos componentes lexicos al analizadorLexico hasta que reciba EOF.
        siguienteComponenteLexico(&c);
        //Si ha habido algún error, o es el fin de fichero no se imprime el componente.
        if (c.componente != FIN_FICHERO && c.componente != ERROR_LEXICO) {
            imprimirComponenteLexico(c);
        }
        //Cuando reciba el fin de fichero, la función termina.
        free(c.lexema);
    } while (c.componente != FIN_FICHERO);


}

/**
 * Función privada que imprime los pares <lexema, componente> que recibe el analizador sintáctico.
 */
void imprimirComponenteLexico(contenedor c) {
    printf("<%d, %s>\n", c.componente, c.lexema);
}
