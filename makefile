CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -MMD -MP \
	-Iinclude \
	-Iinclude/Core \
	-Iinclude/BlackJack \
	-Iinclude/FDP \
	-Iinclude/Paciencia \
	-Iinclude/Pife \
	-Iinclude/Poker \
	-Iinclude/Truco

SRC_DIR := src
OBJ_DIR := obj

SOURCES := $(shell find \
	$(SRC_DIR)/Core \
	$(SRC_DIR)/BlackJack \
	$(SRC_DIR)/FDP \
	$(SRC_DIR)/Paciencia \
	$(SRC_DIR)/Pife \
	$(SRC_DIR)/Poker \
	$(SRC_DIR)/Truco \
	-type f -name '*.cpp')

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPFILES := $(OBJECTS:.o=.d)

all: $(OBJECTS)
	@echo "[Jogos] Todos os módulos ativos foram compilados."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)

rebuild: clean all

-include $(DEPFILES)

.PHONY: all clean rebuild
