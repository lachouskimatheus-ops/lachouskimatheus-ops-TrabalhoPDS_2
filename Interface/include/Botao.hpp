#ifndef BOTAO_HPP
#define BOTAO_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include "RoundedRect.hpp"

class Botao {
private:
    RoundedRect fundo_;
    sf::Text texto_;

    std::optional<sf::Texture> textura_; 
    std::optional<sf::Sprite>  sprite_;
    
    float largura_;
    float altura_;

public:
    //Construtor com texto s
    Botao(const std::string& texto, sf::Font& fonte); //define o texto que o botão vai ter, e qual fonte vai usar

    // Construtor com imagem
    Botao(const std::string& caminhoImagem, float largura, float altura, sf::Font& fonte);

    void definirPosicao(float x, float y); //define onde vai ficar na tela


    void desenharBotao(sf::RenderWindow& janela); //desenha o botao na tela


    bool foiClicado(sf::Vector2f mouse) const; //identifica se foi clicado
};

#endif