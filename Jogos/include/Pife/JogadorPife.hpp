#ifndef JOGADOR_PIFE_HPP
#define JOGADOR_PIFE_HPP

#include "Carta.hpp"
#include <string>
#include <vector>

/**
 * @brief Representa um jogador na partida de Pife.
 *
 * Gerencia o nome e a mão de cartas do jogador, oferecendo operações
 * de recebimento, descarte e organização das cartas.
 */
class JogadorPife {
private:
    std::string nome_;        ///< Nome do jogador
    std::vector<Carta> mao_;  ///< Cartas atualmente na mão do jogador

    /**
     * @brief Verifica se um índice é válido para a mão atual.
     * @param indice Índice a verificar.
     * @return true se o índice está dentro dos limites da mão.
     */
    bool indiceValido(int indice) const;

public:
    /** @brief Construtor padrão. Cria um jogador sem nome e sem cartas. */
    JogadorPife();

    // =========================================================
    /// @name Identidade
    // =========================================================
    ///@{

    /**
     * @brief Define o nome do jogador.
     * @param nome Novo nome a ser atribuído.
     */
    void definirNome(const std::string& nome);

    /**
     * @brief Retorna o nome do jogador.
     * @return Referência constante para o nome.
     */
    const std::string& nome() const;

    ///@}

    // =========================================================
    /// @name Gerenciamento da Mão
    // =========================================================
    ///@{

    /**
     * @brief Adiciona uma carta à mão do jogador.
     * @param carta Carta a ser recebida.
     */
    void receberCarta(const Carta& carta);

    /**
     * @brief Ordena as cartas da mão por naipe e valor.
     *
     * Facilita a visualização e a busca por combinações válidas.
     */
    void organizarMao();

    /**
     * @brief Retorna o número de cartas atualmente na mão.
     * @return Tamanho da mão.
     */
    int tmnhMao() const;

    /**
     * @brief Remove e retorna uma carta da mão pelo índice.
     * @param indice Índice da carta a ser descartada (0-based).
     * @return Cópia da carta descartada.
     */
    Carta descartarCarta(int indice);

    /**
     * @brief Retorna a mão completa do jogador para consulta.
     * @return Referência constante para o vetor de cartas.
     */
    const std::vector<Carta>& verMao() const;

    ///@}
};

#endif