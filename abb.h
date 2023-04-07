#ifndef ABB_H
#define ABB_H

#include "definiciones.h"

///------------------ Definición de los tipos ------------------
typedef char *tipoclave;

typedef contenedor tipoelem;

typedef struct celda *abb;

///------------------------- Funciones -------------------------

/**
 * Crea el arbol vacio.
 * @param A Puntero al arbol. Debe estar inicializado.
 */
void crear(abb *A);

/**
 * Destruye el arbol recursivamente
 * @param A El arbol que queremos destruir
 */
void destruir(abb *A);

/**
 * Comprueba si el arbol esta vacio
 * @param A El arbol binario
 */
unsigned es_vacio(abb A);

/**
 * Devuelve el subarbol izquierdo de A
 * @param A - Arbol original
 */
abb izq(abb A);

/**
 * Devuelve el subarbol derecho de A
 * @param A - Arbol original
 */
abb der(abb A);

/**
 * Recupera la informacion de la celda de la raiz del arbol
 * @param A
 * @param E Puntero al nodo que apuntara a la informacion
 */
void leer(abb A, tipoelem *E);

/**
 * Comprueba si el elemento con clave <cl> existe en el arbol <A>
 * @param A
 * @param cl
 */
unsigned es_miembro(abb A, tipoelem E);

/**
 * Función para informar si una clave está en el árbol
 */
unsigned es_miembro_clave(abb A, tipoclave cl);

/**
 * Busca un nodo con clave <cl> en el arbol y, si existe
 * modifica el puntero <nodo> para que lo apunte.
 * @param A Arbol binario en el que se busca el nodo.
 * @param cl Clave que se buscara.
 * @param nodo Puntero a un tipoelem.
 */
void buscar_nodo(abb A, tipoclave cl, tipoelem *nodo);

/**
 * Inserta un nuevo nodo en el arbol para el elemento E
 * del que toma su clave. Esta clave no debe existir en
 * el arbol.
 * @param A Arbol binario
 * @param E Informacion del nuevo nodo.
 */
void insertar(abb *A, tipoelem E);


#endif /* ABB_H */
