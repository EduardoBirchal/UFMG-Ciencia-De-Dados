# --- Variáveis de Configuração ---
# Diretórios
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include

# Compilador e Flags
# -I$(INCLUDE_DIR) informa ao compilador onde procurar pelos arquivos de cabeçalho (.hpp)
CXX = g++
CXXFLAGS = -I$(INCLUDE_DIR) -std=c++11 -Wall -Wextra -g

# --- Arquivos do Projeto ---
# Lista de todos os nomes de arquivos fonte .cpp.
# Não é necessário incluir o caminho 'src/' aqui.
SOURCES = main.cpp \
          controlador.cpp \
          evento.cpp \
          cliente.cpp \
          pacote.cpp

# Gera a lista de arquivos objeto (.o) a partir dos fontes, colocando-os no OBJ_DIR.
# Não é necessário modificar esta linha.
OBJECTS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Nome do executável final
EXECUTABLE = $(BIN_DIR)/tp3.out

# --- Regras Principais ---

# Regra padrão: é executada quando 'make' é chamado sem argumentos.
all: $(EXECUTABLE)

# Regra para linkar os objetos e criar o executável final.
$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	@echo "Linkando para criar o executável..."
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)
	@echo "Executável '$(EXECUTABLE)' criado com sucesso."

# --- Regra de Compilação Genérica ---

# Regra para compilar qualquer arquivo .cpp da pasta src em um .o na pasta obj.
# O make encontrará o fonte correspondente em $(SRC_DIR).
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras de Dependência ---
# Especifica de quais headers (na pasta include) cada objeto depende.
# Isso garante que um arquivo .cpp seja recompilado se um de seus headers mudar.
# É crucial para a correção do processo de compilação.

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/controlador.hpp
$(OBJ_DIR)/controlador.o: $(SRC_DIR)/controlador.cpp $(INCLUDE_DIR)/controlador.hpp $(INCLUDE_DIR)/evento.hpp $(INCLUDE_DIR)/pacote.hpp $(INCLUDE_DIR)/cliente.hpp $(INCLUDE_DIR)/avlTree.hpp
$(OBJ_DIR)/evento.o: $(SRC_DIR)/evento.cpp $(INCLUDE_DIR)/evento.hpp
$(OBJ_DIR)/cliente.o: $(SRC_DIR)/cliente.cpp $(INCLUDE_DIR)/cliente.hpp $(INCLUDE_DIR)/pacote.hpp $(INCLUDE_DIR)/linkedList.hpp
$(OBJ_DIR)/pacote.o: $(SRC_DIR)/pacote.cpp $(INCLUDE_DIR)/pacote.hpp $(INCLUDE_DIR)/evento.hpp $(INCLUDE_DIR)/linkedList.hpp

# --- Regras Auxiliares ---

# Cria os diretórios se eles não existirem.
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Limpa os arquivos compilados e o executável.
clean:
	@echo "Limpando arquivos compilados..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpeza concluída."

# Declara alvos que não são nomes de arquivos.
.PHONY: all clean