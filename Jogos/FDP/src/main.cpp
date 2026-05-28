#include <iostream>

// Includes do Core do Jogo apontando para a pasta correta
#include <iostream>

// Voltamos apenas UMA pasta (..) para sair de src/ e entramos em include/
#include "../include/BaralhoSujo.hpp"
#include "../include/Placar.hpp"
#include "../include/MesaFDP.hpp"
#include "../include/JogadorFDP.hpp"
#include "../include/Servidor.hpp"
// Include do Servidor de Rede
#include "../include/Servidor.hpp"

int main() {
    std::cout << "Inicializando o servidor FDP (Fodinha)..." << std::endl;

    // 1. Instancia as dependências vitais do jogo
    BaralhoSujo baralho;
    Placar placar;
    
    // 2. Cria a mesa passando o baralho e o placar
    MesaFDP mesa(&baralho, &placar);
    
    // 3. Adiciona os jogadores do seu trio
    // ATENÇÃO: É vital que os IDs sejam 0, 1 e 2 para bater perfeitamente 
    // com a nossa lógica do app.js no frontend!
    mesa.adicionarJogador(new JogadorFDP(0, "Jogador 1"));
    mesa.adicionarJogador(new JogadorFDP(1, "Jogador 2"));
    mesa.adicionarJogador(new JogadorFDP(2, "Jogador 3"));
    mesa.adicionarJogador(new JogadorFDP(3, "Jogador 4"));
    
    // Inicia a primeira rodada do jogo
    mesa.iniciarPartida();

    // 4. Inicia o servidor e passa a mesa para ele gerenciar
    Servidor servidor(&mesa);
    
    // O terminal vai ficar "travado" nesta linha escutando o navegador
    servidor.iniciar(8080); 

    return 0;
}