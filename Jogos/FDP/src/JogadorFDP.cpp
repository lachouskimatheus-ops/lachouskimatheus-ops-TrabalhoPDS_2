#include "JogadorFDP.hpp"
#include "Carta.hpp"

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
	aposta_atual_ = -1;

	mao_.clear();
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

nlohmann::json JogadorFDP::paraJson() const {
    nlohmann::json dadosJogador;
    
    // 1. Dados herdados da classe base (Jogador)
    dadosJogador["id"] = getId();
    dadosJogador["nome"] = getNome();
    
    // 2. Dados específicos do FDP
    dadosJogador["vidas"] = vidas_;
    dadosJogador["aposta_atual"] = aposta_atual_;
    dadosJogador["vezes_ganhas"] = vezes_ganhas_;
    
    // 3. Montando o Array de cartas (A Mão do jogador)
    nlohmann::json jsonMao = nlohmann::json::array();
    
    for (Carta* carta : getMao()) {
        jsonMao.push_back(carta->paraJson());
    };
    
    dadosJogador["mao"] = jsonMao;
    
    return dadosJogador;
}

void JogadorFDP::setVidas(int novasVidas) {
    vidas_ = novasVidas;
}