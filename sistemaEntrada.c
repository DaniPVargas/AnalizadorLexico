#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sistemaEntrada.h"

/* Utilizamos un único buffer fisico, pero lo consideraremos como 2 buffers lógicos. Para ello necesitaremos
 * utilizar aritmética de punteros, aunque intentaremos encapsular esas operaciones en las funciones privadas.*/

char bufferDoble[2 * TAM_BUFFER + 2];
int delantero = -1;
int inicio = 0;
int seHaDevueltoCaracter = 0;
int numCaracteresLeidos = 0;
FILE *f;

//-----------Declaracion de las funciones privadas que se utilizan en las diferentes funciones-----------

//Función que incrementa la posición del puntero delantero, llamada desde siguienteCaracter()
void incrementarDelantero();

//Función que carga el siguiente bloque en el doble buffer correspondiente. Se llama desde siguienteCaracter()
void cargarBloque();

//Funcion que, dado un tamaño de lexema, mueve el puntero de inicio la cantidad correspondiente.
//Se utiliza tras terminar de leer un lexema, dentro de copiarLexema().
void incrementarInicio();

int punteroEnBufferA(int puntero);

int punteroEnBufferB(int puntero);

int tamLexema();

//------------------------- FUNCIONES PUBLICAS -------------------------

/**
 * Funcion que inicializa el buffer A, que es el primero en usarse.
 */
void inicializarDobleBuffer() {
    //Abrimos el fichero concurrentSum.go
    f = fopen(RUTAFICHERO, "r");
    //Comprobamos que la apertura sea correcta.
    if (f == NULL) {
        perror("Error:\n");
        exit(1);
    }
    //Colocamos los EOF al final de cada buffer logico para implementar el sistema de doble centinela.
    bufferDoble[TAM_BUFFER] = EOF;
    bufferDoble[2 * TAM_BUFFER + 1] = EOF;
    //Cargamos el bloque A
    cargarBloque();
}

/**
 * Funcion que devuelve el siguiente caracter del doble buffer.
 * */
char siguienteCaracter() {
    if (delantero == -1) {
        incrementarDelantero();
        numCaracteresLeidos++;
        return bufferDoble[delantero];
    } else {
        incrementarDelantero();
        //Comprobamos si el caracter al que apunta delantero es EOF. Si lo es, tocará hacer las comprobaciones:
        if (bufferDoble[delantero] == EOF) {
            //Si es el fin del fichero, mandamos EOF.
            //Puede darse el caso de cargar el ultimo bloque o parte de bloque del codigo, y que al devolver
            //caracter, se vuelva a un EOF centinela, y por tanto, si no ponemos la segunda comprobación, el
            //sistema dejaría de mandar caracteres
            if (feof(f) && !seHaDevueltoCaracter) {
                return EOF;
            } else {
                //Si no lo es, es el fin de un bloque. Cargamos el siguiente bloque.
                //Si se ha marcado el flag de seHaDevueltoCaracter, no se carga el bloque y se desactiva el flag.
                if (seHaDevueltoCaracter != 1) {
                    cargarBloque();
                } else {
                    //Si estaba marcado, no cargamos el bloque y bajamos el flag
                    seHaDevueltoCaracter = 0;
                }
                //Incrementamos el puntero delantero para que apunte al inicio del siguiente bloque.
                incrementarDelantero();
            }
        }
        numCaracteresLeidos++;
        return bufferDoble[delantero];
    }
}

/**
 * Funcion que copia el lexema desde el buffer al destino que recibe como parámetro.
 * IMPORTANTE: La cadena destino tiene que tener reservada la memoria antes de ser pasada como parámetro
 * Devuelve 0 si funciona, 1 si se supera el tamaño máximo de buffer.
 */

int copiarLexema(char **destino) {
    int contenidoEnBufferA, contenidoEnBufferB;
    int tam = tamLexema();

    //Si el lexema es demasiado grande, se informa del error y se devuelve 1 (ERROR).
    if (numCaracteresLeidos > TAM_BUFFER) {
        //InformarDelError
        numCaracteresLeidos = 0;
        *destino = strdup("ERROR_TAM_LEXEMA");
        return 1;
    }

    //Si no es demasiado grande:
    *destino = malloc((tam + 1)*sizeof(char));

    //Si el lexema esta en un unico buffer
    if ((punteroEnBufferA(inicio) && punteroEnBufferA(inicio + tam - 1)) ||
        (punteroEnBufferB(inicio) && punteroEnBufferB(inicio + tam - 1))) {
        strncpy(*destino, &(bufferDoble[inicio]), tam);
    } else {
        //Si empieza en el buffer A y acaba en el B:
        if (punteroEnBufferA(inicio)) {
            //Calculamos cuantos caracteres hay en cada buffer
            contenidoEnBufferA = TAM_BUFFER - inicio;
            contenidoEnBufferB = delantero - TAM_BUFFER;
            //Se copia el lexema en dos pasos
            strncpy(*destino, &(bufferDoble[inicio]), contenidoEnBufferA);
            strncpy((*destino) + contenidoEnBufferA, &(bufferDoble[TAM_BUFFER + 1]), contenidoEnBufferB);
        } else {
            //Si empieza en el buffer B y acaba en el A
            contenidoEnBufferB = 2 * TAM_BUFFER + 1 - inicio;
            contenidoEnBufferA = delantero + 1;
            //Se copia el lexema en 2 pasos
            strncpy(*destino, &(bufferDoble[inicio]), contenidoEnBufferB);
            strncpy(*(destino) + contenidoEnBufferB, bufferDoble, contenidoEnBufferA);
        }
    }
    *(*destino + tam) = '\0';
    incrementarInicio();
    numCaracteresLeidos = 0;
    return 0;
}

/**
 * Función que actualiza los punteros cuando el analizador lexico "devuelve" un caracter.
 * Cuando al retroceder nos ponemos en la posición anterior a un EOF centinela, hay que marcarlo,
 * para que al leer ese EOF centinela no se cargue un bloque, sobreescribiendo el ya cargado.
 */

void devolverCaracter() {
    if (delantero > 0) {
        if (delantero == TAM_BUFFER + 1) {
            delantero = TAM_BUFFER - 1;
            seHaDevueltoCaracter = 1;
        } else {
            delantero = delantero - 1;
        }
    } else {
        delantero = 2 * TAM_BUFFER;
        seHaDevueltoCaracter = 1;
    }
    numCaracteresLeidos--;
}

/**
 * Función que incrementa el puntero inicio tras comentario, o una sucesión de espacios en blanco, saltos de lineo...
 */
void incrementarInicioEspaciosYComentarios() {
    //Al contrario de la función IncrementarInicio, como esta se llama después de leer algo "no vacío", el lexema
    //comenzará ahí, por lo que ponemos inicio justo donde esté delantero.
    inicio = delantero;
    //Además, como estamos en el primer caracter del nuevo lexema, el numero de caracteres leidos ya es 1.
    numCaracteresLeidos = 1;
}

/**
 * Función que destruye el bufferDoble. Como es estático, lo único que hace es cerrar el fichero de lectura.
 */
void destruirDobleBuffer() {
    fclose(f);
}

//------------------------- FUNCIONES PRIVADAS -------------------------

/**
 * Función que indica si un puntero está en el buffer A
 * Recibe el puntero del que queremos conocer la posición.
 * Devuelve 1 si está, y 0 si no.
 */
int punteroEnBufferA(int puntero) {
    return puntero < TAM_BUFFER;
}

/**
 * Función análoga a la anterior para el buffer B
 */
int punteroEnBufferB(int puntero) {
    return puntero > TAM_BUFFER && puntero < 2 * TAM_BUFFER + 1;
}

/**
 * Función que carga el bloque correspondiente, según la posición de delantero, utilizando la función fread.
 */
void cargarBloque() {
    int bytesLeidos;
    //Si estamos al final del primer buffer, cargamos en el buffer B
    if (delantero == TAM_BUFFER) {
        //fread devuelve el numero de bloque cargados.
        //El segundo parámetro indica el tamaño de bloque que carga, y el tercero el numero de bloques maximo que cargar.
        bytesLeidos = (int) fread(&(bufferDoble[TAM_BUFFER + 1]), sizeof(char), TAM_BUFFER, f);
        //Si no se carga el bloque entero, es que se acabaron los caracteres. Como fread no copia el EOF, lo colocamos manualmente.
       if (bytesLeidos < TAM_BUFFER) {
            bufferDoble[TAM_BUFFER + 1 + bytesLeidos] = EOF;
        }
    }
    //Si estamos al final del segundo buffer o al inicio del primer buffer (esto solo ocurre cuando llamamos la función en inicializarDobleBuffer())
    //cargamos en el buffer A.
    if (delantero == 2 * TAM_BUFFER + 1 || delantero == -1) {
        bytesLeidos = (int) fread(bufferDoble, sizeof(char), TAM_BUFFER, f);
        //La misma comprobacion que en el otro caso.
        if (bytesLeidos < TAM_BUFFER) {
            bufferDoble[bytesLeidos] = EOF;
        }
    }
}

/**
 * Función que incrementa la posición del puntero delantero
 */

void incrementarDelantero() {
    //Si no está al final del buffer B, lo aumenta en una unidad. Si lo está, lo pone a 0.
    if (delantero < 2 * TAM_BUFFER + 1) {
        delantero++;
    } else {
        delantero = 0;
    }
}

/**
 * Función que incrementa la posición del puntero inicio.
 */

void incrementarInicio() {
    //Esta función se llama después de leer un lexema. Como el sistema avanza y lee, si igualamos inicio y delantero
    //nos colocamos un caracter antes de que el lexema empiece. Por tanto, se coloca inicio una posición por delante
    //de delantero.
    if (delantero != TAM_BUFFER - 1 && delantero != 2 * TAM_BUFFER) {
        inicio = delantero + 1;
    } else {
        //Cuando delantero está  al final de un buffer, inicio se coloca al comienzo del siguiente.
        if (delantero == TAM_BUFFER - 1) {
            inicio = TAM_BUFFER + 1;
        } else {
            inicio = 0;
        }
    }
}

/**
 * Función privada que calcula el tamaño del lexema según las posiciones de inicio y delantero.
 */

int tamLexema() {
    //Si ambos punteros están en el mismo buffer, simplemente devolvemos la diferencia más 1. Si además están
    //en la misma posición, devolvemos 1.
    if (delantero == inicio) {
        return 1;
    }
    if ((punteroEnBufferA(delantero) && punteroEnBufferA(inicio)) ||
        (punteroEnBufferB(delantero) && punteroEnBufferB(inicio))) {
        return delantero + 1 - inicio;

    } else {
        //Si inicio está en el buffer A y delantero en el buffer B, devolvemos la diferencia. No hay que sumar 1 ya que hay una posición entre ellos
        //correspondiente al EOF del buffer A.
        if (punteroEnBufferA(inicio) && punteroEnBufferB(delantero)) {
            return delantero - inicio;
        }
            //Si inicio está en el buffer B y delantero en el buffer A, devolvemos la suma de:
            //- los caracteres en el buffer B: (2*TAM_BUFFER + 1) - inicio)
            //- los caracteres en el buffer A: delantero + 1
        else {
            return ((2 * TAM_BUFFER + 1) - inicio) + (delantero + 1);
        }
    }
}
