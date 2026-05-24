#include "TelaMenu.hpp"
using namespace sf;

TelaMenu::TelaMenu(sf::Font& fonte, sf::RenderWindow& janela)
    : TelaBase(fonte, janela),
      titulo_(fonte, "ESCOLHA UM JOGO", 40),
      btnPaciencia_("PACIENCIA", fonte),
      btnPife_("PIFE", fonte),
      btnPoker_("POKER", fonte),
      btnTruco_("TRUCO", fonte),
      btnRoubaMonte_("ROUBA MONTE", fonte),
      btnFDP_("FDP", fonte),
      btnSair_("SAIR", fonte),
      opcaoSelecionada_(OpcaoMenu::Nenhuma)
{
    titulo_.setFillColor(Color::White);
    posicionarBotoes();
}

void TelaMenu::posicionarBotoes() {
    float largura = janela_.getSize().x;
    float centroX = (largura - 300.f) / 2.f;
    float inicioY = 150.f;
    float espacamento = 70.f;

    // Centraliza o título (SFML 3: getLocalBounds().size.x)
    FloatRect limiteTitulo = titulo_.getLocalBounds();
    titulo_.setPosition({(largura - limiteTitulo.size.x) / 2.f, 60.f});

    btnPaciencia_.definirPosicao (centroX, inicioY);
    btnPife_.definirPosicao      (centroX, inicioY + espacamento * 1);
    btnPoker_.definirPosicao     (centroX, inicioY + espacamento * 2);
    btnTruco_.definirPosicao     (centroX, inicioY + espacamento * 3);
    btnRoubaMonte_.definirPosicao(centroX, inicioY + espacamento * 4);
    btnFDP_.definirPosicao       (centroX, inicioY + espacamento * 5);
    btnSair_.definirPosicao      (centroX, inicioY + espacamento * 6);
}

void TelaMenu::desenhar() {
    janela_.clear(Color(30, 30, 30));
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
    // No menu o Esc não faz nada
    (void)evento;
}

OpcaoMenu TelaMenu::getOpcaoSelecionada() const {
    return opcaoSelecionada_;
}