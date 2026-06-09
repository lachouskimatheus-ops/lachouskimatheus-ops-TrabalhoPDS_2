#include "Mesa.hpp"
#include <iostream>
#include <limits>

Mesa::Mesa(Juiz* juizEscolhido, Baralho* baralhoEscolhido) {
    this->juiz = juizEscolhido;
    this->baralho = baralhoEscolhido;
    this->vira = nullptr;
    this->pontosEquipe1 = 0;
    this->pontosEquipe2 = 0;
    this->valorAtualMao = 1;
    this->nivelTruco = 0;
}

Mesa::~Mesa() {

}

void Mesa::adicionarJogador(Jogador_Truco* j) {
    this->jogadores.push_back(j);
}

// Jogadores 0 e 2 = equipe 1 | Jogadores 1 e 3 = equipe 2
int Mesa::getEquipeDoJogador(int indiceJogador) const {
    return (indiceJogador % 2 == 0) ? 1 : 2;
}

void Mesa::prepararRodada() {
    this->cartasNaMesa.clear();
    this->valorAtualMao = 1;
    this->nivelTruco = 0;

    this->baralho->inicializar(); 
    this->baralho->embaralhar();

    this->vira = baralho->puxarCarta();

    for (auto jogador : jogadores) {
        jogador->limparMao(); 
        for (int i = 0; i < 3; i++) {
            jogador->receberCarta(baralho->puxarCarta());
        }
    }
}

void Mesa::exibirMesa() const {
    std::cout << "\n=========================" << std::endl;
    std::cout << "PLACAR: Equipe 1 [" << pontosEquipe1 << "] x [" << pontosEquipe2 << "] Equipe 2" << std::endl;
    std::cout << "Mao vale: " << valorAtualMao << " ponto(s)" << std::endl;
    if (this->vira != nullptr) {
        std::cout << "VIRA: ";
        std::cout << this->vira->toString() << std::endl; 
    }
    std::cout << "=========================" << std::endl;
}

void Mesa::exibirPlacar() const {
    std::cout << "\n==============================" << std::endl;
    std::cout << "  PLACAR FINAL DA PARTIDA" << std::endl;
    std::cout << "  Equipe 1: " << pontosEquipe1 << " pontos" << std::endl;
    std::cout << "  Equipe 2: " << pontosEquipe2 << " pontos" << std::endl;
    std::cout << "==============================" << std::endl;
}

int Mesa::processarPedidoTruco(int indiceJogadorPedindo) {
    // Próximo nível de truco
    int proximoNivel = nivelTruco + 1;
    int equipePedindo = getEquipeDoJogador(indiceJogadorPedindo);
    int equipeAdversaria = (equipePedindo == 1) ? 2 : 1;

    // Valores correspondentes a cada nível
    int valores[] = {1, 3, 6, 9, 12};
    std::string nomes[] = {"", "TRUCO", "SEIS", "NOVE", "DOZE"};

    if (proximoNivel > 4) {
        std::cout << "Nao e possivel pedir mais aumento!" << std::endl;
        return 0;
    }

    std::cout << "\n*** " << jogadores[indiceJogadorPedindo]->getNome()
              << " (Equipe " << equipePedindo << ") pediu "
              << nomes[proximoNivel] << "! ***" << std::endl;

    // Encontra um jogador da equipe adversária para responder
    // (no 1v1 é o índice 1, no 2v2 pega o primeiro da equipe adversária)
    int indiceResponde = -1;
    for (int i = 0; i < (int)jogadores.size(); i++) {
        if (getEquipeDoJogador(i) == equipeAdversaria) {
            indiceResponde = i;
            break;
        }
    }

    std::cout << jogadores[indiceResponde]->getNome()
              << " (Equipe " << equipeAdversaria << "), voce aceita "
              << nomes[proximoNivel] << "? (1=Sim / 2=Nao / 3=Aumentar";
    
    // Só pode aumentar se ainda tiver próximo nível
    if (proximoNivel < 4) {
        std::cout << " para " << nomes[proximoNivel + 1];
    }
    std::cout << "): ";

    int resposta;
    std::cin >> resposta;

    if (resposta == 1) {
        // Aceito
        nivelTruco = proximoNivel;
        valorAtualMao = valores[proximoNivel];
        std::cout << "Truco aceito! A mao agora vale " << valorAtualMao << " ponto(s)." << std::endl;
        return 0; // Jogo continua

    } else if (resposta == 3 && proximoNivel < 4) {
        // Contra-aumento: a equipe adversária aumenta
        nivelTruco = proximoNivel;
        valorAtualMao = valores[proximoNivel];
        std::cout << "Contra-truco! A mao agora vale " << valorAtualMao << " e o pedido passa para a equipe " << equipePedindo << "." << std::endl;
        // Agora a equipe original precisa aceitar o aumento
        return processarPedidoTruco(indiceResponde);

    } else {
        // Recusado: quem pediu ganha valendo o nível anterior
        int pontosGanhos = valores[nivelTruco]; // Valor antes do pedido
        std::cout << "Truco recusado! Equipe " << equipePedindo
                  << " ganha " << pontosGanhos << " ponto(s)!" << std::endl;
        return equipePedindo; // Sinaliza vitória por recusa
    }
}

int Mesa::jogarTurno() {
    prepararRodada();

    int vitoriasEq1 = 0;
    int vitoriasEq2 = 0;
    int vencedorPrimeira = 0;

    for (int queda = 1; queda <= 3; queda++) {
        std::cout << "\n--- " << queda << "a QUEDA ---" << std::endl;
        this->cartasNaMesa.clear();

        for (int i = 0; i < (int)jogadores.size(); i++) {
            exibirMesa();
            std::cout << "\nVez de " << jogadores[i]->getNome()
                      << " (Equipe " << getEquipeDoJogador(i) << ")" << std::endl;

            // Opção de pedir truco antes de jogar
            if (nivelTruco < 4) {
                std::cout << "Deseja pedir truco? (0=Nao / 1=Sim): ";
                int pedido;
                std::cin >> pedido;
                if (pedido == 1) {
                    int resultado = processarPedidoTruco(i);
                    if (resultado != 0) {
                        // Alguém recusou, equipe 'resultado' ganhou
                        return resultado;
                    }
                }
            }

            // Jogar carta
            jogadores[i]->mostrarmao();
            int escolha = -1;
            while (escolha < 0 || escolha >= 3) {
                std::cout << "Escolha o indice da carta: ";
                std::cin >> escolha;
            }

            Carta* cartaJogada = jogadores[i]->jogarCarta(escolha);
            if (cartaJogada != nullptr) {
                this->cartasNaMesa.push_back(cartaJogada);
            }
        }

        // Critério de desempate na 3a queda se empate geral
        bool considerarNaipes = (queda == 3 && vitoriasEq1 == vitoriasEq2);

        int vencedorQueda = juiz->decidirVencedor(this->cartasNaMesa, *(this->vira), considerarNaipes);

        // Converte índice da carta para equipe
        // Carta 0 = jogador 0 (eq1), carta 1 = jogador 1 (eq2), etc.
        int equipeVencedora = 0;
        if (vencedorQueda >= 0 && vencedorQueda < (int)jogadores.size()) {
            equipeVencedora = getEquipeDoJogador(vencedorQueda);
        }

        if (queda == 1) vencedorPrimeira = equipeVencedora;

        if (equipeVencedora == 1) {
            std::cout << "Equipe 1 venceu a queda!" << std::endl;
            vitoriasEq1++;
        } else if (equipeVencedora == 2) {
            std::cout << "Equipe 2 venceu a queda!" << std::endl;
            vitoriasEq2++;
        } else {
            std::cout << "A queda empatou (amarrou)!" << std::endl;
        }

        // LÓGICA DE VITÓRIA DA MÃO

        // 1. Ganhar duas quedas
        if (vitoriasEq1 == 2) { std::cout << "Equipe 1 venceu a mao!" << std::endl; return 1; }
        if (vitoriasEq2 == 2) { std::cout << "Equipe 2 venceu a mao!" << std::endl; return 2; }

        // 2. Empatou a primeira, quem ganhar a segunda leva
        if (queda == 2 && vencedorPrimeira == 0 && equipeVencedora != 0) {
            std::cout << "Equipe " << equipeVencedora << " venceu a mao por desempate na 2a!" << std::endl;
            return equipeVencedora;
        }

        // 3. Ganhou a primeira e a segunda empatou
        if (queda == 2 && vencedorPrimeira != 0 && equipeVencedora == 0) {
            std::cout << "Equipe " << vencedorPrimeira << " venceu a mao (ganhou a 1a e a 2a empatou)!" << std::endl;
            return vencedorPrimeira;
        }

        // 4. Decisão na terceira queda
        if (queda == 3) {
            if (equipeVencedora != 0) {
                std::cout << "Equipe " << equipeVencedora << " venceu a mao na 3a queda!" << std::endl;
                return equipeVencedora;
            } else {
                if (vencedorPrimeira == 1) {
                    std::cout << "3a queda empatada. Equipe 1 vence por ter ganho a 1a!" << std::endl;
                    return 1;
                } else if (vencedorPrimeira == 2) {
                    std::cout << "3a queda empatada. Equipe 2 vence por ter ganho a 1a!" << std::endl;
                    return 2;
                } else {
                    std::cout << "Tudo empatado! Ninguem pontua." << std::endl;
                    return 0;
                }
            }
        }
    }
    return 0;
}

void Mesa::jogarPartida() {
    std::cout << "\n=== INICIANDO PARTIDA ===" << std::endl;
    std::cout << "Primeiro a chegar a 12 pontos vence!" << std::endl;

    while (pontosEquipe1 < 12 && pontosEquipe2 < 12) {
        exibirPlacar();

        int vencedorMao = jogarTurno();

        if (vencedorMao == 1) {
            pontosEquipe1 += valorAtualMao;
            std::cout << "\n+++ Equipe 1 marca " << valorAtualMao << " ponto(s)! +++" << std::endl;
        } else if (vencedorMao == 2) {
            pontosEquipe2 += valorAtualMao;
            std::cout << "\n+++ Equipe 2 marca " << valorAtualMao << " ponto(s)! +++" << std::endl;
        }

        exibirPlacar();

        if (pontosEquipe1 >= 12) {
            std::cout << "\n=== EQUIPE 1 VENCEU A PARTIDA! ===" << std::endl;
            break;
        }
        if (pontosEquipe2 >= 12) {
            std::cout << "\n=== EQUIPE 2 VENCEU A PARTIDA! ===" << std::endl;
            break;
        }

        std::cout << "\nPressione Enter para a proxima mao...";
        std::cin.ignore();
        std::cin.get();
    }
}

int Mesa::determinarVencedorDaQueda() {
    return juiz->decidirVencedor(this->cartasNaMesa, *(this->vira), false);
}