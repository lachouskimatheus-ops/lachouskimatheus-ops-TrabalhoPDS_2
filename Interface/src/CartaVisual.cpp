#include "CartaVisual.hpp"

CartaVisual::CartaVisual(const Carta& carta, const sf::Texture& textura) : carta_(carta), sprite_(textura) {
    
    largura_ = sprite_.getGlobalBounds().size.x;
    altura_ = sprite_.getGlobalBounds().size.y;
}

void CartaVisual::setScale(sf::Vector2f escala) {
    sprite_.setScale(escala);
    
    //Tamanhos atualizados para interação com o clique
    largura_ = sprite_.getGlobalBounds().size.x;
    altura_ = sprite_.getGlobalBounds().size.y;
}

void CartaVisual::definirPosicao(float x, float y) {
    sprite_.setPosition({x, y});
}

void CartaVisual::desenhar(sf::RenderWindow& janela) {
    janela.draw(sprite_);
}

bool CartaVisual::foiClicada(sf::Vector2f mouse) const {
    if (sprite_.getGlobalBounds().contains(mouse)) {
        return true;
    };
    return false;
}

Carta CartaVisual::mostrarCarta() const {
    return carta_;
}