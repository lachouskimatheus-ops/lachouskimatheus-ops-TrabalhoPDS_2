#ifndef REGRAS_H
#define REGRAS_H

#include "Baralho.hpp"
#include <vector>

/**
 * @brief Contém as regras de movimentação do jogo Paciência (Klondike Solitaire).
 *
 * Classe utilitária composta exclusivamente por métodos estáticos.
 * Não deve ser instanciada — o construtor é explicitamente deletado.
 * Centraliza toda a lógica de validação de movimentos, garantindo
 * que as regras sejam aplicadas de forma consistente em todo o jogo.
 */
class Regras {
public:
    /// @brief Construtor deletado: esta classe não deve ser instanciada.
    Regras() = delete;

    /**
     * @brief Verifica se uma carta pode ser movida sobre outra em uma coluna.
     *
     * Regras aplicadas:
     * - A carta de origem deve ter valor exatamente um menor que o destino.
     * - A carta de origem deve ter cor oposta à de destino (vermelho/preto).
     *
     * @param origem  Carta que será movida.
     * @param destino Carta que está no topo da coluna de destino.
     * @return true se o movimento é válido.
     */
    static bool podeMoverParaColuna(const Carta& origem, const Carta& destino);

    /**
     * @brief Verifica se uma carta pode ser colocada em uma pilha de fundação.
     *
     * Regras aplicadas:
     * - Se a pilha estiver vazia, somente um Ás (valor 1) é aceito.
     * - Caso contrário, a carta deve ter o mesmo naipe da pilha e valor
     *   exatamente um maior que a carta do topo.
     *
     * @param carta Carta a ser movida para a fundação.
     * @param pilha Pilha de fundação de destino (pode estar vazia).
     * @return true se o movimento é válido.
     */
    static bool podeMoverParaFundacao(const Carta& carta, const std::vector<Carta>& pilha);

    /**
     * @brief Verifica se uma carta pode ser colocada em uma coluna vazia.
     *
     * Regra aplicada: apenas Reis (valor 13) podem ocupar colunas vazias.
     *
     * @param carta Carta candidata ao movimento.
     * @return true se a carta é um Rei.
     */
    static bool podeMoverParaColunaVazia(const Carta& carta);

private:
    /**
     * @brief Determina se uma carta é de cor vermelha (Copas ou Ouros).
     * @param c Carta a ser verificada.
     * @return true se o naipe da carta é Copa ou Ouro.
     */
    static bool ehVermelha(const Carta& c);
};

#endif