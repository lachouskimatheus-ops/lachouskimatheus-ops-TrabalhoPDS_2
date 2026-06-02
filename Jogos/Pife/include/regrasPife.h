#ifndef REGRAS_PIFE_H
#define REGRAS_PIFE_H

#include <Carta.hpp>
#include <vector>

bool trinca(const std::vector<Carta>& cartas, int i1, int i2, int i3);
bool sequencia(const std::vector<Carta>& cartas, int i1, int i2, int i3);
bool combinacaoValida(const std::vector<Carta>& cartas, int i1, int i2, int i3);
bool indiceValido(const std::vector<Carta>& mao, int i);
bool buscaCombinacao(std::vector<Carta> mao, int paresFormados);

bool verificaVitoria(const std::vector<Carta>& mao);

#endif