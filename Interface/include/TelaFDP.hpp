#ifndef TELAFDP_HPP
#define TELAFDP_HPP

#include "TelaBase.hpp"
#include "Botao.hpp"
#include "CartaVisual.hpp"
#include "../../Jogos/FDP/include/MesaFDP.hpp" // O caminho para backend
#include <vector>

// Os estados visuais do jogo
enum class FaseFDP {
    SELECIONANDO_JOGADORES,
    DISTRIBUINDO_CARTAS, 
    FASE_APOSTAS,
    FASE_JOGADAS,
    FIM_DE_RODADA
};

class TelaFDP : public TelaBase {
private:
    FaseFDP faseAtual_;
    MesaFDP mesaLogica_; 
    
    int quantidadeJogadores_;
    int quantidadeBots_;

    // Elementos Visuais
    std::vector<Botao> botoesSelecao_;
    std::vector<Botao> botoesAposta_;
    std::vector<CartaVisual> maoHumano_;
    
    // Relógio para controle
    sf::Clock relogioAnimacao_;

public:
    TelaFDP(sf::Font& fonte, sf::RenderWindow& janela); 

    // Métodos obrigatórios herdados da TelaBase
    void desenhar() override;
    void processarClique(sf::Vector2f mouse) override;
    void processarEvento(const sf::Event& evento) override;
    
    // Roda a cada frame para calcular o tempo dos Bots e as animações
    void atualizar(); 

private:
    // Funções auxiliares para organizar a classe
    void gerarBotoesAposta();
    void inicializarBotoes();
    void realizarJogadaBot();
    void sincronizarMaoVisual();
};

#endif