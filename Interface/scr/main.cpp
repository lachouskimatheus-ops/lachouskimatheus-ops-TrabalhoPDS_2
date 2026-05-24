#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>
#include <iostream>
#include "CartaVisual.hpp"
// #include "../../Jogos/Core/CoreBaralho1/cartas.h" // Já está incluído na CartaVisual.hpp

int main() {
    // 1. Cria a janela do teste
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Teste Visual - 3 Cartas");
    window.setFramerateLimit(60);

    // 2. Carrega as 3 texturas da pasta Interface
    sf::Texture tex1, tex2, tex3;
    
    // ATENÇÃO: Ajuste os nomes dos arquivos .png para os nomes exatos que você baixou!
    if (!tex1.openFromFile("../../Interface/assets/cartas/ace_of_hearts.png") ||
        !tex2.openFromFile("../../Interface/assets/cartas/7_of_spades.png") ||
        !tex3.openFromFile("../../Interface/assets/cartas/king_of_diamonds2.png")) {
        std::cout << "Erro ao carregar as imagens! Verifique os nomes e os caminhos." << "\n";
        return -1; // Encerra o teste se não achar as imagens
    };

    // 3. Cria as cartas lógicas do backend 
    // (Ajuste os parâmetros abaixo de acordo com o construtor real da sua classe Carta)
    Carta cartaLogica1(1, 1); // Exemplo: 1=Ás, 1=Copas
    Carta cartaLogica2(7, 2); // Exemplo: 7=Sete, 2=Espadas
    Carta cartaLogica3(13, 3); // Exemplo: 13=Rei, 3=Ouros

    // 4. Cria as cartas visuais, unindo a lógica com a imagem
    CartaVisual cartaVis1(cartaLogica1, tex1);
    CartaVisual cartaVis2(cartaLogica2, tex2);
    CartaVisual cartaVis3(cartaLogica3, tex3);

    // 5. Ajusta a escala para todas (reduzindo a imagem de 500x726 para 100x145)
    sf::Vector2f escala({0.20f, 0.20f});
    cartaVis1.setScale(escala);
    cartaVis2.setScale(escala);
    cartaVis3.setScale(escala);

    // 6. Define as posições na tela (Espaçamento de 20px entre elas)
    // Usando a matemática do simulador para centralizar um bloco de 3 cartas
    float startX = (800.f - ((3 * 100.f) + (2 * 20.f))) / 2.f; // Resultado: 230
    float startY = 600.f - 145.2f - 30.f; // Resultado: 424.8

    cartaVis1.definirPosicao(startX, startY);
    cartaVis2.definirPosicao(startX + 120.f, startY); // 100 de largura + 20 de espaço
    cartaVis3.definirPosicao(startX + 240.f, startY);

    // 7. O Game Loop
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            };
        };

        // Pinta a mesa de verde
        window.clear(sf::Color(34, 139, 34));

        // Desenha as 3 cartas na tela
        cartaVis1.desenhar(window);
        cartaVis2.desenhar(window);
        cartaVis3.desenhar(window);

        window.display();
    };

    return 0;
};