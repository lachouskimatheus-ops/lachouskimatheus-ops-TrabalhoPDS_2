#include <SFML/Graphics.hpp>
#include "TelaMenu.hpp"
#include "TelaBase.hpp"

int main() {
    sf::RenderWindow janela(sf::VideoMode({900, 700}), "Jogos");
    janela.setFramerateLimit(60);

    sf::Font fonte;
    if (!fonte.openFromFile("Interface/assets/fonte/PokerInOctober-Demo.otf")) {
        return -1;
    }

    TelaBase* telaAtual = new TelaMenu(fonte, janela);

    while (janela.isOpen()) {
        while (const std::optional evento = janela.pollEvent()) {
            if (evento->is<sf::Event::Closed>())
                janela.close();

            telaAtual->processarEvento(*evento);

            if (const auto* click = evento->getIf<sf::Event::MouseButtonPressed>()) {
                telaAtual->processarClique(
                    janela.mapPixelToCoords(sf::Mouse::getPosition(janela)));
            }
        }

        if (telaAtual->getEstado() == EstadoTela::Sair)
            janela.close();

        telaAtual->desenhar();
        janela.display();
    }

    delete telaAtual;
    return 0;
}