#ifndef CARTAVISUAL_HPP
#define CARTAVISUAL_HPP

#include <SFML/Graphics.hpp>
//#include "../../Jogos/Core/CoreBaralho1/cartas.h"
#include "../../Jogos/Core/CoreBaralhoTruco/Carta.hpp"

class CartaVisual {
private:
    Carta carta_;
    sf::Sprite sprite_;
    float largura_;
    float altura_;
public:
    CartaVisual(const Carta& carta, const sf::Texture& textura);

    void setScale(sf::Vector2f escala);

    void definirPosicao(float x, float y);

    void desenhar(sf::RenderWindow& janela);

    bool foiClicada(sf::Vector2f mouse) const;

    Carta mostrarCarta() const;
};

#endif