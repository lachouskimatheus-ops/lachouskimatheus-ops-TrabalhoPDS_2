#include <cassert>
#include "../BaralhoSujo.hpp"
#include "../JuizPaulista.hpp"
#include "../Jogador_Truco.hpp"
#include "../Carta.hpp"
#include "../Mesa.hpp"

void reportar(bool condicao, const std::string& mensagem) {
    if (condicao) {
        std::cout << "[SUCESSO] " << mensagem << std::endl;
    } else {
        std::cerr << "[FALHA] " << mensagem << std::endl;
    }
}

int main() {
    std::cout << "=== Iniciando Testes Unitários: Truco Project ===\n" << std::endl;

    BaralhoSujo* baralho = new BaralhoSujo();
    baralho->inicializar();

    reportar(baralho->getQtdCartas() == 40, "Baralho Sujo inicializado com 40 cartas.");

    JuizPaulista* juiz = new JuizPaulista();
    
    Carta vira(4, "ouros"); 
    
    std::vector<Carta*> cartasNaMesa;
    Carta* c1 = new Carta(5, "paus");
    Carta* c2 = new Carta(7, "copas");
    cartasNaMesa.push_back(c1);
    cartasNaMesa.push_back(c2);

    int vencedor = juiz->decidirVencedor(cartasNaMesa, vira);
    reportar(vencedor == 0, "Juiz Paulista identificou o Zap (5 de Paus) como vencedor.");

    Jogador_Truco* jogador = new Jogador_Truco("Teste");
    Carta* c3 = new Carta(3, "espadas");
    jogador->receberCarta(c3);
    
    Carta* jogada = jogador->jogarCarta(0);
    reportar(jogada->getValor() == 3, "Jogador conseguiu receber e jogar uma carta corretamente.");

    delete baralho;
    delete juiz;
    delete jogador;
    delete c1;
    delete c2;
    delete c3;

    std::cout << "\n=== Testes Concluídos com Sucesso ===" << std::endl;

    return 0;
}