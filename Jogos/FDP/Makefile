# Nome do executável final
TARGET = bin/fdp_teste

# Compilador e Flags
CXX = g++
# Os -I ensinam o compilador onde procurar os .hpp voltando (../../) para a raiz
CXXFLAGS = -Wall -Wextra -std=c++17 -I../../Interface/include -Iinclude -I../../Jogos/Core/include

# Flags do SFML
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Encontra os arquivos .cpp usando caminhos relativos à pasta FDP
SRCS = $(wildcard ../../Interface/src/*.cpp) \
       $(wildcard src/*.cpp) \
       $(wildcard ../../Jogos/Core/src/*.cpp) \
       $(wildcard testes/*.cpp)

# Remove os caminhos longos e joga todos os .o direto para a pasta local build/
OBJS = $(patsubst %.cpp, build/%.o, $(notdir $(SRCS)))

# Ensina ao Make onde ele deve procurar pelos arquivos .cpp listados acima
vpath %.cpp ../../Interface/src src ../../Jogos/Core/src testes

# ==========================================
# REGRAS DE COMPILAÇÃO
# ==========================================

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	@echo "Linkando o executável..."
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "✨ Build completo! Rode com: ./$(TARGET)"

# Regra para compilar cada .cpp em um .o na pasta local
build/%.o: %.cpp
	@mkdir -p build
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Limpando os arquivos de compilação locais..."
	rm -rf build bin
	@echo "Limpeza concluída!"

run: all
	@echo "Iniciando o teste..."
	./$(TARGET)

.PHONY: all clean run