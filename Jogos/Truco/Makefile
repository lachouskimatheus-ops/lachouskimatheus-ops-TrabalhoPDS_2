CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -MMD -MP \
	-I./include \
	-I../Core/include

SRC_DIR := src
OBJ_DIR := obj

SOURCES := \
	$(SRC_DIR)/BaralhoTruco.cpp \
	$(SRC_DIR)/Jogador_Truco.cpp \
	$(SRC_DIR)/JuizTruco.cpp \
	$(SRC_DIR)/JuizPaulistaTruco.cpp \
	$(SRC_DIR)/JuizMineiroTruco.cpp \
	$(SRC_DIR)/Truco.cpp

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPFILES := $(OBJECTS:.o=.d)

all: dependencias $(OBJECTS)

dependencias:
	$(MAKE) -C ../Core

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

rebuild: clean all

-include $(DEPFILES)

.PHONY: all dependencias clean rebuild