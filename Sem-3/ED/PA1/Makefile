CC = g++
CFLAGS = -Wall

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

all: main

main: $(OBJ_DIR)/pa1.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/pa1.o -o $(BIN_DIR)/pa1.out

$(OBJ_DIR)/pa1.o: $(SRC_DIR)/pa1.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/pa1.c -o $(OBJ_DIR)/pa1.o

clean:
	rm -f $(OBJ_DIR)/pa1.o $(BIN_DIR)/pa1.out       
