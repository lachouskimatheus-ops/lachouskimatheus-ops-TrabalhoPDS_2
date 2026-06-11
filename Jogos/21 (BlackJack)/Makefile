CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Pega todos os arquivos .cpp da pasta atual automaticamente
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

# Nome do executável final
EXEC = jogo21

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
