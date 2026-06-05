CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread \
           -DASIO_STANDALONE \
           -I./include \
           -I./dependencias \
           -I../Jogos/Core \
           -I../Jogos/Paciencia/include \
           -I..

SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(SRC_DIR)/testeAPI.cpp \
          $(SRC_DIR)/coreAPI/Server.cpp \
          $(SRC_DIR)/coreAPI/JsonConversor.cpp \
          $(wildcard $(SRC_DIR)/routes/*.cpp) \
          ../Jogos/Core/Carta.cpp \
          ../Jogos/Core/Baralho.cpp \
          ../Jogos/Paciencia/src/paciencia.cpp \
          ../Jogos/Paciencia/src/regras.cpp \
          ../Jogos/Paciencia/src/pontuacao.cpp

OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = servidor_api

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)