#include <iostream>
#include <vector>
#include <string>
#include "Jogo21.hpp"

int main() {
    std::cout << "=======================================\n";
    std::cout << "        BEM-VINDO AO JOGO DE 21        \n";
    std::cout << "=======================================\n\n";

    int numJogadores;
    std::cout << "Quantos jogadores vao participar? (Excluindo a Banca): ";
    std::cin >> numJogadores;
  
    std::vector<std::string> nomes;
    for (int i = 0; i < numJogadores; ++i) {
        std::string nome;
        std::cout << "Nome do Jogador " << (i + 1) << ": ";
        std::cin >> nome;
        nomes.push_back(nome);
    }

    Jogo21 partida;
    partida.inicializarJogo(nomes);
    partida.distribuirCartasIniciais();

    for (Jogador21* jogador : partida.getJogadores()) {
        partida.turnoJogador(jogador);
    }

    partida.turnoBanca();

    partida.determinarVencedores();

    std::cout << "\nFim de Jogo! Obrigado por jogar.\n";
    
    return 0;
}
