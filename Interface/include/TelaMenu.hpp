#ifndef TELAMENU_HPP
#define TELAMENU_HPP

#include <SFML/Graphics.hpp>
#include "TelaBase.hpp"
#include "Botao.hpp"

enum class OpcaoMenu {
    Nenhuma,
    Paciencia,
    Pife,
    Poker,
    Truco,
    RoubaMonte,
    FDP,
    Sair
};

class TelaMenu : public TelaBase {
private:
    sf::Text titulo_;

    Botao btnPaciencia_;
    Botao btnPife_;
    Botao btnPoker_;
    Botao btnTruco_;
    Botao btnRoubaMonte_;
    Botao btnFDP_;
    Botao btnSair_;

    OpcaoMenu opcaoSelecionada_;

    void posicionarBotoes();

public:
    TelaMenu(sf::Font& fonte, sf::RenderWindow& janela);

    void desenhar() override;
    void processarClique(sf::Vector2f mouse) override;
    void processarEvento(const sf::Event& evento) override;

    OpcaoMenu getOpcaoSelecionada() const;
};

#endif