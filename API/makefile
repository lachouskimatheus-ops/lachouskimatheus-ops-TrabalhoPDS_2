CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread \
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
../Jogos/Pife/obj/pife.o \
../Jogos/Pife/obj/jogadorPife.o \
../Jogos/Pife/obj/regrasPife.o

PACIENCIA_OBJECTS = \
../Jogos/Paciencia/obj/paciencia.o \
../Jogos/Paciencia/obj/regras.o \
../Jogos/Paciencia/obj/pontuacao.o

FDP_OBJECTS = \
../Jogos/FDP/obj/BaralhoSujo.o \
../Jogos/FDP/obj/Excecoes.o \
../Jogos/FDP/obj/Jogador.o \
../Jogos/FDP/obj/JogadorFDP.o \
../Jogos/FDP/obj/Juiz.o \
../Jogos/FDP/obj/JuizPaulista.o \
../Jogos/FDP/obj/Mesa.o \
../Jogos/FDP/obj/MesaFDP.o \
../Jogos/FDP/obj/Placar.o

SOURCES = \
$(SRC_DIR)/testeAPI.cpp \
$(SRC_DIR)/coreAPI/Server.cpp \
$(SRC_DIR)/coreAPI/Responses.cpp \
$(SRC_DIR)/coreAPI/JsonConversor.cpp \
$(SRC_DIR)/coreAPI/SalaBase.cpp \
$(SRC_DIR)/routes/Routes.cpp \
$(SRC_DIR)/routes/FrontendRoutes.cpp \
$(SRC_DIR)/routes/PifeRoutes.cpp \
$(SRC_DIR)/routes/PacienciaRoutes.cpp \
$(SRC_DIR)/multiplayer/Pife/SalaPife.cpp \
$(SRC_DIR)/multiplayer/Pife/PifeWebSocket.cpp

OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(CORE_OBJECTS) $(PIFE_OBJECTS) $(PACIENCIA_OBJECTS) $(FDP_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)