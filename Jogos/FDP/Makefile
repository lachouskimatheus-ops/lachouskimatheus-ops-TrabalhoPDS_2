# Compilador e Flags de compilação
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I include/

# Diretórios
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build


SRCS = $(wildcard $(SRC_DIR)/*.cpp)


OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))


TARGET = jogarFDP


all: $(TARGET)


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsfml-graphics -lsfml-window -lsfml-system
	@echo "----------------------------------------"
	@echo "✅ Compilação concluída! Para jogar, digite: ./$(TARGET)"


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)
	@echo "🧹 Pasta limpa!"

.PHONY: all clean