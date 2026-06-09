#pragma once
#include "Jogador.hpp"

class JogadorFDP : public Jogador {
private:
	int vidas_;
	int aposta_atual_;
	int vezes_ganhas_;
public:
	JogadorFDP(int id, std::string nome, int vidas_ini = 5);
	void fazerAposta(int valor_aposta);
	void registrarGanhador();
	void dano();
	void prepararNovaRodada();
	std::vector<int> ordemJogadoresDaVaza_;

	//Getters
	int getVidas() const;
	int getAposta() const;
	int getVezesGanhas() const;

	//Setters
	void setAposta(int aposta) { 
        aposta_atual_ = aposta;
    }

    void adicionarVazaFeita() { 
        vezes_ganhas_++;
    }

    void setVidas(int novasVidas);
    void setNome(const std::string& novoNome);
};