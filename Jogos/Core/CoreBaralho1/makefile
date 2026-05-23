all: baralho.o cartas.o

baralho.o: baralho.cpp baralho.h cartas.h
	g++ -c baralho.cpp -o baralho.o

cartas.o: cartas.cpp cartas.h
	g++ -c cartas.cpp -o cartas.o

clean:
	rm -f *.o