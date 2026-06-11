#include "Menu.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>

void Menu::addOpcao(std::string nome, std::function<void()> acao) {
	this->opcoes.push_back({nome, acao});
}

void Menu::exibir() {
	std::cout << "\033[2J\033[1;1H"; 
	std::cout << "\n --- Jogos --- \n";

	for(int i = 0; i < opcoes.size(); i++){
		std::cout << (i + 1) << " - " << opcoes[i].nome_opcao << "\n";
	};

	std::cout << "Escolha uma opção: ";
	int escolha;
	std::cin >> escolha;

    // Limpa o buffer caso o jogador digite uma letra
	if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        escolha = -1;
    }

	if (escolha > 0 && escolha <= opcoes.size()){
		opcoes[escolha - 1].acao();
	} else if (escolha == 0) {
		std::cout << "Adeus...\n";
		exit(0);
	} else {
		std::cout << "Opção inválida, panaca!\n";
		std::cin.clear();
	};
}