OBJ_DIR = obj
INCLUDE_DIR = include
BIN_DIR = bin
SRC_DIR = src

# Flags do compilador
CXX = g++
CXXFLAGS = -I$(INCLUDE_DIR) -std=c++11 -Wall -Wextra

# Lista de arquivos objeto - ADICIONADO estatisticas.o
OBJECTS = $(OBJ_DIR)/pacote.o $(OBJ_DIR)/armazem.o $(OBJ_DIR)/rede.o $(OBJ_DIR)/main.o $(OBJ_DIR)/escalonador.o $(OBJ_DIR)/evento.o $(OBJ_DIR)/estatisticas.o

# Nome do executável final
EXECUTABLE = $(BIN_DIR)/tp2

# Regra padrão
all: $(EXECUTABLE)

# Regra de linkagem final
$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# --- Regras de Compilação ---

# ADICIONADO estatisticas.hpp como dependência
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/armazem.hpp $(INCLUDE_DIR)/pacote.hpp $(INCLUDE_DIR)/rede.hpp $(INCLUDE_DIR)/evento.hpp $(INCLUDE_DIR)/escalonador.hpp $(INCLUDE_DIR)/estatisticas.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $@

$(OBJ_DIR)/escalonador.o: $(SRC_DIR)/escalonador.cpp $(INCLUDE_DIR)/escalonador.hpp $(INCLUDE_DIR)/minheap.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/escalonador.cpp -o $@

# ADICIONADO estatisticas.hpp como dependência
$(OBJ_DIR)/evento.o: $(SRC_DIR)/evento.cpp $(INCLUDE_DIR)/evento.hpp $(INCLUDE_DIR)/estatisticas.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/evento.cpp -o $@

$(OBJ_DIR)/rede.o: $(SRC_DIR)/rede.cpp $(INCLUDE_DIR)/rede.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/rede.cpp -o $@

$(OBJ_DIR)/pacote.o: $(SRC_DIR)/pacote.cpp $(INCLUDE_DIR)/pacote.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/pacote.cpp -o $@

# ADICIONADO estatisticas.hpp como dependência
$(OBJ_DIR)/armazem.o: $(SRC_DIR)/armazem.cpp $(INCLUDE_DIR)/armazem.hpp $(INCLUDE_DIR)/stack.hpp $(INCLUDE_DIR)/linkedList.hpp $(INCLUDE_DIR)/estatisticas.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/armazem.cpp -o $@

# NOVA REGRA para compilar estatisticas.cpp
$(OBJ_DIR)/estatisticas.o: $(SRC_DIR)/estatisticas.cpp $(INCLUDE_DIR)/estatisticas.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/estatisticas.cpp -o $@

# --- Regras Auxiliares ---

# Cria os diretórios se não existirem
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Limpa os arquivos compilados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Declara alvos que não são arquivos
.PHONY: all clean
