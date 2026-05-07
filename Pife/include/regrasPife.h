#ifndef REGRAS_PIFE_H
#define REGRAS_PIFE_H

#include <cartas.h>
#include <vector>

//Lógica das regras principais do jogo, como as combinações.

bool Trinca(const std::vector<Carta>& cartas); //par de cartas iguais mas naipes diferentes
bool Sequencia(const std::vector<Carta>& cartas); //par de  mesma sequencia e naipes iguais
bool CombinacaoValida(const std::vector<Carta>& cartas);

#endif