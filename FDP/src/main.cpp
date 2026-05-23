#include <iostream>
#include "../include/MesaFDP.hpp"
#include "../include/JogadorFDP.hpp"
#include "../include/BaralhoSujo.hpp"
#include "../include/Placar.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

enum class EstadoJogo {
    DISTRIBUINDO_CARTAS,
    FASE_APOSTAS,
    FASE_JOGADAS,
    RESOLVENDO_VAZA,
    FIM_DE_RODADA,
    FIM_DE_PARTIDA
};

int main() {
    // SFML 3: Exige chaves {} para criar um Vector2u
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Cassino FDP - Teste SFML 3");
    window.setFramerateLimit(60);

    sf::Font font;
    // SFML 3: O método loadFromFile foi renomeado para openFromFile
    bool fonteCarregada = font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");

    // SFML 3: A ordem dos parâmetros mudou (A fonte vem primeiro)
    sf::Text texto(font, "Mesa do Cassino Aberta!", 30);
    texto.setFillColor(sf::Color::White);
    // SFML 3: Exige chaves {} e o sufixo 'f' (float) para criar um Vector2f
    texto.setPosition({200.f, 250.f});

    while (window.isOpen()) {
        // SFML 3: O pollEvent agora retorna um std::optional moderno
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // Verifica o tipo do evento de forma segura
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color(34, 139, 34));

        if (fonteCarregada) {
            window.draw(texto);
        }

        window.display();
    }

    return 0;
}