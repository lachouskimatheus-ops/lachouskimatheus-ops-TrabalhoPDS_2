/**
 * @file Poker.hpp
 * @brief Definição da classe Poker para gerenciamento e avaliação de uma mão de cartas.
 */

#pragma once

#include <string>
#include <vector>
#include "Carta.hpp"

/**
 * @class Poker
 * @brief Responsável por gerenciar as cartas na mão de um jogador individual e avaliar seu valor no poker clássico.
 *
 * Esta classe armazena uma coleção de objetos do tipo `Carta`, fornecendo mecanismos 
 * para receber cartas, realizar descartes/substituições, além de computar a força 
 * regulamentar da combinação de cartas obtida para fins de comparação e desempate.
 */
class Poker {
private:
    std::vector<Carta> mao_; ///< Vetor contendo as cartas atuais na mão do jogador.

    /**
     * @brief Gera um vetor de pesos numéricos para critérios detalhados de desempate.
     * @return Vetor de inteiros ordenados que definem o valor hierárquico das cartas singulares na jogada.
     */
    std::vector<int> gerarPontuacaoDesempate() const;

public:
    /**
     * @brief Construtor padrão da classe Poker. Inicializa uma mão vazia.
     */
    Poker() = default;

    /**
     * @brief Adiciona uma nova carta à mão do jogador.
     * @param carta Objeto Carta a ser adicionado.
     * @return true se a carta foi recebida com sucesso, false caso contrário.
     */
    bool receberCarta(const Carta& carta);

    /**
     * @brief Remove todas as cartas da mão atual, resetando o estado do jogador.
     */
    void limparMao();

    /**
     * @brief Realiza a substituição de cartas específicas da mão por novas cartas compradas.
     * @param indices Vetor contendo os índices (0 a tamanho-1) das cartas que serão descartadas.
     * @param novasCartas Vetor com as novas entidades Carta que ocuparão os lugares das descartadas.
     * @return true se a operação de troca foi bem-sucedida, false se houver índices inválidos ou tamanhos incompatíveis.
     */
    bool substituirCartas(const std::vector<int>& indices, const std::vector<Carta>& novasCartas);

    /**
     * @brief Avalia a força da mão atual e calcula uma pontuação base para a jogada.
     * @return Inteiro representando a categoria da jogada (ex: Par, Trinca, Flush, etc.). Quanto maior, mais forte a jogada.
     */
    int avaliarMao() const;

    /**
     * @brief Compara a força da mão atual contra a mão de outro jogador para determinar o vencedor.
     * @param outraMao Instância de Poker que será confrontada.
     * @return 1 se a mão atual for a vencedora, -1 se a outra mão vencer, ou 0 em caso de empate absoluto.
     */
    int compararCom(const Poker& outraMao) const;

    /**
     * @brief Obtém o nome textual formatado da combinação de poker atual.
     * @return String contendo o nome da jogada em português (ex: "Dois Pares", "Full House", "Flush").
     */
    std::string nomeDaJogada() const;

    /**
     * @brief Consulta a quantidade atual de cartas na mão do jogador.
     * @return Quantidade de cartas (normalmente 5 em jogos tradicionais).
     */
    int tamanhoMao() const;

    /**
     * @brief Retorna uma referência constante para o vetor interno de cartas da mão.
     * @return Referência constante para std::vector<Carta>.
     */
    const std::vector<Carta>& verMao() const;
};