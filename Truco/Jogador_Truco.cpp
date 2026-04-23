#include "Jogador.hpp"
#include <string>

Jogador::Jogador(std::string nome) {
	this->nome = nome;
}

void Jogador::receberCarta(Carta novacarta){

	this->mao.push_back(novacarta);

}

void Jogador::mostrarmao(){

	for (int i = 0; i < mao.size(); i++) {
		mao[i].imprimir();
	}

}