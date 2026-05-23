#include <vector>
#include "BaralhoSujo.hpp"
#include "Jogador.hpp"


class Carta; //Evitar problema

class Mesa {
protected:
	BaralhoSujo* baralho_;
	std::vector<Jogador*> jogadores_;
	int indiceVez_;
public:
	Mesa(BaralhoSujo* baralho_mesa);
	
	virtual ~Mesa();

	void adicionarJogador(Jogador* j);
	
	//void distribuirCartas(int quantidade);
};