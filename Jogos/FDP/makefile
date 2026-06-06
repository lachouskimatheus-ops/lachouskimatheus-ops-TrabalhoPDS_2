CXX = g++

# Adicionamos o -I../Core/include (ou o caminho correto) para ele achar o Baralho mãe
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I../Core/include

SRC_DIR = src
OBJ_DIR = obj

# Lista EXATA dos arquivos que pertencem à mecânica do FDP.
# NOTA: main.cpp e Servidor.cpp foram removidos de propósito para não conflitar com a API.
# Carta.cpp e Baralho.cpp também não entram aqui, pois vêm do Core da equipe.
SOURCES = \
	$(SRC_DIR)/BaralhoSujo.cpp \
	$(SRC_DIR)/Excecoes.cpp \
	$(SRC_DIR)/Jogador.cpp \
	$(SRC_DIR)/JogadorFDP.cpp \
	$(SRC_DIR)/Juiz.cpp \
	$(SRC_DIR)/JuizPaulista.cpp \
	$(SRC_DIR)/Mesa.cpp \
	$(SRC_DIR)/MesaFDP.cpp \
	$(SRC_DIR)/Placar.cpp

# Transforma magicamente a lista de .cpp em arquivos .o dentro da pasta obj/
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# A regra 'all' agora apenas compila os objetos, sem tentar criar um executável
all: $(OBJECTS)
	@echo "[FDP] Peças compiladas com sucesso! Prontas para a API."

# Regra de compilação individual (cria a pasta obj se não existir)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpa a pasta de objetos para forçar uma compilação do zero
clean:
	rm -rf $(OBJ_DIR)

rebuild: clean all