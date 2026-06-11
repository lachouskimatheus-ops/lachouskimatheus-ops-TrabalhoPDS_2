CXX := g++
TARGET := servidor
BUILD_DIR := build

CXXFLAGS := -std=c++17 -Wall -Wextra -pthread -DASIO_STANDALONE \
	-IAPI \
	-IAPI/include \
	-IAPI/dependencias \
	-IJogos/include \
	-IJogos/include/Core \
	-IJogos/include/FDP \
	-IJogos/include/Paciencia \
	-IJogos/include/Pife \
	-IJogos/include/Poker \
	-IJogos/include/Truco

API_SOURCES := $(shell find API/src -type f -name '*.cpp')

JOGOS_SOURCES := $(shell find \
	Jogos/src/Core \
	Jogos/src/FDP \
	Jogos/src/Paciencia \
	Jogos/src/Pife \
	Jogos/src/Poker \
	Jogos/src/Truco \
	-type f -name '*.cpp')

SOURCES := $(API_SOURCES) $(JOGOS_SOURCES)
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo "Servidor compilado: ./$(TARGET)"
	@echo "Execute com: make run"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild