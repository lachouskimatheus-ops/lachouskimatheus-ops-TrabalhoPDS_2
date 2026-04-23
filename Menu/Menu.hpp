#pragma once
#include <string>
//Biblioteca que "armazena funcoes"
#include <functional>
#include <vector>

struct OpcaoDoMenu {

	std::string nome_opcao;
	std::function<void()> acao; //Bloco que vai armazenar as funções que vão abrir os jogos, ex: "iniciar_Truco()"

};

class Menu {
private:
	std::vector<OpcaoDoMenu> opcoes; //Vetor em que cada elemento é uma opção do menu com nome e ação

public:
	void addOpcao(std::string nome, std::function<void()> acao);
	void exibir();
};