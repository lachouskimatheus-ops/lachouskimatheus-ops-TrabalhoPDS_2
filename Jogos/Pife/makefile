CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I../Core/CoreBaralho1

OBJ = obj/main.o obj/pife.o obj/jogadorPife.o obj/regrasPife.o
CORE_OBJ = ../Core/CoreBaralho1/cartas.o ../Core/CoreBaralho1/baralho.o

all: pife.exe

pife.exe: $(OBJ) $(CORE_OBJ)
	$(CXX) $(OBJ) $(CORE_OBJ) -o pife.exe

obj/main.o: Testes/PifeMain.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c Testes/PifeMain.cpp -o obj/main.o

obj/pife.o: src/pife.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/pife.cpp -o obj/pife.o

obj/jogadorPife.o: src/jogadorPife.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/jogadorPife.cpp -o obj/jogadorPife.o

obj/regrasPife.o: src/regrasPife.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/regrasPife.cpp -o obj/regrasPife.o

clean:
	rm -rf obj pife.exe