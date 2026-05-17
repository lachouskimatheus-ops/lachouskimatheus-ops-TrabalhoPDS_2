#include "JogadorFDP.hpp"

JogadorFDP::JogadorFDP(int id, std::string nome, int vidas_ini) : Jogador(id, nome) {
	vidas_ = vidas_ini;
	aposta_atual_ = 0;
	vezes_ganhas_ = 0;
}

void JogadorFDP::fazerAposta(int aposta) {
	aposta_atual_ = aposta;
}

void JogadorFDP::registrarGanhador() {
	vezes_ganhas_ += 1;
}

void JogadorFDP::prepararNovaRodada() {
	vezes_ganhas_ = 0;
	aposta_atual_ = 0;
}

void JogadorFDP::dano() {
	if(aposta_atual_ != vezes_ganhas_) {
		vidas_ -= 1;
	};
}

int JogadorFDP::getVidas() const {
	return vidas_;
}

int JogadorFDP::getAposta() const {
	return aposta_atual_;
}

int JogadorFDP::getVezesGanhas() const {
	return vezes_ganhas_;
}