main.exe: build/paciencia.o
	g++ build/paciencia.o ../Core/CoreBaralho1/baralho.o ../Core/CoreBaralho1/cartas.o -o main.exe

build/paciencia.o: src/paciencia.cpp
	mkdir -p build
	g++ -Iinclude -I../Core/CoreBaralho1 -Wall -Wextra -std=c++17 -c src/paciencia.cpp -o build/paciencia.o

test: build/paciencia.o
	g++ -Iinclude -I../Core/CoreBaralho1 -Wall -Wextra -std=c++17 test/test_paciencia.cpp build/paciencia.o ../Core/CoreBaralho1/baralho.o ../Core/CoreBaralho1/cartas.o -o test.exe
	./test.exe

clean:
	rm -rf build main.exe test.exe