#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abb.h"

///------------------ Estructuras de datos ------------------

struct celda {
    tipoelem info;
    struct celda *izq, *der;
};

///------------------------- Funciones -------------------------

/**
 * Extrae la clave de una celda
 * */
tipoclave _clave_elem(tipoelem *E) {
    return E->lexema;
}

/**
 * Función que permite abstraer la comparación de claves del resto del programa.
 * */
int _comparar_claves(tipoclave cl1, tipoclave cl2) {
    //Como son strings, usamos strcmp, que funciona de la siguiente manera.
    //Esta funcion devuelve 0 si son iguales, < 0 si cad1 < cad2 y >0 y cad1 > cad2
    return strcmp(cl1, cl2) == 0 ? 0 : strcmp(cl1, cl2) > 0 ? 1 : -1;

}

/**
 * Funcion que permite liberar la memoria de las celdas de la tabla, en particular, de la reservada para
 * el lexema.
 * */
void _destruir_elem(tipoelem *E) {
    free(E->lexema);
}

///-------------------- Operaciones de creación y destrucción --------------------

/**
 * Inicializamos el puntero a abb.
 */
void crear(abb *A) {
    *A = NULL;
}

/**
 * Destruye recursivamente los subarboles hasta liberar toda la memoria utilizada.
 */
void destruir(abb *A) {
    if (!es_vacio(*A)) {
        destruir(&((*A)->izq));
        destruir(&((*A)->der));
        _destruir_elem(&((*A)->info));
        free(*A);
        *A = NULL;
    }
}

/**
 * Indica si el arbol recibido (o subarbol) es vacío
 */
unsigned es_vacio(abb A) {
    return A == NULL;
}

/**
 * Devuelve el subarbol izquierdo.
 */
abb izq(abb A) {
    return A->izq;
}

/**
 * Devuelve el subarbol izquierdo.
 */
abb der(abb A) {
    return A->der;
}

/**
 * Función privada para comparar las claves
 */
int _comparar_clave_elem(tipoclave cl, tipoelem E) {
    return _comparar_claves(cl, _clave_elem(&E));
}

/**
 * Función para informar si una clave está en el árbol
 */
unsigned es_miembro_clave(abb A, tipoclave cl) {
    if (es_vacio(A)) {
        return 0;
    }
    int comp = _comparar_clave_elem(cl, A->info);

    if (comp == 0) { //cl == A->info
        return 1;
    }
    if (comp > 0) { //cl > A->info
        return es_miembro_clave(A->der, cl);
    }
    //cl < A->info
    return es_miembro_clave(A->izq, cl);
}

/**
 * Función que indica si un elemento está en el arbol.
 */
unsigned es_miembro(abb A, tipoelem E) {
    return es_miembro_clave(A, _clave_elem(&E));
}

/**
 * Función que busca un nodo, copiandolo en la dirección del puntero que recibe
 */
void buscar_nodo(abb A, tipoclave cl, tipoelem *nodo) {
    if (es_vacio(A)) {
        return;
    }
    int comp = _comparar_clave_elem(cl, A->info);

    if (comp == 0) { // cl == A->info
        *nodo = A->info;
    } else if (comp < 0) { // cl < A->info
        buscar_nodo(A->izq, cl, nodo);
    } else { // cl > A->info
        buscar_nodo(A->der, cl, nodo);
    }
}

/**
 * Recupera la informacion de la celda de la raiz del arbol
 */
void leer(abb A, tipoelem *E) {
    *E = A->info;
}


///-------------------- Operaciones de modificación --------------------

/**
 * Funcion recursiva para insertar un nuevo nodo en el arbol.
 * Se presupone que no existe un nodo con la misma clave en el arbol.
 **/
void insertar(abb *A, tipoelem E) {
    if (es_vacio(*A)) {
        *A = (abb) malloc(sizeof(struct celda));
        (*A)->info = E;
        (*A)->izq = NULL;
        (*A)->der = NULL;
        return;
    }
    tipoclave cl = _clave_elem(&E);
    int comp = _comparar_clave_elem(cl, (*A)->info);
    if (comp > 0) {
        insertar(&(*A)->der, E);
    } else {
        insertar(&(*A)->izq, E);
    }
}
