#include "Baralho.hpp"
#include <algorithm>
#include <stdexcept>

// ==========================================
// CONSTRUTOR E DESTRUTOR UNIFICADOS
// ==========================================
Baralho::Baralho(int quantidadeBaralhos) {
    if (quantidadeBaralhos > 0) {
        std::vector<Valor> valor = {
            Valor::As, Valor::Dois, Valor::Tres, Valor::Quatro,
            Valor::Cinco, Valor::Seis, Valor::Sete, Valor::Oito,
            Valor::Nove, Valor::Dez, Valor::Valete, Valor::Dama, Valor::Rei
        };

        std::vector<Naipe> naipe = {
            Naipe::Ouro, Naipe::Copa, Naipe::Espada, Naipe::Paus
        };

        // Percorre as cartas e aloca dinamicamente como ponteiros (new Carta)
        for (int b = 0; b < quantidadeBaralhos; b++) {
            for (size_t i = 0; i < naipe.size(); i++) {
                for (size_t j = 0; j < valor.size(); j++) {
                    cartas_.push_back(new Carta(valor[j], naipe[i]));
                }
            }
        }
    }
}

Baralho::~Baralho() {
    for (size_t i = 0; i < cartas_.size(); i++) {
        delete cartas_[i];
    }
    cartas_.clear();
}

Baralho::Baralho(const Baralho& outro) {
    for (size_t i = 0; i < outro.cartas_.size(); i++) {
        cartas_.push_back(new Carta(*outro.cartas_[i]));
    }
}

Baralho& Baralho::operator=(const Baralho& outro) {
    if (this == &outro) return *this;
    for (size_t i = 0; i < cartas_.size(); i++) delete cartas_[i];
    cartas_.clear();
    for (size_t i = 0; i < outro.cartas_.size(); i++) {
        cartas_.push_back(new Carta(*outro.cartas_[i]));
    }
    return *this;
}

void Baralho::inicializar() {
    // Fica vazio na classe mãe.
    // O construtor já fez o trabalho para o Baralho Padrão.
    // As filhas, como BaralhoSujo, sobrescrevem este método para limpar e criar o deck de 40 cartas.
}

// Usamos a versão moderna de embaralhamento do FDP para todos os jogos!
void Baralho::embaralhar() {
    std::random_device rd;
    std::mt19937 gerador(rd());
    std::shuffle(cartas_.begin(), cartas_.end(), gerador);
}

// ==========================================
// LÓGICA DO FDP (Retira do Fundo, retorna Ponteiro)
// ==========================================
Carta* Baralho::puxarCarta() {
    if (cartas_.empty()) throw std::runtime_error("Sem cartas no baralho!");

    Carta* topo = cartas_.back();
    cartas_.pop_back();
    return topo;
}

int Baralho::getQtdCartas() const {
    return cartas_.size();
}

// ==========================================
// LÓGICA DO PIFE (Retira da Frente, retorna Objeto)
// ==========================================
Carta Baralho::retirarCarta() {
    if (estaVazio()) {
        std::cout << "Baralho Vazio!!!" << std::endl;
        return Carta(); // Retorna uma carta inválida da nossa Carta.hpp unificada
    }

    // Pega o ponteiro na frente do vetor
    Carta* primeira = cartas_.front();
    cartas_.erase(cartas_.begin());
    
    // Converte o ponteiro em um objeto sólido para entregar ao Pife
    Carta copia = *primeira;
    
    // Deleta o ponteiro da memória (já que o Pife não gerencia memória manualmente)
    delete primeira; 
    
    return copia;
}

void Baralho::inserirCarta(const Carta& c) {
    // Recebe o objeto do Pife e aloca como ponteiro no vetor interno
    cartas_.push_back(new Carta(c));
}

int Baralho::tamanho() const {
    return cartas_.size();
}

bool Baralho::estaVazio() const {
    return cartas_.empty();
}

void Baralho::limpar() {
    for (int i = 0; i < (int)cartas_.size(); i++) {
        delete cartas_[i];
    }
    cartas_.clear();
}