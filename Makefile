# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I./include -I./dependencias

# Diretorios
SRC_DIR = src
OBJ_DIR = obj

# Arquivos
SOURCES = $(wildcard $(SRC_DIR)/core/*.cpp) $(wildcard $(SRC_DIR)/rede/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

TARGET = servidor_fdp

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Servidor compilado com sucesso! Rode com ./$(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Arquivos de compilacao limpos."
