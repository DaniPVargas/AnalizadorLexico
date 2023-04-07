#include <stdio.h>
#include "abb.h"
#include "definiciones.h"

typedef abb tablaSimbolos;


/**
 * Función que inicializa la tabla de símbolos.
 * Introduce todas las palabras reservadas para el sistema.
 * */
void inicializarTabla();

/**
 * Función que inserta un elemento en la tabla.
 * @param elemento elemento a introducir en la tabla.
 * */
void insertarElemento(contenedor elemento);

/**
 * Función que busca un elemento en la tabla, devolviendo el componente asociado al lexema.
 * @param clave que buscar
 * @return -1 si la clave no esta en la tabla, el componente correspondiente si lo está
 * */
int buscarElemento(char *clave);

/**
 * Función que elimina la tabla de símbolos liberando la memoria.
 * */
void eliminarTabla();

/**
 * Función que imprime el contenido de la tabla de símbolos.
 * */
void imprimirTabla();
