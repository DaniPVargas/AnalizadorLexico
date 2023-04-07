#include <stdio.h>
#include <stdlib.h>

#define TAM_BUFFER 40
#define RUTAFICHERO "C:\\Users\\danPV\\OneDrive\\Escritorio\\Universidad\\Compiladores\\Practicas\\AnalizadorLexico\\concurrentSum.go"

/**
 * Funcion que inicializa el buffer A, que es el primero en usarse.
 */
void inicializarDobleBuffer();

/**
 * Funcion que devuelve el siguiente caracter del doble buffer.
 * @return el caracter correspondiente
 * */
char siguienteCaracter();

/**
 * Funcion que copia el lexema desde el buffer al destino que recibe como parámetro.
 * IMPORTANTE: La cadena destino tiene que tener reservada la memoria antes de ser pasada como parámetro
 * @param destino puntero a la cadena que almacenara el lexema.
 * @param tamLexema tamaño del lexema que se va a copiar.
 * @return 1 si se supera el tamaño máximo de lexema, 0 si no
 */
int copiarLexema(char **destino);

/**
 * Función que actualiza los punteros cuando el analizador lexico "devuelve" un caracter.
 */
void devolverCaracter();


/**
 * Función que incrementa el puntero inicio tras comentario, o una sucesión de espacios en blanco, saltos de lineo...
 */
void incrementarInicioEspaciosYComentarios();

/**
 * Función que destruye el bufferDoble. Como es estático, lo único que hace es cerrar el fichero de lectura.
 */
void destruirDobleBuffer();
