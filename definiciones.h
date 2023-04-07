#ifndef DEFINICIONES_H
#define DEFINICIONES_H

typedef struct {
    char *lexema;
    int componente;
} contenedor;

#define CHAN 200
#define FOR 201
#define FUNC 202
#define GO 203
#define IMPORT 204
#define PACKAGE 205
#define RANGE 206
#define VAR 207

#define IDENTIFICADOR 220

#define MAS_IGUAL 230
#define DOS_PUNTOS_IGUAL 231
#define FLECHA_ASIGNACION 232

#define NUM_ENTERO 240
#define NUM_FLOTANTE 241
#define NUM_HEXADECIMAL 242
#define NUM_IMAGINARIO 243

#define STRING 250

#define ERROR_LEXICO 1000
#define FIN_FICHERO 4000

#endif
