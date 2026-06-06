#ifndef CARTA_HPP
#define CARTA_HPP

#include <iostream>
#include <string>

// O segredo da compatibilidade: Aliases dentro do enum class
enum class Naipe {
    paus = 0,
    Paus = 0,
    copas = 1,
    Copa = 1,
    espadas = 2,
    Espada = 2,
    ouros = 3,
    Ouro = 3,
    Nenhum = 4
};

enum class Valor {
    Invalido = 0,
    As = 1, Dois = 2, Tres = 3, Quatro = 4, Cinco = 5,
    Seis = 6, Sete = 7, Oito = 8, Nove = 9, Dez = 10,
    Valete = 11, Dama = 12, Rei = 13
};

class Carta {
private:
    int valor_;
    Naipe naipe_;

public:
    // Construtores unificados
    Carta();
    Carta(int valor, Naipe naipe);
    Carta(Valor valor, Naipe naipe);

    // ==========================================
    // Métodos originais do jogo FDP
    // ==========================================
    int getValor() const;
    Naipe getNaipe() const;
    int forca() const; 
    int getForcaNaipe() const;
    std::string toString() const;

    // ==========================================
    // Métodos originais do jogo Truco
    // ==========================================
    Valor mostraValor() const;
    Naipe mostraNaipe() const;
    bool validacaoCarta() const;
    std::string valorString() const;
    std::string naipeString() const;
    std::string cartaString() const;
};

#endif