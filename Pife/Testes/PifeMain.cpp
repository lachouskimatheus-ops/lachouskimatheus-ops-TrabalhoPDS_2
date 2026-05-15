#include <iostream>
#include <vector>
#include <limits>

#include "pife.h"

void limparTela() {

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

}

void mostrarMao(const JogadorPife& jogador) {

    const std::vector<Carta>& mao = jogador.verMao();

    for (int i = 0; i < mao.size(); i++) {

        std::cout
            << i + 1
            << " - "
            << mao[i].cartaString()
            << std::endl;
    }
}

void mostrarMesa(const Pife& jogo) {

    const std::vector<Carta>& mesa = jogo.consultarMesa();

    if (mesa.empty()) {

        std::cout << "Mesa vazia." << std::endl;

        return;
    }

    std::cout << "Cartas na mesa:" << std::endl;

    for (int i = 0; i < mesa.size(); i++) {

        std::cout
            << i + 1
            << " - "
            << mesa[i].cartaString()
            << std::endl;
    }

    std::cout
        << "\nUltima carta descartada: "
        << mesa.back().cartaString()
        << std::endl;
}

int main() {

    int quantidadeJogadores;

    std::cout << "Quantidade de jogadores: ";
    std::cin >> quantidadeJogadores;

    Pife jogo(quantidadeJogadores);

    while (true) {

        limparTela();

        std::cout
            << "Jogador atual: "
            << jogo.consultarIndiceJogadorAtual() + 1
            << std::endl;

        std::cout << "\nMesa:\n";

        mostrarMesa(jogo);

        JogadorPife& jogador = jogo.consultarJogador();

        std::cout << "\nSua mao:\n";

        mostrarMao(jogador);

        int opcao;

        std::cout << "\n1 - Comprar carta do baralho" << std::endl;
        std::cout << "2 - Organizar mao" << std::endl;
        std::cout << "3 - Ver mesa" << std::endl;
        std::cout << "0 - Sair" << std::endl;

        std::cout << "\nEscolha: ";

        std::cin >> opcao;

        if (opcao == 0) {
            break;
        }

        if (opcao == 2) {

            jogador.organizarMao();

            continue;
        }

        if (opcao == 3) {

            limparTela();

            std::cout << "\nMesa:\n";

            mostrarMesa(jogo);

            std::cout << "\nPressione ENTER para voltar...";

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();

            continue;
        }

        if (opcao == 1) {

            jogo.comprarCarta();

            jogador.organizarMao();

            limparTela();

            std::cout << "Mesa atual:\n";

            mostrarMesa(jogo);

            std::cout << "\nSua mao apos comprar:\n";

            mostrarMao(jogador);

            int descarte;

            std::cout << "\nCarta para descartar: ";

            std::cin >> descarte;

            jogo.colocarNaMesa(descarte - 1);

            jogo.proximoJogador();
        }
    }

    return 0;
}