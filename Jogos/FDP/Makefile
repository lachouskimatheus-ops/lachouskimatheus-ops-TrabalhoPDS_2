# Compilador e Flags de compilação
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I include/

# Diretórios
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Busca todos os arquivos .cpp na pasta src automaticamente
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Transforma a lista de .cpp em uma lista de arquivos .o na pasta build
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Nome do seu jogo (o executável final)
TARGET = jogarFDP

# Regra padrão: quando você digitar apenas 'make'
all: $(TARGET)

# Passo 2: Linkar os objetos para criar o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "----------------------------------------"
	@echo "✅ Compilação concluída! Para jogar, digite: ./$(TARGET)"

# Passo 1: Compilar cada .cpp em um .o isolado
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar o projeto (quando você digitar 'make clean')
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)
	@echo "🧹 Pasta limpa!"

.PHONY: all clean