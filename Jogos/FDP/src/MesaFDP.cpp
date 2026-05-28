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

    // Na rodada cega (1 carta), o índice só pode ser 0
    if (cartasNaRodada_ == 1) {
        indiceCartaNaMao = 0; 
    } 

    // Validação de segurança: o índice não pode ser menor que 0 nem maior que o tamanho da mão
    if (indiceCartaNaMao < 0 || indiceCartaNaMao >= jogador->getQtdCartasMao()) {
        std::cout << "[ERRO] Indice invalido. Jogador tem " << jogador->getQtdCartasMao() << " cartas, pediu indice " << indiceCartaNaMao << std::endl;
        return false; 
    }

    // ATENÇÃO AQUI: Garanta que o método no seu Jogador.cpp aceita o índice a partir do zero
    Carta* cartaJogada = jogador->jogarCarta(indiceCartaNaMao);
    
    // Se por algum motivo bizarro a carta não existir, aborta a jogada
    if (cartaJogada == nullptr) {
        return false;
    }

    // Anotando a ordem de jogada e botando a carta na mesa
    ordemJogadoresDaVaza_.push_back(jogadorDaVezIndex_);
    cartasNaMesa_.push_back(cartaJogada);

    // Passa a vez para o próximo jogador vivo
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

int MesaFDP::getCartasNaRodada() const {
    return cartasNaRodada_;
};

int MesaFDP::getJogadorDaVezIndex() const {
    return jogadorDaVezIndex_;
}

nlohmann::json MesaFDP::paraJson() const {
    nlohmann::json estadoMesa;
    
    // 1. Variáveis de controlo do fluxo do jogo
    estadoMesa["jogador_da_vez_index"] = jogadorDaVezIndex_;
    estadoMesa["cartas_na_rodada"] = cartasNaRodada_;
    estadoMesa["total_apostas_rodada"] = totalApostasRodada_;
    estadoMesa["jogadores_que_ja_apostaram"] = jogadoresQueJaApostaram_;

    estadoMesa["aposta_proibida"] = getApostaProibida();
    
    // 2. Serialização da carta Vira da rodada atual
    estadoMesa["carta_vira"] = cartaVira_.paraJson();
    
    // 3. Mapeamento das cartas que já foram jogadas na mesa
    nlohmann::json jsonCartasMesa = nlohmann::json::array();
    for (Carta* carta : cartasNaMesa_) {
        jsonCartasMesa.push_back(carta->paraJson());
    };
    estadoMesa["cartas_na_mesa"] = jsonCartasMesa;
    
    // 4. Mapeamento de todos os jogadores ligados à mesa
    nlohmann::json jsonJogadores = nlohmann::json::array();
    for (Jogador* j : jogadores_) {
        JogadorFDP* jFDP = dynamic_cast<JogadorFDP*>(j);
        if (jFDP != nullptr) {
            jsonJogadores.push_back(jFDP->paraJson());
        };
    };
    estadoMesa["jogadores"] = jsonJogadores;
    
    return estadoMesa;
}

void MesaFDP::iniciarRodada() {
    // 1. Limpa a mesa de rodadas anteriores
    cartasNaMesa_.clear();
    ordemJogadoresDaVaza_.clear();
    
    // 2. Prepara e embaralha o baralho sujo (40 cartas)
    static_cast<BaralhoSujo*>(baralho_)->inicializar(); 
    baralho_->embaralhar();

    // 3. Puxa a carta Vira da rodada
    Carta* ponteiroVira = baralho_->puxarCarta();
    if (ponteiroVira != nullptr) {
        cartaVira_ = *ponteiroVira; // Salva o valor da carta para a Juiz analisar depois
    }

    // 4. Distribui as cartas para os jogadores VIVOS
    for (Jogador* j : jogadores_) {
        JogadorFDP* jogadorFDP = static_cast<JogadorFDP*>(j);
        
        // Limpa a mão anterior e prepara a nova
        jogadorFDP->prepararNovaRodada();
        
        // Se o jogador estiver vivo, recebe as cartas da rodada
        if (jogadorFDP->getVidas() > 0) {
            for (int c = 0; c < cartasNaRodada_; ++c) {
                Carta* cartaDaVez = baralho_->puxarCarta();
                if (cartaDaVez != nullptr) {
                    jogadorFDP->receberCarta(cartaDaVez);
                }
            }
        }
    }

    // 5. Inicia a fase de apostas apontando para o jogador correto
    iniciarFaseApostas();
}

void MesaFDP::iniciarPartida() {
    // Reseta o estado geral para começar um jogo do zero
    indicePrimeiro_ = 0;
    cartasNaRodada_ = 1;
    cartasSubindo_ = true;
    
    // Inicia a primeira rodada
    iniciarRodada();
}

bool MesaFDP::rodadaFinalizada() const {
    for (auto* j : jogadores_) {
        JogadorFDP* jogador = static_cast<JogadorFDP*>(j);
        // Se houver pelo menos um jogador VIVO que ainda tenha cartas na mão, a rodada continua
        if (jogador->getVidas() > 0 && jogador->getQtdCartasMao() > 0) {
            return false;
        }
    }
    return true; // Se ninguém tem carta, a rodada acabou!
}

void MesaFDP::finalizarRodada() {
    // 1. A Hora da Verdade: Desconta as vidas de quem errou a aposta
    for (auto* j : jogadores_) {
        JogadorFDP* jogador = static_cast<JogadorFDP*>(j);
        if (jogador->getVidas() > 0) {
            // Calcula o módulo da diferença (ex: apostou 2, fez 0 = perde 2 vidas)
            int diferenca = std::abs(jogador->getAposta() - jogador->getVezesGanhas());
            
            if (diferenca > 0) {
                // ATENÇÃO: Verifique se o seu método é setVidas() ou removerVidas() no seu código real
                jogador->setVidas(jogador->getVidas() - diferenca);
            }
        }
    }

    // 2. Verifica se houve um vencedor final do jogo (apenas 1 ou 0 sobreviventes)
    int vivos = 0;
    for (auto* j : jogadores_) {
        if (static_cast<JogadorFDP*>(j)->getVidas() > 0) vivos++;
    }
    if (vivos <= 1) {
        std::cout << "GAME OVER! Temos um vencedor definitivo." << std::endl;
        return; // Para o loop da rodada aqui
    }

    // 3. Atualiza a quantidade de cartas (a sua função que sobe e desce as cartas)
    atualizarQtdCartas();

    // 4. Passa a ficha do "Dealer" (quem começa as apostas) para o próximo jogador vivo
    do {
        indicePrimeiro_ = (indicePrimeiro_ + 1) % jogadores_.size();
    } while (static_cast<JogadorFDP*>(jogadores_[indicePrimeiro_])->getVidas() <= 0);

    // 5. Inicia o novo ciclo (embaralha, dá as cartas e abre as apostas)
    iniciarRodada();
}