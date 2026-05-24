#include "TelaMenu.hpp"
using namespace sf;

TelaMenu::TelaMenu(sf::Font& fonte, sf::RenderWindow& janela)
    : TelaBase(fonte, janela),
      titulo_(fonte, "ESCOLHA UM JOGO", 40),
      btnPaciencia_("Interface/assets/icones/btn_paciencia.png", 250.f, 100.f, fonte),
      btnPife_("Interface/assets/icones/btn_pife.png", 250.f, 100.f, fonte),
      btnPoker_("Interface/assets/icones/btn_poker.png", 250.f, 100.f, fonte),
      btnTruco_("Interface/assets/icones/btn_truco.png", 250.f, 100.f, fonte),
      btnRoubaMonte_("Interface/assets/icones/btn_roubamonte.png", 250.f, 100.f, fonte),
      btnFDP_("Interface/assets/icones/btn_fdp.png", 250.f, 100.f, fonte),
      btnSair_("SAIR", fonte),
      opcaoSelecionada_(OpcaoMenu::Nenhuma)
{
    // Carrega a textura e cria o sprite depois
 if (textureFundo_.loadFromFile("Interface/assets/fundo.jpg")) {
    fundoSprite_.emplace(textureFundo_);
    float scaleX = 900.f / textureFundo_.getSize().x;
    float scaleY = 700.f / textureFundo_.getSize().y;
    fundoSprite_->setScale({scaleX, scaleY});
}
    titulo_.setFillColor(sf::Color::White);
    posicionarBotoes();

}

void TelaMenu::posicionarBotoes() {
    float largura = janela_.getSize().x;
    float centroX = (largura - 250.f) / 2.f;  // 250 = largura dos botões
    float inicioY = 80.f;
    float espacamento = 90.f;  // 100 altura + margem

    sf::FloatRect limiteTitulo = titulo_.getLocalBounds();
    titulo_.setPosition({(largura - limiteTitulo.size.x) / 2.f, 20.f});

    btnPaciencia_.definirPosicao (centroX, inicioY);
    btnPife_.definirPosicao      (centroX, inicioY + espacamento * 1);
    btnPoker_.definirPosicao     (centroX, inicioY + espacamento * 2);
    btnTruco_.definirPosicao     (centroX, inicioY + espacamento * 3);
    btnRoubaMonte_.definirPosicao(centroX, inicioY + espacamento * 4);
    btnFDP_.definirPosicao       (centroX, inicioY + espacamento * 5);
    btnSair_.definirPosicao      (centroX, inicioY + espacamento * 6);
}

void TelaMenu::desenhar() {
    janela_.clear();
if (fundoSprite_.has_value())
    janela_.draw(*fundoSprite_);
    janela_.draw(titulo_);
    btnPaciencia_.desenharBotao(janela_);
    btnPife_.desenharBotao(janela_);
    btnPoker_.desenharBotao(janela_);
    btnTruco_.desenharBotao(janela_);
    btnRoubaMonte_.desenharBotao(janela_);
    btnFDP_.desenharBotao(janela_);
    btnSair_.desenharBotao(janela_);
}

void TelaMenu::processarClique(sf::Vector2f mouse) {
    if      (btnPaciencia_.foiClicado(mouse))  opcaoSelecionada_ = OpcaoMenu::Paciencia;
    else if (btnPife_.foiClicado(mouse))        opcaoSelecionada_ = OpcaoMenu::Pife;
    else if (btnPoker_.foiClicado(mouse))       opcaoSelecionada_ = OpcaoMenu::Poker;
    else if (btnTruco_.foiClicado(mouse))       opcaoSelecionada_ = OpcaoMenu::Truco;
    else if (btnRoubaMonte_.foiClicado(mouse))  opcaoSelecionada_ = OpcaoMenu::RoubaMonte;
    else if (btnFDP_.foiClicado(mouse))         opcaoSelecionada_ = OpcaoMenu::FDP;
    else if (btnSair_.foiClicado(mouse))        estado_ = EstadoTela::Sair;
}

void TelaMenu::processarEvento(const sf::Event& evento) {
    (void)evento;
}

OpcaoMenu TelaMenu::getOpcaoSelecionada() const {
    return opcaoSelecionada_;
}