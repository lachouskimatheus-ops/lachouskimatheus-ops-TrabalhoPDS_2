#ifndef REGRAS_PIFE_H
#define REGRAS_PIFE_H

#include <cartas.h>
#include <vector>

//Lógica das regras principais do jogo, como as combinações.

bool trinca(const std::vector<Carta>& cartas, int i1, int i2, int i3); //Verifica se as treis cartas são iguais
bool sequencia(const std::vector<Carta>& cartas, int i1, int i2, int i3); //par de  mesma sequencia e naipes iguais
bool combinacaoValida(const std::vector<Carta>& cartas, int i1, int i2, int i3); 
bool indiceValido(const std::vector<Carta>& mao, int i);

#endif