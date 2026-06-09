CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -pthread -MMD -MP \
	-DASIO_STANDALONE \
	-I. \
	-I./include \
	-I./dependencias \
	-I../Jogos/Core/include \
	-I../Jogos/Pife/include \
	-I../Jogos/Paciencia/include \
	-I../Jogos/Truco/include \
	-I../Jogos/FDP/include \
	-I..

SRC_DIR := src
OBJ_DIR := obj
TARGET := servidor_api

CORE_OBJ := \
	../Jogos/Core/obj/Carta.o \
	../Jogos/Core/obj/Baralho.o

PIFE_OBJ := \
	../Jogos/Pife/obj/Pife.o \
	../Jogos/Pife/obj/JogadorPife.o \
	../Jogos/Pife/obj/RegrasPife.o

PACIENCIA_OBJ := \
	../Jogos/Paciencia/obj/Paciencia.o \
	../Jogos/Paciencia/obj/Regras.o \
	../Jogos/Paciencia/obj/Pontuacao.o

TRUCO_OBJ := \
	../Jogos/Truco/obj/BaralhoTruco.o \
	../Jogos/Truco/obj/Jogador_Truco.o \
	../Jogos/Truco/obj/JuizTruco.o \
	../Jogos/Truco/obj/JuizPaulistaTruco.o \
	../Jogos/Truco/obj/JuizMineiroTruco.o \
	../Jogos/Truco/obj/Truco.o

FDP_OBJ := \
	../Jogos/FDP/obj/BaralhoSujo.o \
	../Jogos/FDP/obj/Jogador.o \
	../Jogos/FDP/obj/JogadorFDP.o \
	../Jogos/FDP/obj/Juiz.o \
	../Jogos/FDP/obj/JuizPaulista.o \
	../Jogos/FDP/obj/Mesa.o \
	../Jogos/FDP/obj/MesaFDP.o \
	../Jogos/FDP/obj/Placar.o

EXTERNAL_OBJ := \
	$(CORE_OBJ) \
	$(PIFE_OBJ) \
	$(PACIENCIA_OBJ) \
	$(TRUCO_OBJ) \
	$(FDP_OBJ)

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPFILES := $(OBJECTS:.o=.d)

all: $(TARGET)

dependencias:
	$(MAKE) -C ../Jogos/Core
	$(MAKE) -C ../Jogos/Pife
	$(MAKE) -C ../Jogos/Paciencia
	$(MAKE) -C ../Jogos/Truco
	$(MAKE) -C ../Jogos/FDP

$(TARGET): dependencias $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(EXTERNAL_OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

clean-all: clean
	$(MAKE) -C ../Jogos/Core clean
	$(MAKE) -C ../Jogos/Pife clean
	$(MAKE) -C ../Jogos/Paciencia clean
	$(MAKE) -C ../Jogos/Truco clean
	$(MAKE) -C ../Jogos/FDP clean

rebuild: clean-all all

run: all
	./$(TARGET)

-include $(DEPFILES)

.PHONY: all dependencias clean clean-all rebuild run