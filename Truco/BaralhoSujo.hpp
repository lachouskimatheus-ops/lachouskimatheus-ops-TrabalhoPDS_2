#pragma once
#include "Baralho.hpp"

class BaralhoSujo : public Baralho {
public:
	BaralhoSujo();
	~BaralhoSujo() override = default; //O override é pra sobrescrever o método da classe mãe, basicamente garante a implementação do método certo e não de um método novo aleatório (sem querer)

	void inicializar() override;
};
