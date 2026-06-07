#ifndef REGRAS_PIFE_HPP
#define REGRAS_PIFE_HPP

#include "Carta.hpp"

#include <vector>

bool indiceValido(const std::vector<Carta>& mao, int indice);
Valor valorDoCoringa(const Carta& vira);

bool cartaEhCoringa(const Carta& carta, const Carta& vira);
bool trinca(const std::vector<Carta>& mao,int i1, int i2, int i3, const Carta& vira);
bool sequencia(const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira);
bool combinacaoValida(const std::vector<Carta>& mao,int i1, int i2, int i3, const Carta& vira);
bool buscaCombinacao(std::vector<Carta> mao, int combinacoesFormadas, const Carta& vira);
bool verificaVitoria(const std::vector<Carta>& mao, const Carta& vira);

#endif