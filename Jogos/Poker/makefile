CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -I../Core -I../../API/dependencias

SRC = \
	src/poker.cpp \
	test/teste_poker.cpp \
	../Core/Baralho.cpp \
	../Core/Carta.cpp

EXEC = teste_poker

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC)

run: all
	./$(EXEC)

clean:
	rm -f $(EXEC)