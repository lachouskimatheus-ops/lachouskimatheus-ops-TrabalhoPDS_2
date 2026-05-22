CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRC = \
Core/CoreBaralho1/baralho.cpp \
Core/CoreBaralho1/cartas.cpp \
Menu/Menu.cpp \
Poker/poker.cpp \


EXEC = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC)

run:
	./$(EXEC)

clean:
	rm -f $(EXEC)