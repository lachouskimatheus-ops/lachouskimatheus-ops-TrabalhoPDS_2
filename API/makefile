CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -MMD -MP \
           -DASIO_STANDALONE \
           -I. \
           -I./include \
           -I./dependencias \
           -I../Jogos/Core/include \
           -I../Jogos/Pife/include \
           -I../Jogos/Paciencia/include \
           -I../Jogos/FDP/include \
           -I..

SRC_DIR = src
OBJ_DIR = obj
TARGET = servidor_api

CORE_OBJECTS = \
../Jogos/Core/obj/Carta.o \
../Jogos/Core/obj/Baralho.o

PIFE_OBJECTS = \
../Jogos/Pife/obj/Pife.o \
../Jogos/Pife/obj/JogadorPife.o \
../Jogos/Pife/obj/RegrasPife.o

PACIENCIA_OBJECTS = \
../Jogos/Paciencia/obj/Paciencia.o \
../Jogos/Paciencia/obj/Regras.o \
../Jogos/Paciencia/obj/Pontuacao.o

FDP_OBJECTS = \
../Jogos/FDP/obj/BaralhoSujo.o \
../Jogos/FDP/obj/Jogador.o \
../Jogos/FDP/obj/JogadorFDP.o \
../Jogos/FDP/obj/Juiz.o \
../Jogos/FDP/obj/JuizPaulista.o \
../Jogos/FDP/obj/Mesa.o \
../Jogos/FDP/obj/MesaFDP.o \
../Jogos/FDP/obj/Placar.o

EXTERNAL_OBJECTS = \
$(CORE_OBJECTS) \
$(PIFE_OBJECTS) \
$(PACIENCIA_OBJECTS) \
$(FDP_OBJECTS)

SOURCES = \
$(SRC_DIR)/TesteAPI.cpp \
$(SRC_DIR)/coreAPI/Server.cpp \
$(SRC_DIR)/coreAPI/Responses.cpp \
$(SRC_DIR)/coreAPI/JsonConversor.cpp \
$(SRC_DIR)/coreAPI/SalaBase.cpp \
$(SRC_DIR)/routes/Routes.cpp \
$(SRC_DIR)/routes/FrontendRoutes.cpp \
$(SRC_DIR)/routes/PifeRoutes.cpp \
$(SRC_DIR)/routes/PacienciaRoutes.cpp \
$(SRC_DIR)/routes/FDPRoutes.cpp \
$(SRC_DIR)/multiplayer/Pife/SalaPife.cpp \
$(SRC_DIR)/multiplayer/Pife/GerenciadorSalasPife.cpp \
$(SRC_DIR)/multiplayer/Pife/PifeWebSocket.cpp \
$(SRC_DIR)/multiplayer/FDP/SalaFDP.cpp

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPFILES = $(OBJECTS:.o=.d)

all: $(TARGET)

dependencias:
	$(MAKE) -C ../Jogos/Core
	$(MAKE) -C ../Jogos/Pife
	$(MAKE) -C ../Jogos/Paciencia
	$(MAKE) -C ../Jogos/FDP

$(EXTERNAL_OBJECTS): | dependencias

$(TARGET): $(EXTERNAL_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)

-include $(DEPFILES)

.PHONY: all dependencias clean rebuild run