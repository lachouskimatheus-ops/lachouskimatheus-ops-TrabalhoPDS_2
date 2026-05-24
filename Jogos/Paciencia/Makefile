CXX = g++
CXXFLAGS = -Wall -Wextra -g3

CORE = ../Core/CoreBaralho1
INCLUDES = -I include -I $(CORE)

BUILD = build

SRCS = src/paciencia.cpp \
       src/regras.cpp \
       src/pontuacao.cpp \
       $(CORE)/baralho.cpp \
       $(CORE)/cartas.cpp

OBJS = $(BUILD)/paciencia.o \
       $(BUILD)/regras.o \
       $(BUILD)/pontuacao.o \
       $(BUILD)/baralho.o \
       $(BUILD)/cartas.o

TEST_SRC = test/test_paciencia.cpp
TEST_OBJ = $(BUILD)/test_paciencia.o
TARGET = $(BUILD)/test_paciencia.exe

all: $(BUILD) $(TARGET)

$(BUILD):
	if not exist $(BUILD) mkdir $(BUILD)

$(BUILD)/paciencia.o: src/paciencia.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/regras.o: src/regras.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/pontuacao.o: src/pontuacao.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/baralho.o: $(CORE)/baralho.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/cartas.o: $(CORE)/cartas.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/test_paciencia.o: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TARGET)
	$(TARGET)

clean:
	del /Q $(BUILD)\*.o $(BUILD)\*.exe