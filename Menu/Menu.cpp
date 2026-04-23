#include "Menu.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>

void Menu::addOpcao(std::string nome, std::function<void()> acao) {
	//Compilador já cria um objeto da struct e já manda direto, sem precisar criar manualmente
	this->opcoes.push_back({nome, acao});
}

void Menu::exibir() {

	std::cout << "\033[2J\033[1;1H"; //Limpa a tela usando ANSI 
	std::cout << "\n --- Jogos --- \n";


	//Exibe todos os jogos que foram adicionados
	for(int i = 0; i < opcoes.size(); i++){
		std::cout << (i + 1) << " - " << opcoes[i].nome_opcao << "\n";
	};

	std::cout << "Escolha uma opção: ";

	int escolha;
	std::cin >> escolha;

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