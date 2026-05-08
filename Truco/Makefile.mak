# Variáveis de compilação
CXX = g++
CXXFLAGS = -I include -Wall -std=c++11

# Pastas
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Arquivos fonte (Procura todos os .cpp em src/ e o main.cpp na raiz)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
# Transforma a lista de .cpp em lista de .o dentro da pasta build
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SOURCES)))

# Nome do executável
EXEC = $(BUILD_DIR)/truco_game

# Regra principal
all: prepare $(EXEC)

# Cria a pasta de build se não existir
prepare:
	@mkdir -p $(BUILD_DIR)

# Linkagem final
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC)

# Compilação dos arquivos da pasta src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilação do main.cpp
$(BUILD_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR)/*.o $(EXEC)

.PHONY: all prepare clean