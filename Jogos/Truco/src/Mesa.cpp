#include "Mesa.hpp"
#include <iostream>

Mesa::Mesa(Juiz* juizEscolhido, Baralho* baralhoEscolhido) {
    this->juiz = juizEscolhido;
    this->baralho = baralhoEscolhido;
    this->vira = nullptr;
    this->pontosEquipe1 = 0;
    this->pontosEquipe2 = 0;
}

Mesa::~Mesa() {

}

void Mesa::adicionarJogador(Jogador_Truco* j) {
    this->jogadores.push_back(j);
}

void Mesa::prepararRodada() {
    this->cartasNaMesa.clear();
    
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
    if (this->vira != nullptr) {
        std::cout << "VIRA: ";
        this->vira->imprimir(); 
    }
    std::cout << "Cartas jogadas nesta queda: " << this->cartasNaMesa.size() << std::endl;
    std::cout << "=========================" << std::endl;
}

void Mesa::jogarTurno() {

    prepararRodada();

    int vitoriasEq1 = 0;
    int vitoriasEq2 = 0;
    int vencedorPrimeira = 0;

    for (int queda = 1; queda <= 3; queda++) {
        std::cout << "\n--- " << queda << "a QUEDA ---" << std::endl;
        this->cartasNaMesa.clear();

        // Ciclo de jogadas
        for (auto jogador : jogadores) {
            exibirMesa();
            std::cout << "Vez de " << jogador->getNome() << " [" << jogador->getNome() << "]" << std::endl;
            jogador->mostrarmao(); //
            
            int escolha;
            std::cout << "Escolha o indice da carta: ";
            std::cin >> escolha;

        
            Carta* cartaJogada = jogador->jogarCarta(escolha);
            if (cartaJogada != nullptr) {
                this->cartasNaMesa.push_back(cartaJogada);
            }
        }

        //Criterio de desmpate 
        bool considerarNaipes = (queda == 3 && vitoriasEq1 == vitoriasEq2);
        
        // Vencedor atual
        int vencedorQueda = juiz->decidirVencedor(this->cartasNaMesa, *(this->vira), considerarNaipes);

        if (queda == 1) vencedorPrimeira = vencedorQueda;

        if (vencedorQueda == 1) {
            std::cout << "Equipe 1 venceu a queda!" << std::endl;
            vitoriasEq1++;
        } else if (vencedorQueda == 2) {
            std::cout << "Equipe 2 venceu a queda!" << std::endl;
            vitoriasEq2++;
        } else {
            std::cout << "A queda empatou (amarrou)!" << std::endl;
        }

        // LÓGICA DE VITÓRIA DA MÃO

        // 1. Ganhar duas quedas
        if (vitoriasEq1 == 2) { 
            std::cout << "Equipe 1 venceu a rodada!" << std::endl;
            this->pontosEquipe1++; return; 
        }
        if (vitoriasEq2 == 2) { 
            std::cout << "Equipe 2 venceu a rodada!" << std::endl;
            this->pontosEquipe2++; return; 
        }

        // 2. Empatou a primeira, quem ganhar a segunda leva
        if (queda == 2 && vencedorPrimeira == 0 && vencedorQueda != 0) {
            if (vencedorQueda == 1) this->pontosEquipe1++;
            else this->pontosEquipe2++;
            std::cout << "Equipe " << vencedorQueda << " venceu a rodada por desempate na 2a!" << std::endl;
            return;
        }

        // 3. Ganhou a primeira e a segunda empatou, ganha a rodada
        if (queda == 2 && vencedorPrimeira != 0 && vencedorQueda == 0) {
            if (vencedorPrimeira == 1) this->pontosEquipe1++;
            else this->pontosEquipe2++;
            std::cout << "Equipe " << vencedorPrimeira << " venceu a rodada (ganhou a 1a e a 2a empatou)!" << std::endl;
            return;
        }

        // 4. Decisão na terceira queda
        if (queda == 3) {
            if (vencedorQueda != 0) {
                if (vencedorQueda == 1) this->pontosEquipe1++;
                else this->pontosEquipe2++;
                std::cout << "Equipe " << vencedorQueda << " venceu a rodada na 3a queda!" << std::endl;
            } else {
                // Se a terceira empatar, quem ganhou a primeira leva
                if (vencedorPrimeira == 1) {
                    std::cout << "3a queda empatada. Equipe 1 vence por ter ganho a 1a!" << std::endl;
                    this->pontosEquipe1++;
                } else if (vencedorPrimeira == 2) {
                    std::cout << "3a queda empatada. Equipe 2 vence por ter ganho a 1a!" << std::endl;
                    this->pontosEquipe2++;
                } else {
                    std::cout << "Tudo empatado! Ninguém pontua." << std::endl;
                }
            }
            return;
        }
    }
}

int Mesa::determinarVencedorDaQueda() {
    return juiz->decidirVencedor(this->cartasNaMesa, *(this->vira), false);
}