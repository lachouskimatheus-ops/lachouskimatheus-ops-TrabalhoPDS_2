#include "Jogo21.hpp"
#include <iostream>
#include <stdexcept>


Jogo21::Jogo21() {
    // Inicializa com 1 baralho padrão (52 cartas)
    baralho_ = new Baralho(1); 
    banca_ = new Jogador21(0, "Banca (Dealer)");
}

Jogo21::~Jogo21() {
    delete baralho_;
    delete banca_;
    for (Jogador21* jogador : jogadores_) {
        delete jogador;
    }
    jogadores_.clear();
}

void Jogo21::inicializarJogo(const std::vector<std::string>& nomesLista) {
    baralho_->embaralhar();

    int id_counter = 1;
    for (const std::string& nome : nomesLista) {
        jogadores_.push_back(new Jogador21(id_counter++, nome));
    }
    std::cout << "Jogo inicializado com " << jogadores_.size() << " jogadores e baralho embaralhado.\n";
}

void Jogo21::distribuirCartasIniciais() {
    std::cout << "\n--- Distribuindo Cartas Iniciais ---\n";
    try {
        // No Blackjack, todos recebem 2 cartas inicialmente
        for (int i = 0; i < 2; i++) {
            for (Jogador21* jogador : jogadores_) {
                jogador->receberCarta(baralho_->puxarCarta());
            }
            banca_->receberCarta(baralho_->puxarCarta());
        }
    } catch (const std::runtime_error& e) {
        // Capturamos o erro lançado pelo Baralho.cpp se faltar carta
        std::cerr << "Erro ao distribuir: " << e.what() << "\n";
    }
}


void Jogo21::turnoJogador(Jogador21* jogador) {
    int escolha = 0;
    std::cout << "\n--- Turno de " << jogador->getNome() << " ---\n";

    while (!jogador->estourou()) {
        std::cout << "Sua pontuacao atual: " << jogador->calcularPontuacao() << "\n";
        std::cout << "1. Pedir carta\n2. Parar\nEscolha: ";
        std::cin >> escolha;

        if (escolha == 1) {
            try {
                Carta* novaCarta = baralho_->puxarCarta();
                jogador->receberCarta(novaCarta);
                std::cout << "Voce puxou: " << novaCarta->toString() << "\n";
                
                if (jogador->calcularPontuacao() > 21) {
                    std::cout << "Voce estourou com " << jogador->calcularPontuacao() << " pontos!\n";
                    break;
                }
            } catch (const std::runtime_error& e) {
                std::cout << "O baralho acabou! (" << e.what() << ")\n";
                break;
            }
        } else {
            std::cout << jogador->getNome() << " parou com " << jogador->calcularPontuacao() << " pontos.\n";
            break;
        }
    }
}

void Jogo21::turnoBanca() {
    std::cout << "\n--- Turno da Banca ---\n";
    std::cout << "A Banca revela suas cartas.\n";

    while (banca_->calcularPontuacao() < 17) {
        try {
            Carta* novaCarta = baralho_->puxarCarta();
            banca_->receberCarta(novaCarta);
            std::cout << "Banca puxou: " << novaCarta->toString() << "\n";
        } catch (const std::runtime_error& e) {
            std::cout << "Faltaram cartas para a Banca!\n";
            break;
        }
    }

    int pontosBanca = banca_->calcularPontuacao();
    std::cout << "A Banca encerra seu turno com " << pontosBanca << " pontos.\n";
    if (pontosBanca > 21) {
        std::cout << "A Banca estourou!\n";
    }
}


void Jogo21::determinarVencedores() {
    std::cout << "\n================ RESULTADOS ================\n";
    int pontosBanca = banca_->calcularPontuacao();
    bool bancaEstourou = pontosBanca > 21;

    for (Jogador21* jogador : jogadores_) {
        int pontosJogador = jogador->calcularPontuacao();
        std::cout << "Jogador " << jogador->getNome() << ": " << pontosJogador << " pontos.\n";

        if (pontosJogador > 21) {
            std::cout << "  -> " << jogador->getNome() << " perdeu (estourou).\n";
        } else if (bancaEstourou || pontosJogador > pontosBanca) {
            std::cout << "  -> " << jogador->getNome() << " GANHOU!\n";
        } else if (pontosJogador == pontosBanca) {
            std::cout << "  -> " << jogador->getNome() << " EMPATOU com a Banca.\n";
        } else {
            std::cout << "  -> " << jogador->getNome() << " PERDEU para a Banca.\n";
        }
    }
    std::cout << "============================================\n";
}

const std::vector<Jogador21*>& Jogo21::getJogadores() const {
    return jogadores_;
}

Jogador21* Jogo21::getBanca() const {
    return banca_;
}
