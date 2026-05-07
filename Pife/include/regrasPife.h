#ifndef REGRAS_PIFE_H
#define REGRAS_PIFE_H

#include <cartas.h>
#include <vector>

//Lógica das regras principais do jogo, como as combinações.

bool Trinca(const std::vector<Carta>& cartas);
bool Sequencia(const std::vector<Carta>& cartas);
bool CombinacaoValida(const std::vector<Carta>& cartas);

#endif