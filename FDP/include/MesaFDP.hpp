#pragma once
#include "Mesa.hpp"
#include "Carta.hpp"
#include "Placar.hpp"

class Carta;

class MesaFDP : public Mesa {
private:
	std::vector<Carta*> cartasNaMesa_;
	Carta cartaVira_;
	int indicePrimeiro_;
	int cartasNaRodada_;
	Placar* placar_partida_;
	bool cartasSubindo_;
	int jogadorDaVezIndex_ = 0; 
    int totalApostasRodada_ = 0;
    int jogadoresQueJaApostaram_ = 0;
	std::vector<int> ordemJogadoresDaVaza_;
	
public:
	std::vector<std::string> obterResumoRodada();
	MesaFDP(BaralhoSujo* baralho, Placar* placar);
	virtual ~MesaFDP();
	void recolherCartas();
	void iniciarPartida();
	void apurarResultados();
	void atualizarQtdCartas();
	void anunciarVencedor();
	void iniciarFaseApostas();
    JogadorFDP* getJogadorDaVez();
    int getApostaProibida();
    bool registrarAposta(int apostaDesejada);
    bool faseApostasFinalizada();
    void iniciarFaseDeCartas();
    bool jogarCarta(int indiceCartaNaMao); 
    bool vazaFinalizada();
    void apurarVencedorDaVaza();
};