CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -pthread \
	-DASIO_STANDALONE \
	-I. \
	-IAPI \
	-IAPI/include \
	-IAPI/dependencias \
	-IJogos/include \
	-IJogos/include/Core \
	-IJogos/include/BlackJack \
	-IJogos/include/FDP \
	-IJogos/include/Paciencia \
	-IJogos/include/Pife \
	-IJogos/include/Poker \
	-IJogos/include/Truco

TARGET := API/servidor_api

OBJ_DIR := build/obj

API_SOURCES := $(shell find API/src -type f -name '*.cpp')

JOGOS_SOURCES := $(shell find \
	Jogos/src/Core \
	Jogos/src/BlackJack \
	Jogos/src/FDP \
	Jogos/src/Paciencia \
	Jogos/src/Pife \
	Jogos/src/Poker \
	Jogos/src/Truco \
	-type f -name '*.cpp' \
	! -path 'Jogos/src/BlackJack/Baralho.cpp' \
	! -path 'Jogos/src/BlackJack/Jogador.cpp')

SOURCES := $(API_SOURCES) $(JOGOS_SOURCES)

OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

DEPFILES := $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf build
	rm -f $(TARGET)
	rm -rf API/obj
	rm -rf Jogos/obj
	rm -f *.o
	rm -f *.d
	rm -rf .DS_Store
	rm -rf API/.DS_Store
	rm -rf Jogos/.DS_Store
	rm -rf frontend/.DS_Store
	rm -rf __MACOSX
	rm -rf API/__MACOSX
	rm -rf Jogos/__MACOSX
	rm -rf frontend/__MACOSX

rebuild: clean all

-include $(DEPFILES)

.PHONY: all run clean rebuild