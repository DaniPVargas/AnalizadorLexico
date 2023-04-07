#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tablaSimbolos.h"
#include "abb.h"
#include "definiciones.h"

tablaSimbolos T;

//Función privada que nos indica si una clave está en la tabla.
int esElemento(char *clave);

//Función privada que recorre la tabla para imprimir su contenido.
void inorden(tablaSimbolos T);

contenedor copiaElemento(contenedor elemento);

/**
 * Función que inicializa la tabla de símbolos.
 * Introduce todas las palabras reservadas para el sistema.
 * */
void inicializarTabla() {
    int i;
    contenedor c;

    //Inicializamos de manera manual el vector con las palabras reservadas
    char *keywords[8];
    keywords[0] = "chan";
    keywords[1] = "for";
    keywords[2] = "func";
    keywords[3] = "go";
    keywords[4] = "import";
    keywords[5] = "package";
    keywords[6] = "range";
    keywords[7] = "var";

    //Se crear la tabla de símbolos (Árbol binario de búsqueda)
    crear(&T);

    //Para cada palabra del vector de palabras reservadas, se crea un contenedor con el lexema
    //y el componente correspondientes y se inserta en la tabla.
    for (i = 0; i < 8; i++) {
        //Reservamos espacio para dicha variable y copiamos en él la cadena
        c.lexema = (char *) malloc((strlen(keywords[i]) + 1) * sizeof(char));
        strcpy(c.lexema, keywords[i]);
        c.componente = 200 + i;
        //Insertamos el elemento
        insertar(&T, (tipoelem) c);
    }
}

/**
 * Función que inserta un elemento en la tabla.
 * Recibe un elemento y una tabla de símbolos.
 * */
void insertarElemento(contenedor elemento) {
    //Se inserta una copia del elemento recibido con la función insertar de abb.h
    contenedor aux = copiaElemento(elemento);
    insertar(&T, (tipoelem) aux);
}

/**
 * Función que busca un elemento en la tabla, devolviendo el componente asociado al lexema.
 * Recibe una clave y una tabla de símbolos.
 * Si está en la tabla, devuelve el componente asociado, si no, devuelve -1.
 * */
int buscarElemento(char *clave) {
    tipoelem e;
    //Si el elemento está en la tabla, devuelve el componente.
    if (es_miembro_clave(T, clave) == 1) {
        //Se buscar el elemento recibido con la función insertar de abb.h
        buscar_nodo(T, clave, &e);
        //Se devuelve el componente asociado al lexema.
        return e.componente;
    } else {
        return -1;
    }

}

/**
 * Función que elimina la tabla de símbolos liberando la memoria.
 * Recibe la tabla de símbolos a eliminar.
 * */
void eliminarTabla() {
    //Se utiliza la función destruir de abb.h
    destruir(&T);
}

/**
 * Función recursiva que imprime la tabla de símbolos de izquierda a derecha.
 * */
void imprimirTabla() {
    //Esta función recorre la tabla de izquierda a derecha, imprimiendo los elementos.
    //Al necesitar T como parámetro (para comprobar si está vacía), creo una función privada
    //que lo hace, ya que las funciones de tablaSimbolos no reciben la tabla como parámetro.
    printf("------------Tabla de simbolos------------\n");
    inorden(T);
    printf("-----------------------------------------\n");
}

////////////////// Funciones privadas /////////////////

/**
 * Función privada que recorre la tabla de manera recursiva, imprimiendo los elementos.
 * Recibe una clave y una tabla de símbolos.
 * Devuelve 1 si está y 0 si no no lo está.
 * */
void inorden(tablaSimbolos T) {
    tipoelem E;
    //Se comprueba si es vacío. Si no lo es, recorre el subarbol derecho e izquierdo de manera recursiva.
    //Imprimiendo los elementos de la tabla.
    if (!es_vacio(T)) {
        inorden(izq(T));
        leer(T, &E);
        printf("<%d,\"%s\">\n", E.componente, E.lexema);
        inorden(der(T));
    }
}

/**
 * Función privada que hace una copia del elemento, reservando memoria, y que será la que se almacene en la tabla.
 * */
contenedor copiaElemento(contenedor elemento){
    contenedor aux;
    aux.componente = elemento.componente;
    aux.lexema = (char*)malloc((strlen(elemento.lexema) + 1)*sizeof(char));
    strcpy(aux.lexema, elemento.lexema);
    return aux;
}
