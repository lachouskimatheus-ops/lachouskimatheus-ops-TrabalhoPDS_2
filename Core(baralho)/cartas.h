#ifndef CARTA_H
#define CARTA_H

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
    Rei,
    Coringa
};

class Carta { //para cada carta específica
private:
    Valor valor_; //pode assumir os 13 valores
    Naipe naipe_;  //pode ser ouro, copa, espada, paus

public:
    Carta();
    Carta(Valor valor, Naipe naipe);

};

#endif
