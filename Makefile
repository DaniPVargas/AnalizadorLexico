#############################################
CC=gcc
CFLAGS=-Wall -g
RM = rm
#############################################

main: main.c analizadorLexico.o analizadorSintactico.o sistemaEntrada.o tablaSimbolos.o abb.o gestionErrores.o
	$(CC) $(CFLAGS) -o analizadorLexico main.c analizadorLexico.o analizadorSintactico.o sistemaEntrada.o tablaSimbolos.o abb.o gestionErrores.o
	
analizadorLexico.o: analizadorLexico.c analizadorLexico.h
	$(CC) $(CFLAGS) -c analizadorLexico.c

analizadorSintactico.o: analizadorSintactico.c analizadorSintactico.h
	$(CC) $(CFLAGS) -c analizadorSintactico.c

sistemaEntrada.o: sistemaEntrada.c sistemaEntrada.h
	$(CC) $(CFLAGS) -c sistemaEntrada.c

tablaSimbolos.o: tablaSimbolos.c tablaSimbolos.h
	$(CC) $(CFLAGS) -c tablaSimbolos.c

abb.o: abb.c abb.h
	$(CC) $(CFLAGS) -c abb.c

gestionErrores.o: gestionErrores.c gestionErrores.h
	$(CC) $(CFLAGS) -c gestionErrores.c


clean:
	$(RM) -r *.o
