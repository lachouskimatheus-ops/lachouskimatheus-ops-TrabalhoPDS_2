#include "Jogador.hpp"

Jogador::Jogador(int id, std::string nome) {
	nome_ = nome;
	id_ = id;
}

std::string Jogador::getNome() const {
	return nome_;
}

int Jogador::getId() const {
	return id_;
}