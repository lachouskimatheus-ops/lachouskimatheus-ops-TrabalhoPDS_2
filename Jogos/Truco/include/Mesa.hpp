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

    public:
        Mesa(Juiz* juizEscolhido, Baralho* baralhoEscolhido);

        ~Mesa();

        void adicionarJogador(Jogador_Truco* j);

        void prepararRodada();

        void jogarTurno();

        int determinarVencedorDaQueda();

        void exibirMesa() const;
};