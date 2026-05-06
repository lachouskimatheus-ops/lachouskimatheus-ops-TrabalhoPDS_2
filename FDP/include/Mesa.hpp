#include <vector>
#include "BaralhoSujo.hpp"


class Carta; //Evitar problema

class Mesa {
protected:
	BaralhoSujo* baralho_;
	std::vector<Jogador*> jogadores_;
	int indiceVez;
public:
	Mesa(BaralhoSujo* baralho_mesa);
	virtual ~Mesa();


	void adicionarJogador(Jogador* j);
	void embaralhar();
	void distribuirCartas(int quantidade);
}