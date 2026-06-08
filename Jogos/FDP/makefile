CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I../Core/include

SRC_DIR = src
OBJ_DIR = obj

SOURCES = \
	$(SRC_DIR)/BaralhoSujo.cpp \
	$(SRC_DIR)/Jogador.cpp \
	$(SRC_DIR)/JogadorFDP.cpp \
	$(SRC_DIR)/Juiz.cpp \
	$(SRC_DIR)/JuizPaulista.cpp \
	$(SRC_DIR)/Mesa.cpp \
	$(SRC_DIR)/MesaFDP.cpp \
	$(SRC_DIR)/Placar.cpp

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(OBJECTS)
	@echo "[FDP] Peças compiladas com sucesso! Prontas para a API."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

rebuild: clean all