#include "TelaFDP.hpp"
#include <iostream>
#include "../../Jogos/FDP/include/BaralhoSujo.hpp"
#include "../../Jogos/FDP/include/Placar.hpp"
#include "GerenciadorTexturas.hpp"

TelaFDP::TelaFDP(sf::Font& fonte, sf::RenderWindow& janela) 
    : TelaBase(fonte, janela), 
      faseAtual_(FaseFDP::SELECIONANDO_JOGADORES),
      mesaLogica_(new BaralhoSujo(), new Placar()) 
{
    inicializarBotoes();
};

void TelaFDP::inicializarBotoes() {
    float centroX = janela_.getSize().x / 2.f;
    float startY = 250.f;

    botoesSelecao_.emplace_back("2 Jogadores (1v1)", fonte_);
    botoesSelecao_.emplace_back("3 Jogadores (1v2)", fonte_);
    botoesSelecao_.emplace_back("4 Jogadores (1v3)", fonte_);

    //Posiciona os botões na tela
    botoesSelecao_[0].definirPosicao(centroX - 100.f, startY);
    botoesSelecao_[1].definirPosicao(centroX - 100.f, startY + 80.f);
    botoesSelecao_[2].definirPosicao(centroX - 100.f, startY + 160.f);
};

void TelaFDP::desenhar() {
    janela_.clear(sf::Color(34, 139, 34));

    if (faseAtual_ == FaseFDP::SELECIONANDO_JOGADORES) {
        sf::Text titulo(fonte_, "Selecione a quantidade de jogadores", 30);
        titulo.setFillColor(sf::Color::White);
        sf::FloatRect limites = titulo.getLocalBounds();
        titulo.setOrigin({limites.size.x / 2.f, limites.size.y / 2.f});
        titulo.setPosition({janela_.getSize().x / 2.f, 100.f});
        janela_.draw(titulo);

        for (auto& botao : botoesSelecao_) {
            botao.desenharBotao(janela_);
        };
    }
    else if (faseAtual_ == FaseFDP::FASE_APOSTAS || faseAtual_ == FaseFDP::FASE_JOGADAS) {
        // Desenha a mão do jogador humano sempre visível na parte inferior
        for (auto& carta : maoHumano_) {
            carta.desenhar(janela_);
        };
        
        if (faseAtual_ == FaseFDP::FASE_APOSTAS) {
            // Se for a vez do humano, mostra o painel de apostas empilhado na direita
            if (mesaLogica_.getJogadorDaVezIndex() == 0) {
                sf::Text textoPainel(fonte_, "Quantas\nvinco/vazas\nvocê faz?", 22);
                textoPainel.setFillColor(sf::Color::Yellow);
                textoPainel.setPosition({janela_.getSize().x - 220.f, 80.f});
                janela_.draw(textoPainel);

                for (auto& botao : botoesAposta_) {
                    botao.desenharBotao(janela_);
                };
            } else {
                // Se for a vez do bot, mostra um texto de espera centralizado
                sf::Text textoEspera(fonte_, "Bots estão apostando...", 26);
                textoEspera.setFillColor(sf::Color::White);
                textoEspera.setPosition({350.f, 300.f});
                janela_.draw(textoEspera);
            };
        };
    };
}


void TelaFDP::processarClique(sf::Vector2f mouse) {
    if (faseAtual_ == FaseFDP::SELECIONANDO_JOGADORES) {
        for (size_t i = 0; i < botoesSelecao_.size(); ++i) {
            if (botoesSelecao_[i].foiClicado(mouse)) { 
                quantidadeJogadores_ = i + 2; 
                quantidadeBots_ = quantidadeJogadores_ - 1;

                mesaLogica_.prepararNovaPartida(quantidadeJogadores_);
                sincronizarMaoVisual();
                
                //Gera as opções de aposta logo após iniciar a partida
                gerarBotoesAposta(); 
                
                faseAtual_ = FaseFDP::FASE_APOSTAS;
                break;
            };
        };
    }
    else if (faseAtual_ == FaseFDP::FASE_APOSTAS) {
        // Só processa o clique se for a vez do humano (Jogador 0)
        if (mesaLogica_.getJogadorDaVezIndex() == 0) {
            for (size_t i = 0; i < botoesAposta_.size(); ++i) {
                if (botoesAposta_[i].foiClicado(mouse)) {
                    
                    // Tenta registrar a aposta no backend
                    if (mesaLogica_.registrarAposta(static_cast<int>(i))) {
                        std::cout << "Você apostou que faz " << i << " vaza(s)!\n";
                        
                        // Se você foi o último a apostar e a fase fechou
                        if (mesaLogica_.faseApostasFinalizada()) {
                            faseAtual_ = FaseFDP::FASE_JOGADAS;
                            mesaLogica_.iniciarFaseDeCartas();
                        };
                        break;
                    } else {
                        std::cout << "Aposta proibida pela regra do FDP!\n";
                    };
                };
            };
        };
    };
}

void TelaFDP::processarEvento(const sf::Event& evento) {
    TelaBase::processarEvento(evento);
};

void TelaFDP::atualizar() {
    if (faseAtual_ == FaseFDP::FASE_APOSTAS) {
        
        int indexDaVez = mesaLogica_.getJogadorDaVezIndex();

        //Se for diferente de 0 é vez do PC
        if (indexDaVez != 0) { 
            
            // Espera 1.5 segundos
            if (relogioAnimacao_.getElapsedTime().asSeconds() > 1.5f) {
                
                // Pega os limites da aposta
                // (Nota: você precisará criar um 'getCartasNaRodada()' no MesaFDP.hpp se ainda não tiver)
                int maxPossivel = mesaLogica_.getCartasNaRodada(); 
                int apostaProibida = mesaLogica_.getApostaProibida();
                
                int apostaBot;
                
                do {
                    apostaBot = rand() % (maxPossivel + 1);
                } while (apostaBot == apostaProibida);

                mesaLogica_.registrarAposta(apostaBot);
                std::cout << "Bot " << indexDaVez << " apostou que faz " << apostaBot << " vaza(s)!\n";
                
                if (mesaLogica_.faseApostasFinalizada()) {
                    faseAtual_ = FaseFDP::FASE_JOGADAS;
                    mesaLogica_.iniciarFaseDeCartas(); // Limpa a mesa para as vazas
                    std::cout << "--- FASE DE JOGADAS INICIADA ---\n";
                };

                // Zera o cronômetro para o próximo bot pensar
                relogioAnimacao_.restart();
            };
        };
    };
};

void TelaFDP::sincronizarMaoVisual() {
    maoHumano_.clear(); // Limpa a mão da rodada anterior

    JogadorFDP* humano = static_cast<JogadorFDP*>(mesaLogica_.getJogadores()[0]);

    for (int i = 0; i < humano->getQtdCartasMao(); ++i) {
        
        // Puxa a carta lógica usando o método novo que acabamos de criar
        Carta* cartaLogica = humano->getCartaMao(i); 
        
        // Pede a textura instantaneamente para o nosso Singleton
        sf::Texture& tex = GerenciadorTexturas::getInstancia().getTexturaCarta(cartaLogica);
        
        CartaVisual cartaVis(*cartaLogica, tex);
        cartaVis.setScale({0.20f, 0.20f});
        
        maoHumano_.push_back(cartaVis);
    };

    // Centraliza as cartas na parte inferior da tela
    float espacamento = 20.0f;
    float larguraFinal = 100.f; // 500px da textura original * 0.20 de escala
    float larguraTotalMao = (maoHumano_.size() * larguraFinal) + ((maoHumano_.size() - 1) * espacamento);
    float startX = (janela_.getSize().x - larguraTotalMao) / 2.f;
    float startY = janela_.getSize().y - 145.2f - 30.f;

    for (size_t i = 0; i < maoHumano_.size(); ++i) {
        float posX = startX + (i * (larguraFinal + espacamento));
        maoHumano_[i].definirPosicao(posX, startY);
    };
}

void TelaFDP::gerarBotoesAposta() {
    botoesAposta_.clear();
    
    int maxCartas = mesaLogica_.getCartasNaRodada();
    int proibida = mesaLogica_.getApostaProibida();

    // Posiciona no canto direito: largura da janela menos o tamanho do botão (200px) e uma margem (20px)
    float posX = janela_.getSize().x - 220.f; 
    float startY = 180.f;

    for (int i = 0; i <= maxCartas; ++i) {
        std::string textoBotao = std::to_string(i);
        
        // Se for a aposta proibida pela regra do FDP, podemos sinalizar no texto
        if (i == proibida) {
            textoBotao += " (X)";
        };

        botoesAposta_.emplace_back(textoBotao, fonte_);
        botoesAposta_.back().definirPosicao(posX, startY + (i * 75.f)); // 60px de altura + 15px de espaçamento
    };
}