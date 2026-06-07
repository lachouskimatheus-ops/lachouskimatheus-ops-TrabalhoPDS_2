#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include "Menu.hpp"
#include "Mesa.hpp"
#include "BaralhoSujo.hpp"
#include "JuizPaulista.hpp"
#include "JuizMineiro.hpp"
#include "Jogador_Truco.hpp"

void configurarJogadores(Mesa& mesa, int qtdJogadores) {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < qtdJogadores; i++) {
        std::string nome;
        int equipe = (i % 2 == 0) ? 1 : 2;
        std::cout << "Nome do jogador " << (i + 1) << " (Equipe " << equipe << "): ";
        std::getline(std::cin, nome);
        mesa.adicionarJogador(new Jogador_Truco(nome));
    }
}

void iniciar_truco_paulista() {
    JuizPaulista* juiz = new JuizPaulista();
    BaralhoSujo* baralho = new BaralhoSujo();
    Mesa mesa(juiz, baralho);

    std::cout << "\nQuantos jogadores? (2 = 1v1 / 4 = 2v2): ";
    int qtd;
    std::cin >> qtd;
    if (qtd != 2 && qtd != 4) qtd = 2;

    configurarJogadores(mesa, qtd);

    mesa.jogarPartida();

    std::cout << "\nPressione Enter para voltar ao menu...";
    std::cin.ignore();
    std::cin.get();

    delete juiz;
    delete baralho;
}

void iniciar_truco_mineiro() {
    JuizMineiro* juiz = new JuizMineiro();
    BaralhoSujo* baralho = new BaralhoSujo();
    Mesa mesa(juiz, baralho);

    std::cout << "\nQuantos jogadores? (2 = 1v1 / 4 = 2v2): ";
    int qtd;
    std::cin >> qtd;
    if (qtd != 2 && qtd != 4) qtd = 2;

    configurarJogadores(mesa, qtd);

    mesa.jogarPartida();

    std::cout << "\nPressione Enter para voltar ao menu...";
    std::cin.ignore();
    std::cin.get();

    delete juiz;
    delete baralho;
}

int main() {
    Menu menu;

    menu.addOpcao("Jogar Truco Paulista", iniciar_truco_paulista);
    menu.addOpcao("Jogar Truco Mineiro",  iniciar_truco_mineiro);

    while (true) {
        menu.exibir();
    }

    return 0;
}