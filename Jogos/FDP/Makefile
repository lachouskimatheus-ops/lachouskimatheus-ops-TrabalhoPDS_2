CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I../Core -I../../include/dependencias -pthread

# Adicionamos obj/Placar.o na lista do FDP
OBJ = obj/main.o obj/Servidor.o obj/MesaFDP.o obj/BaralhoSujo.o obj/JogadorFDP.o obj/JuizPaulista.o obj/Placar.o

# Adicionamos obj/Mesa.o e obj/Jogador.o na lista do Core
CORE_OBJ = obj/Carta.o obj/Baralho.o obj/Mesa.o obj/Jogador.o

all: fdp_server.exe

fdp_server.exe: $(OBJ) $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(CORE_OBJ) -o fdp_server.exe

# ==========================================
# REGRAS DE COMPILAÇÃO DO FDP
# ==========================================
obj/main.o: src/main.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o obj/main.o

obj/Servidor.o: src/Servidor.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/Servidor.cpp -o obj/Servidor.o

obj/MesaFDP.o: src/MesaFDP.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/MesaFDP.cpp -o obj/MesaFDP.o

obj/BaralhoSujo.o: src/BaralhoSujo.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/BaralhoSujo.cpp -o obj/BaralhoSujo.o

obj/JogadorFDP.o: src/JogadorFDP.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/JogadorFDP.cpp -o obj/JogadorFDP.o

obj/JuizPaulista.o: src/JuizPaulista.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/JuizPaulista.cpp -o obj/JuizPaulista.o

# Nova regra para o Placar
obj/Placar.o: src/Placar.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/Placar.cpp -o obj/Placar.o

# ==========================================
# REGRAS DO CORE UNIFICADO E CLASSES MÃE
# ==========================================
obj/Carta.o: ../Core/Carta.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c ../Core/Carta.cpp -o obj/Carta.o

obj/Baralho.o: ../Core/Baralho.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c ../Core/Baralho.cpp -o obj/Baralho.o

obj/Mesa.o: src/Mesa.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/Mesa.cpp -o obj/Mesa.o

obj/Jogador.o: src/Jogador.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/Jogador.cpp -o obj/Jogador.o

clean:
	rm -rf obj fdp_server.exe