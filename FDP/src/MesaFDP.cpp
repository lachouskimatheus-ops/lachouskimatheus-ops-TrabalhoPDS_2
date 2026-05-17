#include "MesaFDP.hpp"
#include "JuizPaulista.hpp"

void MesaFDP::recolherCartas() {
	cartasNaMesa_.clear();

	int indice_atual = indiceVez_;
	int qtd_jogadores = jogadores_.size();

	for(int i = 0; i < qtd_jogadores; i++) {
    JogadorFDP* jogadorAtual = static_cast<JogadorFDP*>(jogadores_[indice_atual]); //aqui tem um casting porque o compilador é burro
    
    std::cout << "\nVez de: " << jogadorAtual->getNome() << std::endl;
    
    int qtdCartasMao = jogadorAtual->getQtdCartasMao();
    int cartaEscolhida;

    //Rodada cega
    if (cartasNaRodada_ == 1) {
        std::cout << "[RODADA CEGA] Você tira a carta da sua testa e joga na mesa!" << std::endl;
        cartaEscolhida = 1; // Como ele só tem 1 carta, o sistema escolhe automaticamente
    } 
    //Rodada normal
    else {
        jogadorAtual->mostrarMao();
        
        std::cout << "Escolha a carta que quer jogar: ";
        std::cin >> cartaEscolhida;
        
        while((cartaEscolhida < 1) || (cartaEscolhida > qtdCartasMao)) {
            std::cout << "Carta invalida, selecione uma carta que você tenha!" << std::endl;
            std::cin >> cartaEscolhida;
        };
    };

		Carta* cartaMao = jogadorAtual->jogarCarta(cartaEscolhida - 1);
		cartasNaMesa_.push_back(cartaMao);

		indice_atual = (indice_atual + 1) % qtd_jogadores; //Ciclar os indices, já que a divisão de um número < qtd_jogadores sempre resulta em resto igual aquele número, e quando for múltiplo a situação é semelhante.
	};
}

void MesaFDP::resolverVaza() {
	recolherCartas();


	
	//Utilizando um método já implementado
	JuizPaulista juizRodada;
	int indiceVencedor = juizRodada.decidirVencedor(cartasNaMesa_, cartaVira_, true);

	int indiceGanhadorReal = (indicePrimeiro_ + indiceVencedor) % jogadores_.size();

	JogadorFDP* ganhador = static_cast<JogadorFDP*>(jogadores_[indiceGanhadorReal]);
	ganhador->registrarGanhador();
	indicePrimeiro_ = indiceGanhadorReal;
}

MesaFDP::~MesaFDP() {
}

MesaFDP::MesaFDP(BaralhoSujo* baralho, Placar* placar) : Mesa(baralho) {
    placar_partida_ = placar;
    indicePrimeiro_ = 0;
    cartasNaRodada_ = 1; 
    cartasSubindo_ = true;
}

void MesaFDP::faseApostas() {
	int totalApostas = 0;
	int qtd_jogadores = jogadores_.size();

	int indiceUltimo = (indicePrimeiro_ - 1 + qtd_jogadores) % qtd_jogadores;

	int indiceAtual = indicePrimeiro_;

	std::cout << "\n---> Fase de apostas! <--- " << std::endl;

	for (int i = 0; i < qtd_jogadores; i++) {
		JogadorFDP* jogadorDaVez = static_cast<JogadorFDP*>(jogadores_[indiceAtual]);

		if (jogadorDaVez->getVidas() <= 0) {
			indiceAtual = (indiceAtual + 1) % qtd_jogadores;
			continue;
		};

		std::cout << "Vez de " << jogadorDaVez->getNome() << " apostar." << std::endl;
		
        if (cartasNaRodada_ == 1) {
            std::cout << "[RODADA CEGA] Sua carta está na testa!\n";
            std::cout << "--- CARTAS DOS ADVERSARIOS ---\n";
            
            //Rodada cega
            for (int k = 0; k < qtd_jogadores; k++) {
                // Só mostra se for um adversário E se o adversário estiver vivo
                JogadorFDP* adv = static_cast<JogadorFDP*>(jogadores_[k]);
                if (k != indiceAtual && adv->getVidas() > 0) { 
                    std::cout << adv->getNome() << " tem na testa:\n";
                    adv->mostrarMao();
                }
            }
            std::cout << "------------------------------\n";
        } else {
            //Rodada nomrmal
            jogadorDaVez->mostrarMao();
        }
        

		int apostaEscolhida;
		int maxPossivel = cartasNaRodada_;

		if (indiceAtual == indiceUltimo) {
            int apostaProibida = cartasNaRodada_ - totalApostas;
            
            // Verifica se a aposta proibida é um número que o jogador realmente poderia digitar
            bool temRestricao = (apostaProibida >= 0 && apostaProibida <= maxPossivel);

            if (temRestricao) {
                std::cout << "[REGRA DO FDP] -> Você nao pode apostar: " << apostaProibida << std::endl;
            }
            
            do {
                std::cout << "Digite sua aposta (0 a " << maxPossivel << "): ";
                std::cin >> apostaEscolhida;
                
                if (temRestricao && apostaEscolhida == apostaProibida) {
                    std::cout << "Aposta inválida! A soma bate com as cartas da rodada. Tente de novo.\n";
                } else if (apostaEscolhida < 0 || apostaEscolhida > maxPossivel) {
                    std::cout << "Valor fora do limite da rodada.\n";
                }
            
            // Repete se ele tentar a aposta proibida (caso ela exista) ou se digitar um número absurdo
            } while ((temRestricao && apostaEscolhida == apostaProibida) || apostaEscolhida < 0 || apostaEscolhida > maxPossivel);

        } else {

			do {
				std::cout << "Digite sua aposta (0 a " << maxPossivel << "): ";
				std::cin >> apostaEscolhida;

				if (apostaEscolhida < 0 || apostaEscolhida > maxPossivel) {
					std::cout << "Valor fora do limite da rodada.\n";
				};

			} while (apostaEscolhida < 0 || apostaEscolhida > maxPossivel);
		};

		jogadorDaVez->fazerAposta(apostaEscolhida);
		totalApostas += apostaEscolhida;

		indiceAtual = (indiceAtual + 1) % qtd_jogadores;
	};
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

void MesaFDP::iniciarPartida() {
	bool jogoAcabou = false;

	while (!jogoAcabou) {
		std::cout << "\n--- INICIANDO RODADA COM " << cartasNaRodada_ << " CARTAS ---" << std::endl;
		Carta* ponteiroVira = baralho_->puxarCarta();
		cartaVira_ = *ponteiroVira;
		delete ponteiroVira;

		std::cout << "O vira é: ";
		cartaVira_.imprimir();
		std::cout << std::endl;


		//Distribuição da mão de cartas
		for (auto j : jogadores_) {
		JogadorFDP* jogadorDaVez = static_cast<JogadorFDP*>(j); //Convertendo cada jogador em um jogador FDP 
		jogadorDaVez->prepararNovaRodada(); //zerando seus stats iniciais

		for (int c = 0; c < cartasNaRodada_; c++) {
			Carta* cartaPuxada = baralho_->puxarCarta();
			jogadorDaVez->receberCarta(cartaPuxada); //Guarda a carta na mão dele

			};
		};

		//Apostas
		faseApostas();

		//Combate
		for(int rodada = 0; rodada < cartasNaRodada_; rodada++) {
			std::cout << "\n--- RODADA " << rodada + 1 << " ---" << std::endl;
			resolverVaza();
		};

		//Apuração dos resultados
		system("clear"); // Dá aquele respiro na tela antes da grande revelação
        std::cout << "\n\n---> FIM DA RODADA! <---\n";

        for (size_t i = 0; i < jogadores_.size(); i++) {
            
            JogadorFDP* jogador = static_cast<JogadorFDP*>(jogadores_[i]);
            
            // ⚠️ ATENÇÃO: Troque os nomes abaixo para os getters reais que você criou na sua classe!
            int apostou = jogador->getAposta(); 
            int fez = jogador->getVezesGanhas(); 
            
            if (apostou == fez) {
                std::cout << "[SALVO] " << jogador->getNome() << " acertou a aposta de quebradinha!\n";
            } else {
                std::cout << "[TOMOU] " << jogador->getNome() << " apostou " << apostou 
                          << " mas fez " << fez << " rodada(s). Perdeu uma vida!\n";
            }
        }

        std::cout << "\n"; 

        placar_partida_->calcularResultados(jogadores_);
        placar_partida_->exibirPlacar(jogadores_);
		
		//Meio autoexplicativo
		atualizarQtdCartas();

		//Verifica se o jogo acabou
		if(placar_partida_->jogadoresVivos(jogadores_) <= 1) {
			jogoAcabou = true;
		}
	};
}