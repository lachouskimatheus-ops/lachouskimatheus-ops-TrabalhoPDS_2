#ifndef BOTAO_HPP
#define BOTAO_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Botao {
private:
    sf::RectangleShape fundo_;
    sf::Text texto_;

    float largura_;
    float altura_;

public:
    Botao(const std::string& texto, sf::Font& fonte); //define o texto que o botão vai ter, e qual fonte vai usar

    void definirPosicao(float x, float y); //define onde vai ficar na tela


    void desenharBotao(sf::RenderWindow& janela); //desenha o botao na tela


    bool foiClicado(sf::Vector2f mouse) const; //identifica se foi clicado
};

#endif