#include "GerenciadorTexturas.hpp"

// Método para qualquer imagem do jogo
sf::Texture& GerenciadorTexturas::getTexturaGeral(const std::string& caminhoCompleto) {
    if (cacheTexturas_.find(caminhoCompleto) == cacheTexturas_.end()) {
        if (!cacheTexturas_[caminhoCompleto].loadFromFile(caminhoCompleto)) {
            std::cout << "ERRO FATAL: Não achei a imagem: " << caminhoCompleto << "\n";
        } else {
            cacheTexturas_[caminhoCompleto].setSmooth(true);
        };
    };
    return cacheTexturas_[caminhoCompleto];
}

// O tradutor de cartas
std::string GerenciadorTexturas::obterNomeArquivoCarta(Carta* carta) {
    int valor = carta->getValor(); 
    Naipe naipe = carta->getNaipe();

    //Traduz o naipe para o inglês
    std::string strNaipe;
    switch(naipe) {
        case Naipe::copas:   strNaipe = "hearts"; break;
        case Naipe::espadas: strNaipe = "spades"; break;
        case Naipe::ouros:   strNaipe = "diamonds"; break;
        case Naipe::paus:    strNaipe = "clubs"; break;
    };

    //Traduz o valor para a formatação do asset
    std::string strValor;
    if (valor == 1) {
        strValor = "ace";
    } else if (valor == 11) {
        strValor = "jack";
    } else if (valor == 12) {
        strValor = "queen";
    } else if (valor == 13) {
        strValor = "king";
    } else if (valor >= 2 && valor <= 9) {
        strValor = "0" + std::to_string(valor);
    } else {
        strValor = std::to_string(valor); 
    };

    // Concatena tudo no padrão "naipe_valor.png"
    return strNaipe + "_" + strValor + ".png"; 
}

// Método específico para cartas
sf::Texture& GerenciadorTexturas::getTexturaCarta(Carta* carta) {
    std::string nomeArquivo = obterNomeArquivoCarta(carta);
    std::string caminhoCompleto = "../../Interface/assets/cartas/" + nomeArquivo;
    
    // Chama o método geral repassando o caminho montado
    return getTexturaGeral(caminhoCompleto);
}