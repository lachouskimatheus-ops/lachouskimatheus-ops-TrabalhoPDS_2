#include "MesaFDP.hpp"
#include "JuizPaulista.hpp"
#include <stdexcept>
#include <iostream>

MesaFDP::~MesaFDP() {
}

MesaFDP::MesaFDP(BaralhoSujo* baralho, Placar* placar) : Mesa(baralho) {
    placar_partida_ = placar;
    indicePrimeiro_ = 0;
    cartasNaRodada_ = 1; 
    cartasSubindo_ = true;
}

void MesaFDP::iniciarFaseApostas() {
    jogadorDaVezIndex_ = indicePrimeiro_;
    totalApostasRodada_ = 0;
    jogadoresQueJaApostaram_ = 0;

    while (getJogadorDaVez()->getVidas() <= 0) {
        jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
    }
}

JogadorFDP* MesaFDP::getJogadorDaVez() {
    return static_cast<JogadorFDP*>(jogadores_[jogadorDaVezIndex_]);
}

int MesaFDP::getApostaProibida() const {
    // 1. Conta quantos jogadores vivos restam na mesa
    int vivos = 0;
    for (auto j : jogadores_) {
        if (static_cast<JogadorFDP*>(j)->getVidas() > 0) vivos++;
    }

    if (jogadoresQueJaApostaram_ == vivos - 1) {
        return cartasNaRodada_ - totalApostasRodada_; 
    }
    
    return -99; 
}

bool MesaFDP::registrarAposta(int apostaDesejada) {
    int maxPossivel = cartasNaRodada_;
    int proibida = getApostaProibida();

    if (apostaDesejada < 0 || apostaDesejada > maxPossivel) return false;
    
    bool temRestricao = (proibida >= 0 && proibida <= maxPossivel);
    if (temRestricao && apostaDesejada == proibida) return false;

    JogadorFDP* jogador = getJogadorDaVez();
    jogador->setAposta(apostaDesejada); 
    
    totalApostasRodada_ += apostaDesejada;
    jogadoresQueJaApostaram_++;
    
    if (faseApostasFinalizada()) {
        jogadorDaVezIndex_ = indicePrimeiro_;
        while (getJogadorDaVez()->getVidas() <= 0) {
            jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
        }
    } else {
        do {
            jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
        } while (getJogadorDaVez()->getVidas() <= 0);
    }

    return true; 
}

bool MesaFDP::faseApostasFinalizada() {
    int vivos = 0;
    for (auto j : jogadores_) {
        if (static_cast<JogadorFDP*>(j)->getVidas() > 0) vivos++;
    }
    return (jogadoresQueJaApostaram_ >= vivos);
}

void MesaFDP::atualizarQtdCartas() {
    if (cartasSubindo_) {
        cartasNaRodada_++;
        if (cartasNaRodada_ >= 5) {
            cartasNaRodada_ = 5;       
            cartasSubindo_ = false;    
        }
    } else {
        cartasNaRodada_--;
        if (cartasNaRodada_ <= 1) {
            cartasNaRodada_ = 1;       
            cartasSubindo_ = true;     
        }
    }
}

std::vector<std::string> MesaFDP::obterResumoRodada() {
    std::vector<std::string> resumo;
    resumo.push_back("---> FIM DA RODADA! <---");

    for (size_t i = 0; i < jogadores_.size(); i++) {
        JogadorFDP* jogador = static_cast<JogadorFDP*>(jogadores_[i]);
        
        int apostou = jogador->getAposta();
        int fez = jogador->getVezesGanhas();

        if (apostou == fez) {
            resumo.push_back("[SALVO] " + jogador->getNome() + " acertou a aposta de quebradinha!");
        } else {
            resumo.push_back("[TOMOU] " + jogador->getNome() + " apostou " + 
                             std::to_string(apostou) + " mas fez " + 
                             std::to_string(fez) + " vaza(s). Perdeu 1 vida!");
        };
    };

    return resumo;
};

void MesaFDP::iniciarFaseDeCartas() {
    cartasNaMesa_.clear(); 
    ordemJogadoresDaVaza_.clear();
}

bool MesaFDP::jogarCarta(int indiceCartaNaMao) {
    if (vazaFinalizada()) {
        std::cout << "[AVISO] Jogada ignorada. A vaza já acabou e a mesa está em apuração." << std::endl;
        return false;
    };
    
    JogadorFDP* jogador = getJogadorDaVez();

    if (cartasNaRodada_ == 1) {
        indiceCartaNaMao = 0; 
    } 

    if (indiceCartaNaMao < 0 || indiceCartaNaMao >= jogador->getQtdCartasMao()) {
        throw std::out_of_range("Índice de carta inválido ou fora dos limites da mão do jogador.");
    }

    Carta* cartaJogada = jogador->jogarCarta(indiceCartaNaMao);
    
    if (cartaJogada == nullptr) {
        throw std::runtime_error("Erro interno: Falha ao tentar extrair a carta da mão do jogador.");
    }

    std::cout << "[DEBUG FDP] Jogador " << jogadorDaVezIndex_ 
              << " jogou a carta: " << cartaJogada->getValor() 
              << " de naipe " << static_cast<int>(cartaJogada->getNaipe()) << std::endl;

    ordemJogadoresDaVaza_.push_back(jogadorDaVezIndex_);
    cartasNaMesa_.push_back(cartaJogada);

    do {
        jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
    } while (getJogadorDaVez()->getVidas() <= 0 && !vazaFinalizada());

    return true; 
}

bool MesaFDP::vazaFinalizada() {
    int vivos = 0;
    for (auto j : jogadores_) {
        if (static_cast<JogadorFDP*>(j)->getVidas() > 0) vivos++;
    }
    return ((int)cartasNaMesa_.size() >= vivos);
}

void MesaFDP::apurarVencedorDaVaza() {
    if (cartasNaMesa_.empty() || ordemJogadoresDaVaza_.empty()) {
        throw std::logic_error("O Juiz não pode apurar o vencedor de uma mesa vazia.");
    }

    JuizPaulista juiz;
    int indiceVencedorVaza = juiz.decidirVencedor(cartasNaMesa_, cartaVira_);

    if (indiceVencedorVaza < 0 || indiceVencedorVaza >= (int)ordemJogadoresDaVaza_.size()) {
        throw std::runtime_error("Erro Crítico: O Juiz retornou um índice fora dos limites.");
    }

    int indiceVencedorReal = ordemJogadoresDaVaza_[indiceVencedorVaza];
    JogadorFDP* jogadorVencedor = static_cast<JogadorFDP*>(jogadores_[indiceVencedorReal]);

    jogadorVencedor->adicionarVazaFeita(); 
    jogadorDaVezIndex_ = indiceVencedorReal;

    cartasNaMesa_.clear(); 
    ordemJogadoresDaVaza_.clear();
}

int MesaFDP::getCartasNaRodada() const {
    return cartasNaRodada_;
}

int MesaFDP::getJogadorDaVezIndex() const {
    return jogadorDaVezIndex_;
}

void MesaFDP::iniciarRodada() {
    cartasNaMesa_.clear();
    ordemJogadoresDaVaza_.clear();
    
    static_cast<BaralhoSujo*>(baralho_)->inicializar(); 
    baralho_->embaralhar();

    Carta* ponteiroVira = baralho_->puxarCarta();
    
    if (ponteiroVira != nullptr) {
        cartaVira_ = *ponteiroVira; 
    } else {
        throw std::runtime_error("Erro Crítico: O baralho esgotou ao tentar puxar a carta Vira.");
    }

    for (Jogador* j : jogadores_) {
        JogadorFDP* jogadorFDP = static_cast<JogadorFDP*>(j);
        jogadorFDP->prepararNovaRodada();
        
        if (jogadorFDP->getVidas() > 0) {
            for (int c = 0; c < cartasNaRodada_; ++c) {
                Carta* cartaDaVez = baralho_->puxarCarta();
                
                if (cartaDaVez != nullptr) {
                    jogadorFDP->receberCarta(cartaDaVez);
                } else {
                    throw std::runtime_error("Erro Crítico: O baralho esgotou durante a distribuição de cartas.");
                }
            }
        }
    }

    iniciarFaseApostas();
}

void MesaFDP::iniciarPartida() {
    indicePrimeiro_ = 0;
    cartasNaRodada_ = 1;
    cartasSubindo_ = true;
    iniciarRodada();
}

bool MesaFDP::rodadaFinalizada() const {
    for (auto* j : jogadores_) {
        JogadorFDP* jogador = static_cast<JogadorFDP*>(j);
        if (jogador->getVidas() > 0 && jogador->getQtdCartasMao() > 0) {
            return false;
        }
    }
    return true; 
}

void MesaFDP::finalizarRodada() {
    placar_partida_->calcularResultados(jogadores_);

    int vivos = 0;
    for (auto* j : jogadores_) {
        if (static_cast<JogadorFDP*>(j)->getVidas() > 0) vivos++;
    };
    if (vivos <= 1) {
        std::cout << "GAME OVER! Temos um vencedor definitivo." << std::endl;
        return; 
    };

    atualizarQtdCartas();

    do {
        indicePrimeiro_ = (indicePrimeiro_ + 1) % jogadores_.size();
    } while (static_cast<JogadorFDP*>(jogadores_[indicePrimeiro_])->getVidas() <= 0);

    iniciarRodada();
}