#ifndef ROUNDEDRECT_HPP
#define ROUNDEDRECT_HPP

#include <SFML/Graphics.hpp>
#include <cmath>

class RoundedRect : public sf::Shape {
public:
    RoundedRect(sf::Vector2f size, float radius, int pontos = 8)
        : tamanho_(size), raio_(radius), pontos_(pontos) {
        update();
    }

    void setSize(sf::Vector2f size) { tamanho_ = size; update(); }
    void setRadius(float r)         { raio_ = r;       update(); }

    std::size_t getPointCount() const override {
        return pontos_ * 4;
    }

    sf::Vector2f getPoint(std::size_t index) const override {
        float angulo = (float)index / pontos_ * 90.f;
        float rad    = angulo * 3.14159265f / 180.f;

        int canto = index / pontos_;
        sf::Vector2f centro;

        switch (canto) {
            case 0: centro = {tamanho_.x - raio_, raio_};                        break; // sup direito
            case 1: centro = {raio_,              raio_};                        break; // sup esquerdo
            case 2: centro = {raio_,              tamanho_.y - raio_};           break; // inf esquerdo
            case 3: centro = {tamanho_.x - raio_, tamanho_.y - raio_};           break; // inf direito
        }

        float localIndex = index % pontos_;
        float localAngle = (localIndex / pontos_) * 90.f;
        float localRad   = (localAngle + canto * 90.f) * 3.14159265f / 180.f;

        return {
            centro.x + raio_ * std::cos(localRad),
            centro.y - raio_ * std::sin(localRad)
        };
    }

private:
    sf::Vector2f tamanho_;
    float raio_;
    int pontos_;
};

#endif