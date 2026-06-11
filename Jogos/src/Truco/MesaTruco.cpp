#include "Truco/MesaTruco.hpp"

#include <iostream>
#include <limits>

MesaTruco::MesaTruco(Juiz* juizEscolhido, Baralho* baralhoEscolhido)
    : vira(nullptr),
      baralho(baralhoEscolhido),
      juiz(juizEscolhido),
      pontosEquipe1(0),
      pontosEquipe2(0),
      valorAtualMao(1),
      nivelTruco(0) {}

MesaTruco::~MesaTruco() {}

void MesaTruco::adicionarJogador(Jogador_Truco* jogador) {
    if (jogador != nullptr) jogadores.push_back(jogador);
}

int MesaTruco::getEquipeDoJogador(int indiceJogador) const {
    return indiceJogador % 2 == 0 ? 1 : 2;
}

void MesaTruco::prepararRodada() {
    cartasNaMesa.clear();
    valorAtualMao = 1;
    nivelTruco = 0;

    baralho->inicializar();
    baralho->embaralhar();

    vira = baralho->puxarCarta();

    for (Jogador_Truco* jogador : jogadores) {
        jogador->limparMao();

        for (int i = 0; i < 3; i++)
            jogador->receberCarta(baralho->puxarCarta());
    }
}

void MesaTruco::exibirMesa() const {
    std::cout << "\n=========================" << std::endl;
    std::cout << "PLACAR: Equipe 1 [" << pontosEquipe1
              << "] x [" << pontosEquipe2 << "] Equipe 2" << std::endl;
    std::cout << "Mao vale: " << valorAtualMao << " ponto(s)" << std::endl;

    if (vira != nullptr)
        std::cout << "VIRA: " << vira->toString() << std::endl;

    std::cout << "=========================" << std::endl;
}

void MesaTruco::exibirPlacar() const {
    std::cout << "\n==============================" << std::endl;
    std::cout << "  PLACAR DA PARTIDA" << std::endl;
    std::cout << "  Equipe 1: " << pontosEquipe1 << " pontos" << std::endl;
    std::cout << "  Equipe 2: " << pontosEquipe2 << " pontos" << std::endl;
    std::cout << "==============================" << std::endl;
}

int MesaTruco::processarPedidoTruco(int indiceJogadorPedindo) {
    int proximoNivel = nivelTruco + 1;
    int equipePedindo = getEquipeDoJogador(indiceJogadorPedindo);
    int equipeAdversaria = equipePedindo == 1 ? 2 : 1;

    const int valores[] = {1, 3, 6, 9, 12};
    const std::string nomes[] = {"", "TRUCO", "SEIS", "NOVE", "DOZE"};

    if (proximoNivel > 4) {
        std::cout << "Nao e possivel pedir mais aumento!" << std::endl;
        return 0;
    }

    std::cout << "\n*** "
              << jogadores[indiceJogadorPedindo]->getNome()
              << " (Equipe " << equipePedindo << ") pediu "
              << nomes[proximoNivel] << "! ***" << std::endl;

    int indiceResponde = -1;

    for (int i = 0; i < static_cast<int>(jogadores.size()); i++) {
        if (getEquipeDoJogador(i) == equipeAdversaria) {
            indiceResponde = i;
            break;
        }
    }

    if (indiceResponde == -1) return 0;

    std::cout << jogadores[indiceResponde]->getNome()
              << " (Equipe " << equipeAdversaria << "), voce aceita "
              << nomes[proximoNivel]
              << "? (1=Sim / 2=Nao";

    if (proximoNivel < 4)
        std::cout << " / 3=Aumentar para " << nomes[proximoNivel + 1];

    std::cout << "): ";

    int resposta;
    std::cin >> resposta;

    if (resposta == 1) {
        nivelTruco = proximoNivel;
        valorAtualMao = valores[proximoNivel];

        std::cout << "Pedido aceito! A mao agora vale "
                  << valorAtualMao << " ponto(s)." << std::endl;

        return 0;
    }

    if (resposta == 3 && proximoNivel < 4) {
        nivelTruco = proximoNivel;
        valorAtualMao = valores[proximoNivel];

        std::cout << "Contra-truco! A mao agora vale "
                  << valorAtualMao
                  << " ponto(s) e o pedido volta para a Equipe "
                  << equipePedindo << "." << std::endl;

        return processarPedidoTruco(indiceResponde);
    }

    int pontosGanhos = valores[nivelTruco];

    std::cout << "Pedido recusado! Equipe "
              << equipePedindo << " ganha "
              << pontosGanhos << " ponto(s)!" << std::endl;

    valorAtualMao = pontosGanhos;
    return equipePedindo;
}

int MesaTruco::jogarTurno() {
    prepararRodada();

    int vitoriasEquipe1 = 0;
    int vitoriasEquipe2 = 0;
    int vencedorPrimeiraQueda = 0;

    for (int queda = 1; queda <= 3; queda++) {
        std::cout << "\n--- " << queda << "a QUEDA ---" << std::endl;

        cartasNaMesa.clear();

        for (int i = 0; i < static_cast<int>(jogadores.size()); i++) {
            exibirMesa();

            std::cout << "\nVez de " << jogadores[i]->getNome()
                      << " (Equipe " << getEquipeDoJogador(i) << ")"
                      << std::endl;

            if (nivelTruco < 4) {
                std::cout << "Deseja pedir truco? (0=Nao / 1=Sim): ";

                int pedido;
                std::cin >> pedido;

                if (pedido == 1) {
                    int resultadoPedido = processarPedidoTruco(i);

                    if (resultadoPedido != 0)
                        return resultadoPedido;
                }
            }

            jogadores[i]->getMao();

            int escolha = -1;

            while (escolha < 0 || escolha >= 3) {
                std::cout << "Escolha o indice da carta: ";
                std::cin >> escolha;
            }

            Carta* cartaJogada = jogadores[i]->jogarCarta(escolha);

            if (cartaJogada != nullptr)
                cartasNaMesa.push_back(cartaJogada);
        }

        int vencedorQueda = determinarVencedorDaQueda();
        int equipeVencedora = 0;

        if (vencedorQueda >= 0 &&
            vencedorQueda < static_cast<int>(jogadores.size()))
            equipeVencedora = getEquipeDoJogador(vencedorQueda);

        if (queda == 1)
            vencedorPrimeiraQueda = equipeVencedora;

        if (equipeVencedora == 1) {
            std::cout << "Equipe 1 venceu a queda!" << std::endl;
            vitoriasEquipe1++;
        } else if (equipeVencedora == 2) {
            std::cout << "Equipe 2 venceu a queda!" << std::endl;
            vitoriasEquipe2++;
        } else {
            std::cout << "A queda empatou!" << std::endl;
        }

        if (vitoriasEquipe1 == 2) {
            std::cout << "Equipe 1 venceu a mao!" << std::endl;
            return 1;
        }

        if (vitoriasEquipe2 == 2) {
            std::cout << "Equipe 2 venceu a mao!" << std::endl;
            return 2;
        }

        if (queda == 2 && vencedorPrimeiraQueda == 0 && equipeVencedora != 0) {
            std::cout << "Equipe " << equipeVencedora
                      << " venceu a mao por desempate na segunda queda!"
                      << std::endl;

            return equipeVencedora;
        }

        if (queda == 2 && vencedorPrimeiraQueda != 0 && equipeVencedora == 0) {
            std::cout << "Equipe " << vencedorPrimeiraQueda
                      << " venceu a mao por ter vencido a primeira queda!"
                      << std::endl;

            return vencedorPrimeiraQueda;
        }

        if (queda == 3) {
            if (equipeVencedora != 0) {
                std::cout << "Equipe " << equipeVencedora
                          << " venceu a mao na terceira queda!"
                          << std::endl;

                return equipeVencedora;
            }

            if (vencedorPrimeiraQueda != 0) {
                std::cout << "Terceira queda empatada. Equipe "
                          << vencedorPrimeiraQueda
                          << " vence por ter ganhado a primeira!"
                          << std::endl;

                return vencedorPrimeiraQueda;
            }

            std::cout << "Todas as quedas empataram. Ninguem pontua."
                      << std::endl;

            return 0;
        }
    }

    return 0;
}

void MesaTruco::jogarPartida() {
    std::cout << "\n=== INICIANDO PARTIDA ===" << std::endl;
    std::cout << "Primeiro a chegar a 12 pontos vence!" << std::endl;

    while (pontosEquipe1 < 12 && pontosEquipe2 < 12) {
        exibirPlacar();

        int vencedorMao = jogarTurno();

        if (vencedorMao == 1) {
            pontosEquipe1 += valorAtualMao;

            std::cout << "\n+++ Equipe 1 marca "
                      << valorAtualMao << " ponto(s)! +++"
                      << std::endl;
        } else if (vencedorMao == 2) {
            pontosEquipe2 += valorAtualMao;

            std::cout << "\n+++ Equipe 2 marca "
                      << valorAtualMao << " ponto(s)! +++"
                      << std::endl;
        }

        exibirPlacar();

        if (pontosEquipe1 >= 12) {
            std::cout << "\n=== EQUIPE 1 VENCEU A PARTIDA! ==="
                      << std::endl;
            break;
        }

        if (pontosEquipe2 >= 12) {
            std::cout << "\n=== EQUIPE 2 VENCEU A PARTIDA! ==="
                      << std::endl;
            break;
        }

        std::cout << "\nPressione Enter para a proxima mao...";

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cin.get();
    }
}

int MesaTruco::determinarVencedorDaQueda() {
    if (juiz == nullptr || vira == nullptr) return -1;

    return juiz->decidirVencedor(cartasNaMesa, *vira);
}