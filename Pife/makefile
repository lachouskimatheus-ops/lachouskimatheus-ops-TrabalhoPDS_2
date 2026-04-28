CORE = ../Core/CoreBaralho1

main.exe: pife.o jogadorPife.o distribuirPife.o regrasPife.o
	g++ pife.o jogadorPife.o distribuirPife.o regrasPife.o $(CORE)/baralho.o $(CORE)/cartas.o -o main.exe

pife.o: pife.cpp pife.h
	g++ -I$(CORE) -c pife.cpp -o pife.o

jogadorPife.o: jogadorPife.cpp jogadorPife.h
	g++ -I$(CORE) -c jogadorPife.cpp -o jogadorPife.o

distribuirPife.o: distribuirPife.cpp distribuirPife.h
	g++ -I$(CORE) -c distribuirPife.cpp -o distribuirPife.o

regrasPife.o: regrasPife.cpp regrasPife.h
	g++ -I$(CORE) -c regrasPife.cpp -o regrasPife.o

clean:
	rm -f *.o main.exe