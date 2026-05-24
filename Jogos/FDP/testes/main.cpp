#include <SFML/Graphics.hpp>
#include <iostream>
#include "TelaFDP.hpp"

int main() {
    // 1. Cria a janela do SFML 3
    sf::RenderWindow janela(sf::VideoMode({1024, 768}), "Teste Isolado - FDP");
    janela.setFramerateLimit(60);

    // 2. Carrega a fonte (A TelaBase da sua equipe exige uma fonte no construtor)
    sf::Font fonte;
    // Lembre-se: SFML 3 usa openFromFile para fontes! Ajuste o caminho se necessário
    if (!fonte.openFromFile("../../Interface/assets/fonte/PokerInOctober-Demo.otf")) {
        std::cout << "Erro ao carregar a fonte!" << "\n";
        return -1;
    };

    // 3. Instancia a nossa máquina de estados principal
    TelaFDP telaJogo(fonte, janela);

    // 4. O Game Loop Padrão
    while (janela.isOpen()) {
        
        // Loop de Eventos do SFML 3
        while (const std::optional<sf::Event> event = janela.pollEvent()) {
            
            if (event->is<sf::Event::Closed>()) {
                janela.close();
            };
            
            // Se o mouse for clicado, converte a posição e manda para a TelaFDP
            if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseClick->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePosJanela = sf::Mouse::getPosition(janela);
                    sf::Vector2f mousePosMundo = janela.mapPixelToCoords(mousePosJanela);
                    
                    telaJogo.processarClique(mousePosMundo);
                };
            };

            // Repassa outros eventos (como o botão Esc para pausar)
            telaJogo.processarEvento(*event);
        };

        // 5. Atualiza o Cérebro do Bot (Roda a cada frame)
        telaJogo.atualizar();

        // 6. Desenha tudo na tela
        telaJogo.desenhar();
        janela.display();
    };

    return 0;
}