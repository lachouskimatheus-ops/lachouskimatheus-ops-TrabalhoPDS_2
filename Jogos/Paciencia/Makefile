CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread \
           -DASIO_STANDALONE \
           -I. \
           -I./include \
           -I./dependencias \
           -I../Jogos/Core/include \
           -I../Jogos/Paciencia/include \
           -I../Jogos/Pife/include \
           -I../Jogos/FDP/include \
           -I..

SRC_DIR = src
OBJ_DIR = obj

CORE_OBJECTS = \
../Jogos/Core/obj/Carta.o \
../Jogos/Core/obj/Baralho.o

PIFE_OBJECTS = \
../Jogos/Pife/obj/pife.o \
../Jogos/Pife/obj/jogadorPife.o \
../Jogos/Pife/obj/regrasPife.o

SOURCES = \
$(SRC_DIR)/testeAPI.cpp \
$(SRC_DIR)/coreAPI/Server.cpp \
$(SRC_DIR)/coreAPI/JsonConversor.cpp \
$(wildcard $(SRC_DIR)/routes/*.cpp) \
../Jogos/Paciencia/src/paciencia.cpp \
../Jogos/Paciencia/src/regras.cpp \
../Jogos/Paciencia/src/pontuacao.cpp \
../Jogos/FDP/src/Baralho.cpp \
../Jogos/FDP/src/BaralhoSujo.cpp \
../Jogos/FDP/src/Carta.cpp \
../Jogos/FDP/src/Excecoes.cpp \
../Jogos/FDP/src/Jogador.cpp \
../Jogos/FDP/src/JogadorFDP.cpp \
../Jogos/FDP/src/Juiz.cpp \
../Jogos/FDP/src/JuizPaulista.cpp \
../Jogos/FDP/src/Mesa.cpp \
../Jogos/FDP/src/MesaFDP.cpp \
../Jogos/FDP/src/Placar.cpp

OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = servidor_api

all: $(TARGET)

$(TARGET): $(CORE_OBJECTS) $(PIFE_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)