#pragma once
#include "Baralho.hpp"

class BaralhoFDP : public Baralho {
public:
	BaralhoFDP();
	~BaralhoFDP() override = default; //O override é pra sobrescrever o método da classe mãe, basicamente garante a implementação do método certo e não de um método novo aleatório (sem querer)

	void inicializar() override;
};
