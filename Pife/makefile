main.exe: obj/pife.o obj/jogadorPife.o obj/distribuirPife.o obj/regrasPife.o
	g++ obj/pife.o obj/jogadorPife.o obj/distribuirPife.o obj/regrasPife.o ../Core/CoreBaralho1/baralho.o ../Core/CoreBaralho1/cartas.o -o main.exe

obj/pife.o: src/pife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/pife.cpp -o obj/pife.o

obj/jogadorPife.o: src/jogadorPife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/jogadorPife.cpp -o obj/jogadorPife.o

obj/distribuirPife.o: src/distribuirPife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/distribuirPife.cpp -o obj/distribuirPife.o

obj/regrasPife.o: src/regrasPife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/regrasPife.cpp -o obj/regrasPife.o

clean:
	rm -rf obj main.exe