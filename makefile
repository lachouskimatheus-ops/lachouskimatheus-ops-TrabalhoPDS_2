# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I./include -I./dependencias

# Diretórios
SRC_DIR = src
OBJ_DIR = obj

# Encontra os arquivos .cpp (Tudo na mesma linha para evitar erro de espaço fantasma)
SOURCES = $(wildcard $(SRC_DIR)/core/*.cpp) $(wildcard $(SRC_DIR)/rede/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)

# Transforma a lista de .cpp em uma lista de .o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Nome do executável final
TARGET = servidor_fdp

# Regra principal de compilação
all: $(TARGET)

$(TARGET): $(OBJECTS)
    $(CXX) $(CXXFLAGS) -o $@ $^
    @echo "Servidor compilado com sucesso! Rode com ./$(TARGET)"

# Regra para compilar os arquivos objeto (.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
    @mkdir -p $(dir $@)
    $(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados
clean:
    rm -rf $(OBJ_DIR) $(TARGET)
    @echo "Arquivos de compilação limpos."