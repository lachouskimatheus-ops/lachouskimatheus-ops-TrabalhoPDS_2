#include "Botao.hpp"

Botao::Botao(const std::string& texto, sf::Font& fonte) {
    largura_ = 200.f; //parametros para o tamnho do botao, da pra mudar se precisar
    altura_ = 60.f;

    fundo_.setSize(sf::Vector2f(largura_, altura_)); //define o tamanho do retangulo
    fundo_.setFillColor(sf::Color(40, 120, 200)); //cor do botao
    fundo_.setOutlineColor(sf::Color::White); //cor da borda
    fundo_.setOutlineThickness(2.f);  //espessura da borda
    texto_.setFont(fonte);  //tem que imporntar a font dps
    texto_.setString(texto);
    texto_.setCharacterSize(28); //tamanho da fonte
    texto_.setFillColor(sf::Color::White);
    sf::FloatRect areaTexto = texto_.getLocalBounds(); //calcula os parametros do texto para centralizar

    texto_.setOrigin(areaTexto.left + areaTexto.width / 2.f, areaTexto.top + areaTexto.height / 2.f); //define o centro do texto como ponto de referência

    texto_.setPosition(largura_ / 2.f, altura_ / 2.f); //centraliza o texto
}

void Botao::definirPosicaoBotao(float x, float y) {
    fundo_.setPosition(x, y); //define a posição do retanglo
    texto_.setPosition(x + largura_ / 2.f, y + altura_ / 2.f); //posição do texto

}

void Botao::desenharBotao(sf::RenderWindow& janela) {
    janela.draw(fundo_); //desenha o retangulo
    janela.draw(texto_); //desenha o texto

}

bool Botao::foiClicado(sf::Vector2f mouse) const {
    return fundo_.getGlobalBounds().contains(mouse);
}