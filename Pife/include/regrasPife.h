#ifndef REGRAS_PIFE_H
#define REGRAS_PIFE_H

#include <vector>
#include "carta.h"

bool ehTrinca(const std::vector<Carta>& cartas);
bool ehSequencia(const std::vector<Carta>& cartas);
bool possuiCombinacaoValida(const std::vector<Carta>& cartas);

#endif