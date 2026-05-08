#pragma once
#include <iostream>
#include <vector>
#include <random>

class Carta; //Evita dor de cabeça

class Baralho {

protected:
	std::vector<Carta*> cartas_;

public:
	Baralho() = default; //O default basicamente diz pro C++ ficar de boa e construir o objeto assim mesmo (já que o vetor já nasce vazio)
	void embaralhar();
	virtual ~Baralho();
	Carta* puxarCarta();

	virtual void inicializar() = 0; //Esse método garante que a classe mãe vai servir apenas de molde, cada classe filha de baralho vai TER que implementar seu próprio baralho específico ex: "BaralhoFDP"

	//Getter
	int getQtdCartas() const;

	void limpar();
};