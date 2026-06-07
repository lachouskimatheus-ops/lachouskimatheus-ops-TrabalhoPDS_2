#pragma once
#include <vector>
#include "Jogador_Truco.hpp"
#include "Baralho.hpp"
#include "Juiz.hpp"  
#include "Carta.hpp"

class Mesa {
    private:

        std::vector<Jogador_Truco*> jogadores;
        std::vector<Carta*> cartasNaMesa; 
        Carta* vira; 

        Baralho* baralho;
        Juiz* juiz;

        int pontosEquipe1;
        int pontosEquipe2;

        // Pontuação da mão atual
        int valorAtualMao;       // Quanto vale a mão no momento (1, 3, 6, 9 ou 12)
        int nivelTruco;          // 0=normal, 1=truco(3), 2=seis(6), 3=nove(9), 4=doze(12)

        // Retorna a equipe do jogador (1 ou 2) baseado no índice
        // Jogadores 0 e 2 = equipe 1, jogadores 1 e 3 = equipe 2
        int getEquipeDoJogador(int indiceJogador) const;

        // Lida com o pedido de truco. Retorna a equipe que ganhou por recusa (1 ou 2),
        // ou 0 se o pedido foi aceito.
        int processarPedidoTruco(int indiceJogadorPedindo);

    public:
        Mesa(Juiz* juizEscolhido, Baralho* baralhoEscolhido);

        ~Mesa();

        void adicionarJogador(Jogador_Truco* j);

        void prepararRodada();

        // Joga uma mão completa (até 3 quedas). Retorna a equipe vencedora (1 ou 2), ou 0 se empate total.
        int jogarTurno();

        // Roda partidas até alguém chegar a 12 pontos
        void jogarPartida();

        int determinarVencedorDaQueda();

        void exibirMesa() const;
        void exibirPlacar() const;
};