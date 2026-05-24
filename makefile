CXX = g++
CXXFLAGS = -Wall -Wextra -g3 -std=c++17

# Caminhos
ROOT = .
INTERFACE_INC = $(ROOT)/Interface/include
INTERFACE_SRC = $(ROOT)/Interface/scr
CORE = $(ROOT)/Jogos/Core/CoreBaralho1
PACIENCIA_INC = $(ROOT)/Jogos/Paciencia/include
PACIENCIA_SRC = $(ROOT)/Jogos/Paciencia/src

# SFML
SFML_INC = -I/mingw64/include
SFML_LIB = -L/mingw64/lib -lsfml-graphics -lsfml-window -lsfml-system

INCLUDES = -I$(INTERFACE_INC) -I$(CORE) -I$(PACIENCIA_INC) $(SFML_INC)

BUILD = $(ROOT)/build

# Fontes
SRCS = $(INTERFACE_SRC)/Botao.cpp \
       $(INTERFACE_SRC)/TelaMenu.cpp \
       $(INTERFACE_SRC)/TelaPaciencia.cpp \
       $(INTERFACE_SRC)/TelaPause.cpp \
       $(INTERFACE_SRC)/CartaVisual.cpp \
       $(CORE)/baralho.cpp \
       $(CORE)/cartas.cpp \
       $(PACIENCIA_SRC)/paciencia.cpp \
       $(PACIENCIA_SRC)/regras.cpp \
       $(PACIENCIA_SRC)/pontuacao.cpp \
       $(ROOT)/main.cpp

# Objetos
OBJS = $(BUILD)/Botao.o \
       $(BUILD)/TelaMenu.o \
       $(BUILD)/TelaPaciencia.o \
       $(BUILD)/TelaPause.o \
       $(BUILD)/CartaVisual.o \
       $(BUILD)/baralho.o \
       $(BUILD)/cartas.o \
       $(BUILD)/paciencia.o \
       $(BUILD)/regras.o \
       $(BUILD)/pontuacao.o \
       $(BUILD)/main.o

TARGET = $(BUILD)/jogo.exe

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/Botao.o:        $(INTERFACE_SRC)/Botao.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/TelaMenu.o:     $(INTERFACE_SRC)/TelaMenu.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/TelaPaciencia.o: $(INTERFACE_SRC)/TelaPaciencia.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/TelaPause.o:    $(INTERFACE_SRC)/TelaPause.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/CartaVisual.o:  $(INTERFACE_SRC)/CartaVisual.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/baralho.o:      $(CORE)/baralho.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/cartas.o:       $(CORE)/cartas.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/paciencia.o:    $(PACIENCIA_SRC)/paciencia.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/regras.o:       $(PACIENCIA_SRC)/regras.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/pontuacao.o:    $(PACIENCIA_SRC)/pontuacao.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/main.o:         $(ROOT)/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIB)

run: $(TARGET)
	$(TARGET)

clean:
	rm -f $(BUILD)/*.o $(BUILD)/*.exe