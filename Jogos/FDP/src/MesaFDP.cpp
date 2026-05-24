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

//L[ogica] da aposta de FDP
int MesaFDP::getApostaProibida() {
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

    // Validações para a interface gr[afica]
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

    return true; //Retorno verdadiero para contorle da interface
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

	int maxCartasPossivel = 39 / vivos;

	if (cartasSubindo_) {
		if (cartasNaRodada_ < maxCartasPossivel) {
			cartasNaRodada_++;
		} else {
			cartasSubindo_ = false;
			cartasNaRodada_--;
		};
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
            
            resumo.push_back("[TOMOU] " + jogador->getNome() + " apostou " + 
                             std::to_string(apostou) + " mas fez " + 
                             std::to_string(fez) + " rodada(s). Perdeu uma vida!");
        };
    };

    return resumo;
};

void MesaFDP::iniciarFaseDeCartas() {
    cartasNaMesa_.clear(); 
    ordemJogadoresDaVaza_.clear();
}

bool MesaFDP::jogarCarta(int indiceCartaNaMao) {
    JogadorFDP* jogador = getJogadorDaVez();

   //rodada cega
    if (cartasNaRodada_ == 1) {
        indiceCartaNaMao = 0; //Unica carta
    } 

    //rodada normal
    else {
        
        if (indiceCartaNaMao < 0 || indiceCartaNaMao >= jogador->getQtdCartasMao()) {
            return false; // Retorno de controle para saber se o jogaodr clicou certo
        }
    }

    //Remove da mao e joga na mesa
    Carta* cartaJogada = jogador->jogarCarta(indiceCartaNaMao + 1);
    //Anotando a ordem de jogada
    ordemJogadoresDaVaza_.push_back(jogadorDaVezIndex_);

    cartasNaMesa_.push_back(cartaJogada);

    // Passa a vez para o próximo jogador vivo
    do {
        jogadorDaVezIndex_ = (jogadorDaVezIndex_ + 1) % jogadores_.size();
    } while (getJogadorDaVez()->getVidas() <= 0 && !vazaFinalizada());

    return true; //Retorno de controle pro SFML
}

bool MesaFDP::vazaFinalizada() {
    
    int vivos = 0;
    for (auto j : jogadores_) {
        if (static_cast<JogadorFDP*>(j)->getVidas() > 0) vivos++;
    }
    
    // A vaza acaba quando o número de cartas na mesa for igual ao número de vivos
    return ((int)cartasNaMesa_.size() >= vivos);
}

void MesaFDP::apurarVencedorDaVaza() {
    JuizPaulista juiz;
    
    int indiceVencedorVaza = juiz.decidirVencedor(cartasNaMesa_, cartaVira_, true);
    
    int indiceVencedorReal = ordemJogadoresDaVaza_[indiceVencedorVaza];
    
    JogadorFDP* jogadorVencedor = static_cast<JogadorFDP*>(jogadores_[indiceVencedorReal]);

    jogadorVencedor->adicionarVazaFeita(); // Substitua pelo nome correto do seu método, caso seja outro

    // A regra do truco/FDP diz que quem ganha a vaza é o primeiro a jogar a próxima carta
    jogadorDaVezIndex_ = indiceVencedorReal;

    // Limpa a mesa e o caderninho para a próxima rodada de cartas
    cartasNaMesa_.clear(); 
    ordemJogadoresDaVaza_.clear();
}

void MesaFDP::prepararNovaPartida(int qtdJogadores) {
    jogadores_.clear(); 
    
    // Passando 0 como ID do jogador Humano
    adicionarJogador(new JogadorFDP(0, "Você", 3));

    for (int j = 1; j < qtdJogadores; ++j) {
        // Passando a variável 'j' como ID do Bot
        adicionarJogador(new JogadorFDP(j, "Bot " + std::to_string(j), 3));
    };

    // 3. Prepara as variáveis de controle da rodada 1
    cartasNaRodada_ = 1;
    indicePrimeiro_ = 0;
    cartasSubindo_ = true;
    
    // 4. Embaralha usando o ponteiro do Baralho que a Mesa mãe já tem
    baralho_->embaralhar();

    // 5. Distribui as cartas (Como é a primeira rodada, cartasNaRodada_ vale 1)
    for (auto* jogadorAbstrato : jogadores_) {
        // Converte para FDP
        JogadorFDP* jogador = static_cast<JogadorFDP*>(jogadorAbstrato);
        
        for (int c = 0; c < cartasNaRodada_; ++c) {
            // ATENÇÃO: Substitua 'puxarCarta()' e 'receberCarta()' 
            // pelos nomes exatos que estão no seu Baralho.cpp e Jogador.cpp
            Carta* carta = baralho_->puxarCarta();
            jogador->receberCarta(carta);
        };
    };

    // 6. começar as apostas!
    iniciarFaseApostas();
}

int MesaFDP::getCartasNaRodada() const {
    return cartasNaRodada_;
};

int MesaFDP::getJogadorDaVezIndex() const {
    return jogadorDaVezIndex_;
}