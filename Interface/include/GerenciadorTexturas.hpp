#ifndef GERENCIADORTEXTURAS_HPP
#define GERENCIADORTEXTURAS_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>
#include "../../Jogos/Core/CoreBaralhoTruco/Carta.hpp" 

class GerenciadorTexturas {
private:
    std::map<std::string, sf::Texture> cacheTexturas_;
    GerenciadorTexturas() {}

public:
    static GerenciadorTexturas& getInstancia() {
        static GerenciadorTexturas instancia; 
        return instancia;
    }

    GerenciadorTexturas(GerenciadorTexturas const&) = delete;
    void operator=(GerenciadorTexturas const&) = delete;

    sf::Texture& getTexturaGeral(const std::string& caminhoCompleto);
    sf::Texture& getTexturaCarta(Carta* carta);

private:
    std::string obterNomeArquivoCarta(Carta* carta);
};

#endif