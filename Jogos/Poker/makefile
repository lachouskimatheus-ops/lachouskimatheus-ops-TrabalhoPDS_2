CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra \
	-Iinclude \
	-I../Core/include

SRC_DIR := src
OBJ_DIR := obj

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

rebuild: clean all

.PHONY: all clean rebuild