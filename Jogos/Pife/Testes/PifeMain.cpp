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

void pausar() {
    std::cout << "\nPressione ENTER para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void mostrarMao(const JogadorPife& jogador) {
    const std::vector<Carta>& mao = jogador.verMao();

    for (int i = 0; i < static_cast<int>(mao.size()); i++) {
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

    for (int i = 0; i < static_cast<int>(mesa.size()); i++) {
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

    while (!jogo.jogoFinalizado()) {
        JogadorPife& jogador = jogo.consultarJogador();

        jogador.organizarMao();

        limparTela();

        std::cout
            << "Jogador atual: "
            << jogo.consultarIndiceJogadorAtual() + 1
            << std::endl;

        std::cout << "\nMesa:\n";
        mostrarMesa(jogo);

        std::cout << "\nSua mao:\n";
        mostrarMao(jogador);

        int opcao;

        std::cout << "\n1 - Comprar carta do baralho";
        std::cout << "\n2 - Comprar carta da mesa";
        std::cout << "\n3 - Ver mesa";
        std::cout << "\n4 - Bati";
        std::cout << "\n0 - Sair";

        std::cout << "\n\nEscolha: ";
        std::cin >> opcao;

        if (opcao == 0) {
            break;
        }

        if (opcao == 3) {
            limparTela();

            std::cout << "\nMesa:\n";
            mostrarMesa(jogo);

            pausar();
            continue;
        }

        if (opcao == 4) {
            if (jogo.bati()) {
                limparTela();

                std::cout << "\n===================================";
                std::cout << "\n           VOCE GANHOU!";
                std::cout << "\n===================================\n";

                std::cout
                    << "\nJogador "
                    << jogo.consultarIndiceJogadorAtual() + 1
                    << " venceu!"
                    << std::endl;

                std::cout << "\nMao vencedora:\n";
                mostrarMao(jogador);

                break;
            }

            std::cout
                << "\nBatida invalida!"
                << "\nVoce perdeu o direito de comprar da mesa neste turno."
                << std::endl;

            pausar();
            continue;
        }

        if (opcao == 1 || opcao == 2) {
            int tamanhoAntes = static_cast<int>(jogador.verMao().size());

            if (opcao == 1) {
                jogo.comprarBaralho();
            }

            if (opcao == 2) {
                jogo.comprarMesa();
            }

            int tamanhoDepois = static_cast<int>(jogador.verMao().size());

            if (tamanhoDepois == tamanhoAntes) {
                std::cout << "\nVoce nao conseguiu comprar essa carta." << std::endl;
                std::cout << "Escolha outra acao." << std::endl;

                pausar();
                continue;
            }

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