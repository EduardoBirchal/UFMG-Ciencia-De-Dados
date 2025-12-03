# --- Variáveis de Configuração ---
# Diretórios
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include

# Compilador e Flags
# -I$(INCLUDE_DIR) informa ao compilador onde procurar pelos arquivos de cabeçalho (.hpp)
CXX = g++
CXXFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror

# --- Arquivos do Projeto ---
# Lista de todos os nomes de arquivos fonte .cpp.
# Não é necessário incluir o caminho 'src/' aqui.
SOURCES = 	main.cpp \
			solver.cpp \
			utils.cpp \

# Gera a lista de arquivos objeto (.o) a partir dos fontes, colocando-os no OBJ_DIR.
# Não é necessário modificar esta linha.
OBJECTS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Nome do executável final
EXECUTABLE = tp3.out
# --- Regras Principais ---

# Regra padrão: é executada quando 'make' é chamado sem argumentos.
all: $(EXECUTABLE)

# Regra para linkar os objetos e criar o executável final.
$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

# --- Regra de Compilação Genérica ---

# Regra para compilar qualquer arquivo .cpp da pasta src em um .o na pasta obj.
# O make encontrará o fonte correspondente em $(SRC_DIR).
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras de Dependência ---
# Especifica de quais headers (na pasta include) cada objeto depende.
# Isso garante que um arquivo .cpp seja recompilado se um de seus headers mudar.
# É crucial para a correção do processo de compilação.

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/solver.hpp
$(OBJ_DIR)/solver.o: $(SRC_DIR)/solver.cpp $(INCLUDE_DIR)/solver.hpp $(INCLUDE_DIR)/utils.hpp
$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(INCLUDE_DIR)/utils.hpp

# --- Regras Auxiliares ---

# Cria os diretórios se eles não existirem.
$(OBJ_DIR) $(BIN_DIR):
	mkdir $@

# Limpa os arquivos compilados e o executável.
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Declara alvos que não são nomes de arquivos.
.PHONY: all clean