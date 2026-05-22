#ifndef CARTAVISUAL_HPP
#define CARTAVISUAL_HPP

#include <SFML/Graphics.hpp>

#include "../../Jogos/Core/CoreBaralho1/cartas.h"

class CartaVisual {

private:
    Carta carta_;
    sf::RectangleShape fundo_;
    sf::Text texto_;

    float largura_;
    float altura_;
public:
    CartaVisual(const Carta& carta, sf::Font& fonte); //recebe a carta do cartas.h, e cria o visual

    void definirPosicao(float x, float y);

    void desenhar(sf::RenderWindow& janela); //desenha a carta na janela

    bool foiClicada(sf::Vector2f mouse) const; //verifica se o mause clicou

    Carta mostrarCarta() const; //retorna a carta do carta.h
};

#endif