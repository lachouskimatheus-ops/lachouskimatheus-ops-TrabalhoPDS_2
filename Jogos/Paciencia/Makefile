# Variáveis para facilitar a manutenção
CXX = g++
CXXFLAGS = -Iinclude -I../Core/CoreBaralho1 -Wall -Wextra -std=c++17
CORE_OBJS = ../Core/CoreBaralho1/baralho.o ../Core/CoreBaralho1/cartas.o
GAME_OBJS = build/paciencia.o build/regras.o build/pontuacao.o

# Alvo principal: Gera o jogo
main.exe: $(GAME_OBJS)
	$(CXX) $(GAME_OBJS) $(CORE_OBJS) -o main.exe

# Compilação do Paciencia
build/paciencia.o: src/paciencia.cpp include/paciencia.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/paciencia.cpp -o build/paciencia.o

# Compilação das Regras (Novo!)
build/regras.o: src/regras.cpp include/regras.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/regras.cpp -o build/regras.o

# Compilação da Pontuação (Novo!)
build/pontuacao.o: src/pontuacao.cpp include/pontuacao.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/pontuacao.cpp -o build/pontuacao.o

# Alvo de Teste: Compila o test_paciencia.cpp e linka com tudo
test: $(GAME_OBJS)
	$(CXX) $(CXXFLAGS) test/test_paciencia.cpp $(GAME_OBJS) $(CORE_OBJS) -o test.exe
	./test.exe

clean:
	rm -rf build main.exe test.exe