#include "MesaFDP.hpp"
#include "JuizPaulista.hpp"

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

    //Controle para saber se o primeiro jogador tiver morto (pula pro próximo vivo)
    while (getJogadorDaVez()->getVidas() <= 0) {
        jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
    }
}

JogadorFDP* MesaFDP::getJogadorDaVez() {
    //Tem que transformar de jogador para jogadorFDP pq o compilador é burro
    return static_cast<JogadorFDP*>(jogadores_[jogadorDaVezIndex_]);
}

//Logica da aposta de FDP
int MesaFDP::getApostaProibida() const {
    int qtd_jogadores = jogadores_.size();
    int indiceUltimo = (indicePrimeiro_ - 1 + qtd_jogadores) % qtd_jogadores;
    
    if (jogadorDaVezIndex_ == indiceUltimo) {
        return cartasNaRodada_ - totalApostasRodada_; 
    }
    return -99; //Valor impossível para controle
}

bool MesaFDP::registrarAposta(int apostaDesejada) {
    int maxPossivel = cartasNaRodada_;
    int proibida = getApostaProibida();

    // Validações para a interface grafica
    if (apostaDesejada < 0 || apostaDesejada > maxPossivel) return false;
    
    bool temRestricao = (proibida >= 0 && proibida <= maxPossivel);
    if (temRestricao && apostaDesejada == proibida) return false;

    //Salva a aposta
    JogadorFDP* jogador = getJogadorDaVez();
    jogador->setAposta(apostaDesejada); // Salva no jogador
    
    totalApostasRodada_ += apostaDesejada;
    jogadoresQueJaApostaram_++;
    
    // Passa a vez para o próximo jogador VIVO
    do {
        jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
    } while (getJogadorDaVez()->getVidas() <= 0 && !faseApostasFinalizada());

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
    int vivos = placar_partida_->jogadoresVivos(jogadores_);
    
    // Segurança contra divisão por zero caso todos morram simultaneamente
    if (vivos == 0) vivos = 1; 

    int maxCartasPossivel = 39 / vivos;

    if (cartasSubindo_) {
        if (cartasNaRodada_ < maxCartasPossivel) {
            cartasNaRodada_++;
        } else {
            cartasSubindo_ = false;
            cartasNaRodada_--;
        }
    } else {
        if (cartasNaRodada_ > 1) {
            cartasNaRodada_--;
        } else {
            cartasSubindo_ = true;
            cartasNaRodada_++;
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
            // Mensagem atualizada para a regra fixa de 1 vida
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

    // Na rodada cega (1 carta), o índice só pode ser 0
    if (cartasNaRodada_ == 1) {
        indiceCartaNaMao = 0; 
    } 

    if (indiceCartaNaMao < 0 || indiceCartaNaMao >= jogador->getQtdCartasMao()) {
        std::cout << "[ERRO] Indice invalido. Jogador tem " << jogador->getQtdCartasMao() << " cartas, pediu indice " << indiceCartaNaMao << std::endl;
        return false; 
    }

    Carta* cartaJogada = jogador->jogarCarta(indiceCartaNaMao);
    
    if (cartaJogada == nullptr) {
        return false;
    }

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
    // BLINDAGEM 1: Se a função for chamada quando a mesa já estiver limpa, aborta silenciosamente.
    if (cartasNaMesa_.empty() || ordemJogadoresDaVaza_.empty()) {
        return;
    }

    JuizPaulista juiz;
    int indiceVencedorVaza = juiz.decidirVencedor(cartasNaMesa_, cartaVira_, true);

    // BLINDAGEM 2: Se o Juiz falhar e retornar um índice que não existe no vetor, aborta a execução.
    if (indiceVencedorVaza < 0 || indiceVencedorVaza >= (int)ordemJogadoresDaVaza_.size()) {
        std::cout << "[ERRO CRITICO] Juiz retornou indice fora dos limites: " << indiceVencedorVaza << std::endl;
        return;
    }

    int indiceVencedorReal = ordemJogadoresDaVaza_[indiceVencedorVaza];
    JogadorFDP* jogadorVencedor = static_cast<JogadorFDP*>(jogadores_[indiceVencedorReal]);

    jogadorVencedor->adicionarVazaFeita(); 
    jogadorDaVezIndex_ = indiceVencedorReal;

    // Limpa a mesa para a próxima vaza
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
    }

    for (Jogador* j : jogadores_) {
        JogadorFDP* jogadorFDP = static_cast<JogadorFDP*>(j);
        jogadorFDP->prepararNovaRodada();
        
        if (jogadorFDP->getVidas() > 0) {
            for (int c = 0; c < cartasNaRodada_; ++c) {
                Carta* cartaDaVez = baralho_->puxarCarta();
                if (cartaDaVez != nullptr) {
                    jogadorFDP->receberCarta(cartaDaVez);
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
    for (auto* j : jogadores_) {
        JogadorFDP* jogador = static_cast<JogadorFDP*>(j);
        if (jogador->getVidas() > 0) {
            // A MÁGICA ACONTECE AQUI: Só perde vida se errar a aposta!
            if (jogador->getAposta() != jogador->getVezesGanhas()) {
                jogador->setVidas(jogador->getVidas() - 1);
            };
        };
    };

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