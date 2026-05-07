all: obj/pife.o obj/jogadorPife.o obj/regrasPife.o

obj/pife.o: src/pife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/pife.cpp -o obj/pife.o

obj/jogadorPife.o: src/jogadorPife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/jogadorPife.cpp -o obj/jogadorPife.o

obj/regrasPife.o: src/regrasPife.cpp
	mkdir -p obj
	g++ -Iinclude -I../Core/CoreBaralho1 -c src/regrasPife.cpp -o obj/regrasPife.o

clean:
	rm -rf obj