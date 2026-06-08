CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -I../Core/include -I../../API/dependencias

SRC = \
	src/poker.cpp \
	test/teste_poker.cpp \
	../Core/src/Baralho.cpp \
	../Core/src/Carta.cpp

EXEC = teste_poker

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXEC)

run: all
	./$(EXEC)

clean:
	rm -f $(EXEC)
