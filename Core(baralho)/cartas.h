#ifndef CARTAS_H
#define CARTAS_H

#include <string>

//enquanto pesquisava a melhor forma de fazer a classe carta,
//encontrei esse (enum class), que deixa o naipe/valor fixo para esses listados
enum class Naipe { 
    Ouro,
    Copa,
    Espada,
    Paus,
    Nenhum
};

enum class Valor {
    Invalido = 0,
    As = 1,
    Dois,
    Tres,
    Quatro,
    Cinco,
    Seis,
    Sete,
    Oito,
    Nove,
    Dez,
    Valete,
    Dama,
    Rei
};

class Carta { //para cada carta específica
private:
    Valor valor_; //pode assumir os 13 valores
    Naipe naipe_;  //pode ser ouro, copa, espada, paus

public:
    Carta();
    Carta(Valor valor, Naipe naipe);

    Valor mostraValor() const; //Retorna ex: Valor::REI
    Naipe mostraNaipe() const; //Retorna ex: Naipe::Copa

    bool validacaoCarta() const;

    std::string valorString() const; //tranforma mostraValor() em string
    std::string naipeString() const; //tranforma mostraNaipe() em string
    std::string cartaString() const; //tranforma a carta em string, ex: As de Copa

};

#endif
