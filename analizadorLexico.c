#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "analizadorLexico.h"
#include "sistemaEntrada.h"
#include "tablaSimbolos.h"
#include "definiciones.h"
#include "gestionErrores.h"

#define ESTADO_FINAL 100

int casoPuntoYComa = 0;

///----------------- Declaración previa de funciones privadas -----------------

void avanzarComentario(char aux);

void automataIdentificador();

void automataOperador(char siguiente, contenedor *c);

//Estas funciones devuelven un int, ya que a partir de dicho valor se controla si ha habido algún error
//en la construcción de alguno de esos componentes
int automataString();

int automataNumeros(char aux, contenedor *c);

int automataHexadecimal(contenedor *c);

///------------------------ Funciones públicas ------------------------

/**
 * Función que devuelve el par (siguiente componente léxico-lexema)
 */
void siguienteComponenteLexico(contenedor *c) {
    int busqueda;
    char sig = siguienteCaracter(), aux;

    //Si es fin de fichero, devolvemos el contenedor con el lexema vacío y el componente
    //con el código correspondiente.
    if (sig == EOF) {
        c->componente = FIN_FICHERO;
        copiarLexema(&(c->lexema));
        return;
    }


    /// ------------- SALTAR COMENTARIOS Y ESPACIOS EN BLANCO -------------
    //Mientras sea un caracter de espacio, o un inicio de comentario, vamos a avanzar
    //sin hacer nada hasta encontrar un caracter "no vacio"

    while (sig == '\n' || sig == ' ' || sig == '/' || sig == '\r' || sig == '\t') {

        //Si es un '\n' comprobamos si estamos en el caso de un ';' implicito
        if (sig == '\n' && casoPuntoYComa) {
            //Si lo estamos, enviamos el componente asociado a ';'
            //Pero antes, actualizamos los punteros en el buffer, para un correcto funcionamiento de las funciones del doble buffer,
            //ya que se ha podido avanzar una gran cantidad de caracteres sin actualizar los punteros, haciendo que el calculo del
            //tamaño del lexema tal como está planteado no tenga sentido.
            incrementarInicioEspaciosYComentarios();
            casoPuntoYComa = 0;
            c->componente = (int) (';');
            c->lexema = strdup(";");
            return;
        }

        //Si es un inicio de comentario, comprobamos si es un comentario o solo una barra.
        if (sig == '/') {
            aux = siguienteCaracter();
            //Si es un comentario lo saltamos.
            if (aux == '/' || aux == '*') {
                avanzarComentario(aux);
            } else {
                //Si no, devolvemos el caracter leido de más, y se envía el componente asociado a '/'
                devolverCaracter();
                c->componente = (int) sig;
                copiarLexema(&(c->lexema));
                return;
            }
        }

        sig = siguienteCaracter();
    }
    //Actualizamos el puntero de inicio para que se coloque en el primer caracter que formará parte del lexema.
    incrementarInicioEspaciosYComentarios();


    // ------------ Comienzo del analisis de lexemas ------------

    /// ------------------ CADENAS ALFANUMERICAS -----------------
    if (isalpha(sig) || sig == '_') {
        automataIdentificador();
        //Tras leer la cadena, se devuelve el siguiente caracter, que ya no formaba parte de ella.
        devolverCaracter();

        //Comprobamos que no haya habido un error de tamaño de lexema demasiado largo.
        if (copiarLexema(&(c->lexema))) {
            c->componente = ERROR_LEXICO;
            imprimirErrorTamLexema();
            //Como es un identificador, aunque no lo leamos, es necesario leer correctamente el resto de
            //componentes, así que actualizamos el estado para la lectura de puntos y coma implicitos.
            if (!casoPuntoYComa) {
                casoPuntoYComa = 1;
            }
            return;
        }

        //Si no ha habido un error, se continua
        busqueda = buscarElemento(c->lexema);
        //Si no está, lo introducimos en la tabla.
        if (busqueda == -1) {
            c->componente = IDENTIFICADOR;
            insertarElemento(*c);
        } else {
            //Si ya estaba, le asignamos el componente que hay en la tabla.
            c->componente = busqueda;
        }

        //Actualizamos el estado de punto y coma
        if (c->componente == IDENTIFICADOR && !casoPuntoYComa) {
            casoPuntoYComa = 1;
        }

        return;
    }


    /// ------------------ STRINGS -----------------
    if (sig == '"') {
        //Comprobamos que no haya habido errores en la formación del string
        if (automataString()) {
            //Si lo hay, mandamos el contenedor con un lexema vacío y el componente con el
            //codigo asociado al error.
            c->componente = ERROR_LEXICO;
            return;
        }
        //Tras identificar el string, y ver que no hay errores, seguimos

        //Comprobamos que no haya habido un error de tamaño de lexema demasiado largo.
        if (copiarLexema(&(c->lexema))) {
            c->componente = ERROR_LEXICO;
            imprimirErrorTamLexema();
            //Como es un string, aunque no lo leamos, es necesario leer correctamente el resto de
            //componentes, así que actualizamos el estado para la lectura de puntos y coma implicitos.
            if (!casoPuntoYComa) {
                casoPuntoYComa = 1;
            }
            return;
        }

        //Si no lo ha habido:
        c->componente = STRING;

        //Actualizamos el estado para los posibles punto y coma implicitos.
        if (!casoPuntoYComa) {
            casoPuntoYComa = 1;
        }

        return;
    }


    /// ------------------ NUMEROS -----------------
    if (isdigit(sig) || sig == '.') {
        //Comprobamos que este bien formado.
        if (automataNumeros(sig, c)) {
            //Si no lo está, devolvemos el ultimo caracter, y procedemos como antes.
            devolverCaracter();
            c->componente = ERROR_LEXICO;
            return;
        }

        //Si no hay error:

        //Tras coger el numero, vemos si es o no imaginario. A partir de aquí se procede como en otros casos.
        if (siguienteCaracter() == 'i') {
            c->componente = NUM_IMAGINARIO;

            if (copiarLexema(&(c->lexema))) {
                c->componente = ERROR_LEXICO;
                imprimirErrorTamLexema();
                if (!casoPuntoYComa) {
                    casoPuntoYComa = 1;
                } else {
                    casoPuntoYComa = 0;
                }
                return;
            }

            if (!casoPuntoYComa) {
                casoPuntoYComa = 1;
            }

            return;
        } else {
            devolverCaracter();
            if (copiarLexema(&(c->lexema))) {
                c->componente = ERROR_LEXICO;
                imprimirErrorTamLexema();
                if (!casoPuntoYComa) {
                    casoPuntoYComa = 1;
                }
                return;
            }

            if (!casoPuntoYComa) {
                casoPuntoYComa = 1;
            }

            return;
        }
    }



    /// ------------------ OPERADORES -----------------
    //Comprobamos si es alguno de los operadores
    if (sig == '(' || sig == ')' || sig == '{' || sig == '}' || sig == '=' || sig == '[' || sig == ']' ||
        sig == ',' || sig == ';' || sig == '<' || sig == ':' || sig == '+' || sig == '-' || sig == '*') {
        //Los posibles operadores de más de un caracter se evaluan aquí.
        if (sig == '<' || sig == ':' || sig == '+') {
            automataOperador(sig, c);
        } else {
            //Si no, se le asigna su ASCII
            c->componente = (int) sig;
        }
        //Se copia el lexema y se devuelve el contenedor
        copiarLexema(&(c->lexema));

        //Por último, se actualiza el estado de los puntos y coma implicitos.
        if (sig == ')' || sig == '}' || sig == ']') {
            casoPuntoYComa = 1;
        } else {
            casoPuntoYComa = 0;
        }
        return;
    }
}


///----------------------------------- FUNCIONES DE LOS AUTOMATAS -----------------------------------

/**
 * NOTA: Para analizar todas estas funciones se recomienda ver los automatas en la carpeta README,
 * que explican las funciones mucho mejor que cualquier comentario, ya que son implementaciones de
 * estos automatas con una estructura de switch;
 */

void automataIdentificador() {
    int estado = 0;
    char sig;

    while(estado != ESTADO_FINAL){
        sig = siguienteCaracter();
        if (!isalnum(sig) && sig != '_') {
            estado = ESTADO_FINAL;
        }
    }
}

void automataOperador(char siguiente, contenedor *c) {
    char aux = siguienteCaracter();
    if (siguiente == '<' && aux == '-') {
        (c->componente) = FLECHA_ASIGNACION;
        return;
    } else if (siguiente == ':' && aux == '=') {
        (c->componente) = DOS_PUNTOS_IGUAL;
        return;
    } else if (siguiente == '+' && aux == '=') {
        (c->componente) = MAS_IGUAL;
        return;
    } else {
        //Si no era ninguna de esas opciones, el operador era de un solo caracter.
        //Actualizamos el componente y devolvemos el caracter leido de más.
        (c->componente) = (int) siguiente;
        devolverCaracter();
    }
}

int automataString() {
    int estado = 0;
    char siguiente;
    while (estado != ESTADO_FINAL) {
        siguiente = siguienteCaracter();
        switch (estado) {
            case 0:
                if (siguiente == '\\') {
                    estado = 1;
                }
                if (siguiente == '"') {
                    estado = ESTADO_FINAL;
                }
                break;
            case 1:
                if (siguiente != 'a' && siguiente != 'b' && siguiente != 'f' && siguiente != 'n'
                    && siguiente != 'r' && siguiente != 't' && siguiente != 'v' && siguiente != '\\'
                    && siguiente != '"' && siguiente != '\'') {
                    imprimirErrorString();
                    return 1;
                } else {
                    estado = 0;
                }
                break;
        }
    }
    return 0;
}

void avanzarComentario(char aux) {
    char siguiente;
    int estado = 0;
    if (aux == '/') {
        siguiente = siguienteCaracter();
        while (siguiente != '\n') {
            siguiente = siguienteCaracter();
        }
    }

    if (aux == '*') {
        siguiente = siguienteCaracter();
        while (estado != ESTADO_FINAL) {
            switch (estado) {
                case 0:
                    if (siguiente == '*') {
                        estado = 1;
                    }
                    break;
                case 1:
                    if (siguiente == '/') {
                        estado = ESTADO_FINAL;
                    } else {
                        estado = 0;
                    }
                    break;
            }
            siguiente = siguienteCaracter();
        }
    }
}

int automataNumeros(char aux, contenedor *c) {
    int estado;
    char siguiente;

    if (aux == '0') {
        estado = 1;
    } else {
        if (isdigit(aux)) {
            estado = 2;
        } else {
            estado = 3;
        }
    }

    //Una vez conocido el estado en el que estamos, comienza el automata.
    while (estado != ESTADO_FINAL) {
        siguiente = siguienteCaracter();
        switch (estado) {
            case 1:
                if (isdigit(siguiente)) {
                    //Empezaria un numero octal, pero como no hay en el codigo, ignoramos este caso
                } else {
                    if (siguiente == 'x' || siguiente == 'X') {
                        if (automataHexadecimal(c)) {
                            imprimirErrorHexaMalFormado();
                            return 1;
                        }
                        estado = ESTADO_FINAL;
                    } else {
                        estado = ESTADO_FINAL;
                        (c->componente) = NUM_ENTERO;
                    }
                }
                break;
            case 2:
                if (!isdigit(siguiente)) {
                    if (siguiente == '_') {
                        estado = 4;
                    } else {
                        if (siguiente == '.') {
                            estado = 7;
                        } else {
                            if (siguiente == 'e' || siguiente == 'E') {
                                estado = 6;
                            } else {
                                estado = ESTADO_FINAL;
                                (c->componente) = NUM_ENTERO;
                            }
                        }
                    }
                }
                break;
            case 3:
                if (!isdigit(siguiente)) {
                    estado = ESTADO_FINAL;
                    (c->componente) = (int) aux;
                } else {
                    estado = 5;
                }
                break;
            case 4:
                if (!isdigit(siguiente)) {
                    imprimirErrorEnteroMalFormado();
                    return 1;
                } else {
                    estado = 2;
                }
                break;
            case 5:
                if (!isdigit(siguiente)) {
                    if (siguiente == 'e' || siguiente == 'E') {
                        estado = 6;
                    } else {
                        estado = ESTADO_FINAL;
                        (c->componente) = NUM_FLOTANTE;
                    }
                }
                break;
            case 6:
                if (isdigit(siguiente)) {
                    estado = 9;
                } else {
                    if (siguiente == '+' || siguiente == '-') {
                        estado = 8;
                    } else {
                        imprimirErrorFloatMalFormado();
                        return 1;
                    }
                }
                break;
            case 7:
                if (!isdigit(siguiente)) {
                    if (siguiente == 'e' || siguiente == 'E') {
                        estado = 6;
                    } else {
                        estado = ESTADO_FINAL;
                        (c->componente) = NUM_FLOTANTE;
                    }
                }
                break;
            case 8:
                if (isdigit(siguiente)) {
                    estado = 9;
                } else {
                    imprimirErrorFloatMalFormado();
                    return 1;
                }
                break;
            case 9:
                if (!isdigit(siguiente)) {
                    estado = ESTADO_FINAL;
                    (c->componente) = NUM_FLOTANTE;
                }
        }
    }
    //Como todos los estados finales de este autómata, incluidos los del automataHexadecimal leen un caracter
    //de más, lo devolvemos.
    devolverCaracter();
    return 0;
}

int automataHexadecimal(contenedor *c) {
    int estado = 0;
    char siguiente;
    while (estado != ESTADO_FINAL) {
        siguiente = siguienteCaracter();
        switch (estado) {
            case 0:
                if (isxdigit(siguiente)) {
                    estado = 1;
                } else {
                    if (siguiente == '_') {
                        estado = 2;
                    } else {
                        imprimirErrorHexaMalFormado();
                        return 1;
                    }
                }
                break;
            case 1:
                if (!isxdigit(siguiente)) {
                    if (siguiente == '_') {
                        estado = 2;
                    } else {
                        estado = ESTADO_FINAL;
                        (c->componente) = NUM_HEXADECIMAL;
                    }
                }
                break;
            case 2:
                if (!isxdigit(siguiente)) {
                    imprimirErrorHexaMalFormado();
                    return 1;
                } else {
                    estado = 1;
                }
        }
    }

    //Si se llega al final es que no hubo error, por lo que se devuelve un 0.
    return 0;
}
