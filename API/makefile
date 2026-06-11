CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -pthread \
	-DASIO_STANDALONE \
	-I. \
	-Iinclude \
	-Idependencias \
	-I../Jogos/include \
	-I../Jogos/include/Core \
	-I../Jogos/include/FDP \
	-I../Jogos/include/Paciencia \
	-I../Jogos/include/Pife \
	-I../Jogos/include/Poker \
	-I../Jogos/include/Truco

SRC_DIR := src
OBJ_DIR := obj
TARGET := servidor_api

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): dependencias $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) \
		$$(find ../Jogos/obj/Core \
			../Jogos/obj/FDP \
			../Jogos/obj/Paciencia \
			../Jogos/obj/Pife \
			../Jogos/obj/Poker \
			../Jogos/obj/Truco \
			-type f -name '*.o') \
		-o $@

dependencias:
	$(MAKE) -C ../Jogos

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

clean-all: clean
	$(MAKE) -C ../Jogos clean

rebuild: clean-all all

run: all
	./$(TARGET)

.PHONY: all dependencias clean clean-all rebuild run