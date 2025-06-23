OBJ_DIR = obj
INCLUDE_DIR = include
BIN_DIR = bin
SRC_DIR = src

CFLAGS = -I $(INCLUDE_DIR)


all: output

output: $(OBJ_DIR)/pacote.o $(OBJ_DIR)/armazem.o $(OBJ_DIR)/main.o
	g++ -o $(BIN_DIR)/tp2.out $(OBJ_DIR)/pacote.o $(OBJ_DIR)/armazem.o $(OBJ_DIR)/main.o

$(OBJ_DIR)/main.o: $(SRC_DIR)/temp.cpp $(INCLUDE_DIR)/linkedList.hpp $(INCLUDE_DIR)/stack.hpp | $(OBJ_DIR)
	g++ $(CFLAGS) -c $(SRC_DIR)/temp.cpp -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/pacote.o: $(SRC_DIR)/pacote.cpp $(INCLUDE_DIR)/pacote.hpp | $(OBJ_DIR)
	g++ $(CFLAGS) -c $(SRC_DIR)/pacote.cpp -o $(OBJ_DIR)/pacote.o

$(OBJ_DIR)/armazem.o: $(SRC_DIR)/armazem.cpp $(INCLUDE_DIR)/armazem.hpp | $(OBJ_DIR)
	g++ $(CFLAGS) -c $(SRC_DIR)/armazem.cpp -o $(OBJ_DIR)/armazem.o