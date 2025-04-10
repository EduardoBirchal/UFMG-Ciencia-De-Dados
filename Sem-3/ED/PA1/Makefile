CC = gcc
CFLAGS = -Wall

DIR_SRC = src
DIR_OBJ = obj
DIR_BIN = bin

all: main

main: $(DIR_OBJ)/pa1.o
	$(CC) $(CFLAGS) $(DIR_OBJ)/pa1.o -o $(DIR_BIN)/pa1.out

$(DIR_OBJ)/pa1.o: $(DIR_SRC)/pa1.c
	$(CC) $(CFLAGS) -c $(DIR_SRC)/pa1.c -o $(DIR_OBJ)/pa1.o

clean:
	rm -f $(DIR_OBJ)/pa1.o $(DIR_BIN)/pa1.out       