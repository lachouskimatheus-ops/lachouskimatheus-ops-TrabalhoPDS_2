#include "Mesa.hpp"

Mesa::Mesa(BaralhoSujo* baralho_mesa) {
	baralho_mesa->embaralhar();
	indiceVez = 0;
	baralho_ = baralho_mesa;
}

void Mesa::adicionarJogador(Jogador* j) {
	jogadores_.push_back(j);
}