#include "Botao.hpp"

Botao::Botao(const std::string& texto, sf::Font& fonte)
    : fundo_({200.f, 60.f}, 15.f), // tamanho do butao e o arredondamento dos cantos
     texto_(fonte, texto, 28)  // SFML 3: fonte vai no construtor
{
    largura_ = 200.f; //parametros para o tamnho do botao, da pra mudar se precisar
    altura_  = 60.f;

    fundo_.setFillColor(sf::Color(30, 120, 30)); //cor do botao
    fundo_.setOutlineColor(sf::Color::White); //cor da borda
    fundo_.setOutlineThickness(2.f); //espessura da borda
    texto_.setFillColor(sf::Color::White); 
    sf::FloatRect areaTexto = texto_.getLocalBounds(); //calcula os parametros do texto para centralizar

    texto_.setOrigin({
        areaTexto.position.x + areaTexto.size.x / 2.f, //define o centro do texto como ponto de referência
        areaTexto.position.y + areaTexto.size.y / 2.f
    });

    texto_.setPosition({largura_ / 2.f, altura_ / 2.f}); //centraliza o texto
}


// Construtor com imagem
Botao::Botao(const std::string& caminhoImagem, float largura, float altura, sf::Font& fonte)
    : fundo_({largura, altura}, 15.f),
      texto_(fonte, "", 28)
{
    largura_ = largura;
    altura_  = altura;

    fundo_.setFillColor(sf::Color::Transparent);
    fundo_.setOutlineColor(sf::Color::Transparent);
    fundo_.setOutlineThickness(0.f);

    textura_.emplace();
    if (textura_->loadFromFile(caminhoImagem)) {
        sprite_.emplace(*textura_);
        // Escala a imagem para caber no botão
        float scaleX = largura_ / textura_->getSize().x;
        float scaleY = altura_  / textura_->getSize().y;
        sprite_->setScale({scaleX, scaleY});
    }
}


void Botao::definirPosicao(float x, float y) {  
    fundo_.setPosition({x, y}); //define a posição do retanglo
    texto_.setPosition({x + largura_ / 2.f, y + altura_ / 2.f}); //posição do texto
    if (sprite_.has_value()) 
        sprite_->setPosition({x, y});// posição do icones
}

void Botao::desenharBotao(sf::RenderWindow& janela) {
    janela.draw(fundo_); //desenha o retangulo
        if (sprite_.has_value())
        janela.draw(*sprite_);
    else
    janela.draw(texto_); //desenha o texto
}

bool Botao::foiClicado(sf::Vector2f mouse) const {
    return fundo_.getGlobalBounds().contains(mouse);
}