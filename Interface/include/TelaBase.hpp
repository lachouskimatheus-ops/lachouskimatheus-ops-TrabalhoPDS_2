#ifndef TELABASE_HPP
#define TELABASE_HPP

#include <SFML/Graphics.hpp>

enum class EstadoTela {
    Rodando,
    Pausado,
    Voltar,
    Sair
};

class TelaBase {
protected:
    sf::Font& fonte_;
    sf::RenderWindow& janela_;
    EstadoTela estado_;

public:
    TelaBase(sf::Font& fonte, sf::RenderWindow& janela)
        : fonte_(fonte), janela_(janela), estado_(EstadoTela::Rodando) {}

    virtual void desenhar() = 0;
    virtual void processarClique(sf::Vector2f mouse) = 0;
    virtual void processarEvento(const sf::Event& evento) {
        if (const auto* key = evento.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape) {
                estado_ = EstadoTela::Pausado;
            }
        }
    }

    EstadoTela getEstado() const { return estado_; }
    void setEstado(EstadoTela e) { estado_ = e; }

    virtual ~TelaBase() {}
};

#endif