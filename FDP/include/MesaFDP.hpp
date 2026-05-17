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
public:
	MesaFDP(BaralhoSujo* baralho, Placar* placar);
	virtual ~MesaFDP();
	void recolherCartas();
	void resolverVaza();
	void iniciarPartida();
	void apurarResultados();
	void atualizarQtdCartas();
	void anunciarVencedor();
	void faseApostas();
};